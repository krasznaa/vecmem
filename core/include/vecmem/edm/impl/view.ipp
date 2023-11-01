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
VECMEM_HOST_AND_DEVICE view<schema<VARTYPES...>>::view(
    const view<schema<OTHERTYPES...>>& other)
    : m_capacity(other.capacity()),
      m_size(other.size_ptr()),
      m_views(other.variables()) {}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<schema<VARTYPES...>>::size() const
    -> size_type {

    return (m_size == nullptr ? m_capacity : *m_size);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<schema<VARTYPES...>>::capacity() const
    -> size_type {

    return m_capacity;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<schema<VARTYPES...>>::size_ptr()
    -> size_pointer {

    return m_size;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE auto view<schema<VARTYPES...>>::size_ptr() const
    -> const_size_pointer {

    return m_size;
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE typename std::tuple_element<
    INDEX, std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
view<schema<VARTYPES...>>::get() {

    return std::get<INDEX>(m_views);
}

template <typename... VARTYPES>
template <std::size_t INDEX>
VECMEM_HOST_AND_DEVICE const typename std::tuple_element<
    INDEX, std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
view<schema<VARTYPES...>>::get() const {

    return std::get<INDEX>(m_views);
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE typename view<schema<VARTYPES...>>::view_tuple_type&
view<schema<VARTYPES...>>::variables() {

    return m_views;
}

template <typename... VARTYPES>
VECMEM_HOST_AND_DEVICE const typename view<
    schema<VARTYPES...>>::view_tuple_type&
view<schema<VARTYPES...>>::variables() const {

    return m_views;
}

}  // namespace vecmem::edm
