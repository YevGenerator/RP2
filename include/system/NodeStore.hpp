#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "Node.hpp"

namespace NodeSystem {
    class NodeStore {
    public:
        NodeStore();

        std::shared_ptr<Node> getNode(NodeId id);

        void addNode(const std::shared_ptr<Node> &node_ptr);

        std::shared_ptr<Node> createAndBindNode(NodeId id);

        void removeNode(const std::shared_ptr<Node> &node_ptr);

        void bind(NodeId from, NodeId to);

        void bind(const std::shared_ptr<Node> &from, const std::shared_ptr<Node> &to);

        void unbind(NodeId from, NodeId to);

        std::shared_ptr<Node> createLevel(int level);

        std::shared_ptr<const std::vector<NodeId>> getRoutes(NodeId from);
        std::shared_ptr<Node> finalNode;
    private:
        std::mutex nodes_mutex;
        std::mutex routes_mutex;
        std::unordered_map<NodeId, std::shared_ptr<Node> > nodes;
        std::unordered_map<NodeId, std::shared_ptr<std::vector<NodeId> > > routes;

    };
}