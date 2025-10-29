#include  "../include/NodeId.hpp"


bool NodeId::operator==(const NodeId &other) const {
    return level == other.level && index == other.index;
}

std::string NodeId::to_string() const  {
    return std::to_string(this->level) + "." + std::to_string(this->index);
}

