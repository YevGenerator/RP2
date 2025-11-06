#pragma once
#include <map>
#include <sstream>
#include <string>

#include "SystemConfig.hpp"

namespace RpdefConfig {
    class Deparser {
    public:
        static std::string reduceRange(const int start, const int end) {
            if (start == end) return std::to_string(start);
            return std::to_string(start) + "-" + std::to_string(end);
        }

        static std::string deparseConfig(const SystemConfig &config) {
            return
                    deparseId(config.id) +
                    deparseWorkers(config.workerCount) +
                    deparseLevels(config.levelsToCreate) +
                    deparseNodes(config.nodesToCreate) +
                    deparseBinds(config.bindingsToCreate);
        }

        static std::string deparseLevels(const std::vector<int> &values) {
            return compressValues(compressRange(values), 'l');
        }

        static std::string deparseId(const int id) {
            return "i" + std::to_string(id);
        }

        static std::string deparseWorkers(const int workers) {
            return "w" + std::to_string(workers);
        }

        static std::string deparseNodes(const std::vector<NodeSystem::NodeId> &nodes) {
            return compressNodes(nodes);
        }

        static std::string
        deparseBinds(const std::vector<std::pair<NodeSystem::NodeId, NodeSystem::NodeId> > &bindings) {
            std::ostringstream result;
            for (const auto [from, to]: bindings) {
                result << "b" << from.to_string() << ">" << to.to_string();
            }
            return result.str();
        }

        static std::vector<std::string> compressRange(const std::vector<int> &values) {
            if (values.empty()) return {};
            std::vector<std::string> result;
            std::vector<int> sorted = values;
            std::sort(sorted.begin(), sorted.end());

            int first = 0;
            for (int i = 1; i < sorted.size(); i++) {
                if (sorted[i] != sorted[i - 1] + 1) {
                    result.push_back(reduceRange(sorted[first], sorted[i - 1]));
                    first = i;
                }
            }
            result.push_back(reduceRange(sorted[first], sorted[sorted.size() - 1]));
            return result;
        }

        static std::string compressValues(const std::vector<std::string> &values, const char c) {
            std::string result;
            for (const auto &value: values) {
                result += c + value;
            }
            return result;
        }

        static std::string compressNodes(const std::vector<NodeSystem::NodeId> &nodes) {
            if (nodes.empty()) return "";

            std::map<int, std::vector<int> > byLevel;
            for (const auto &[level, index]: nodes)
                byLevel[level].push_back(index);

            std::map<std::string, std::vector<int> > patternToLevels;
            for (auto &[level, ids]: byLevel) {
                for (auto patterns = compressRange(ids);
                     auto &compressedId: patterns) {
                    patternToLevels[compressedId].push_back(level);
                }
            }
            std::ostringstream result;
            for (auto &[idPattern, levels]: patternToLevels) {
                for (const auto compressed = compressRange(levels);
                     const auto &level: compressed) {
                    result << "n" << level << "." << idPattern;
                }
            }
            return result.str();
        }
    };
}
