#pragma once

#include <zmq_addon.hpp>
#include "Message.hpp"
#include "NodeStore.hpp"

namespace NodeSystem {
    class Worker {
    public:
        explicit Worker(zmq::context_t *context, std::shared_ptr<NodeStore> store);

        static MessageReceive readMessage(zmq::socket_t &task_receiver);

        void run() const;
        std::function<std::string()> taskOutQueue;
    private:
        zmq::context_t *zmq_context;
        std::shared_ptr<NodeStore> nodeStore;
    };
}
