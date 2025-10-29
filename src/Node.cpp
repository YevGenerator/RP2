#include "../include/Node.hpp"

#include <ranges>

#include "../include/Message.hpp"
#include "../include/Printer.hpp"

Node::Node(const NodeId &id) {
    this->id = id;
    this->cf = -1.0;
}

NodeId Node::get_id() const { return this->id; }
double Node::get_cf() const { return this->cf; }

void Node::process_input(const MessageReceive &message) {
    double new_cf = 0.0;
    {
        std::lock_guard lock(this->mutex);
        this->slots[message.senderId] = {message.data, std::chrono::steady_clock::now()};
        new_cf = this->recalculate();
        Printer::print_safe("[Вузол " + this->id.to_string() + "]: отримав нові дані: " + std::to_string(new_cf));
        this->cf = new_cf;
    }
    OnRecalculated({this->id, new_cf});
}


double Node::recalculate() {
    double sum = 0.0;
    const auto time_now = std::chrono::steady_clock::now();
    if (slots.empty()) {
        return 0.0;
    }
    for (const auto &dataSlot: this->slots | std::views::values) {
        auto time_dif = time_now - dataSlot.time;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_dif).count();
        auto percentage = static_cast<double>(ms) / this->age_limit_ms;
        if (percentage >= 1) {
            percentage = 1;
        }
        sum += dataSlot.data * (1 - percentage);
    }
    return sum / static_cast<double>(this->slots.size());
}
