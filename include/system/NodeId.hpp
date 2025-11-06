#pragma once
#include <functional>
#include <string>

namespace NodeSystem {
    struct NodeId {
        int level;
        int index;

        bool operator==(const NodeId &other) const;

        std::string to_string() const;
    };

}
template<>
    struct std::hash<NodeSystem::NodeId> {
    size_t operator()(const NodeSystem::NodeId &id) const noexcept {
        const size_t h1 = std::hash<int>{}(id.level);
        const size_t h2 = std::hash<int>{}(id.index);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};