#pragma once
#include <string>

#include "HashMap.hpp"
#include "SystemConfig.hpp"

namespace RpdefConfig {
    class ConfigParser {
    public:
        ConfigParser();

        SystemConfig parse(const std::string &filename);
        SystemConfig parseFromString(const std::string& str);
        SystemConfig parse();
    private:
        using ParseFunc = void(ConfigParser::*)(std::string_view);
        std::string storage;
        std::string_view view;
        size_t position = 0;
        SystemConfig config;

        char nextCommand();

        std::string_view readUntilNextCommand();

        void parse_i(std::string_view data);

        void parse_w(std::string_view data);

        void parse_l(std::string_view data);

        void parse_n(std::string_view data);

        void parse_b(std::string_view data);


        constexpr static HashMap<ParseFunc> hash_map = {
            {'i', &ConfigParser::parse_i}, {'w', &ConfigParser::parse_w}, {'l', &ConfigParser::parse_l},
            {'n', &ConfigParser::parse_n}, {'b', &ConfigParser::parse_b}
        };

        static std::pair<int, int> parseRange(std::string_view rangeToken);

        void parseAndAddLevels(std::string_view singleToken);

        void parseAndAddNodes(std::string_view singleToken);

        void parseAndAddBinds(std::string_view singleToken);

        static std::vector<NodeSystem::NodeId> parseNodes(std::string_view str);
    };
}
