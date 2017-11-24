#ifndef _SERVER_GRAPHDB_PAGETABLE_HPP
#define _SERVER_GRAPHDB_PAGETABLE_HPP

#include <cstddef>
#include <cinttypes>

namespace graphdb { namespace pagetable {
 
/**
 * This class holds the mapping between a hash-range and a PageId.
 * When the approximated number of elements exceeds a threshold (Params::MAX_ELEMENTS_IN_PAGE)
 *
 **/
template <typename Key, typename Hasher>
class PageTable {
    enum class Params: std::size_t { MAX_ELEMENTS_IN_PAGE = 1000u };
    using hashtype = std::uint32_t;

    public:
        
    private:
        Hasher hasher;
        
};

}}
#endif
