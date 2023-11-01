/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/jagged_vector.hpp"
#include "vecmem/containers/vector.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

namespace vecmem::edm::details {

/// @name Traits for the host types for the individual variables
/// @{

template <typename TYPE>
struct host_type_base {
    using raw_type = TYPE;
};  // struct host_type_base

template <typename TYPE>
struct host_type : public host_type_base<TYPE> {
    struct UNKNOWN_TYPE {};
    using type = UNKNOWN_TYPE;
};  // struct host_type

template <typename TYPE>
struct host_type<type::scalar<TYPE> > : public host_type_base<TYPE> {
    using type = unique_obj_ptr<TYPE>;
};  // struct host_type

template <typename TYPE>
struct host_type<type::vector<TYPE> > : public host_type_base<TYPE> {
    using type = vector<TYPE>;
};  // struct host_type

template <typename TYPE>
struct host_type<type::jagged_vector<TYPE> > : public host_type_base<TYPE> {
    using type = jagged_vector<TYPE>;
};  // struct host_type

/// @}

/// @name Traits for allocating host variables
/// @{

template <typename TYPE>
struct host_alloc {};

template <typename TYPE>
struct host_alloc<type::scalar<TYPE> > {
    static typename host_type<type::scalar<TYPE> >::type make(
        memory_resource& mr) {
        return make_unique_obj<TYPE>(mr);
    }
};  // struct host_alloc

template <typename TYPE>
struct host_alloc<type::vector<TYPE> > {
    static typename host_type<type::vector<TYPE> >::type make(
        memory_resource& mr) {
        return typename host_type<type::vector<TYPE> >::type{&mr};
    }
};  // struct host_alloc

template <typename TYPE>
struct host_alloc<type::jagged_vector<TYPE> > {
    static typename host_type<type::vector<TYPE> >::type make(
        memory_resource& mr) {
        return typename host_type<type::jagged_vector<TYPE> >::type{&mr};
    }
};  // struct host_alloc

/// @}

}  // namespace vecmem::edm::details
