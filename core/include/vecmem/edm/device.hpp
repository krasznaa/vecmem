/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/device_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/edm/view.hpp"

// System include(s).
#include <tuple>

namespace vecmem::edm {

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
    /// The tuple type holding all of the the individual "device objects"
    typedef std::tuple<typename details::device_type<VARTYPES>::type...>
        device_tuple_type;

    /// Constructor from an approptiate view
    device(const view<schema_type>& view);

    /// Get the vector of a specific variable (non-const)
    template <std::size_t INDEX>
    typename std::tuple_element<
        INDEX,
        std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
    get();
    /// Get the vector of a specific variable (const)
    template <std::size_t INDEX>
    const typename std::tuple_element<
        INDEX,
        std::tuple<typename details::device_type<VARTYPES>::type...>>::type&
    get() const;

    /// Direct (non-const) access to the underlying tuple of variables
    device_tuple_type& variables();
    /// Direct (const) access to the underlying tuple of variables
    const device_tuple_type& variables() const;

private:
    /// The tuple holding all of the individual "device objects"
    device_tuple_type m_data;

};  // class device

}  // namespace vecmem::edm

// Include the implementation.
#include "vecmem/edm/impl/device.ipp"
