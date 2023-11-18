/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/jagged_vector_data.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/details/tuple.hpp"
#include "vecmem/edm/details/tuple_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/memory_resource.hpp"

// System include(s).
#include <tuple>
#include <variant>

namespace vecmem::edm::details {

template <typename TYPE>
struct data_type {
    using type = std::monostate;
};  // struct data_type

template <typename TYPE>
struct data_type<type::jagged_vector<TYPE>> {
    using type = vecmem::data::jagged_vector_data<TYPE>;
};  // struct data_type

template <typename TYPE>
struct data_alloc {
    static typename data_type<TYPE>::type make_data(std::size_t,
                                                    memory_resource&) {
        return {};
    }
    static void assign_data(typename view_type<TYPE>::type&,
                            const typename data_type<TYPE>::type&) {}
};  // struct data_alloc

template <typename TYPE>
struct data_alloc<type::jagged_vector<TYPE>> {
    static typename data_type<type::jagged_vector<TYPE>>::type make_data(
        std::size_t size, memory_resource& mr) {
        return {size, mr};
    }
    static void assign_data(
        typename view_type<type::jagged_vector<TYPE>>::type& view,
        const typename data_type<type::jagged_vector<TYPE>>::type& data) {
        view = data;
    }
};  // struct data_alloc

template <typename... VARTYPES, std::size_t INDEX, std::size_t... Is>
void data_view_assign(
    details::tuple<typename view_type<VARTYPES>::type...>& view,
    const std::tuple<typename data_type<VARTYPES>::type...>& data,
    std::index_sequence<INDEX, Is...>) {

    // Resize this variable.
    if constexpr (type::details::is_jagged_vector<typename std::tuple_element<
                      INDEX, std::tuple<VARTYPES...>>::type>::value) {
        details::get<INDEX>(view) = std::get<INDEX>(data);
    }
    // Terminate, or continue.
    if constexpr (sizeof...(Is) > 0) {
        data_view_assign<VARTYPES...>(view, data, std::index_sequence<Is...>{});
    }
}

}  // namespace vecmem::edm::details
