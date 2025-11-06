#pragma once
#include <thread>
#include <vector>
#include <memory>
#include <zmq_addon.hpp>

#include "NetworkConfig.hpp"
#include "NodeStore.hpp"

#include "Router.hpp"

namespace NodeSystem {
    class SystemTemplate {
    public:
        SystemTemplate(int id, zmq::context_t* zmq_context);

        ~SystemTemplate() {
            // TODO: Треба реалізувати коректну зупинку потоків
        }

        int get_id() const;

        void run_async();

        std::shared_ptr<Node> createNode(NodeId id);

        void bind(NodeId from, NodeId to) const;

        void bindToSystem(const SystemTemplate &system, int targetNodeId) const;

        void triggerSensor(NodeId sensorId);

        void createLevel(int level);

        void setId(const int id) {
            this->id = id;
        }

        int getId() const {
            return this->id;
        }

        void setWorkers(const int workers) {
            this->workers = workers;
        }

        int getWorkers() const {
            return this->workers;
        }

        std::string queueTaskIn() const {
            return std::string(NetworkConfig::TaskQueueIn) + "_" + std::to_string(this->id);
        }

        std::string queueTaskOut() const {
            return std::string(NetworkConfig::TaskQueueOut) + "_" + std::to_string(this->id);
        }

        std::string queueOutput() const {
            return std::string(NetworkConfig::OutputQueue) + "_" + std::to_string(this->id);
        }

    private:
        void setupNodeCallback(const std::shared_ptr<Node> &node);

        int id;
        int workers;
        zmq::context_t* zmq_context;
        std::shared_ptr<NodeStore> nodeStore;
        std::unique_ptr<Router> router;
        std::thread router_thread;
        std::thread proxy_thread;
        std::vector<std::thread> worker_threads;
        zmq::socket_t cli_task_sender;
    };
}
