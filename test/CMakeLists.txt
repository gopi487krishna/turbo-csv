add_executable(core core.cpp)
add_executable(file_reader file_reader.cpp)
target_link_libraries(core PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Threads::Threads)
target_link_libraries(file_reader PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Threads::Threads)

add_test(turbo_csv_core_test core)
add_test(turbo_csv_file_reader_test file_reader)

