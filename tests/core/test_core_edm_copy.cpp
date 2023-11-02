/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Local include(s).
#include "../common/edm_simple_container.hpp"
#include "vecmem/memory/host_memory_resource.hpp"
#include "vecmem/utils/copy.hpp"

// GoogleTest include(s).
#include <gtest/gtest.h>

/// Convenience type declaration
using test_edm = vecmem::testing::simple_container;

/// Test case for the EDM code
class core_edm_copy_test : public testing::Test {

protected:
    /// Prepare a host container with some dummy data
    void prepare(test_edm::host& obj) {
        obj.resize(10);
        test_edm::count::get(obj) = 55;
        test_edm::average::get(obj) = 3.141592f;
        for (std::size_t i = 0; i < obj.size(); ++i) {
            test_edm::measurement::get(obj)[i] = 1.0f * static_cast<float>(i);
            test_edm::index::get(obj)[i] = static_cast<int>(i);
        }
    }
    /// Prepare a device container with some dummy data
    void prepare(test_edm::device& obj) {
        test_edm::count::get(obj) = 55;
        test_edm::average::get(obj) = 3.141592f;
        for (std::size_t i = 0; i < obj.capacity(); ++i) {
            const test_edm::device::size_type ii = obj.push_back_default();
            test_edm::measurement::get(obj)[ii] = 1.0f * static_cast<float>(ii);
            test_edm::index::get(obj)[ii] = static_cast<int>(ii);
        }
    }

    /// Memory resource for the test(s)
    vecmem::host_memory_resource m_resource;
    /// Helper object for the memory copies.
    vecmem::copy m_copy;

};  // class core_edm_copy_test

TEST_F(core_edm_copy_test, host_to_host) {

    // Create the "input" host container.
    test_edm::host input{m_resource};

    // Fill it with some data.
    prepare(input);

    // Create a target host container.
    test_edm::host target{m_resource};

    // Copy the data.
    m_copy(vecmem::get_data(input), target);

    // Compare the two.
    vecmem::testing::compare(input, target);
}

TEST_F(core_edm_copy_test, host_to_fixed_device) {

    // Create the "input" host container.
    test_edm::host input{m_resource};

    // Fill it with some data.
    prepare(input);

    // Create the target buffer.
    test_edm::buffer target{
        static_cast<test_edm::buffer::size_type>(input.size()), m_resource};
    m_copy.setup(target);

    // Copy the data.
    m_copy(vecmem::get_data(input), target);

    // Compare the two.
    test_edm::const_device device{target};
    vecmem::testing::compare(input, device);
}

TEST_F(core_edm_copy_test, host_to_resiable_device) {

    // Create the "input" host container.
    test_edm::host input{m_resource};

    // Fill it with some data.
    prepare(input);

    // Create the target buffer.
    test_edm::buffer target{
        static_cast<test_edm::buffer::size_type>(input.size()), m_resource,
        vecmem::data::buffer_type::resizable};
    m_copy.setup(target);

    // Copy the data.
    m_copy(vecmem::get_data(input), target);

    // Compare the two.
    test_edm::const_device device{target};
    vecmem::testing::compare(input, device);
}

TEST_F(core_edm_copy_test, device_to_host) {

    // Create the "input" device buffer.
    test_edm::buffer input{10, m_resource,
                           vecmem::data::buffer_type::resizable};
    m_copy.setup(input);

    // Fill it with some data.
    test_edm::device device{input};
    prepare(device);

    // Create the target host container.
    test_edm::host target{m_resource};

    // Copy the data.
    m_copy(input, target);

    // Compare the two.
    test_edm::const_device cdevice{input};
    vecmem::testing::compare(target, cdevice);
}

TEST_F(core_edm_copy_test, device_to_fixed_device) {

    // Create the "input" device buffer.
    test_edm::buffer input{10, m_resource,
                           vecmem::data::buffer_type::resizable};
    m_copy.setup(input);

    // Fill it with some data.
    test_edm::device device{input};
    prepare(device);

    // Create the target buffer.
    test_edm::buffer target{input.size(), m_resource,
                            vecmem::data::buffer_type::fixed_size};
    m_copy.setup(target);

    // Copy the data.
    m_copy(input, target);

    // Compare the two.
    test_edm::const_device cdevice{input};
    test_edm::const_device ctarget{target};
    vecmem::testing::compare(cdevice, ctarget);
}

TEST_F(core_edm_copy_test, device_to_resizable_device) {

    // Create the "input" device buffer.
    test_edm::buffer input{10, m_resource,
                           vecmem::data::buffer_type::resizable};
    m_copy.setup(input);

    // Fill it with some data.
    test_edm::device device{input};
    prepare(device);

    // Create the target buffer.
    test_edm::buffer target{input.capacity(), m_resource,
                            vecmem::data::buffer_type::resizable};
    m_copy.setup(target);

    // Copy the data.
    m_copy(input, target);

    // Compare the two.
    test_edm::const_device cdevice{input};
    test_edm::const_device ctarget{target};
    vecmem::testing::compare(cdevice, ctarget);
}
