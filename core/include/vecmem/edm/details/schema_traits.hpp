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

namespace vecmem::edm {
namespace type::details {

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

/// @name Traits checking the type of a variable
/// @{

template <typename TYPE>
struct is_scalar {
    static constexpr bool value = false;
};  // struct is_scalar

template <typename TYPE>
struct is_scalar<type::scalar<TYPE> > {
    static constexpr bool value = true;
};  // struct is_scalar

template <typename TYPE>
struct is_vector {
    static constexpr bool value = false;
};  // struct is_vector

template <typename TYPE>
struct is_vector<type::vector<TYPE> > {
    static constexpr bool value = true;
};  // struct is_vector

template <typename TYPE>
struct is_vector<type::jagged_vector<TYPE> > {
    static constexpr bool value = true;
};  // struct is_vector

template <typename TYPE>
struct is_jagged_vector {
    static constexpr bool value = false;
};  // struct is_jagged_vector

template <typename TYPE>
struct is_jagged_vector<type::jagged_vector<TYPE> > {
    static constexpr bool value = true;
};  // struct is_jagged_vector

/// @}

}  // namespace type::details

/// @name Trait(s) making an entire schema into a constant one
/// @{

template <typename... VARTYPES>
struct add_const {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};

template <typename... VARTYPES>
struct add_const<schema<VARTYPES...> > {
    using type = schema<typename type::details::add_const<VARTYPES>::type...>;
};

/// @}

}  // namespace vecmem::edm
