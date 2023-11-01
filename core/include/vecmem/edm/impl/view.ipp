/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

namespace vecmem::edm {

template <typename... VARTYPES>
template <typename... OTHERTYPES,
          std::enable_if_t<
              std::conjunction_v<std::is_constructible<
                  typename details::view_type<VARTYPES>::type,
                  typename details::view_type<OTHERTYPES>::type>...> &&
                  std::disjunction_v<std::negation<std::is_same<
                      typename details::view_type<VARTYPES>::type,
                      typename details::view_type<OTHERTYPES>::type>>...>,
              bool>>
VECMEM_HOST_AND_DEVICE view<VARTYPES...>::view(const view<OTHERTYPES...>& other)
    : m_capacity(other.capacity()),
      m_size(other.size_ptr()),
      m_views(other.views()) {}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<VARTYPES...>::size() const -> size_type {

    return (m_size == nullptr ? m_capacity : *m_size);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<VARTYPES...>::capacity() const -> size_type {

    return m_capacity;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<VARTYPES...>::size_ptr() -> size_pointer {

    return m_size;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<VARTYPES...>::size_ptr() const
    -> const_size_pointer {

    return m_size;
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE typename std::tuple_element<
    INDEX, std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
view<VARTYPES...>::get_view() {

    return std::get<INDEX>(m_views);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE const typename std::tuple_element<
    INDEX, std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
view<VARTYPES...>::get_view() const {

    return std::get<INDEX>(m_views);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename view<VARTYPES...>::view_tuple_type&
view<VARTYPES...>::views() {

    return m_views;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE const typename view<VARTYPES...>::view_tuple_type&
view<VARTYPES...>::views() const {

    return m_views;
}

}  // namespace vecmem::edm
