#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MtlDataTest
#include <boost/test/unit_test.hpp>

#include <MtlLoader.h>

BOOST_AUTO_TEST_CASE(Ctor_test)
{
    MaterialData mtl;

    BOOST_CHECK(mtl.ambient == glm::vec3(0.2, 0.2, 0.2));
    BOOST_CHECK(mtl.diffuse == glm::vec3(0.8, 0.8, 0.8));
    BOOST_CHECK(mtl.specular == glm::vec3(0.0, 0.0, 0.0));
    BOOST_CHECK(mtl.emission == glm::vec3(0.0, 0.0, 0.0));
    BOOST_CHECK_EQUAL(mtl.shininess, 1.0f);
}
