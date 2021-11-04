/**
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Local include(s).
#include "vecmem/containers/details/reverse_iterator.hpp"
#include "vecmem/utils/type_traits.hpp"
#include "vecmem/utils/types.hpp"

// System include(s).
#include <cstddef>
#include <type_traits>
#include <utility>

namespace vecmem {
/**
 * @brief Simple statically-sized array-like class designed for use in
 * device code.
 *
 * This class is designed to be an almost-drop-in replacement for std::array
 * which can be used in device code.
 *
 * @tparam T The array type.
 * @tparam N The size of the array.
 */
template <typename T, std::size_t N>
class static_array {
public:
    /// @name Type definitions, mimicking @c std::array
    /// @{

    /// Type of the array elements
    using value_type = T;
    /// Size type for the array
    using size_type = std::size_t;
    /// Pointer difference type
    using difference_type = std::ptrdiff_t;

    /// Value reference type
    using reference = value_type &;
    /// Constant value reference type
    using const_reference = const value_type &;
    /// Value pointer type
    using pointer = value_type *;
    /// Constant value pointer type
    using const_pointer = const value_type *;

    /// Forward iterator type
    using iterator = pointer;
    /// Constant forward iterator type
    using const_iterator = const_pointer;
    /// Reverse iterator type
    using reverse_iterator = vecmem::details::reverse_iterator<iterator>;
    /// Constant reverse iterator type
    using const_reverse_iterator =
        vecmem::details::reverse_iterator<const_iterator>;

    /// @}

    /// @name Constructors
    /// @{

    /**
     * @brief Trivial constructor.
     *
     * This constructor does nothing, and leaves the inner array
     * uninitialized.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr static_array(void);

    /**
     * @brief Construct an array from a parameter pack of arbitrary size.
     *
     * This constructor is of arbitrary arity, and inserts those elements in
     * order into the array.
     *
     * @tparam Tp The parameter pack for the arguments.
     *
     * @warning The std::array implementation of this constructor requires
     * the parameter list to be of size equal to or less than the array
     * itself. This class is slightly different, as it requires the argument
     * list to be exactly the same length. This protects the user from
     * accidentally initializing an array with fewer values than necessary.
     *
     * @note All parameters passed to this function must be convertible to
     * the array value type, but the values do not need to be homogeneous.
     */
    template <typename... Tp, typename = std::enable_if_t<sizeof...(Tp) == N>,
              typename = std::enable_if_t<
                  details::conjunction_v<std::is_convertible<Tp, T>...> > >
    VECMEM_HOST_AND_DEVICE constexpr static_array(Tp &&... a) : m_array() {

        static_array_impl(0, std::forward<Tp>(a)...);
    }

    /// @}

    /// @name Array element access functions
    /// @{

    /**
     * @brief Bounds-checked accessor method.
     *
     * Since this method can throw an exception, this is not usable on the
     * device side.
     *
     * @param[in] i The index to access.
     *
     * @return The value at index i if i less less than N, otherwise an
     * exception is thrown.
     */
    VECMEM_HOST
    constexpr reference at(size_type i);

    /**
     * @brief Constant bounds-checked accessor method.
     *
     * Since this method can throw an exception, this is not usable on the
     * device side.
     *
     * @param[in] i The index to access.
     *
     * @return The value at index i if i less less than N, otherwise an
     * exception is thrown.
     */
    VECMEM_HOST
    constexpr const_reference at(size_type i) const;

    /**
     * @brief Accessor method.
     *
     * @param[in] i The index to access.
     *
     * @return The value at index i if i less less than N, otherwise the
     * behaviour is undefined.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr reference operator[](size_type i);

    /**
     * @brief Constant accessor method.
     *
     * @param[in] i The index to access.
     *
     * @return The value at index i if i less less than N, otherwise the
     * behaviour is undefined.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr const_reference operator[](size_type i) const;

    /**
     * @brief Access the front element of the array.
     *
     * @return The first element of the array.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr reference front(void);

    /**
     * @brief Access the front element of the array in a const fashion.
     *
     * @return The first element of the array.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr const_reference front(void) const;

    /**
     * @brief Access the back element of the array.
     *
     * @return The last element of the array.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr reference back(void);

    /**
     * @brief Access the back element of the array in a const fashion.
     *
     * @return The last element of the array.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr const_reference back(void) const;

    /**
     * @brief Access the underlying data of the array.
     *
     * @return A pointer to the underlying memory.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr pointer data(void);

    /**
     * @brief Access the underlying data of the array in a const fasion.
     *
     * @return A pointer to the underlying memory.
     */
    VECMEM_HOST_AND_DEVICE
    constexpr const_pointer data(void) const;

    /// @}

    /// @name Iterator providing functions
    /// @{

    /// Return a forward iterator pointing at the beginning of the array
    VECMEM_HOST_AND_DEVICE
    constexpr iterator begin();
    /// Return a constant forward iterator pointing at the beginning of the
    /// array
    VECMEM_HOST_AND_DEVICE
    constexpr const_iterator begin() const;
    /// Return a constant forward iterator pointing at the beginning of the
    /// array
    VECMEM_HOST_AND_DEVICE
    constexpr const_iterator cbegin() const;

    /// Return a forward iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr iterator end();
    /// Return a constant forward iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr const_iterator end() const;
    /// Return a constant forward iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr const_iterator cend() const;

    /// Return a reverse iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr reverse_iterator rbegin();
    /// Return a constant reverse iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr const_reverse_iterator rbegin() const;
    /// Return a constant reverse iterator pointing at the end of the array
    VECMEM_HOST_AND_DEVICE
    constexpr const_reverse_iterator crbegin() const;

    /// Return a reverse iterator pointing at the beginning of the array
    VECMEM_HOST_AND_DEVICE
    constexpr reverse_iterator rend();
    /// Return a constant reverse iterator pointing at the beginning of the
    /// array
    VECMEM_HOST_AND_DEVICE
    constexpr const_reverse_iterator rend() const;
    /// Return a constant reverse iterator pointing at the beginning of the
    /// array
    VECMEM_HOST_AND_DEVICE
    constexpr const_reverse_iterator crend() const;

    /// @}

    /// @name Capacity checking functions
    /// @{

    /// Check whether the array is empty
    VECMEM_HOST_AND_DEVICE
    constexpr bool empty() const;
    /// Return the number of elements in the array
    VECMEM_HOST_AND_DEVICE
    constexpr size_type size() const;
    /// Return the maximum (fixed) number of elements in the array
    VECMEM_HOST_AND_DEVICE
    constexpr size_type max_size() const;

    /// @}

    /// @name Payload modification functions
    /// @{

    /// Fill the array with a constant value
    VECMEM_HOST_AND_DEVICE
    void fill(const_reference value);

    /// @}

private:
    /**
     * @brief Private helper-constructor for the parameter pack constructor.
     */
    template <typename Tp1, typename... Tp>
    VECMEM_HOST_AND_DEVICE constexpr void static_array_impl(size_type i,
                                                            Tp1 &&a1,
                                                            Tp &&... a);
    /**
     * @brief Private helper-constructor for the parameter pack constructor.
     */
    template <typename Tp1>
    VECMEM_HOST_AND_DEVICE constexpr void static_array_impl(size_type i,
                                                            Tp1 &&a1);

    /// Array holding the container's data
    typename details::array_type<T, N>::type m_array;
};

/// Equality check on two arrays
template <typename T, std::size_t N>
VECMEM_HOST_AND_DEVICE bool operator==(const static_array<T, N> &lhs,
                                       const static_array<T, N> &rhs);
/// Non-equality check on two arrays
template <typename T, std::size_t N>
VECMEM_HOST_AND_DEVICE bool operator!=(const static_array<T, N> &lhs,
                                       const static_array<T, N> &rhs);

/// Get one element from a @c vecmem::static_array
template <std::size_t I, class T, std::size_t N,
          std::enable_if_t<I<N, bool> = true> VECMEM_HOST_AND_DEVICE constexpr T
              &get(static_array<T, N> &a) noexcept;
/// Get one element from a @c vecmem::static_array
template <std::size_t I, class T, std::size_t N,
          std::enable_if_t<I<N, bool> = true>
              VECMEM_HOST_AND_DEVICE constexpr const T &get(
                  const static_array<T, N> &a) noexcept;

}  // namespace vecmem

namespace std {

/// Make the @c vecmem::get functions available in the @c std namespace as well
using vecmem::get;

}  // namespace std

// Include the implementation.
#include "impl/static_array.ipp"
