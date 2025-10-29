#include <ranges>
#include "../include/NodeStore.hpp"
#include "../include/Message.hpp"
#include "../include/Printer.hpp"

NodeStore::NodeStore() {
    this->finalNode = std::make_shared<Node>(NodeId{999, 0});
    this->finalNode->OnRecalculated = [](const MessageSend &message) {
        Printer::print_safe(std::to_string(message.data));
    };
    this->nodes[finalNode->get_id()] = finalNode;
}

std::shared_ptr<Node> NodeStore::getNode(const NodeId id) {
    std::lock_guard lock(nodes_mutex);
    const auto pointer = nodes.find(id);
    return pointer == nodes.end() ? nullptr : pointer->second;
}

void NodeStore::addNode(const std::shared_ptr<Node> &node_ptr) {
    const auto id = node_ptr->get_id();
    std::lock_guard lock(this->nodes_mutex);
    this->nodes[id] = node_ptr;
}

std::shared_ptr<Node> NodeStore::createAndBindNode(NodeId id) {
    const auto node = std::make_shared<Node>(id);
    this->addNode(node);
    this->bind(id, {id.level, 0});
    return node;
}

void NodeStore::removeNode(const std::shared_ptr<Node> &node_ptr) {
    const auto id = node_ptr->get_id();
    std::lock_guard lock(this->nodes_mutex);
    this->nodes.erase(id);
    for (auto &vec: this->routes | std::views::values) {
        std::erase(*vec, id);
    }
}

void NodeStore::bind(const NodeId from, const NodeId to) {
    std::lock_guard lock(this->routes_mutex);

    auto it = this->routes.find(from);

    if (it == this->routes.end()) {
        this->routes[from] = std::make_shared<std::vector<NodeId> >(std::vector{to});
        return;
    }

    auto &current_vec_ptr = it->second;

    if (current_vec_ptr.use_count() == 1) {
        current_vec_ptr->push_back(to);
    } else {
        auto new_vec_ptr = std::make_shared<std::vector<NodeId> >(*current_vec_ptr);
        new_vec_ptr->push_back(to);
        current_vec_ptr = std::move(new_vec_ptr);
    }
}

void NodeStore::bind(const std::shared_ptr<Node> &from, const std::shared_ptr<Node> &to) {
    this->bind(from->get_id(), to->get_id());
}

void NodeStore::unbind(const NodeId from, const NodeId to) {
    std::lock_guard lock(this->routes_mutex);
    std::erase(*this->routes[from], to);
}

void NodeStore::createLevel(const int level) {
    NodeId id{level, 0};
    const auto generalNode = std::make_shared<Node>(id);
    std::lock_guard lock(this->nodes_mutex);
    this->nodes[id] = generalNode;
    this->bind(generalNode, this->finalNode);
}

std::shared_ptr<const std::vector<NodeId>> NodeStore::getRoutes(const NodeId from) {
    std::lock_guard lock(this->routes_mutex);
    if (const auto it = routes.find(from); it != routes.end()) {
        return it->second;
    }

    return std::make_shared<const std::vector<NodeId>>();
}

