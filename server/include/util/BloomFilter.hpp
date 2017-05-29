#ifndef _UTIL_BLOOMFILTER_HPP
#define _UTIL_BLOOMFILTER_HPP
#include <array>
#include <bitset>
#include <cmath>
#include <random>
#include <iostream>
namespace util {

/**
 * Implementation of Bloom-Filters.
 * Hasher type is expected to have the following contract:   
 *      size_t operator()(const KeyType& val, size_t seed) const;
 * This is essentially similar to std::hash<KeyType> only with
 * support of adding a seed, that's to support multiple different 
 * hash function required by Bloom-Filter.
 *
 * Type is copyable.
 * Type is easily persistable - Just do memcpy
 */ 
template <typename KeyType, typename Hasher, size_t m, uint8_t k>
class BloomFilter {
private:
    using type = BloomFilter<KeyType, Hasher, m, k>;
public:
    // How much storage this Bloom-Filter takes.
    // This is especially useful when need to memcpy.
    static constexpr size_t storageSize = sizeof(type);

    BloomFilter(): bits(0), seeds{0} {
        std::mt19937_64 randomEngine(1234567);
        for (auto i = 0U; i < k ; i++) {
            seeds[i] = static_cast<size_t>(randomEngine());
        }            
    }

    void add(const KeyType& val) {
        for (auto i = 0u; i < k; i++) {
            bits.set(hash(seeds[i], val) % m);
        }
    }
   
    bool test(const KeyType& val) const {
        for (auto i = 0u; i < k; i++) {
            if (!bits.test(hash(seeds[i], val) % m)) {
                return false;
            }
        }
        return true;
    }
    
    size_t amortizedSize() const {
        auto ratio = static_cast<double>(bits.count()) / static_cast<double>(m);
        auto t = 1.0 - ratio;
        return static_cast<size_t>(std::fabs(std::ceil(-1.0 * std::log(t) * m / k)));
    }
private:
   std::bitset<m> bits;
   std::array<size_t, k> seeds;
   Hasher hash;
};

}
#endif
