#include "../../include/system/SystemTemplate.hpp"
#include "../../include/system/Message.hpp"
#include "../../include/Printer.hpp"
#include "../../include/system/Worker.hpp"
#include "../../include/system/NetworkConfig.hpp"

namespace NodeSystem {
    SystemTemplate::SystemTemplate(const int id)
        : zmq_context(1),
          nodeStore(std::make_shared<NodeStore>()),
          cli_task_sender(zmq_context, zmq::socket_type::push) {
        this->id = id;
    }

    int SystemTemplate::get_id() const {
        return this->id;
    }

    void SystemTemplate::run_async(int num_workers) {
        Printer::print_safe("[Manager] Запуск проксі черги завдань...");

        auto proxy_runner = [this]() {
            try {
                zmq::socket_t frontend(this->zmq_context, zmq::socket_type::pull);
                frontend.bind(NetworkConfig::TaskQueueIn.data());

                zmq::socket_t backend(this->zmq_context, zmq::socket_type::push);
                backend.bind(NetworkConfig::TaskQueueOut.data());

                Printer::print_safe("[Proxy] Проксі запущено. Слухаю " + std::string(NetworkConfig::TaskQueueIn) +
                                    ", відправляю на " + std::string(NetworkConfig::TaskQueueOut));

                zmq::proxy(frontend, backend);
            } catch (const std::exception &e) {
                Printer::print_safe("[Proxy] Помилка: " + std::string(e.what()));
            }
        };

        this->proxy_thread = std::thread(proxy_runner);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        try {
            cli_task_sender.connect(NetworkConfig::TaskQueueIn.data());
            Printer::print_safe("[Manager] CLI Sender підключено до " + std::string(NetworkConfig::TaskQueueIn));
        } catch (const zmq::error_t &e) {
            Printer::print_safe("[Manager] Помилка підключення CLI Sender: " + std::string(e.what()));
        }

        Printer::print_safe("[Manager] Запуск Router...");
        this->router = std::make_unique<Router>(&zmq_context, nodeStore);
        this->router_thread = std::thread(&Router::run, this->router.get());

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        for (int i = 0; i < num_workers; ++i) {
            auto worker_ptr = new Worker(&zmq_context, nodeStore);
            worker_threads.emplace_back(&Worker::run, worker_ptr);
        }

        proxy_thread.detach();
        router_thread.detach();
        for (auto &t: worker_threads) t.detach();
    }


    std::shared_ptr<Node> SystemTemplate::createNode(const NodeId id) {
        auto node = nodeStore->createAndBindNode(id);
        setupNodeCallback(node);
        return node;
    }

    void SystemTemplate::bind(const NodeId from, const NodeId to) const {
        nodeStore->bind(from, to);
    }

    void SystemTemplate::triggerSensor(NodeId sensorId) {
        MessageReceive task{NodeId{999, 999}, sensorId, 0.0};
        zmq::message_t out_msg(sizeof(MessageReceive));
        memcpy(out_msg.data(), &task, sizeof(MessageReceive));
        try {
            auto result = cli_task_sender.send(out_msg, zmq::send_flags::none);
            if (result) {
                Printer::print_safe(
                    "[Manager] Повідомлення тригера успішно надіслано (" + std::to_string(result.value()) + " байт).");
            } else {
                Printer::print_safe("[Manager] Помилка надсилання повідомлення тригера (send повернув false).");
            }
        } catch (const zmq::error_t &e) {
            Printer::print_safe("[Manager] ZMQ Помилка при надсиланні тригера: " + std::string(e.what()));
        }
    }

    void SystemTemplate::createLevel(const int level) {
        const auto node = this->nodeStore->createLevel(level);
        setupNodeCallback(node);
    }

    void SystemTemplate::setupNodeCallback(const std::shared_ptr<Node> &node) {
        auto sender_ptr = std::make_shared<zmq::socket_t>(this->zmq_context, zmq::socket_type::push);
        sender_ptr->connect(NetworkConfig::OutputQueue.data());

        node->OnRecalculated =
                [sender = sender_ptr](const MessageSend &message) {
                    zmq::message_t zmq_msg(sizeof(MessageSend));
                    memcpy(zmq_msg.data(), &message, sizeof(MessageSend));
                    sender->send(zmq_msg, zmq::send_flags::none);
        };
    }
}
