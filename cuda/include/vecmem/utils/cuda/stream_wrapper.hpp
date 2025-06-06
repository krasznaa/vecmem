/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021-2025 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/vecmem_cuda_export.hpp"

// System include(s).
#include <memory>
#include <string>

namespace vecmem {
namespace cuda {

/// Wrapper class for @c cudaStream_t
///
/// It is necessary for passing around CUDA stream objects in code that should
/// not be directly exposed to the CUDA header(s).
///
class stream_wrapper {

public:
    /// Invalid/default device identifier
    static constexpr int INVALID_DEVICE = -1;

    /// Construct a new stream (for the specified device)
    VECMEM_CUDA_EXPORT
    explicit stream_wrapper(int device = INVALID_DEVICE);
    /// Wrap an existing @c cudaStream_t object
    ///
    /// Without taking ownership of it!
    ///
    VECMEM_CUDA_EXPORT
    stream_wrapper(void* stream);

    /// Copy constructor
    VECMEM_CUDA_EXPORT
    stream_wrapper(const stream_wrapper& parent);
    /// Move constructor
    VECMEM_CUDA_EXPORT
    stream_wrapper(stream_wrapper&& parent) noexcept;

    /// Destructor
    VECMEM_CUDA_EXPORT
    ~stream_wrapper();

    /// Copy assignment
    VECMEM_CUDA_EXPORT
    stream_wrapper& operator=(const stream_wrapper& rhs);
    /// Move assignment
    VECMEM_CUDA_EXPORT
    stream_wrapper& operator=(stream_wrapper&& rhs) noexcept;

    /// Access a typeless pointer to the managed @c cudaStream_t object
    VECMEM_CUDA_EXPORT
    void* stream() const;

    /// Wait for all queued tasks from the stream to complete
    VECMEM_CUDA_EXPORT
    void synchronize();

    /// Get the name of the device that the stream operates on
    VECMEM_CUDA_EXPORT
    std::string device_name() const;

private:
    /// Structure holding the internals of the class
    struct impl;
    /// Pointer to the internal structure
    std::unique_ptr<impl> m_impl;

};  // class stream_wrapper

}  // namespace cuda
}  // namespace vecmem
