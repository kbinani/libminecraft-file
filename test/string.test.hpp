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

    SUBCASE("Utf8FromJavaUtf8/JavaUtf8FromUtf8") {
        static auto Check = [](vector<uint8_t> const &input, u8string const &output) {
            {
                string s;
                copy(input.begin(), input.end(), back_inserter(s));
                auto u8 = String::Utf8FromJavaUtf8(s);
                CHECK(u8);
                if (u8) {
                    CHECK(*u8 == output);
                }
            }
            {
                auto ju8 = String::JavaUtf8FromUtf8(output);
                CHECK(ju8);
                if (ju8) {
                    CHECK(ju8->size() == input.size());
                    for (size_t i = 0; i < ju8->size(); i++) {
                        CHECK(*(uint8_t *)(ju8->data() + i) == input.at(i));
                    }
                }
            }
        };

        SUBCASE("case1") {
            vector<uint8_t> javaUtf8 = {
                0x7b,
                0x22,
                0x74,
                0x65,
                0x78,
                0x74,
                0x22,
                0x3a,
                0x22,
                0xe3,
                0x81,
                0xad,
                0xe3,
                0x81,
                0x93,
                0xe3,
                0x81,
                0xad,
                0xe3,
                0x81,
                0x93,
                0xed,
                0xa0,
                0xbc,
                0xed,
                0xbf,
                0x9d,
                0xe3,
                0x81,
                0x82,
                0xe3,
                0x81,
                0x84,
                0xe3,
                0x82,
                0x89,
                0xe3,
                0x82,
                0x93,
                0xe3,
                0x81,
                0xa9,
                0x22,
                0x7d,
            };
            u8string utf8 = u8R"({"text":"ねこねこ🏝あいらんど"})";
            Check(javaUtf8, utf8);
        }
        SUBCASE("case2") {
            vector<uint8_t> input = {0x22, 0xED, 0x82, 0xA4, 0xEC, 0xA6, 0x88, 0xEB, 0x82, 0x98, 0x20, 0xEC, 0x95, 0x84, 0xEC, 0x9D, 0xB4, 0x22};
            u8string expected = u8R"("키즈나 아이")";
            Check(input, expected);
        }
    }
}
