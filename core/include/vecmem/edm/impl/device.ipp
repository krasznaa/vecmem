/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/device_traits.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/memory/device_atomic_ref.hpp"

// System include(s).
#include <cassert>

namespace vecmem {
namespace edm {

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE device<schema<VARTYPES...>>::device(
    const view<schema_type>& view)
    : m_capacity(view.capacity()),
      m_size(view.size_ptr()),
      m_data{details::make_device_tuple<VARTYPES...>(
          view.variables(), std::index_sequence_for<VARTYPES...>())} {

    // The container cannot be resizable if there are jagged vectors in it.
    assert(!((m_size != nullptr) &&
             vecmem::details::disjunction_v<
                 type::details::is_jagged_vector<VARTYPES>...>));
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename device<schema<VARTYPES...>>::size_type
device<schema<VARTYPES...>>::size() const {

    return (m_size == nullptr ? m_capacity : *m_size);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename device<schema<VARTYPES...>>::size_type
device<schema<VARTYPES...>>::capacity() const {

    return m_capacity;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename device<schema<VARTYPES...>>::size_type
device<schema<VARTYPES...>>::push_back_default() {

    // This can only be done on a resizable container.
    assert(m_size != nullptr);
    // There must be no jagged vector variables for this to work.
    static_assert(!vecmem::details::disjunction_v<
                      type::details::is_jagged_vector<VARTYPES>...>,
                  "Containers with jagged vector variables cannot be resized!");
    // There must be at least one vector variable in the container.
    static_assert(
        vecmem::details::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "This function requires at least one vector variable.");

    // Increment the size of the container at first. So that we would "claim"
    // the index from other threads.
    device_atomic_ref<size_type> asize(*m_size);
    const size_type index = asize.fetch_add(1);
    assert(index < m_capacity);

    // Construct the new elements in all of the vector variables.
    construct_default(index, std::index_sequence_for<VARTYPES...>{});

    // Return the position of the new variable(s).
    return index;
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE typename std::tuple_element<
    INDEX, std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
device<schema<VARTYPES...>>::get() {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE const typename std::tuple_element<
    INDEX, std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
device<schema<VARTYPES...>>::get() const {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename device<schema<VARTYPES...>>::device_tuple_type&
device<schema<VARTYPES...>>::variables() {

    return m_data;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE const typename device<
    schema<VARTYPES...>>::device_tuple_type&
device<schema<VARTYPES...>>::variables() const {

    return m_data;
}

template <typename... VARTYPES>
template <std::size_t INDEX, std::size_t... Is>
VECMEM_HOST_AND_DEVICE void device<schema<VARTYPES...>>::construct_default(
    size_type index, std::index_sequence<INDEX, Is...>) {

    // Construct the new element in this variable, if it's a vector.
    construct_vector(index, std::get<INDEX>(m_data));
    // Continue the recursion.
    construct_default<Is...>(index, std::index_sequence<Is...>{});
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE void device<schema<VARTYPES...>>::construct_default(
    size_type index, std::index_sequence<INDEX>) {

    // Construct the new element in this variable, if it's a vector.
    construct_vector(index, std::get<INDEX>(m_data));
}

template <typename... VARTYPES>
template <typename T>
VECMEM_HOST_AND_DEVICE void device<schema<VARTYPES...>>::construct_vector(
    size_type, T&) {}

template <typename... VARTYPES>
template <typename T>
VECMEM_HOST_AND_DEVICE void device<schema<VARTYPES...>>::construct_vector(
    size_type index, device_vector<T>& vec) {

    vec.construct(index, {});
}

}  // namespace edm
}  // namespace vecmem
