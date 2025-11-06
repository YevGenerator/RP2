#pragma once
#include "RpdefSymbolHasher.hpp"

namespace RpdefConfig {
    template<typename ValueType>
    class HashMap {
    private:
        static constexpr RpdefSymbolHasher hasher{};
        static constexpr int map_size = 5;
        std::array<ValueType, map_size> data;

        static constexpr int get_unsafe(const char key) {
            return hasher(key);
        }

    public:
        constexpr HashMap() : data{} {
        }

        constexpr HashMap(std::pair<char, ValueType> p0,
                          std::pair<char, ValueType> p1,
                          std::pair<char, ValueType> p2,
                          std::pair<char, ValueType> p3,
                          std::pair<char, ValueType> p4) {
            (*this)[p0.first] = p0.second;
            (*this)[p1.first] = p1.second;
            (*this)[p2.first] = p2.second;
            (*this)[p3.first] = p3.second;
            (*this)[p4.first] = p4.second;
        }

        constexpr ValueType &operator[](const char key) {
            return data[get_unsafe(key)];
        }

        constexpr const ValueType &operator[](const char key) const {
            return data[get_unsafe(key)];
        }
    };
}
