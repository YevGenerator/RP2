#pragma once
#include <random>

#include "Node.hpp"

class SensorNode final : public Node {
public:
    SensorNode() : SensorNode(NodeId{}) {
    }

    explicit SensorNode(const NodeId &id);

    void process_input(const MessageReceive &message) override;

private:
    std::mt19937 generator;
    std::uniform_real_distribution<> distribution;
};
