#include "gtest/gtest.h"
#include "util/BloomFilter.hpp"
#include <cstring>
#include <type_traits>
#include <functional>
#include <iostream>

template <typename KeyType>
struct Hasher {
};

template <>
struct Hasher<uint64_t> {
    size_t operator()(const uint64_t& val, size_t seed) const {
        return std::hash<uint64_t>{}(val) ^ seed;
    }
};

TEST(BloomFilter, test1) {
    util::BloomFilter<uint64_t, Hasher<uint64_t>, 16, 2> bloom;
    bloom.add(1U);
    EXPECT_TRUE(bloom.test(1U));
    EXPECT_FALSE(bloom.test(2U));
    EXPECT_FALSE(bloom.test(3U));
    EXPECT_FALSE(bloom.test(4U));
    EXPECT_FALSE(bloom.test(5U));
    EXPECT_FALSE(bloom.test(6U));
    EXPECT_FALSE(bloom.test(7U));
    EXPECT_FALSE(bloom.test(8U));
}

TEST(BloomFilter, serialize_deserialize) {
    using BF = util::BloomFilter<uint64_t, Hasher<uint64_t>, 4096, 10>;
    BF bloom;
    {
        std::mt19937_64 randomEngine(123);
        for (auto i = 1u; i <= 120u; i++) {
            bloom.add(randomEngine());
        }
    }
    EXPECT_TRUE(bloom.amortizedSize() * 0.95 < 120.0);
    EXPECT_TRUE(120.0 < bloom.amortizedSize() * 1.05);
    BF bloom2;
    std::memcpy(&bloom2, &bloom, BF::storageSize);
    {
        std::mt19937_64 randomEngine(123);
        for (auto i = 1u; i <= 120u; i++) {
            EXPECT_TRUE(bloom.test(randomEngine()));
        }
        EXPECT_FALSE(bloom.test(randomEngine()));
    }

    uint8_t buffer[BF::storageSize];
    std::memcpy(buffer, &bloom, BF::storageSize);
    BF* ptr = static_cast<BF*>(static_cast<void*>(buffer));
    {
        std::mt19937_64 randomEngine(123);
        for (auto i = 1u; i <= 120u; i++) {
            EXPECT_TRUE(ptr->test(randomEngine()));
        }
        
        for (auto i = 1u; i <= 120u; i++) {
           ptr->add(randomEngine());
        }
    }

    {
        std::mt19937_64 randomEngine(123);
        for (auto i = 1u; i <= 240u; i++) {
            EXPECT_TRUE(ptr->test(randomEngine()));
        }
    }

    std::memcpy(&bloom, buffer, BF::storageSize);
    {
        std::mt19937_64 randomEngine(123);
        for (auto i = 1u; i <= 240u; i++) {
            EXPECT_TRUE(bloom.test(randomEngine()));
        }
        EXPECT_FALSE(bloom.test(randomEngine()));
    }

}
