#include<benchmark/benchmark.h>
#include<csv_file_reader.hpp>
#include<sstream>

auto examples_dir(){
    return std::string(EXAMPLES);
}

static void seq_fstream_read(benchmark::State& state){
    std::fstream file(examples_dir()+"business_price_index.csv");
    for(auto _ :state){
        std::string data;
        data.reserve(12000000); ////Change if testing on different file to avoid allocation time
        benchmark::DoNotOptimize(data);
        char c;
        while(file.get(c)){
            data.push_back(c);
        }
    }
    
}

static void parallel_buffered_file_reader(benchmark::State& state){
    turbo_csv::file_reader<1000000> my_reader(examples_dir()+"business_price_index.csv");
    for(auto _ : state){
        std::string data;
        data.reserve(12000000);//Change if testing on different file to avoid allocation time
        benchmark::DoNotOptimize(data);
        std::optional<std::uint8_t> c;
        while(true){
            c= my_reader.get_byte();
            if(c.has_value()){data.push_back(c.value());}
            else{break;}
        }

    }
}

static void rd_buf_fstream_read(benchmark::State& state){
    std::fstream file(examples_dir()+"business_price_index.csv");
    for(auto _:state){
        std::string data;
        data.reserve(21000000);//Change if testing on different file to avoid allocation time
        benchmark::DoNotOptimize(data);
        char c;
        std::stringstream ss;
        ss<<file.rdbuf();
        while(ss.get(c)){
            data.push_back(c);
        }
        
    }

}

BENCHMARK(seq_fstream_read)->Unit(benchmark::kMillisecond);
BENCHMARK(parallel_buffered_file_reader)->Unit(benchmark::kMillisecond);
BENCHMARK(rd_buf_fstream_read)->Unit(benchmark::kMillisecond);
