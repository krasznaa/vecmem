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

namespace vecmem {
namespace edm {
namespace details {

/// @name Traits for the view types for the individual variables
/// @{

template <typename TYPE>
struct view_type_base {
    using raw_type = TYPE;
    using pointer_type = raw_type*;
};  // struct view_type_base

template <typename TYPE>
struct view_type : public view_type_base<TYPE> {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};  // struct view_type

template <typename TYPE>
struct view_type<type::scalar<TYPE> > : public view_type_base<TYPE> {
    using type = TYPE*;
};  // struct view_type

template <typename TYPE>
struct view_type<type::vector<TYPE> > : public view_type_base<TYPE> {
    using type = data::vector_view<TYPE>;
};  // struct view_type

template <typename TYPE>
struct view_type<type::jagged_vector<TYPE> > : public view_type_base<TYPE> {
    using type = data::jagged_vector_view<TYPE>;
};  // struct view_type

/// @}

}  // namespace details
}  // namespace edm
}  // namespace vecmem
