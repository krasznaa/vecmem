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
#include "vecmem/edm/schema.hpp"
#include "vecmem/edm/view.hpp"
#include "vecmem/utils/types.hpp"

// System include(s).
#include <utility>

namespace vecmem {
namespace edm {

template <typename DUMMY>
class device {
    /// Delete the constructor of this type
    device() = delete;
};

/// SoA device container type
///
/// @tparam ...VARTYPES The variable types stored in the container
///
template <typename... VARTYPES>
class device<schema<VARTYPES...>> {

public:
    /// The schema describing the device-accessible variables
    using schema_type = schema<VARTYPES...>;
    /// Size type used for the container
    using size_type = typename view<schema_type>::size_type;
    /// Pointer type to the size of the container
    using size_pointer = typename view<schema_type>::size_pointer;
    /// The tuple type holding all of the the individual "device objects"
    typedef details::tuple<typename details::device_type<VARTYPES>::type...>
        device_tuple_type;

    /// Constructor from an approptiate view
    VECMEM_HOST_AND_DEVICE
    device(const view<schema_type>& view);

    /// Get the size of the container
    VECMEM_HOST_AND_DEVICE
    size_type size() const;
    /// Get the maximum capacity of the container
    VECMEM_HOST_AND_DEVICE
    size_type capacity() const;

    /// Add one default element to all (vector) variables (thread safe)
    VECMEM_HOST_AND_DEVICE
    size_type push_back_default();

    /// Get the vector of a specific variable (non-const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE details::tuple_element_t<
        INDEX,
        details::tuple<typename details::device_type<VARTYPES>::type...>>&
    get();
    /// Get the vector of a specific variable (const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE const details::tuple_element_t<
        INDEX,
        details::tuple<typename details::device_type<VARTYPES>::type...>>&
    get() const;

    /// Direct (non-const) access to the underlying tuple of variables
    VECMEM_HOST_AND_DEVICE
    device_tuple_type& variables();
    /// Direct (const) access to the underlying tuple of variables
    VECMEM_HOST_AND_DEVICE
    const device_tuple_type& variables() const;

private:
    /// Construct a default element for every vector variable
    template <std::size_t INDEX, std::size_t... Is>
    VECMEM_HOST_AND_DEVICE void construct_default(
        size_type index, std::index_sequence<INDEX, Is...>);
    /// Construct a default element for every vector variable (terminal node)
    VECMEM_HOST_AND_DEVICE void construct_default(size_type index,
                                                  std::index_sequence<>);

    /// Default, no-op vector construction helper function
    template <typename T>
    VECMEM_HOST_AND_DEVICE void construct_vector(size_type, T&);
    template <typename T>
    VECMEM_HOST_AND_DEVICE void construct_vector(size_type index,
                                                 device_vector<T>& vec);

    /// Maximum capacity of the container
    size_type m_capacity = 0;
    /// (Resizable) Size of the container described by this view
    size_pointer m_size = nullptr;
    /// The tuple holding all of the individual "device objects"
    device_tuple_type m_data;

};  // class device

}  // namespace edm
}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/device.ipp"
