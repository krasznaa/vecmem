/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2022 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// System include(s).
#include <cstdint>
#include <stdexcept>
#include <string>

namespace vecmem::cuda::details {

/// Exception type thrown when the code asks for an invalid alignment
class invalid_alignment : public std::invalid_argument {

public:
    /// Constructor with the requested (invalid) alignment
    invalid_alignment(std::size_t alignment)
        : std::invalid_argument("Invalid CUDA memory alignment (" +
                                std::to_string(alignment) + ") requested") {}

};  // class invalid_alignment

/// Check that the requested alignment is fulfilled by CUDA
///
/// The CUDA memory allocator functions guarantee a minimum of 256 byte
/// alignment.
///
/// https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#device-memory-accesses
///
/// However, unfortunately they don't provide any way of requesting any custom
/// alignment from them through their API. So this function just checks that
/// whatever alignment is requested at runtime from the CUDA memory resources,
/// would be compatible with that 256 byte minimum alignment.
///
/// In the unlikely case that it is not, a
/// @c vecmem::cuda::details::invalid_alignment exception is thrown.
///
/// @param alignment The alignment requested from the memory resource
///
inline void alignment_check(std::size_t alignment) {

    // Check that the default 256 byte alignment of CUDA memory allocations
    // is compatible with the alignment explicitly requested.
    static const std::size_t CUDA_ALIGNMENT = 256;
    if ((CUDA_ALIGNMENT % alignment) != 0) {
        throw invalid_alignment(alignment);
    }
}

}  // namespace vecmem::cuda::details
