add_executable(reader turbo_csv.cpp)
add_executable(file_reader file_reader.cpp)
add_executable(csv_record record.cpp)

target_link_libraries(reader PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Threads::Threads)
target_link_libraries(file_reader PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Threads::Threads)
target_link_libraries(csv_record PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Threads::Threads)

add_test(turbo_csv_reader_test reader)
add_test(turbo_csv_file_reader_test file_reader)
add_test(turbo_csv_record_test csv_record)

