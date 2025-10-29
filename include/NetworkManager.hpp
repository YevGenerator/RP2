#pragma once
#include <thread>
#include <vector>
#include <memory>
#include <zmq_addon.hpp>

#include "NodeStore.hpp"
#include "NetworkConfig.hpp"

#include "Router.hpp"
class SensorNode;

class NetworkManager {
public:
    NetworkManager()
        : zmq_context(1),
          nodeStore(std::make_shared<NodeStore>()),
          cli_task_sender(zmq_context, zmq::socket_type::push) {

        cli_task_sender.connect(NetworkConfig::TaskQueue.data());
    }

    ~NetworkManager() {
        // TODO: Треба реалізувати коректну зупинку потоків
    }

    void run_async(int num_workers = 1);

    std::shared_ptr<Node> createNode(NodeId id);

    std::shared_ptr<SensorNode> createSensorNode(NodeId id);

    void bind(NodeId from, NodeId to) const;

    void triggerSensor(NodeId sensorId);

    void createLevel(int level);

private:
    void setupNodeCallback(const std::shared_ptr<Node>& node);

    zmq::context_t zmq_context;
    std::shared_ptr<NodeStore> nodeStore;
    std::unique_ptr<Router> router;
    std::thread router_thread;
    std::vector<std::thread> worker_threads;
    zmq::socket_t cli_task_sender;
};
