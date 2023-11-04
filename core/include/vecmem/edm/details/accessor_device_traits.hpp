/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/device_traits.hpp"
#include "vecmem/edm/details/tuple.hpp"
#include "vecmem/edm/details/tuple_traits.hpp"
#include "vecmem/edm/schema.hpp"

// System include(s).
#include <tuple>

namespace vecmem {
namespace edm {
namespace details {

/// @name Traits for figuring out the return value for a given variable type
/// @{

template <typename TYPE>
struct accessor_device_type {
     struct UNKNOWN_TYPE {};
    using stored_type = UNKNOWN_TYPE;
    using return_type = UNKNOWN_TYPE;
    using const_return_type = UNKNOWN_TYPE;
};  // struct accessor_device_type

template <typename TYPE>
struct accessor_device_type<type::scalar<TYPE> > {
    using stored_type = typename device_type<type::scalar<TYPE> >::type;
    using return_type = TYPE&;
    using const_return_type = const TYPE&;
};  // struct accessor_device_type

template <typename TYPE>
struct accessor_device_type<type::vector<TYPE> > {
    using stored_type = typename device_type<type::vector<TYPE> >::type;
    using return_type = stored_type&;
    using const_return_type = const stored_type&;
};  // struct accessor_device_type

template <typename TYPE>
struct accessor_device_type<type::jagged_vector<TYPE> > {
    using stored_type = typename device_type<type::jagged_vector<TYPE> >::type;
    using return_type = stored_type&;
    using const_return_type = const stored_type&;
};  // struct accessor_device_type

template <std::size_t INDEX, typename... VARTYPES>
struct accessor_device_type_at {
    using stored_type = typename accessor_device_type<
        tuple_element_t<INDEX, tuple<VARTYPES...> > >::stored_type;
    using return_type = typename accessor_device_type<
        tuple_element_t<INDEX, tuple<VARTYPES...> > >::return_type;
    using const_return_type = typename accessor_device_type<
        tuple_element_t<INDEX, tuple<VARTYPES...> > >::const_return_type;
};  // struct accessor_device_type_at

/// @}

/// @name Traits used in the @c vecmem::edm::accessor::get functions
/// @{

template <typename TYPE>
struct accessor_device_get {};

template <typename TYPE>
struct accessor_device_get<type::scalar<TYPE> > {

    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type<type::scalar<TYPE> >::return_type
        get(typename accessor_device_type<type::scalar<TYPE> >::stored_type&
                obj) {

        return *obj;
    }
    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type<type::scalar<TYPE> >::const_return_type
        get(const typename accessor_device_type<
            type::scalar<TYPE> >::stored_type& obj) {

        return *obj;
    }

};  // struct accessor_device_get

template <typename TYPE>
struct accessor_device_get<type::vector<TYPE> > {

    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type<type::vector<TYPE> >::return_type
        get(typename accessor_device_type<type::vector<TYPE> >::stored_type&
                obj) {

        return obj;
    }
    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type<type::vector<TYPE> >::const_return_type
        get(const typename accessor_device_type<
            type::vector<TYPE> >::stored_type& obj) {

        return obj;
    }

};  // struct accessor_device_get

template <typename TYPE>
struct accessor_device_get<type::jagged_vector<TYPE> > {

    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type<type::jagged_vector<TYPE> >::return_type
        get(typename accessor_device_type<
            type::jagged_vector<TYPE> >::stored_type& obj) {

        return obj;
    }
    VECMEM_HOST_AND_DEVICE
    static constexpr typename accessor_device_type<
        type::jagged_vector<TYPE> >::const_return_type
    get(const typename accessor_device_type<
        type::jagged_vector<TYPE> >::stored_type& obj) {

        return obj;
    }

};  // struct accessor_device_get

template <std::size_t INDEX, typename... VARTYPES>
struct accessor_device_get_at {

    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type_at<INDEX, VARTYPES...>::return_type
        get(typename accessor_device_type_at<INDEX, VARTYPES...>::stored_type&
                obj) {

        return accessor_device_get<
            tuple_element_t<INDEX, tuple<VARTYPES...> > >::get(obj);
    }
    VECMEM_HOST_AND_DEVICE
    static constexpr
        typename accessor_device_type_at<INDEX, VARTYPES...>::const_return_type
        get(const typename accessor_device_type_at<
            INDEX, VARTYPES...>::stored_type& obj) {

        return accessor_device_get<
            tuple_element_t<INDEX, tuple<VARTYPES...> > >::get(obj);
    }

};  // struct accessor_device_get_at

/// @}

}  // namespace details
}  // namespace edm
}  // namespace vecmem
