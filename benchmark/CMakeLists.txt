add_executable(read_benchmark reader_benchmark.cpp)
target_link_libraries(read_benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)
