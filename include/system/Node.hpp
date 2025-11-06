#pragma once
#include <mutex>

#include "NodeId.hpp"
#include "DataSlot.hpp"
#include "Message.hpp"

namespace NodeSystem {
    class Node {
    public:
        virtual ~Node() = default;

        Node() = default;

        explicit Node(const NodeId &id);

        NodeId get_id() const;

        double get_cf() const;

        using SignalOut = std::function<void(const MessageSend &)>;
        SignalOut OnRecalculated = [](const MessageSend &) {
        };

        virtual void process_input(const MessageReceive &message);

        double recalculate();

    protected:
        std::mutex mutex;
        std::unordered_map<NodeId, DataSlot> slots;
        NodeId id{0, 0};
        double cf = 0;
        double age_limit_ms = 10000.0;
    };
}