#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MtlLoaderTest
#include <boost/test/unit_test.hpp>

#include <MtlLoader.h>

const std::string source = R"==(
newmtl MyMaterial

Ka 0.0435 0.0435 0.0435
Kd 0.1086 0.1086 0.1086
Ks 0.0000 0.0000 0.0000

map_Ka ambient.tga
map_Kd diffuse.tga
map_Ks specular.tga
map_Ns normals.tga
)==";

BOOST_AUTO_TEST_CASE(Color_test)
{
    std::istringstream ss{source};

    MtlLoader ldr;
    ldr.load(ss);

    BOOST_CHECK(!ldr.materials().empty());

    auto mtl = ldr.materials()[0];

    BOOST_CHECK_EQUAL(mtl.name, "MyMaterial");

    BOOST_CHECK(mtl.ambient == glm::vec3(0.0435, 0.0435, 0.0435));
    BOOST_CHECK(mtl.diffuse == glm::vec3(0.1086, 0.1086, 0.1086));
    BOOST_CHECK(mtl.specular == glm::vec3(0.0, 0.0, 0.0));
}

BOOST_AUTO_TEST_CASE(Textures_test)
{
    std::istringstream ss{source};

    MtlLoader ldr;
    ldr.load(ss);

    BOOST_CHECK(!ldr.materials().empty());

    auto mtl = ldr.materials()[0];

    BOOST_CHECK_EQUAL(mtl.ambientTex, "ambient.tga");
    BOOST_CHECK_EQUAL(mtl.diffuseTex, "diffuse.tga");
    BOOST_CHECK_EQUAL(mtl.specularTex, "specular.tga");
    BOOST_CHECK_EQUAL(mtl.normalsTex, "normals.tga");
}
