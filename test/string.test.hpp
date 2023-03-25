TEST_CASE("string") {
    using namespace mcfile;
    using namespace std;

    SUBCASE("Split") {
        SUBCASE("char") {
            string sentence = "ab\xd\xa"
                              "c"
                              "\xd"
                              "d"
                              "\xa"
                              "e";
            auto tokens = String::Split(sentence, '\xa');
            REQUIRE(tokens.size() == 3);
            CHECK(tokens[0] == "ab\xd");
            CHECK(tokens[1] == "c\xd"
                               "d");
            CHECK(tokens[2] == "e");
        }
        SUBCASE("wchar_t") {
            wstring sentence = L"ab\xd\xa"
                               "c"
                               "\xd"
                               "d"
                               "\xa"
                               "e";
            auto tokens = String::Split(sentence, L'\xa');
            REQUIRE(tokens.size() == 3);
            CHECK(tokens[0] == L"ab\xd");
            CHECK(tokens[1] == L"c\xd"
                               L"d");
            CHECK(tokens[2] == L"e");
        }
        SUBCASE("char8_t") {
            u8string sentence = u8"ab\xd\xa"
                                "c"
                                "\xd"
                                "d"
                                "\xa"
                                "e";
            auto tokens = String::Split(sentence, u8'\xa');
            REQUIRE(tokens.size() == 3);
            CHECK(tokens[0] == u8"ab\xd");
            CHECK(tokens[1] == u8"c\xd"
                               "d");
            CHECK(tokens[2] == u8"e");
        }
    }
}
