/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/utils/types.hpp"

// System include(s).
#include <tuple>
#include <type_traits>

namespace vecmem::edm {

/// Dummy base type, which only gets used with incorrect template arguments
///
/// @tparam DUMMY A non @c vecmem::edm::schema type
///
template <typename DUMMY>
class view {
    /// Delete the constructor of this type
    view() = delete;
};

/// Base class for SoA views
///
/// @tparam ...VARTYPES The variable types to describe by the view
///
template <typename... VARTYPES>
class view<schema<VARTYPES...>> {

public:
    /// The schema describing the container view
    using schema_type = schema<VARTYPES...>;
    /// Size type used for the container
    typedef unsigned int size_type;
    /// Pointer type to the size of the container
    typedef typename std::conditional<
        std::disjunction_v<
            std::is_const<typename details::view_type<VARTYPES>::raw_type>...>,
        const size_type*, size_type*>::type size_pointer;
    /// Constant pointer type to the size of the container
    typedef const typename std::remove_const<size_pointer>::type
        const_size_pointer;
    /// The tuple type holding all of the views for the individual variables
    typedef std::tuple<typename details::view_type<VARTYPES>::type...>
        view_tuple_type;

    /// Default constructor
    view() = default;
    /// Constructor from a (slightly) different view
    ///
    /// As with @c vecmem::data::vector_view and
    /// @c vecmem::data::jagged_vector_view, this constructor must only be
    /// active for non-const to const conversions. As we need to use the default
    /// copy and move constructors for copying/moving identical types. Otherwise
    /// SYCL is not happy with sending these as kernel parameters.
    ///
    template <typename... OTHERTYPES,
              std::enable_if_t<
                  std::conjunction_v<std::is_constructible<
                      typename details::view_type<VARTYPES>::type,
                      typename details::view_type<OTHERTYPES>::type>...> &&
                      std::disjunction_v<std::negation<std::is_same<
                          typename details::view_type<VARTYPES>::type,
                          typename details::view_type<OTHERTYPES>::type>>...>,
                  bool> = true>
    VECMEM_HOST_AND_DEVICE view(const view<schema<OTHERTYPES...>>& other);

    /// Get the size of the container
    VECMEM_HOST_AND_DEVICE
    size_type size() const;
    /// Get the maximum capacity of the container
    VECMEM_HOST_AND_DEVICE
    size_type capacity() const;

    /// Get a pointer to the size of the container (non-const)
    VECMEM_HOST_AND_DEVICE
    size_pointer size_ptr();
    /// Get a pointer to the size of the container (const)
    VECMEM_HOST_AND_DEVICE
    const_size_pointer size_ptr() const;

    /// Get the view of a specific variable (non-const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE typename std::tuple_element<
        INDEX,
        std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
    get_view();
    /// Get the view of a specific variable (const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE const typename std::tuple_element<
        INDEX,
        std::tuple<typename details::view_type<VARTYPES>::type...>>::type&
    get_view() const;

    /// Direct (non-const) access to the underlying tuple of views
    VECMEM_HOST_AND_DEVICE
    view_tuple_type& views();
    /// Direct (const) access to the underlying tuple of views
    VECMEM_HOST_AND_DEVICE
    const view_tuple_type& views() const;

protected:
    /// Maximum capacity of the container
    size_type m_capacity = 0;
    /// (Resizable) Size of the container described by this view
    size_pointer m_size = nullptr;
    /// Views for the individual variables
    view_tuple_type m_views;

};  // class view

}  // namespace vecmem::edm

// Include the implementation.
#include "vecmem/edm/impl/view.ipp"
