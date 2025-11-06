#pragma once
#include <array>

namespace RpdefConfig {
    class RpdefSymbolHasher {
        static constexpr std::array<char, 8> HASH_LOOKUP = {
            // індекс: 0,  1(i), 2(b), 3, 4(l), 5, 6(n), 7(w)
            -1, 0, 1, -1, 3, -1, 4, 2
        };

    public:
        constexpr int operator()(const char c) const {
            const int index = static_cast<unsigned char>(c) & 7;
            return HASH_LOOKUP[index];
        }
    };
}
