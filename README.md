# turbo-csv ![turbo-engine (1)](https://user-images.githubusercontent.com/56198900/111509789-1815c200-8773-11eb-80c2-8392475ef294.png)

## About turbo-csv

turbo_csv is a header only simple,flexible and fast csv parser written for Boost.xml competency test

## Requirements

C++20 (Currently C++20 is only required at few places in turbo_csv. This restriction will be removed in future )



## turbo csv design

![image](https://user-images.githubusercontent.com/56198900/112767684-bccab600-9035-11eb-8e34-c7b7b4a0196e.png)


The design of turbo_csv consists of 4 main components
* Reader
* Dialect
* Record
* File Reader

**Reader** : 
* Manager class reponsible for directing other components and supplying them with the appropriate data
* Responsible for providing an external interface to the user to interact with other components
* Allows the programmer to plug in custom components **(custom dialect,file_reader)** for parsing the csv files

**Dialect** :
* Specifies the dialect of csv_parsing to be used.
* Dialect allows us to set the following properties associated with csv file
  * record_seperator
  * field_seperator
  * ignore_characters (Characters to not include in record/skip them such as \t)
  * escape_character `"`
* turbo_csv allows custom dialect classes that must follow the interface

**Record** :
* Stores a record of csv file
* Does not parse the raw record data unless used once **(Lazy)**
* Provides operations on the fields of records
* Allows user to query metadata associated with the record

**File Reader**:
* Reads data from the file and supplies to the reader as per requirement
* turbo_csv allows custom FileReader classes that must follow the interface but can be implemented in any way (eg. memory_mapping technique, mutithreaded input buffer)

## Including turbo-csv in your project

As turbo-csv is a header only library making it a part of a project is just a matter of seconds

* Download the source from the releases section
* Unzip the source
* Place the include folder in some directory
* Set the compiler include path to point to that directory 

> Another way to include turbo-csv is to add the turbo-csv directory (includes,test,benchmarks) as a subdirectory in cmake

## Running Tests

In case you wanna run the bundled tests inside turbo-csv folder
```
mkdir build
cd build
cmake ../
cmake --build .
ctest
```
### Containerized Development Environment
If you want to have an exact development environment as mine while testing, there is also a .devcontainer directory along with the dockerfile supplied in turbo-csv directory. This allows vs-code to open the folder in a container ( defined by me) containing all the necessary dependencies (vcpkg,boost,cmake), vscode extensions as well as configuration settings(cmake,intellisense etc) 😲

>Note that gk487/cpp_base_image:latest currently does not have benchmark package in vcpkg. Hence you will have to install benchmark using the command `/vcpkg/vcpkg install benchmark:x64-linux`
  
## Using turbo-csv

### Creating a new reader instance

A new reader instance basically requires file path as the argument.Along with that there is also an optional `treat_first_record_as_header` parameter which allows you to treat the first record as the **header record**

```cpp
turbo_csv::reader csv_reader("path/filename.csv");
```

### Reading a single record from a file

`next()` function allows you to read the next record from the file. If there are no more records left in the file then `next()` returns `empty_record` ( this can be tested using `is_empty()` method of record)

```cpp
auto rec=csv_reader.next();
```

### Reading multiple records from a file

Mutliple records can be read from the file in two ways

* Calling `next()` repeatedly until an empty record is found
  ```cpp
  while(true){
		auto& rec=csv_reader.next();
		if(rec.is_empty()){break;}
		
	}
  ```
* Using range based for loop (Much cleaner)
  ```cpp
  for(auto& rec:csv_reader){
		
		record_count++;
	}
  ```

### Accessing Records in a random fashion

turbo_csv does allow accessing the records randomly using `[]` operator. If the has already cached the record at index then it simply returns the record, otherwise it keeps on reading the records until the record at **index** is reached.

```cpp
//getting the total number of fields in 3 row/record
csv_reader[3].get_field_count();
```

### Reading a perticular column from csv file
To read a perticular column from csv file simply call `get_column<T>()`. get_column<T>() will process and deserialize the data in parallel

```cpp
csv_reader.get_column<double>(1);
//Same as above
csv_reader.get_column<double>(csv_reader.get_indexof("Period"));

```


### Processing Records

Some of the methods are

**get_field<T>(index)**

Deserializes and returns the field data at "index" 
```cpp
rec.get_field<double>(1)
```
**get_fields()**

Returns all the field values as a vector of string views

```cpp
rec.get_fields()
```
**get_field_count()**

Returns the number of field in the record

```cpp
rec.get_field_count()
```

**get_raw_size()**

Returns the raw size of the record in bytes

```cpp
rec.get_raw_size()
```

## experimental_reader

There is also an experimental_reader in tubo_csv.hpp which supports 2way multithreaded input buffers for better performance. Its still buggy in nature.Hence it is advised only to use it for experimental purposes

**csv_file_reader.hpp** is the implementation of mulithreaded input buffering system. The code is actually buggy and the quality is quite pathetic. So please do not use experimental_reader for any other purposes than testing

## custom_dialect and custom_file_reader

The users of this library can use their own custom dialect classe for supporting a number of variations in the csv format. 

Similarily a custom file reader can also be written to support memory mapping the data


>To know more about using the library please refer to the documentation tests ( all tests are documentation tests ) in tests folder

