#include "../include/NetworkManager.hpp"
#include "../include/Message.hpp"
#include "../include/Printer.hpp"
#include "../include/Worker.hpp"
#include "../include/SensorNode.hpp"


NetworkManager::NetworkManager()
    : zmq_context(1),
      nodeStore(std::make_shared<NodeStore>()),
      cli_task_sender(zmq_context, zmq::socket_type::push) {
}

void NetworkManager::run_async(int num_workers) {
    Printer::print_safe("[Manager] Запуск...");

    this->router = std::make_unique<Router>(&zmq_context, nodeStore);
    this->router_thread = std::thread(&Router::run, this->router.get());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    try {
        cli_task_sender.connect(NetworkConfig::TaskQueue.data());
        Printer::print_safe("[Manager] CLI Sender підключено до TaskQueue");
    } catch (const zmq::error_t &e) {
        Printer::print_safe("[Manager] Помилка підключення CLI Sender: " + std::string(e.what()));
    }
    for (int i = 0; i < num_workers; ++i) {
        auto worker_ptr = new Worker(&zmq_context, nodeStore);
        worker_threads.emplace_back(&Worker::run, worker_ptr);
    }
    router_thread.detach();
    for (auto &t: worker_threads) t.detach();

    Printer::print_safe("[Manager] Запущено " + std::to_string(num_workers) + " worker'ів.");
}


std::shared_ptr<Node> NetworkManager::createNode(const NodeId id) {
    auto node = nodeStore->createAndBindNode(id);
    setupNodeCallback(node);
    Printer::print_safe("[Manager] Створено вузол " + std::to_string(id.level) + "." + std::to_string(id.index));
    return node;
}

std::shared_ptr<SensorNode> NetworkManager::createSensorNode(const NodeId id) {
    auto node = std::make_shared<SensorNode>(id);
    setupNodeCallback(node);
    nodeStore->addNode(node);
    this->bind(id, {id.level, 0});
    Printer::print_safe("[Manager] Створено СЕНСОР " + std::to_string(id.level) + "." + std::to_string(id.index));
    return node;
}

void NetworkManager::bind(const NodeId from, const NodeId to) const {
    nodeStore->bind(from, to);
    Printer::print_safe("[Manager] Зв'язок: " + std::to_string(from.level) + "." + std::to_string(from.index) +
                        " -> " + std::to_string(to.level) + "." + std::to_string(to.index));
}

void NetworkManager::triggerSensor(NodeId sensorId) {
    Printer::print_safe(
        "[Manager] Тригер сенсора " + std::to_string(sensorId.level) + "." + std::to_string(sensorId.index));
    MessageReceive task{NodeId{999, 999}, sensorId, 0.0};
    zmq::message_t out_msg(sizeof(MessageReceive));
    memcpy(out_msg.data(), &task, sizeof(MessageReceive));
    try {
        auto result = cli_task_sender.send(out_msg, zmq::send_flags::none);
        if (result) {
            Printer::print_safe(
                "[Manager] Повідомлення тригера успішно надіслано (" + std::to_string(result.value()) + " байт).");
            // <-- Успіх
        } else {
            Printer::print_safe("[Manager] Помилка надсилання повідомлення тригера (send повернув false).");
            // <-- Помилка?
        }
    } catch (const zmq::error_t &e) {
        Printer::print_safe("[Manager] ZMQ Помилка при надсиланні тригера: " + std::string(e.what()));
        // <-- Помилка ZMQ?
    }
}

void NetworkManager::createLevel(const int level) {
    const auto node = std::make_shared<Node>(NodeId{level, 0});
    this->nodeStore->addNode(node);
    setupNodeCallback(node);
    nodeStore->bind(node->get_id(), {999, 0});
    Printer::print_safe(
        "[Manager] Створено генералізований вузол рівня " + std::to_string(level) + " і зв'язано з finalNode");
}

void NetworkManager::setupNodeCallback(const std::shared_ptr<Node> &node) {
    auto sender_ptr = std::make_shared<zmq::socket_t>(this->zmq_context, zmq::socket_type::push);
    sender_ptr->connect(NetworkConfig::OutputQueue.data());

    node->OnRecalculated =
            [sender = sender_ptr](const MessageSend &message) {
                zmq::message_t zmq_msg(sizeof(MessageSend));
                memcpy(zmq_msg.data(), &message, sizeof(MessageSend));
                sender->send(zmq_msg, zmq::send_flags::none);
                Printer::print_safe(
                    "[Вузол " + message.senderId.to_string() + "]: - відправлено дані " + std::to_string(message.data));
            };
}
