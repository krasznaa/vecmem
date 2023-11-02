/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/device_traits.hpp"

namespace vecmem::edm {

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE device<schema<VARTYPES...>>::device(
    const view<schema_type>& view)
    : m_capacity(view.capacity()),
      m_size(view.size_ptr()),
      m_data{details::make_device_tuple<VARTYPES...>(
          view.variables(), std::index_sequence_for<VARTYPES...>())} {}

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
template <std::size_t INDEX>
typename std::tuple_element<
    INDEX, std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
device<schema<VARTYPES...>>::get() {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
const typename std::tuple_element<
    INDEX, std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
device<schema<VARTYPES...>>::get() const {

    return std::get<INDEX>(m_data);
}

template <typename... VARTYPES>
typename device<schema<VARTYPES...>>::device_tuple_type&
device<schema<VARTYPES...>>::variables() {

    return m_data;
}

template <typename... VARTYPES>
const typename device<schema<VARTYPES...>>::device_tuple_type&
device<schema<VARTYPES...>>::variables() const {

    return m_data;
}

}  // namespace vecmem::edm
