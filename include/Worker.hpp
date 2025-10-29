#pragma once

#include <zmq_addon.hpp>

struct MessageReceive;
class NodeStore;

class Worker {
public:
    explicit Worker(zmq::context_t *context, std::shared_ptr<NodeStore> store);

    static MessageReceive readMessage(zmq::socket_t &task_receiver);

    void run() const;

private:
    zmq::context_t *zmq_context;
    std::shared_ptr<NodeStore> nodeStore;
};
