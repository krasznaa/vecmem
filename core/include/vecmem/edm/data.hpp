/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/data_traits.hpp"
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/edm/view.hpp"
#include "vecmem/memory/memory_resource.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <tuple>

namespace vecmem {
namespace edm {

/// Generic data template
template <typename DUMMY>
class data {
    /// Delete the constructor of this type
    data() = delete;
};

/// Base class for SoA "data objects"
///
/// @tparam ...VARTYPES The variable types to store in the data object
///
template <typename... VARTYPES>
class data<schema<VARTYPES...>> : public view<schema<VARTYPES...>> {

public:
    /// The schema describing the buffer's payload
    using schema_type = schema<VARTYPES...>;
    /// Base view type
    using view_type = view<schema_type>;
    /// Size type used for the container
    using size_type = typename view_type::size_type;
    /// The tuple type holding all of the data objects for the individual
    /// variables
    typedef std::tuple<typename details::data_type<VARTYPES>::type...>
        data_tuple_type;

    /// Default constructor
    data() = default;
    /// Constructor for the data object
    ///
    /// @param size The size of the (outer) arrays in the data object
    /// @param mr   The memory resource to use for the allocation(s)
    ///
    data(size_type size, memory_resource& mr);

    /// Direct (non-const) access to the underlying tuple of data objects
    data_tuple_type& data_variables();
    /// Direct (const) access to the underlying tuple of data objects
    const data_tuple_type& data_variables() const;

private:
    /// Variable holding data for the jagged vector variables
    data_tuple_type m_data;

};  // class data

}  // namespace edm

/// Helper function for getting a (possibly non-const) view for a data object
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param data The data object to get a view for
/// @return A (possibly non-const) view into for the data object
///
template <typename... VARTYPES>
edm::view<edm::schema<VARTYPES...>> get_data(
    edm::data<edm::schema<VARTYPES...>>& data);

/// Helper function for getting a (const) view for a data object
///
/// @tparam ...VARTYPES The variable types describing the container
/// @param data The data object to get a view for
/// @return A (const) view into for the data object
///
template <typename... VARTYPES>
edm::view<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::data<edm::schema<VARTYPES...>>& data);

}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/data.ipp"
