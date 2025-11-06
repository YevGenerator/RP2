#pragma once
#include "NodeId.hpp"

namespace NodeSystem {
    struct MessageSend {
        NodeId senderId;
        double data;
    };

    struct MessageReceive {
        NodeId senderId;
        NodeId targetId;
        double data;
    };
}
