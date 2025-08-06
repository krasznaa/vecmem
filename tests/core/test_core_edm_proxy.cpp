/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2025 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Local include(s).
#include "vecmem/edm/proxy.hpp"

// GoogleTest include(s).
#include <gtest/gtest.h>

TEST(core_edm_proxy_test, construct_assign) {

    using schema = vecmem::edm::schema<vecmem::edm::type::scalar<int>,
                                       vecmem::edm::type::vector<float>>;

    int myInt = 42;
    float myFloat = 3.14f;

    // Create proxy objects from scratch.
    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::non_constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy1(myInt, myFloat);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy2(myInt, myFloat);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::non_constant,
                       vecmem::edm::details::proxy_type::standalone>
        proxy3(myInt, myFloat);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::standalone>
        proxy4(myInt, myFloat);

    // Copy construct identical proxies.
    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::non_constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy5(proxy1);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy6(proxy2);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::non_constant,
                       vecmem::edm::details::proxy_type::standalone>
        proxy7(proxy3);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::standalone>
        proxy8(proxy4);

    // Copy assign identical proxies.
    proxy1 = proxy5;
    proxy3 = proxy7;

    // Copy construct different proxies.
    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy9(proxy1);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::constant,
                       vecmem::edm::details::proxy_type::standalone>
        proxy10(proxy3);

    vecmem::edm::proxy<schema, vecmem::edm::details::proxy_domain::host,
                       vecmem::edm::details::proxy_access::non_constant,
                       vecmem::edm::details::proxy_type::reference>
        proxy11(proxy3);
}
