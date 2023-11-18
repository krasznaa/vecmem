/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/jagged_vector_view.hpp"
#include "vecmem/containers/data/vector_view.hpp"
#include "vecmem/edm/schema.hpp"

// System include(s).
#include <type_traits>

namespace vecmem {
namespace edm {
namespace details {

/// @name Traits for the view types for the individual variables
/// @{

template <typename TYPE>
struct view_type_base {
    using payload_type = TYPE;
    using payload_ptr = typename std::add_pointer<payload_type>::type;
    using size_type = unsigned int;
    using size_ptr = typename std::add_pointer<size_type>::type;
};  // struct view_type_base

template <typename TYPE>
struct view_type : public view_type_base<TYPE> {
    struct UNKNOWN_TYPE {};
    using layout_type = UNKNOWN_TYPE;
    using layout_ptr = UNKNOWN_TYPE;
    using type = UNKNOWN_TYPE;
};  // struct view_type

template <typename TYPE>
struct view_type<type::scalar<TYPE> > : public view_type_base<TYPE> {
    using layout_type = int;
    using layout_ptr = typename std::add_pointer<layout_type>::type;
    using type = typename view_type_base<TYPE>::payload_ptr;
};  // struct view_type

template <typename TYPE>
struct view_type<type::vector<TYPE> > : public view_type_base<TYPE> {
    using layout_type = int;
    using layout_ptr = typename std::add_pointer<layout_type>::type;
    using type = vecmem::data::vector_view<TYPE>;
};  // struct view_type

template <typename TYPE>
struct view_type<type::jagged_vector<TYPE> > : public view_type_base<TYPE> {
    using layout_type = vecmem::data::vector_view<TYPE>;
    using layout_ptr = typename std::add_pointer<layout_type>::type;
    using type = vecmem::data::jagged_vector_view<TYPE>;
};  // struct view_type

/// @}

}  // namespace details
}  // namespace edm
}  // namespace vecmem
