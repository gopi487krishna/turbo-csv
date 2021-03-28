#define BOOST_TEST_MODULE reader_test

#include<turbo_csv.hpp>
#include<csv_file_reader.hpp>
#include<dialect.hpp>
#include<boost/test/unit_test.hpp>


auto get_examples_dir() {
    return std::string(EXAMPLES);
}

BOOST_AUTO_TEST_SUITE(methods_reader)


BOOST_AUTO_TEST_CASE(return_next_row) {
    turbo_csv::reader csv_reader(get_examples_dir() + "cars.csv");

    auto first_record = csv_reader.next();

    BOOST_REQUIRE_EQUAL(2014, first_record.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Ford", first_record[1]);
    BOOST_REQUIRE_EQUAL("Fiesta Classic", first_record[2]);
    BOOST_REQUIRE_EQUAL("1.6", first_record[3]);

    auto second_record = csv_reader.next();
    BOOST_REQUIRE_EQUAL(2020, second_record.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Maruti Suzuki", second_record[1]);
    BOOST_REQUIRE_EQUAL("Brezza", second_record[2]);
    BOOST_REQUIRE_EQUAL("1.6", second_record[3]);


}

BOOST_AUTO_TEST_CASE(range_based_for) {
    turbo_csv::reader csv_reader(get_examples_dir() + "cars.csv");

    std::vector<std::string> expected_rec0{ "2014","Ford","Fiesta Classic","1.6" };
    std::vector<std::string> expected_rec1{ "2020","Maruti Suzuki","Brezza","1.6" };

    std::vector<std::vector<std::string_view>> records;
    records.resize(2);

    int count = 0;
    for (auto& rec : csv_reader) {
        records[count++] = rec.get_fields();
    }

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_rec0.begin(), expected_rec0.end(),
    records[0].begin(), records[0].end()
    );

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_rec1.begin(), expected_rec1.end(),
        records[1].begin(), records[1].end()
    );
}


BOOST_AUTO_TEST_CASE(get_totalrecords){
    turbo_csv::reader csv_reader(get_examples_dir()+"cars.csv");
    
    BOOST_REQUIRE_EQUAL(2,csv_reader.get_totalrecords());
}


BOOST_AUTO_TEST_CASE(subscript_op_numeric_index) {
    turbo_csv::reader csv_reader(get_examples_dir() + "cars.csv");
    std::vector<std::string> expected_record_fields{ "2020","Maruti Suzuki","Brezza","1.6" };

    auto& rec1 = csv_reader[1];
    auto& rec1_fields = rec1.get_fields();

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_record_fields.begin(), expected_record_fields.end(),
    rec1_fields.begin(), rec1_fields.end()
    );


}

BOOST_AUTO_TEST_CASE(active_record_count){
    turbo_csv::reader csv_reader(get_examples_dir()+"cars.csv");
    int expected_record_count=1;

    csv_reader.next(); // Force reading 1 record

    BOOST_REQUIRE_EQUAL(expected_record_count,csv_reader.get_active_recordcount());
}

BOOST_AUTO_TEST_CASE(get_column){
    turbo_csv::reader csv_reader(get_examples_dir()+"cars.csv");
    
    std::vector<int> expected_col0_items={2014,2020};

    auto actual_column0_items=csv_reader.get_column<int>(0);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_col0_items.begin(),expected_col0_items.end(),
     actual_column0_items.begin(),actual_column0_items.end()
    );
}

BOOST_AUTO_TEST_CASE(get_index_of_column){
    turbo_csv::reader csv_reader(get_examples_dir()+"business-price-index.csv",true);

    BOOST_REQUIRE_EQUAL(7,csv_reader.get_indexof("Series_title_1"));

}



BOOST_AUTO_TEST_SUITE_END()


// Same regression tests for experimental reader. These will be removed/replaced in future
BOOST_AUTO_TEST_SUITE(methods_experimental_reader)


BOOST_AUTO_TEST_CASE(return_next_row) {
    turbo_csv::experimental_reader csv_reader(get_examples_dir() + "cars.csv");

    auto first_record = csv_reader.next();

    BOOST_REQUIRE_EQUAL(2014, first_record.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Ford", first_record[1]);
    BOOST_REQUIRE_EQUAL("Fiesta Classic", first_record[2]);
    BOOST_REQUIRE_EQUAL("1.6", first_record[3]);

    auto second_record = csv_reader.next();
    BOOST_REQUIRE_EQUAL(2020, second_record.get_field<int>(0));
    BOOST_REQUIRE_EQUAL("Maruti Suzuki", second_record[1]);
    BOOST_REQUIRE_EQUAL("Brezza", second_record[2]);
    BOOST_REQUIRE_EQUAL("1.6", second_record[3]);


}

BOOST_AUTO_TEST_CASE(range_based_for) {
    turbo_csv::experimental_reader csv_reader(get_examples_dir() + "cars.csv");

    std::vector<std::string> expected_rec0{ "2014","Ford","Fiesta Classic","1.6" };
    std::vector<std::string> expected_rec1{ "2020","Maruti Suzuki","Brezza","1.6" };

    std::vector<std::vector<std::string_view>> records;
    records.resize(2);

    int count = 0;
    for (auto& rec : csv_reader) {
        records[count++] = rec.get_fields();
    }

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_rec0.begin(), expected_rec0.end(),
    records[0].begin(), records[0].end()
    );

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_rec1.begin(), expected_rec1.end(),
        records[1].begin(), records[1].end()
    );
}


BOOST_AUTO_TEST_CASE(get_totalrecords){
    turbo_csv::experimental_reader csv_reader(get_examples_dir()+"cars.csv");
    
    BOOST_REQUIRE_EQUAL(2,csv_reader.get_totalrecords());
}


BOOST_AUTO_TEST_CASE(subscript_op_numeric_index) {
    turbo_csv::experimental_reader csv_reader(get_examples_dir() + "cars.csv");
    std::vector<std::string> expected_record_fields{ "2020","Maruti Suzuki","Brezza","1.6" };

    auto& rec1 = csv_reader[1];
    auto& rec1_fields = rec1.get_fields();

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_record_fields.begin(), expected_record_fields.end(),
    rec1_fields.begin(), rec1_fields.end()
    );


}

BOOST_AUTO_TEST_CASE(active_record_count){
    turbo_csv::experimental_reader csv_reader(get_examples_dir()+"cars.csv");
    int expected_record_count=1;

    csv_reader.next(); // Force reading 1 record

    BOOST_REQUIRE_EQUAL(expected_record_count,csv_reader.get_active_recordcount());
}

BOOST_AUTO_TEST_CASE(get_column){
    turbo_csv::experimental_reader csv_reader(get_examples_dir()+"cars.csv");
    
    std::vector<int> expected_col0_items={2014,2020};

    auto actual_column0_items=csv_reader.get_column<int>(0);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(expected_col0_items.begin(),expected_col0_items.end(),
     actual_column0_items.begin(),actual_column0_items.end()
    );
}

BOOST_AUTO_TEST_CASE(get_index_of_column){
    turbo_csv::experimental_reader csv_reader(get_examples_dir()+"business-price-index.csv",true);

    BOOST_REQUIRE_EQUAL(7,csv_reader.get_indexof("Series_title_1"));

}



BOOST_AUTO_TEST_SUITE_END()


