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

// System include(s).
#include <type_traits>

namespace vecmem {
namespace cuda {

/// Custom implementation for atomic operations in CUDA device code
///
/// @note All member functions are declared @c VECMEM_HOST_DEVICE, because
///       this class may be used from functions that also carry that setup.
///       (Like functions in @c vecmem::device_vector.) Even though this class
///       cannot be used in host code, CUDA and HIP are sensitive to these
///       sort of declarations being consistent.
///
/// @tparam T Type to perform atomic operations on
/// @tparam address The device address space to use
///
template <typename T,
          device_address_space address = device_address_space::global>
class device_atomic_ref {

public:
    /// @name Type definitions
    /// @{

    /// Type managed by the object
    typedef T value_type;
    /// Difference between two objects
    typedef value_type difference_type;
    /// Pointer to the value in global memory
    typedef value_type* pointer;
    /// Reference to a value given by the user
    typedef value_type& reference;

    /// @}

    /// @name Check(s) on the value type
    /// @{

    static_assert(
        std::is_integral<value_type>::value,
        "vecmem::cuda::atomic_ref only accepts built-in integral types");

    /// @}

    /// Constructor, with a pointer to the managed variable
    VECMEM_HOST_AND_DEVICE
    explicit device_atomic_ref(reference ref);
    /// Copy constructor
    VECMEM_HOST_AND_DEVICE
    device_atomic_ref(const device_atomic_ref& parent);

    /// Disable the assignment operator
    device_atomic_ref& operator=(const device_atomic_ref&) = delete;

    /// @name Value setter/getter functions
    /// @{

    /// Assigns a value desired to the referenced object
    ///
    /// @see vecmem::cuda::atomic_ref::store
    ///
    VECMEM_HOST_AND_DEVICE
    value_type operator=(value_type data) const;

    /// Set the variable to the desired value
    VECMEM_HOST_AND_DEVICE
    void store(value_type data,
               memory_order order = memory_order::seq_cst) const;
    /// Get the value of the variable
    VECMEM_HOST_AND_DEVICE
    value_type load(memory_order order = memory_order::seq_cst) const;

    /// Exchange the current value of the variable with a different one
    VECMEM_HOST_AND_DEVICE
    value_type exchange(value_type data,
                        memory_order order = memory_order::seq_cst) const;

    /// Compare against the current value, and exchange only if different
    VECMEM_HOST_AND_DEVICE
    bool compare_exchange_strong(reference expected, value_type desired,
                                 memory_order success,
                                 memory_order failure) const;
    /// Compare against the current value, and exchange only if different
    VECMEM_HOST_AND_DEVICE
    bool compare_exchange_strong(
        reference expected, value_type desired,
        memory_order order = memory_order::seq_cst) const;

    /// @}

    /// @name Value modifier functions
    /// @{

    /// Add a chosen value to the stored variable
    VECMEM_HOST_AND_DEVICE
    value_type fetch_add(value_type data,
                         memory_order order = memory_order::seq_cst) const;
    /// Substitute a chosen value from the stored variable
    VECMEM_HOST_AND_DEVICE
    value_type fetch_sub(value_type data,
                         memory_order order = memory_order::seq_cst) const;

    /// Replace the current value with the specified value AND-ed to it
    VECMEM_HOST_AND_DEVICE
    value_type fetch_and(value_type data,
                         memory_order order = memory_order::seq_cst) const;
    /// Replace the current value with the specified value OR-d to it
    VECMEM_HOST_AND_DEVICE
    value_type fetch_or(value_type data,
                        memory_order order = memory_order::seq_cst) const;
    /// Replace the current value with the specified value XOR-d to it
    VECMEM_HOST_AND_DEVICE
    value_type fetch_xor(value_type data,
                         memory_order order = memory_order::seq_cst) const;

    /// @}

private:
    /// Pointer to the value to perform atomic operations on
    pointer m_ptr;

};  // class device_atomic_ref

}  // namespace cuda
}  // namespace vecmem

// Include the implementation.
#include "vecmem/memory/impl/cuda_device_atomic_ref.ipp"