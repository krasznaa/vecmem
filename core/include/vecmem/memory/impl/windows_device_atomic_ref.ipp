/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2022-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// System include(s).
#include <winnt.h>

#include <cassert>

namespace vecmem {

template <device_address_space address>
VECMEM_HOST windows_device_atomic_ref<long, address>::windows_device_atomic_ref(
    reference ref)
    : m_ptr(&ref) {}

template <device_address_space address>
VECMEM_HOST windows_device_atomic_ref<long, address>::windows_device_atomic_ref(
    const windows_device_atomic_ref& parent)
    : m_ptr(parent.m_ptr) {}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::operator=(
    value_type data) const -> value_type {

    store(data);
    return data;
}

template <device_address_space address>
VECMEM_HOST void windows_device_atomic_ref<long, address>::store(
    value_type data, memory_order order) const {

    __atomic_store_n(m_ptr, data, details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::load(
    memory_order order) const -> value_type {

    return __atomic_load_n(m_ptr, details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::exchange(
    value_type data, memory_order order) const -> value_type {

    return __atomic_exchange_n(m_ptr, data,
                               details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST bool
windows_device_atomic_ref<long, address>::compare_exchange_strong(
    reference expected, value_type desired, memory_order order) const {

    return compare_exchange_strong(expected, desired, order, order);
}

template <device_address_space address>
VECMEM_HOST bool
windows_device_atomic_ref<long, address>::compare_exchange_strong(
    reference expected, value_type desired, memory_order, memory_order) const {

    const long initial = InterlockedCompareExchange(m_ptr, desired, expected);
    return (initial == expected);
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::fetch_add(
    value_type data, memory_order order) const -> value_type {

    return __atomic_fetch_add(m_ptr, data,
                              details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::fetch_sub(
    value_type data, memory_order order) const -> value_type {

    return __atomic_fetch_add(m_ptr, -data,
                              details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::fetch_and(
    value_type data, memory_order order) const -> value_type {

    return __atomic_fetch_and(m_ptr, data,
                              details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::fetch_or(
    value_type data, memory_order order) const -> value_type {

    return __atomic_fetch_or(m_ptr, data,
                             details::memorder_to_posix_builtin(order));
}

template <device_address_space address>
VECMEM_HOST auto windows_device_atomic_ref<long, address>::fetch_xor(
    value_type data, memory_order order) const -> value_type {

    return __atomic_fetch_xor(m_ptr, data,
                              details::memorder_to_posix_builtin(order));
}

}  // namespace vecmem
