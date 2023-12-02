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

namespace vecmem {
namespace edm {

/// Technical base type for @c element<BASE<schema<VARTYPES...>>>
template <typename T>
class element;

template <template <typename> class INTERFACE, template <typename> class BASE,
          typename... VARTYPES>
class element<BASE<INTERFACE, schema<VARTYPES...>>> {

public:
    /// The schema describing the payload
    using schema_type = schema<VARTYPES...>;
    /// Type of the interface class
    template <typename T>
    using interface_type = INTERFACE<T>;
    /// Type of the base container
    using base_type = BASE<interface_type, schema_type>;
    /// Reference type to the base container
    using base_reference = typename std::add_lvalue_reference<base_type>::type;
    /// Size type used by the base container
    using size_type = typename base_type::size_type;

    /// Constructor with a base container and an index
    element(base_reference base, size_type index);

    /// Get the container of the element
    base_reference base() const;
    /// Get the index of the element inside of its container
    size_type index() const;

private:
    /// Reference to the base container
    base_reference m_base;
    /// Index of the element in the base container
    size_type m_index;

};  // class element

}  // namespace edm
}  // namespace vecmem

// Include the implementation.
#include "vecmem/edm/impl/element.ipp"
