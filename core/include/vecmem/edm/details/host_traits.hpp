/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/jagged_vector.hpp"
#include "vecmem/containers/vector.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <stdexcept>
#include <tuple>

namespace vecmem::edm::details {

/// @name Traits for the host types for the individual variables
/// @{

template <typename TYPE>
struct host_type_base {
    using raw_type = TYPE;
};  // struct host_type_base

template <typename TYPE>
struct host_type : public host_type_base<TYPE> {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};  // struct host_type

template <typename TYPE>
struct host_type<type::scalar<TYPE> > : public host_type_base<TYPE> {
    using type = unique_obj_ptr<TYPE>;
};  // struct host_type

template <typename TYPE>
struct host_type<type::vector<TYPE> > : public host_type_base<TYPE> {
    using type = vector<TYPE>;
};  // struct host_type

template <typename TYPE>
struct host_type<type::jagged_vector<TYPE> > : public host_type_base<TYPE> {
    using type = jagged_vector<TYPE>;
};  // struct host_type

/// @}

/// @name Traits for allocating host variables
/// @{

template <typename TYPE>
struct host_alloc {};

template <typename TYPE>
struct host_alloc<type::scalar<TYPE> > {
    static typename host_type<type::scalar<TYPE> >::type make(
        memory_resource& mr) {
        return make_unique_obj<TYPE>(mr);
    }
};  // struct host_alloc

template <typename TYPE>
struct host_alloc<type::vector<TYPE> > {
    static typename host_type<type::vector<TYPE> >::type make(
        memory_resource& mr) {
        return typename host_type<type::vector<TYPE> >::type{&mr};
    }
};  // struct host_alloc

template <typename TYPE>
struct host_alloc<type::jagged_vector<TYPE> > {
    static typename host_type<type::jagged_vector<TYPE> >::type make(
        memory_resource& mr) {
        return typename host_type<type::jagged_vector<TYPE> >::type{&mr};
    }
};  // struct host_alloc

/// @}

/// Recursive function getting the size of a host vector
///
/// Note that before calling this function, there is a check that at least one
/// of the variables is a (jagged) vector type. So the index sequence must
/// always contain at least a single element when this function is first called.
///
template <typename... VARTYPES, std::size_t INDEX, std::size_t... Is>
std::size_t get_host_size(
    const std::tuple<typename host_type<VARTYPES>::type...>& data,
    std::index_sequence<INDEX, Is...>, std::size_t size = 0,
    bool size_known = false) {

    // Get the size of this variable.
    std::size_t var_size = 0;
    bool var_size_known = false;
    if constexpr (type::details::is_vector<typename std::tuple_element<
                      INDEX, std::tuple<VARTYPES...> >::type>::value) {
        var_size = std::get<INDEX>(data).size();
        var_size_known = true;
    } else {
        var_size = size;
    }
    // Make sure that it's the same as what has been found before.
    if (size_known && var_size_known && (var_size != size)) {
        throw std::runtime_error(
            "Inconsistent variable sizes in host container!");
    }
    // Terminate, or continue.
    if constexpr (sizeof...(Is) == 0) {
        if (!(size_known || var_size_known)) {
            throw std::runtime_error(
                "Could not determine the size of the host container?!?");
        }
        return var_size;
    } else {
        return get_host_size<VARTYPES...>(data, std::index_sequence<Is...>{},
                                          var_size,
                                          size_known || var_size_known);
    }
}

/// Recursive function resizing a host vector
///
/// Note that before calling this function, there is a check that at least one
/// of the variables is a (jagged) vector type. So the index sequence must
/// always contain at least a single element when this function is first called.
///
template <typename... VARTYPES, std::size_t INDEX, std::size_t... Is>
void host_resize(std::tuple<typename host_type<VARTYPES>::type...>& data,
                 std::size_t size, std::index_sequence<INDEX, Is...>) {

    // Resize this variable.
    if constexpr (type::details::is_vector<typename std::tuple_element<
                      INDEX, std::tuple<VARTYPES...> >::type>::value) {
        std::get<INDEX>(data).resize(size);
    }
    // Terminate, or continue.
    if constexpr (sizeof...(Is) > 0) {
        host_resize<VARTYPES...>(data, size, std::index_sequence<Is...>{});
    }
}

/// Recursive function reserving memory for/in a host vector
///
/// Note that before calling this function, there is a check that at least one
/// of the variables is a (jagged) vector type. So the index sequence must
/// always contain at least a single element when this function is first called.
///
template <typename... VARTYPES, std::size_t INDEX, std::size_t... Is>
void host_reserve(std::tuple<typename host_type<VARTYPES>::type...>& data,
                  std::size_t size, std::index_sequence<INDEX, Is...>) {

    // Resize this variable.
    if constexpr (type::details::is_vector<typename std::tuple_element<
                      INDEX, std::tuple<VARTYPES...> >::type>::value) {
        std::get<INDEX>(data).reserve(size);
    }
    // Terminate, or continue.
    if constexpr (sizeof...(Is) > 0) {
        host_reserve<VARTYPES...>(data, size, std::index_sequence<Is...>{});
    }
}

}  // namespace vecmem::edm::details
