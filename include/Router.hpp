#pragma once

#include <zmq_addon.hpp>

class NodeStore;

class Router {
public:
    explicit Router(zmq::context_t *context, std::shared_ptr<NodeStore> store);

    void run();

private:
    zmq::context_t *zmq_context = nullptr;
    std::shared_ptr<NodeStore> nodeStore;
    zmq::socket_t task_sender;
    zmq::socket_t output_receiver;
};
