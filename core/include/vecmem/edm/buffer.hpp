/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/buffer_type.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/edm/view.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <type_traits>
#include <utility>
#include <vector>

namespace vecmem {
namespace edm {

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
    buffer(
        size_type capacity, memory_resource& mr,
        vecmem::data::buffer_type type = vecmem::data::buffer_type::fixed_size);
    /// Constructor for a 2D buffer
    ///
    /// @param capacities The capacities of the 1D/2D arrays in the buffer
    /// @param mr         The (main) memory resource to use for the allocation
    /// @param host_mr    The memory resource to use for the host allocation(s)
    /// @param type       The type of the buffer (fixed or variable size)
    ///
    template <typename SIZE_TYPE = std::size_t,
              std::enable_if_t<std::is_integral<SIZE_TYPE>::value &&
                                   std::is_unsigned<SIZE_TYPE>::value,
                               bool> = true>
    buffer(
        const std::vector<SIZE_TYPE>& capacities, memory_resource& mr,
        memory_resource* host_mr = nullptr,
        vecmem::data::buffer_type type = vecmem::data::buffer_type::fixed_size);

private:
    /// Set up a fixed sized buffer
    template <typename SIZE_TYPE = std::size_t, std::size_t... INDICES>
    void setup_fixed(const std::vector<SIZE_TYPE>& capacities,
                     memory_resource& mr, memory_resource* host_mr,
                     std::index_sequence<INDICES...>);
    /// Set up a resizable buffer
    template <typename SIZE_TYPE = std::size_t, std::size_t... INDICES>
    void setup_resizable(const std::vector<SIZE_TYPE>& capacities,
                         memory_resource& mr, memory_resource* host_mr,
                         std::index_sequence<INDICES...>);

    /// The full allocated block of (device) memory
    unique_alloc_ptr<char[]> m_memory;
    /// The full allocated block of host accessible memory
    unique_alloc_ptr<char[]> m_host_memory;

};  // class buffer

}  // namespace edm

/// Helper function for getting a (possibly non-const) view for a buffer
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param buffer The buffer to get a view for
/// @return A (possibly non-const) view into for the buffer
///
template <typename... VARTYPES>
edm::view<edm::schema<VARTYPES...>> get_data(
    edm::buffer<edm::schema<VARTYPES...>>& buffer);

/// Helper function for getting a (const) view for a buffer
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param buffer The buffer to get a view for
/// @return A (const) view into for the buffer
///
template <typename... VARTYPES>
edm::view<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::buffer<edm::schema<VARTYPES...>>& buffer);

}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/buffer.ipp"
