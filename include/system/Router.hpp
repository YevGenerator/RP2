#pragma once

#include <zmq_addon.hpp>
#include "NodeStore.hpp"

namespace NodeSystem {
    class Router {
    public:
        explicit Router(zmq::context_t *context, std::shared_ptr<NodeStore> store);

        void run();
        std::function<std::string()> outputQueue;
        std::function<std::string()> taskInQueue;
    private:
        zmq::context_t *zmq_context = nullptr;
        std::shared_ptr<NodeStore> nodeStore;
        zmq::socket_t task_sender;
        zmq::socket_t output_receiver;
    };
}
