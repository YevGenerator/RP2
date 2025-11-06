#pragma once
#include <vector>

#include "../system/NodeId.hpp"
#include "../system/SystemTemplate.hpp"

namespace RpdefConfig {
    struct SystemConfig {
        int id;
        int workerCount = 1;
        std::vector<int> levelsToCreate;
        std::vector<NodeSystem::NodeId> nodesToCreate;
        std::vector<std::pair<NodeSystem::NodeId, NodeSystem::NodeId> > bindingsToCreate;
    };
}
