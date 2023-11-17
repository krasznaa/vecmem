/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/data/vector_view.hpp"
#include "vecmem/edm/details/tuple.hpp"
#include "vecmem/edm/details/tuple_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/utils/type_traits.hpp"
#include "vecmem/utils/types.hpp"

// System include(s).
#include <type_traits>

namespace vecmem {
namespace edm {

/// Dummy base type, which only gets used with incorrect template arguments
template <typename T>
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

    // Sanity check(s).
    static_assert(sizeof...(VARTYPES) > 0, "Empty views are not supported");

public:
    /// The schema describing the container view
    using schema_type = schema<VARTYPES...>;
    /// Size type used for the container
    typedef unsigned int size_type;
    /// Pointer type to the size of the container
    typedef typename std::conditional<
        vecmem::details::disjunction<std::is_const<
            typename details::view_type<VARTYPES>::payload_type>...>::value,
        const size_type*, size_type*>::type size_pointer;
    /// Constant pointer type to the size of the container
    typedef const typename std::remove_const<size_pointer>::type
        const_size_pointer;
    /// The tuple type holding all of the views for the individual variables
    typedef details::tuple<typename details::view_type<VARTYPES>::type...>
        view_tuple_type;

    /// Default constructor
    view() = default;
    /// Constructor with a capacity and size.
    ///
    /// @param capacity The maximum capacity of the container
    /// @param size The size of the container
    ///
    VECMEM_HOST_AND_DEVICE
    view(size_type capacity, size_pointer size = nullptr);
    /// Constructor from a (slightly) different view
    ///
    /// As with @c vecmem::data::vector_view and
    /// @c vecmem::data::jagged_vector_view, this constructor must only be
    /// active for non-const to const conversions. As we need to use the default
    /// copy and move constructors for copying/moving identical types. Otherwise
    /// SYCL is not happy with sending these as kernel parameters.
    ///
    template <
        typename... OTHERTYPES,
        std::enable_if_t<
            vecmem::details::conjunction_v<std::is_constructible<
                typename details::view_type<VARTYPES>::type,
                typename details::view_type<OTHERTYPES>::type>...> &&
                vecmem::details::disjunction_v<
                    vecmem::details::negation<std::is_same<
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

    /// Get a pointer to the size(es) of the container (non-const)
    VECMEM_HOST_AND_DEVICE
    size_pointer size_ptr();
    /// Get a pointer to the size(es) of the container (const)
    VECMEM_HOST_AND_DEVICE
    const_size_pointer size_ptr() const;
    /// Size of the array that @c size_ptr() points to
    VECMEM_HOST_AND_DEVICE
    size_type size_ptr_size() const;

    /// Get the view of a specific variable (non-const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE typename details::tuple_element_t<
        INDEX, details::tuple<typename details::view_type<VARTYPES>::type...>>&
    get();
    /// Get the view of a specific variable (const)
    template <std::size_t INDEX>
    VECMEM_HOST_AND_DEVICE const typename details::tuple_element_t<
        INDEX, details::tuple<typename details::view_type<VARTYPES>::type...>>&
    get() const;

    /// Direct (non-const) access to the underlying tuple of views
    VECMEM_HOST_AND_DEVICE
    view_tuple_type& variables();
    /// Direct (const) access to the underlying tuple of views
    VECMEM_HOST_AND_DEVICE
    const view_tuple_type& variables() const;

    /// View at the single (device) memory allocation of the container
    VECMEM_HOST_AND_DEVICE
    const data::vector_view<char>& payload() const;

    /// View at the memory that describes the layout of the container
    VECMEM_HOST_AND_DEVICE
    const data::vector_view<char>& layout() const;
    /// View at the memory that describes the layout of the container (in host
    /// accessible memory)
    VECMEM_HOST_AND_DEVICE
    const data::vector_view<char>& host_layout() const;

protected:
    /// Maximum capacity of the container
    size_type m_capacity = 0;
    /// (Resizable) Size(s) of the container described by this view
    size_pointer m_size = nullptr;
    /// Size of the array that @c m_size points to
    size_type m_size_size = 1;
    /// Views for the individual variables
    view_tuple_type m_views;

    /// View into the single (device) memory allocation for the "payload"
    data::vector_view<char> m_payload;

    /// View into the memory that describes the layout of the container
    data::vector_view<char> m_layout;
    /// View into the memory that describes the layout of the container (in host
    /// accessible memory)
    data::vector_view<char> m_host_layout;

};  // class view

}  // namespace edm
}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/view.ipp"
