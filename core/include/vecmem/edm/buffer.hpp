/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/buffer_type.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/edm/view.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <tuple>
#include <type_traits>
#include <vector>

namespace vecmem::edm {

/// Generic buffer template
template <typename DUMMY>
class buffer {
    /// Delete the constructor of this type
    buffer() = delete;
};

/// Base class for SoA buffers
///
/// @tparam ...VARTYPES The variable types to store in the buffer
///
template <typename... VARTYPES>
class buffer<schema<VARTYPES...>> : public view<schema<VARTYPES...>> {

    // Make sure that all variable types are supported.
    static_assert(
        std::conjunction<
            std::is_trivially_destructible<typename VARTYPES::type>...>::value,
        "Unsupported variable type");

public:
    /// The schema describing the buffer's payload
    using schema_type = schema<VARTYPES...>;
    /// Base view type
    using view_type = view<schema_type>;
    /// Size type used for the container
    using size_type = typename view_type::size_type;

    /// Constructor for a 1D buffer
    ///
    /// @param capacity The capacity of the 1D arrays in the buffer
    /// @param mr       The memory resource to use for the allocation
    /// @param type     The type of the buffer (fixed or variable size)
    ///
    buffer(size_type capacity, memory_resource& mr,
           data::buffer_type type = data::buffer_type::fixed_size);
    /// Constructor for a 2D buffer
    ///
    /// @param capacities The capacities of the 1D/2D arrays in the buffer
    /// @param mr         The (main) memory resource to use for the allocation
    /// @param host_mr    The memory resource to use for the host allocation(s)
    /// @param type       The type of the buffer (fixed or variable size)
    ///
    buffer(const std::vector<size_type>& capacities, memory_resource& mr,
           memory_resource* host_mr = nullptr,
           data::buffer_type type = data::buffer_type::fixed_size);

private:
    /// The full allocated block of memory
    unique_alloc_ptr<char[]> m_memory;

};  // class buffer

}  // namespace vecmem::edm

// Include the implementation.
#include "vecmem/edm/impl/buffer.ipp"
