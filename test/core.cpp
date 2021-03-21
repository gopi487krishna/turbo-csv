#define BOOST_TEST_MODULE core_test
#include<turbo_csv.hpp>
#include<vector>
#include<string>
#include<boost/test/unit_test.hpp>

namespace turbo_csv{
    
    class file_reader{};
    
    class record{
        public:
        auto get_fields(){
            return std::vector<std::string>{"2014","Ford","Fiesta Classic","1.6"};
        }
    };

    template<typename FileReader>
    class reader{
        FileReader file_reader;
        public:
        reader(const std::string& filename){
            
        }
        record next(){
            return record();
        }

    };
}

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE(read_data_cstyle){
    //Setup data
    
    std::vector<std::string> expected_field_data{"2014","Ford","Fiesta Classic","1.6"};
    

    // Reader to parse csv_file
    turbo_csv::reader<turbo_csv::file_reader> csv_reader("car_info.csv");

    //Read first row from file
    auto row1 = csv_reader.next();
    
    //Assert
    auto actual_field_data=row1.get_fields();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        actual_field_data.begin(),actual_field_data.end(),
        expected_field_data.begin(),expected_field_data.end()
        );

}   


BOOST_AUTO_TEST_SUITE_END()