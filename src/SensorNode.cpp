#include "../include/SensorNode.hpp"
#include "../include/Message.hpp"
#include "../include/Printer.hpp"

SensorNode::SensorNode(const NodeId &id) : Node(id) {
    std::random_device rd;
    this->generator = std::mt19937{rd()};
    this->distribution = std::uniform_real_distribution(0.0, 1.0);
}

void SensorNode::process_input(const MessageReceive &message) {
    double new_cf = 0.0;
    {
        std::lock_guard lock(mutex);
        new_cf = this->distribution(this->generator);
        this->cf = new_cf;
    }
    Printer::print_safe("[Sensor " + this->id.to_string() + "]: випущено " + std::to_string(new_cf));
    OnRecalculated({this->id, new_cf});
}
