/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/buffer_type.hpp"
#include "vecmem/edm/details/traits.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <tuple>
#include <type_traits>
#include <vector>

namespace vecmem::edm {

/// Base class for SoA buffers
///
/// @tparam ...VARTYPES The variable types to store in the buffer
///
template <typename... VARTYPES>
class buffer {

    // Make sure that all variable types are supported.
    static_assert(
        std::conjunction<
            std::is_trivially_destructible<typename VARTYPES::type>...>::value,
        "Unsupported variable type");

public:
    /// Constructor for a 1D buffer
    ///
    /// @param capacity The capacity of the 1D arrays in the buffer
    /// @param mr       The memory resource to use for the allocation
    /// @param type     The type of the buffer (fixed or variable size)
    ///
    buffer(std::size_t capacity, memory_resource& mr,
           data::buffer_type type = data::buffer_type::fixed_size);
    /// Constructor for a 2D buffer
    ///
    /// @param capacities The capacities of the 1D/2D arrays in the buffer
    /// @param mr         The (main) memory resource to use for the allocation
    /// @param host_mr    The memory resource to use for the host allocation(s)
    /// @param type       The type of the buffer (fixed or variable size)
    ///
    buffer(const std::vector<std::size_t>& capacities, memory_resource& mr,
           memory_resource* host_mr = nullptr,
           data::buffer_type type = data::buffer_type::fixed_size);

private:
    /// The full allocated block of memory
    unique_alloc_ptr<char[]> m_memory;
    /// (Resizable) Size of the container described by this buffer
    unsigned int* m_size = nullptr;
    /// Views for the individual variables, pointing into the allocated memory
    std::tuple<typename details::view_type<VARTYPES>::type...> m_views;

};  // class buffer

}  // namespace vecmem::edm

// Include the implementation.
#include "vecmem/edm/impl/buffer.ipp"
