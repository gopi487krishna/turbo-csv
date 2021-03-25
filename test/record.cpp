#define BOOST_TEST_MODULE record_test

#include<record.hpp>
#include<boost/test/unit_test.hpp>



BOOST_AUTO_TEST_SUITE(record_methods)

BOOST_AUTO_TEST_CASE(get_raw_size){

    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(28,rec.get_raw_size());

}
BOOST_AUTO_TEST_CASE(get_field_count){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,1.6");
    
    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
}
BOOST_AUTO_TEST_CASE(get_field_from_index){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(std::string("2014"),rec[0]);
    BOOST_REQUIRE_EQUAL(std::string("Ford"),rec[1]);
    BOOST_REQUIRE_EQUAL(std::string("Fiesta Classic"),rec[2]);
    BOOST_REQUIRE_EQUAL(std::string("1.6"),rec[3]);
    
}
BOOST_AUTO_TEST_CASE(get_converted_field_value){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(2014,rec.get_field<int>(0));
    BOOST_REQUIRE_EQUAL(1.6,rec.get_field<double>(3));
}
BOOST_AUTO_TEST_CASE(get_converted_field_value_trimmed_spaces){
    turbo_csv::record<> rec("2014   ,Ford, Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(2014,rec.get_field<int>(0,true));
}
BOOST_AUTO_TEST_CASE(get_converted_field_value_trimmed_double_quotes){
    turbo_csv::record<> rec("\"2014\"   ,Ford, Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(2014,rec.get_field<int>(0,true,true));
}



BOOST_AUTO_TEST_SUITE_END()

// RFC 4180 along with other common conventions as well
BOOST_AUTO_TEST_SUITE(rfc4180_conformance_testing)

BOOST_AUTO_TEST_CASE(normal_record){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,1.6");

    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
    BOOST_REQUIRE_EQUAL(2014,rec.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Ford",rec.get_field<std::string>(1));
    BOOST_REQUIRE_EQUAL("Fiesta Classic",rec[2]);
    BOOST_REQUIRE_EQUAL(1.6,rec.get_field<double>(3));
}


BOOST_AUTO_TEST_CASE(embedded_comma_dbl_quote){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,\"Super, fast car\"",{25,42});

    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
    BOOST_REQUIRE_EQUAL("Super, fast car",rec.get_field<std::string>(3,true,true));
}

BOOST_AUTO_TEST_CASE(spaces_preserved){
    turbo_csv::record<> rec("2014, \"Ford\" ,1.6",{6,11});

    BOOST_REQUIRE_EQUAL(" \"Ford\" ",rec[1]);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(compatiblity_feature_support)

BOOST_AUTO_TEST_CASE(seperator_escape_on_dbl_quote){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,\"Super, fast car\"",{25,42});

    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
    BOOST_REQUIRE_EQUAL("Super, fast car",rec.get_field<std::string>(3,true,true));
}
BOOST_AUTO_TEST_CASE(empty_field){
    turbo_csv::record<> rec("2014,Ford,Fiesta Classic,");

    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
    BOOST_REQUIRE_EQUAL("",rec.get_field<std::string>(3,true,true));
}
BOOST_AUTO_TEST_CASE(empty_field_with_dbl_quote){
     turbo_csv::record<> rec("2014,Ford,Fiesta Classic,\"\"",{25,26});

    BOOST_REQUIRE_EQUAL(4,rec.get_field_count());
    BOOST_REQUIRE_EQUAL("\"\"",rec.get_field<std::string>(3,true,false));
}
BOOST_AUTO_TEST_SUITE_END()
