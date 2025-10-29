#include "../include/Router.hpp"
#include "../include/Printer.hpp"
#include "../include/Message.hpp"
#include "../include/NodeStore.hpp"
#include "../include/NetworkConfig.hpp"

void Router::run() {
    try {
        output_receiver.bind(NetworkConfig::OutputQueue.data());
        Printer::print_safe("[Router] Прив'язано output_receiver до " + std::string(NetworkConfig::OutputQueue)); // <-- Лог 1

        task_sender.connect(NetworkConfig::TaskQueue.data());
        Printer::print_safe("[Router] Прив'язано task_sender до " + std::string(NetworkConfig::TaskQueue)); // <-- Лог 2

        Printer::print_safe("[Router] Запущено. Слухаю " + std::string(NetworkConfig::OutputQueue));
        while (true) {
            zmq::message_t in_msg;
            if (!output_receiver.recv(in_msg)) continue;

            MessageSend message{};
            memcpy(&message, in_msg.data(), sizeof(MessageSend));

            auto targets = nodeStore->getRoutes(message.senderId);
            for (const auto &targetId: *targets) {
                MessageReceive task{message.senderId, targetId, message.data};
                zmq::message_t out_msg(sizeof(MessageReceive));
                memcpy(out_msg.data(), &task, sizeof(MessageReceive));
                this->task_sender.send(out_msg, zmq::send_flags::none);
            }
        }
    } catch (const zmq::error_t& e) { // Ловимо помилки ZMQ
        Printer::print_safe("[Router] ZMQ Помилка: " + std::string(e.what()) + " (" + std::to_string(e.num()) + ")");
    } catch (const std::exception& e) { // Ловимо інші помилки
        Printer::print_safe("[Router] Помилка: " + std::string(e.what()));
    }
}
