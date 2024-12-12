/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2022-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/memory/device_address_space.hpp"
#include "vecmem/memory/memory_order.hpp"
#include "vecmem/utils/types.hpp"

namespace vecmem {

/// Base class for the Windows implementation of atomic operations
template <typename T,
          device_address_space address = device_address_space::global>
class windows_device_atomic_ref;

template <device_address_space address>
class windows_device_atomic_ref<long, address> {

public:
    /// @name Type definitions
    /// @{

    /// Type managed by the object
    typedef long value_type;
    /// Difference between two objects
    typedef value_type difference_type;
    /// Pointer to the value in global memory
    typedef value_type* pointer;
    /// Reference to a value given by the user
    typedef value_type& reference;

    /// @}

    /// Constructor, with a pointer to the managed variable
    VECMEM_HOST
    explicit windows_device_atomic_ref(reference ref);
    /// Copy constructor
    VECMEM_HOST
    windows_device_atomic_ref(const windows_device_atomic_ref& parent);

    /// Disable the assignment operator
    windows_device_atomic_ref& operator=(const windows_device_atomic_ref&) =
        delete;

    /// @name Value setter/getter functions
    /// @{

    /// Assigns a value desired to the referenced object
    ///
    /// @see vecmem::cuda::atomic_ref::store
    ///
    VECMEM_HOST
    value_type operator=(value_type data) const;

    /// Set the variable to the desired value
    VECMEM_HOST
    void store(value_type data,
               memory_order order = memory_order::seq_cst) const;
    /// Get the value of the variable
    VECMEM_HOST
    value_type load(memory_order order = memory_order::seq_cst) const;

    /// Exchange the current value of the variable with a different one
    VECMEM_HOST
    value_type exchange(value_type data,
                        memory_order order = memory_order::seq_cst) const;

    /// Compare against the current value, and exchange only if different
    VECMEM_HOST
    bool compare_exchange_strong(reference expected, value_type desired,
                                 memory_order success,
                                 memory_order failure) const;
    /// Compare against the current value, and exchange only if different
    VECMEM_HOST
    bool compare_exchange_strong(
        reference expected, value_type desired,
        memory_order order = memory_order::seq_cst) const;

    /// @}

    /// @name Value modifier functions
    /// @{

    /// Add a chosen value to the stored variable
    VECMEM_HOST
    value_type fetch_add(value_type data,
                         memory_order order = memory_order::seq_cst) const;
    /// Substitute a chosen value from the stored variable
    VECMEM_HOST
    value_type fetch_sub(value_type data,
                         memory_order order = memory_order::seq_cst) const;

    /// Replace the current value with the specified value AND-ed to it
    VECMEM_HOST
    value_type fetch_and(value_type data,
                         memory_order order = memory_order::seq_cst) const;
    /// Replace the current value with the specified value OR-d to it
    VECMEM_HOST
    value_type fetch_or(value_type data,
                        memory_order order = memory_order::seq_cst) const;
    /// Replace the current value with the specified value XOR-d to it
    VECMEM_HOST
    value_type fetch_xor(value_type data,
                         memory_order order = memory_order::seq_cst) const;

    /// @}

private:
    /// Pointer to the value to perform atomic operations on
    pointer m_ptr;
};

}  // namespace vecmem

// Include the implementation.
#include "vecmem/memory/impl/windows_device_atomic_ref.ipp"
