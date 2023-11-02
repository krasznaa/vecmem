/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/device_vector.hpp"
#include "vecmem/containers/jagged_device_vector.hpp"
#include "vecmem/edm/details/view_traits.hpp"
#include "vecmem/edm/schema.hpp"

// System include(s).
#include <tuple>
#include <utility>

namespace vecmem::edm::details {

/// @name Traits for the device types for the individual variables
/// @{

template <typename TYPE>
struct device_type_base {
    using raw_type = TYPE;
};  // struct device_type_base

template <typename TYPE>
struct device_type : public device_type_base<TYPE> {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};  // struct device_type

template <typename TYPE>
struct device_type<type::scalar<TYPE> > : public device_type_base<TYPE> {
    using type = TYPE*;
};  // struct device_type

template <typename TYPE>
struct device_type<type::vector<TYPE> > : public device_type_base<TYPE> {
    using type = device_vector<TYPE>;
};  // struct device_type

template <typename TYPE>
struct device_type<type::jagged_vector<TYPE> > : public device_type_base<TYPE> {
    using type = jagged_device_vector<TYPE>;
};  // struct device_type

/// @}

/// Helper function making the internal tuple of a device object
template <typename... VARTYPES, std::size_t... Is>
std::tuple<typename details::device_type<VARTYPES>::type...> make_device_tuple(
    const typename std::tuple<typename details::view_type<VARTYPES>::type...>&
        views,
    std::index_sequence<Is...>) {

    return {{std::get<Is>(views)}...};
}

}  // namespace vecmem::edm::details
