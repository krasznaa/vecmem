/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/host_traits.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"

// System include(s).
#include <type_traits>

namespace vecmem {
namespace edm {

template <typename... VARTYPES>
host<schema<VARTYPES...>>::host(memory_resource& mr)
    : m_data{details::host_alloc<VARTYPES>::make(mr)...} {}

template <typename... VARTYPES>
std::size_t host<schema<VARTYPES...>>::size() const {

    // Make sure that there are some (jagged) vector types in the container.
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one (jagged) vector variable.");

    // Get the size of the vector(s).
    return details::get_host_size<VARTYPES...>(
        m_data, std::index_sequence_for<VARTYPES...>{});
}

template <typename... VARTYPES>
void host<schema<VARTYPES...>>::resize(std::size_t size) {

    // Make sure that there are some (jagged) vector types in the container.
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one (jagged) vector variable.");

    // Resize the vector(s).
    details::host_resize<VARTYPES...>(m_data, size,
                                      std::index_sequence_for<VARTYPES...>{});
}

template <typename... VARTYPES>
template <std::size_t INDEX>
typename std::tuple_element<
    INDEX, std::tuple<typename details::host_type<VARTYPES>::type...>>::type&
host<schema<VARTYPES...>>::get() {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
const typename std::tuple_element<
    INDEX, std::tuple<typename details::host_type<VARTYPES>::type...>>::type&
host<schema<VARTYPES...>>::get() const {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
typename host<schema<VARTYPES...>>::host_tuple_type&
host<schema<VARTYPES...>>::variables() {

    return m_data;
}

template <typename... VARTYPES>
const typename host<schema<VARTYPES...>>::host_tuple_type&
host<schema<VARTYPES...>>::variables() const {

    return m_data;
}

}  // namespace edm

/// (Non-const) Overload of @c vecmem::get_data for scalar types
///
/// @tparam TYPE The scalar type to get the data for
/// @param obj The (non-const) scalar object
/// @return A simple (non-const) pointer to the scalar object
///
template <typename TYPE>
typename edm::details::view_type<edm::type::scalar<TYPE>>::type get_data(
    unique_obj_ptr<TYPE>& obj) {

    return obj.get();
}

/// Helper function terminal node
template <typename... VARTYPES>
void get_data_impl(edm::host<edm::schema<VARTYPES...>>&,
                   edm::view<edm::schema<VARTYPES...>>&,
                   std::index_sequence<>) {}

/// Helper function recursive node
template <typename... VARTYPES, std::size_t I, std::size_t... Is>
void get_data_impl(edm::host<edm::schema<VARTYPES...>>& host,
                   edm::view<edm::schema<VARTYPES...>>& view,
                   std::index_sequence<I, Is...>) {

    view.template get<I>() = get_data(host.template get<I>());
    get_data_impl(host, view, std::index_sequence<Is...>{});
}

template <typename... VARTYPES>
edm::view<edm::schema<VARTYPES...>> get_data(
    edm::host<edm::schema<VARTYPES...>>& host) {

    // Create the result object.
    edm::view<edm::schema<VARTYPES...>> result;
    // Set its size, if that's available.
    if constexpr (std::disjunction_v<
                      edm::type::details::is_vector<VARTYPES>...>) {
        result = {static_cast<
            typename edm::view<edm::schema<VARTYPES...>>::size_type>(
            host.size())};
    }
    // Fill it with the helper function.
    get_data_impl<VARTYPES...>(host, result,
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
typename edm::details::view_type<
    edm::type::scalar<typename std::add_const<TYPE>::type>>::type
get_data(const unique_obj_ptr<TYPE>& obj) {

    return obj.get();
}

/// Helper function terminal node
template <typename... VARTYPES>
void get_data_impl(
    const edm::host<edm::schema<VARTYPES...>>&,
    edm::view<edm::schema<
        typename edm::type::details::add_const<VARTYPES>::type...>>&,
    std::index_sequence<>) {}

/// Helper function recursive node
template <typename... VARTYPES, std::size_t I, std::size_t... Is>
void get_data_impl(
    const edm::host<edm::schema<VARTYPES...>>& host,
    edm::view<
        edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>&
        view,
    std::index_sequence<I, Is...>) {

    view.template get<I>() = vecmem::get_data(host.template get<I>());
    get_data_impl(host, view, std::index_sequence<Is...>{});
}

template <typename... VARTYPES>
edm::view<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::host<edm::schema<VARTYPES...>>& host) {

    // Create the result object.
    edm::view<
        edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
        result;
    // Set its size, if that's available.
    if constexpr (std::disjunction_v<
                      edm::type::details::is_vector<VARTYPES>...>) {
        result = {static_cast<
            typename edm::view<edm::schema<VARTYPES...>>::size_type>(
            host.size())};
    }
    // Fill it with the helper function.
    get_data_impl<VARTYPES...>(host, result,
                               std::index_sequence_for<VARTYPES...>{});
    // Return the filled object.
    return result;
}

}  // namespace vecmem
