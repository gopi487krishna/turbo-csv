#define BOOST_TEST_MODULE file_reader_test

#include <default_file_reader.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>


auto examples_dir(){
    return std::string(EXAMPLES);
}

BOOST_AUTO_TEST_SUITE(file_reader_ctor)

BOOST_AUTO_TEST_CASE(open_file_if_it_exists){
    file_reader<> reader(examples_dir()+"cars.csv");
    BOOST_REQUIRE(reader.is_open());

}
BOOST_AUTO_TEST_CASE(throw_if_file_not_exists){

    BOOST_REQUIRE_THROW(file_reader<>(examples_dir()+"meow.csv"),std::fstream::failure);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(file_reader_methods)

BOOST_AUTO_TEST_CASE(is_open){
    file_reader<> my_reader(examples_dir()+"cars.csv");

    BOOST_REQUIRE(my_reader.is_open());
}

BOOST_AUTO_TEST_CASE(file_size){
    file_reader<> my_reader(examples_dir()+"cars.csv");

    BOOST_REQUIRE_EQUAL(my_reader.get_filesize(),58);
}


BOOST_AUTO_TEST_CASE(get_method_small_file){
   
    file_reader<> my_reader(examples_dir()+"cars.csv");
    std::string cars_data="2014,Ford,Fiesta Classic,1.6\n2020,Maruti Suzuki,Brezza,1.6";
    std::string data;

    while(true){
        auto byte= my_reader.get_byte();
        if(byte.has_value()){data.push_back(byte.value());}
        else{break;}
    }

    BOOST_REQUIRE_EQUAL(cars_data,data);
}

BOOST_AUTO_TEST_CASE(get_method_small_file_large_buffer){
   
    file_reader<2048> my_reader(examples_dir()+"cars.csv");
    std::string cars_data="2014,Ford,Fiesta Classic,1.6\n2020,Maruti Suzuki,Brezza,1.6";
    std::string data;

    while(true){
        auto byte= my_reader.get_byte();
        if(byte.has_value()){data.push_back(byte.value());}
        else{break;}
    }

    BOOST_REQUIRE_EQUAL(cars_data,data);
}

BOOST_AUTO_TEST_CASE(get_method_large_file){
    file_reader<1000000> my_reader(examples_dir()+"business_price_index.csv");
    std::fstream file(examples_dir()+"business_price_index.csv");
    std::stringstream original_data;
    std::string data;

    
    original_data<<file.rdbuf();

    while(true){
        auto byte= my_reader.get_byte();
        if(byte.has_value()){data.push_back(byte.value());}
        else{break;}
    }
    
    BOOST_REQUIRE_EQUAL(original_data.str(),data);
}


BOOST_AUTO_TEST_CASE(get_method_read_only_few_bytes){
    file_reader<5> my_reader(examples_dir()+"cars.csv");
    std::string cars_data="2014,Ford,Fiesta";
    std::string data;

    int bytes_to_read=16;

    while(bytes_to_read--){
        auto byte=my_reader.get_byte();
        if(byte.has_value()){data.push_back(byte.value());}
    }

    BOOST_REQUIRE_EQUAL(cars_data,data);
}
BOOST_AUTO_TEST_SUITE_END()