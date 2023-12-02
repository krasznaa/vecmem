/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

namespace vecmem {
namespace edm {

template <template <typename> class BASE, typename... VARTYPES>
element<BASE<schema<VARTYPES...>>>::element(base_reference base,
                                            size_type index)
    : m_base(base), m_index(index) {}

template <template <typename> class BASE, typename... VARTYPES>
typename element<BASE<schema<VARTYPES...>>>::base_reference
element<BASE<schema<VARTYPES...>>>::base() const {

    return m_base;
}

template <template <typename> class BASE, typename... VARTYPES>
typename element<BASE<schema<VARTYPES...>>>::size_type
element<BASE<schema<VARTYPES...>>>::index() const {

    return m_index;
}

}  // namespace edm
}  // namespace vecmem
