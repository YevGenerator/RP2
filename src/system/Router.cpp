#include "../../include/system/Router.hpp"
#include "../../include/Printer.hpp"
#include "../../include/system/Message.hpp"
#include "../../include/system/NodeStore.hpp"


namespace NodeSystem {
    Router::Router(zmq::context_t *context, std::shared_ptr<NodeStore> store)
        : zmq_context(context),
          nodeStore(std::move(store)),
          task_sender(*zmq_context, zmq::socket_type::push),
          output_receiver(*zmq_context, zmq::socket_type::pull) {
    }

    void Router::run() {
        try {
            output_receiver.bind(outputQueue());
            Printer::print_safe("[Router] Прив'язано output_receiver до " +outputQueue());

            task_sender.connect(taskInQueue());
            Printer::print_safe("[Router] Підключено task_sender до " + taskInQueue());

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
        } catch (const zmq::error_t &e) {
            // Ловимо помилки ZMQ
            Printer::print_safe("[Router] ZMQ Помилка: " + std::string(e.what()) + " (" + std::to_string(e.num()) + ")");
        } catch (const std::exception &e) {
            // Ловимо інші помилки
            Printer::print_safe("[Router] Помилка: " + std::string(e.what()));
        }
    }
}