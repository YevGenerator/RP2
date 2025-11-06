#include "../../include//rpdefParser/ConfigParser.hpp"

#include <charconv>
#include <fstream>
#include <functional>

namespace RpdefConfig {
    ConfigParser::ConfigParser() {
    }

    SystemConfig ConfigParser::parse(const std::string &filename) {
        std::ifstream file(filename);
        this->storage.assign(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
        return this->parse();
    }

    SystemConfig ConfigParser::parseFromString(const std::string &str) {
        this->storage.assign(str);
        return this->parse();
    }

    SystemConfig ConfigParser::parse() {
        this->view = this->storage;
        this->position = 0;
        this->config = SystemConfig();
        while (this->position < this->view.size()) {
            const char cmd = nextCommand();
            this->position++;
            const auto data = readUntilNextCommand();
            (this->*hash_map[cmd])(data);
        }
        return this->config;
    }

    char ConfigParser::nextCommand() {
        while (this->position < this->view.size() && std::isspace(this->view[this->position])) {
            this->position++;
        }
        return this->view[this->position];
    }

    std::string_view ConfigParser::readUntilNextCommand() {
        const auto start = this->position;
        while (this->position < this->view.size()) {
            const auto c = static_cast<unsigned char>(this->view[this->position]);
            if (std::isalpha(c)) break;
            ++this->position;
        }
        return this->view.substr(start, this->position - start);
    }

    void ConfigParser::parse_i(const std::string_view data) {
        std::from_chars(data.data(), data.data() + data.size(), this->config.id);
    }

    void ConfigParser::parse_w(const std::string_view data) {
        std::from_chars(data.data(), data.data() + data.size(), this->config.workerCount);
    }

    void ConfigParser::parse_l(const std::string_view data) {
        this->parseAndAddLevels(data);
    }

    void ConfigParser::parse_n(std::string_view data) {
        this->parseAndAddNodes(data);
    }

    void ConfigParser::parse_b(std::string_view data) {
        this->parseAndAddBinds(data);
    }

    void ConfigParser::parseAndAddLevels(std::string_view singleToken) {
        for (auto [start, end] = parseRange(singleToken); start <= end; start++) {
            this->config.levelsToCreate.push_back(start);
        }
    }

    void ConfigParser::parseAndAddNodes(std::string_view singleToken) {
        auto vecs = parseNodes(singleToken);
        this->config.nodesToCreate.insert(config.nodesToCreate.end(),
                                          std::make_move_iterator(vecs.begin()), std::make_move_iterator(vecs.end()));
    }

    void ConfigParser::parseAndAddBinds(std::string_view singleToken) {
        const auto bindPivot = singleToken.find('>');
        const auto firstNodeToken = singleToken.substr(0, bindPivot);
        const auto secondNodeToken = singleToken.substr(bindPivot + 1);
        const auto firstNodes = parseNodes(firstNodeToken);
        const auto secondNodes = parseNodes(secondNodeToken);
        for (const auto &from: firstNodes)
            for (const auto &to: secondNodes) {
                this->config.bindingsToCreate.emplace_back(from, to);
            }
    }

    std::vector<NodeSystem::NodeId> ConfigParser::parseNodes(std::string_view singleToken) {
        std::vector<NodeSystem::NodeId> nodeIds;
        const auto nodePivot = singleToken.find('.');
        const auto levelSubToken = singleToken.substr(0, nodePivot);
        const auto idSubToken = singleToken.substr(nodePivot + 1);
        auto [levelStart, levelEnd] = parseRange(levelSubToken);
        auto [idStart, idEnd] = parseRange(idSubToken);
        for (int i = levelStart; i <= levelEnd; i++) {
            for (int j = idStart; j <= idEnd; j++) {
                nodeIds.push_back({i, j});
            }
        }
        return nodeIds;
    }


    std::pair<int, int> ConfigParser::parseRange(std::string_view rangeToken) {
        int start = 0, end = 0;
        const auto pivot = rangeToken.find('-');
        if (pivot == std::string_view::npos) {
            std::from_chars(rangeToken.data(), rangeToken.data() + rangeToken.size(), start);
            return {start, start};
        }
        std::from_chars(rangeToken.data(), rangeToken.data() + pivot, start);
        std::from_chars(rangeToken.data() + pivot + 1, rangeToken.data() + rangeToken.size(), end);
        return {start, end};
    }
}
