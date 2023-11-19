/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Make Doxygen ignore the contents of this file.
#ifndef VECMEM_DOXYGEN_IGNORE

// Local include(s).
#include "vecmem/edm/details/host_traits.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"

// System include(s).
#include <type_traits>

namespace vecmem {
namespace edm {

template <typename... VARTYPES>
VECMEM_HOST host<schema<VARTYPES...>>::host(memory_resource& resource)
    : m_data{details::host_alloc<VARTYPES>::make(resource)...},
      m_resource{resource} {}

template <typename... VARTYPES>
VECMEM_HOST std::size_t host<schema<VARTYPES...>>::size() const {

    // Make sure that there are some (jagged) vector types in the container.
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one (jagged) vector variable.");

    // Get the size of the vector(s).
    return details::get_host_size<VARTYPES...>(
        m_data, std::index_sequence_for<VARTYPES...>{});
}

template <typename... VARTYPES>
VECMEM_HOST void host<schema<VARTYPES...>>::resize(std::size_t size) {

    // Make sure that there are some (jagged) vector types in the container.
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one (jagged) vector variable.");

    // Resize the vector(s).
    details::host_resize<VARTYPES...>(m_data, size,
                                      std::index_sequence_for<VARTYPES...>{});
}

template <typename... VARTYPES>
VECMEM_HOST void host<schema<VARTYPES...>>::reserve(std::size_t size) {

    // Make sure that there are some (jagged) vector types in the container.
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one (jagged) vector variable.");

    // Resize the vector(s).
    details::host_reserve<VARTYPES...>(m_data, size,
                                       std::index_sequence_for<VARTYPES...>{});
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST auto host<schema<VARTYPES...>>::get() ->
    typename std::tuple_element<INDEX, tuple_type>::type& {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST auto host<schema<VARTYPES...>>::get() const -> const
    typename std::tuple_element<INDEX, tuple_type>::type& {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
VECMEM_HOST typename host<schema<VARTYPES...>>::tuple_type&
host<schema<VARTYPES...>>::variables() {

    return m_data;
}

template <typename... VARTYPES>
VECMEM_HOST const typename host<schema<VARTYPES...>>::tuple_type&
host<schema<VARTYPES...>>::variables() const {

    return m_data;
}

template <typename... VARTYPES>
VECMEM_HOST memory_resource& host<schema<VARTYPES...>>::resource() const {

    return m_resource;
}

}  // namespace edm

/// (Non-const) Overload of @c vecmem::get_data for scalar types
///
/// @tparam TYPE The scalar type to get the data for
/// @param obj The (non-const) scalar object
/// @return A simple (non-const) pointer to the scalar object
///
template <typename TYPE>
VECMEM_HOST typename edm::details::view_type<edm::type::scalar<TYPE>>::type
get_data(unique_obj_ptr<TYPE>& obj) {

    return obj.get();
}

/// Helper function terminal node
template <typename... VARTYPES>
VECMEM_HOST void get_data_impl(edm::host<edm::schema<VARTYPES...>>&,
                               edm::data<edm::schema<VARTYPES...>>&,
                               memory_resource&, std::index_sequence<>) {}

/// Helper function recursive node
template <typename... VARTYPES, std::size_t I, std::size_t... Is>
VECMEM_HOST void get_data_impl(edm::host<edm::schema<VARTYPES...>>& host,
                               edm::data<edm::schema<VARTYPES...>>& data,
                               memory_resource& mr,
                               std::index_sequence<I, Is...>) {

    if constexpr (edm::type::details::is_jagged_vector<
                      typename std::tuple_element<
                          I, std::tuple<VARTYPES...>>::type>::value) {
        std::get<I>(data.variables()) = get_data(host.template get<I>(), &mr);
        data.template get<I>() = get_data(std::get<I>(data.variables()));
    } else {
        data.template get<I>() = get_data(host.template get<I>());
    }
    get_data_impl(host, data, mr, std::index_sequence<Is...>{});
}

template <typename... VARTYPES>
VECMEM_HOST edm::data<edm::schema<VARTYPES...>> get_data(
    edm::host<edm::schema<VARTYPES...>>& host, memory_resource* resource) {

    // Create the result object.
    edm::data<edm::schema<VARTYPES...>> result;
    // Decide what memory resource to use for setting it up.
    memory_resource& mr = (resource != nullptr ? *resource : host.resource());
    // Set its size, if that's available.
    if constexpr (std::disjunction_v<
                      edm::type::details::is_vector<VARTYPES>...>) {
        result = {static_cast<
                      typename edm::data<edm::schema<VARTYPES...>>::size_type>(
                      host.size()),
                  mr};
    }
    // Fill it with the helper function.
    get_data_impl<VARTYPES...>(host, result, mr,
                               std::index_sequence_for<VARTYPES...>{});
    // Return the filled object.
    return result;
}

/// (Const) Overload of @c vecmem::get_data for scalar types
///
/// @tparam TYPE The scalar type to get the data for
/// @param obj The (const) scalar object
/// @return A simple (const) pointer to the scalar object
///
template <typename TYPE>
VECMEM_HOST typename edm::details::view_type<
    edm::type::scalar<typename std::add_const<TYPE>::type>>::type
get_data(const unique_obj_ptr<TYPE>& obj) {

    return obj.get();
}

/// Helper function terminal node
template <typename... VARTYPES>
VECMEM_HOST void get_data_impl(
    const edm::host<edm::schema<VARTYPES...>>&,
    edm::data<edm::schema<
        typename edm::type::details::add_const<VARTYPES>::type...>>&,
    memory_resource&, std::index_sequence<>) {}

/// Helper function recursive node
template <typename... VARTYPES, std::size_t I, std::size_t... Is>
VECMEM_HOST void get_data_impl(
    const edm::host<edm::schema<VARTYPES...>>& host,
    edm::data<
        edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>&
        data,
    memory_resource& mr, std::index_sequence<I, Is...>) {

    if constexpr (edm::type::details::is_jagged_vector<
                      typename std::tuple_element<
                          I, std::tuple<VARTYPES...>>::type>::value) {
        std::get<I>(data.variables()) = get_data(host.template get<I>(), &mr);
        data.template get<I>() = get_data(std::get<I>(data.variables()));
    } else {
        data.template get<I>() = get_data(host.template get<I>());
    }
    get_data_impl(host, data, mr, std::index_sequence<Is...>{});
}

template <typename... VARTYPES>
VECMEM_HOST edm::data<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::host<edm::schema<VARTYPES...>>& host,
         memory_resource* resource) {

    // Create the result object.
    edm::data<
        edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
        result;
    // Decide what memory resource to use for setting it up.
    memory_resource& mr = (resource != nullptr ? *resource : host.resource());
    // Set its size, if that's available.
    if constexpr (std::disjunction_v<
                      edm::type::details::is_vector<VARTYPES>...>) {
        result = {static_cast<
                      typename edm::view<edm::schema<VARTYPES...>>::size_type>(
                      host.size()),
                  mr};
    }
    // Fill it with the helper function.
    get_data_impl<VARTYPES...>(host, result, mr,
                               std::index_sequence_for<VARTYPES...>{});
    // Return the filled object.
    return result;
}

}  // namespace vecmem

#endif  // not VECMEM_DOXYGEN_IGNORE
