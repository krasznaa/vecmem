/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/schema.hpp"

// System include(s).
#include <type_traits>

namespace vecmem::edm::type::details {

/// @name Traits turning variable types into constant types
/// @{

template <typename TYPE>
struct add_const {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};  // struct add_const

template <typename TYPE>
struct add_const<type::scalar<TYPE> > {
    using type = type::scalar<typename std::add_const<TYPE>::type>;
};  // struct add_const

template <typename TYPE>
struct add_const<type::vector<TYPE> > {
    using type = type::vector<typename std::add_const<TYPE>::type>;
};  // struct add_const

template <typename TYPE>
struct add_const<type::jagged_vector<TYPE> > {
    using type = type::jagged_vector<typename std::add_const<TYPE>::type>;
};  // struct add_const

/// @}

}  // namespace vecmem::edm::type::details
