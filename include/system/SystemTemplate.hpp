#pragma once
#include <thread>
#include <vector>
#include <memory>
#include <zmq_addon.hpp>

#include "NodeStore.hpp"

#include "Router.hpp"

namespace NodeSystem {
    class SystemTemplate {
    public:
        SystemTemplate(int id = 0);

        ~SystemTemplate() {
            // TODO: Треба реалізувати коректну зупинку потоків
        }
        int get_id() const;
        void run_async(int num_workers = 1);

        std::shared_ptr<Node> createNode(NodeId id);

        void bind(NodeId from, NodeId to) const;

        void triggerSensor(NodeId sensorId);

        void createLevel(int level);

    private:
        void setupNodeCallback(const std::shared_ptr<Node>& node);
        int id;
        zmq::context_t zmq_context;
        std::shared_ptr<NodeStore> nodeStore;
        std::unique_ptr<Router> router;
        std::thread router_thread;
        std::thread proxy_thread;
        std::vector<std::thread> worker_threads;
        zmq::socket_t cli_task_sender;
    };
}
