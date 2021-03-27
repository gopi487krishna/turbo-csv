#define BOOST_TEST_MODULE dialect_test

#include<dialect.hpp>
#include<boost/test/unit_test.hpp>


using namespace turbo_csv;

BOOST_AUTO_TEST_SUITE(dialect_methods)

BOOST_AUTO_TEST_CASE(record_seperator){

    BOOST_REQUIRE_EQUAL('\n',dialect::get_recordseperator());
    BOOST_ASSERT(dialect::is_recordseperator('\n'));
}

BOOST_AUTO_TEST_CASE(field_seperator){

    BOOST_REQUIRE_EQUAL(',',dialect::get_fieldseperator());
    BOOST_ASSERT(dialect::is_fieldseperator(','));
}

BOOST_AUTO_TEST_CASE(ignore_character){

    std::vector<char> expected_ignore_characters={'\t'};
    auto& actual_ignore_characters=dialect::get_ignore_characters();

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_ignore_characters.begin(),expected_ignore_characters.end(),
    actual_ignore_characters.begin(),actual_ignore_characters.end()
    );

    BOOST_ASSERT(dialect::is_ignorecharacter('\t'));

}

BOOST_AUTO_TEST_CASE(escape_character){

    BOOST_REQUIRE_EQUAL('\"',dialect::get_escapecharacter());
    BOOST_ASSERT(dialect::is_escapecharacter('\"'));
}



BOOST_AUTO_TEST_SUITE_END()