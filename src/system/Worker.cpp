#include "../../include/system/Worker.hpp"
#include "../../include/Printer.hpp"
#include "../../include/system/Message.hpp"
#include "../../include/system/NodeStore.hpp"
#include "../../include/system/NetworkConfig.hpp"

namespace NodeSystem {
    Worker::Worker(zmq::context_t *context, std::shared_ptr<NodeStore> store) : zmq_context(context),
        nodeStore(std::move(store)) {
    }

    MessageReceive Worker::readMessage(zmq::socket_t &task_receiver) {
        zmq::message_t zmq_message;
        Printer::print_safe("[Worker]: чекаємо дані на recv...");
        const auto result = task_receiver.recv(zmq_message);
        if (!result) {
            Printer::print_safe("[Worker]: recv повернув помилку або був перерваний.");
            throw std::runtime_error("Worker recv failed");
        }
        Printer::print_safe("[Worker]: recv отримав " + std::to_string(result.value()) + " байт.");
        MessageReceive message{};
        memcpy(&message, zmq_message.data(), sizeof(MessageReceive));
        return message;
    }

    void Worker::run() const {
        Printer::print_safe("[Worker] Запущено");
        zmq::socket_t task_receiver(*this->zmq_context, zmq::socket_type::pull);
        try {
            task_receiver.connect(this->taskOutQueue());
            Printer::print_safe("[Worker]: підключено до " + std::string(this->taskOutQueue()));

            while (true) {
                auto message = readMessage(task_receiver);
                Printer::print_safe("[Worker]: отримано дані " + std::to_string(message.data) +
                                    " від " + message.senderId.to_string() + " до " + message.targetId.to_string());
                if (const auto node = nodeStore->getNode(message.targetId)) {
                    node->process_input(message);
                } else {
                    Printer::print_safe("[Worker]: Вузол " + message.targetId.to_string() + " не знайдено!");
                }
            }
        } catch (const zmq::error_t &e) {
            Printer::print_safe(
                "[Worker] ZMQ Помилка: " + std::string(e.what()) + " (" + std::to_string(e.num()) + ")");
        } catch (const std::exception &e) {
            Printer::print_safe("[Worker] Помилка: " + std::string(e.what()));
        }
        Printer::print_safe("[Worker] Завершує роботу.");
    }
}
