/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/data.hpp"
#include "vecmem/edm/details/host_traits.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/memory_resource.hpp"

// System include(s).
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace vecmem {
namespace edm {

template <typename DUMMY>
class host {
    /// Delete the constructor of this type
    host() = delete;
};

/// Base class for SoA host containers
///
/// @tparam ...VARTYPES The variable types to store in the container
///
template <typename... VARTYPES>
class host<schema<VARTYPES...>> {

public:
    /// The schema describing the host's payload
    using schema_type = schema<VARTYPES...>;
    /// The tuple type holding all of the the individual variable vectors
    typedef std::tuple<typename details::host_type<VARTYPES>::type...>
        host_tuple_type;

    /// Constructor with a mandatory memory resource
    host(memory_resource& mr);

    /// Get the size of the container
    std::size_t size() const;
    /// Resize the container
    void resize(std::size_t size);
    /// Reserve memory for the container
    void reserve(std::size_t size);

    /// Get the vector of a specific variable (non-const)
    template <std::size_t INDEX>
    typename std::tuple_element<
        INDEX,
        std::tuple<typename details::host_type<VARTYPES>::type...>>::type&
    get();
    /// Get the vector of a specific variable (const)
    template <std::size_t INDEX>
    const typename std::tuple_element<
        INDEX,
        std::tuple<typename details::host_type<VARTYPES>::type...>>::type&
    get() const;

    /// Direct (non-const) access to the underlying tuple of variables
    host_tuple_type& variables();
    /// Direct (const) access to the underlying tuple of variables
    const host_tuple_type& variables() const;

    /// The memory resource used by the host container
    memory_resource& resource() const;

private:
    /// The tuple holding the individual variable vectors
    host_tuple_type m_data;
    /// The memory resource used by the host container
    std::reference_wrapper<memory_resource> m_resource;

};  // class host

}  // namespace edm

/// Helper function for getting a (non-const) data object for a host container
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param host The host container to get a data object for
/// @param resource The memory resource to use for any allocation(s)
/// @return A (non-const) data object describing the host container
///
template <typename... VARTYPES>
edm::data<edm::schema<VARTYPES...>> get_data(
    edm::host<edm::schema<VARTYPES...>>& host,
    memory_resource* resource = nullptr);

/// Helper function for getting a (const) data object for a host container
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param host The host container to get a data object for
/// @param resource The memory resource to use for any allocation(s)
/// @return A (const) data object describing the host container
///
template <typename... VARTYPES>
edm::data<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::host<edm::schema<VARTYPES...>>& host,
         memory_resource* resource = nullptr);

}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/host.ipp"
