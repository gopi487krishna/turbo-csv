#define BOOST_TEST_MODULE file_reader_test
#include <fstream>
#include <default_file_reader.hpp>
#include <boost/test/unit_test.hpp>

class file_reader{
    std::fstream file;

public:
    file_reader(const std::string &filename){
        //file.open(filename, std::ios::in);
    }
    bool is_open(){
        return true;
    }
};

BOOST_AUTO_TEST_SUITE(file_reader_methods)

BOOST_AUTO_TEST_CASE(is_open){
    file_reader my_reader("cars.csv");

    BOOST_REQUIRE(my_reader.is_open());
}

BOOST_AUTO_TEST_SUITE_END()