#include <lib/number.h>
#include <gtest/gtest.h>
#include <tuple>

class ConvertingTestsSuite : public testing::TestWithParam<std::tuple<uint32_t, const char*, bool>> {
};

TEST_P(ConvertingTestsSuite, EqualTest) {
    uint2022_t a = from_uint(std::get<0>(GetParam()));
    uint2022_t b = from_string(std::get<1>(GetParam()));

    if(std::get<2>(GetParam()))
        ASSERT_EQ(a,b) << std::get<0>(GetParam()) << " == " << std::get<1>(GetParam());
    else
        ASSERT_NE(a,b) << std::get<0>(GetParam()) << " != " << std::get<1>(GetParam());
}

INSTANTIATE_TEST_SUITE_P(
    Group,
    ConvertingTestsSuite,
    testing::Values(
        // positive
        std::make_tuple(0, "0", true),
        std::make_tuple(239,"239", true),
        std::make_tuple(255,"255", true),
        std::make_tuple(256,"256", true),
        std::make_tuple(10000,"10000", true),
        std::make_tuple(32767,"32767", true),
        std::make_tuple(32768,"32768", true),
        std::make_tuple(65535,"65535", true),
        std::make_tuple(2147483647,"2147483647", true),

        // negative
        std::make_tuple(0, "1", false),
        std::make_tuple(32768,"32769", false),
        std::make_tuple(255,"256", false),
        std::make_tuple(256,"255", false),
        std::make_tuple(31251,"31252", false),
        std::make_tuple(2147483647,"2147483648", false)
    )
);


class OperationTestsSuite
    : public testing::TestWithParam<
        std::tuple<
            const char*, // lhs
            const char*, // rhs
            const char*, // +
            const char*, // -
            const char*  // *
        >
    > {
};

TEST_P(OperationTestsSuite, AddTest) {
    uint2022_t a = from_string(std::get<0>(GetParam()));
    uint2022_t b = from_string(std::get<1>(GetParam()));

    uint2022_t result = a + b;
    uint2022_t expected = from_string((std::get<2>(GetParam())));

    ASSERT_EQ(result, expected);
}

TEST_P(OperationTestsSuite, SubstTest) {
    uint2022_t a = from_string(std::get<0>(GetParam()));
    uint2022_t b = from_string(std::get<1>(GetParam()));

    uint2022_t result = a - b;
    uint2022_t expected = from_string((std::get<3>(GetParam())));

    ASSERT_EQ(result, expected);
}

TEST_P(OperationTestsSuite, MultTest) {
    uint2022_t a = from_string(std::get<0>(GetParam()));
    uint2022_t b = from_string(std::get<1>(GetParam()));

    uint2022_t result = a * b;
    uint2022_t expected = from_string((std::get<4>(GetParam())));

    ASSERT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(
    Group,
    OperationTestsSuite,
    testing::Values(
        std::make_tuple("1", "1", "2", "0", "1"),
        std::make_tuple("1024", "1", "1025", "1023", "1024"),
        std::make_tuple("21944496275174754733023745004748837080297570543729328280448007953824789527038691788660702798145451174453138901351488446979832735450978591612896414872982681198457994802840025058142360791167736098566050165049439180766375815715632675961171034001565824849041810386302038359368560295224574744242597208206082048", "21944496275174754733023745004748837080297570543729328280448007953824789527038691788660702798145451174453138901351488446979832735450978591612896414872982681198457994802840025058142360791167736098566050165049439180766375815715632675961171034001565824849041810386302038359368560295224574744242597208206082047", "43888992550349509466047490009497674160595141087458656560896015907649579054077383577321405596290902348906277802702976893959665470901957183225792829745965362396915989605680050116284721582335472197132100330098878361532751631431265351922342068003131649698083620772604076718737120590449149488485194416412164095", "1", "481560916771158684800786922703235625631274322714142263414417884163925873322306437689024231009526751394401758326916367106052034484602375642882110959089521812209947069992139877256008949136579813164413834190131240610432508865633901300457687591589632190325582710683886781973951695733384278544896131740867054224748076756454395514858337677898936088606855793085527087362685513722672253758379374906456654782617718453853885826647392979514488056668649233028256085733492081292756538811516237650176497225745443953723058091108343595458799712641493582783927375315617181262019553889368093356452580550001802727316570301792256"),
        std::make_tuple("405272312330606683982498447530407677486444946329741970511324085183808429621437",
                        "1",
                        "405272312330606683982498447530407677486444946329741970511324085183808429621438",
                        "405272312330606683982498447530407677486444946329741970511324085183808429621436",
                        "405272312330606683982498447530407677486444946329741970511324085183808429621437"
        ),
        std::make_tuple("405272312330606683982498447530407677486444946329741977764879002871583477858493",
                        "0",
                        "405272312330606683982498447530407677486444946329741977764879002871583477858493",
                        "405272312330606683982498447530407677486444946329741977764879002871583477858493",
                        "0"
        ),
        std::make_tuple(
            "405272312330606683982498447530407677486444946329741974138101544027695953739965",
            "3626777458843887524118528",
            "405272312330606683982498447530407677486444946329741977764879002871583477858493",
            "405272312330606683982498447530407677486444946329741970511324085183808429621437",
            "1469832487054184013178321496623041557517329857560238757278117847507488415462666081345922349701550571520"
        )
    )
);