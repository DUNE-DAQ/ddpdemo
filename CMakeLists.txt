cmake_minimum_required(VERSION 3.12)
project(ddpdemo VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

# 08-Sep-2020, KAB: the inclusion of the appfwk package brings in several other packages,
# include ERS, TRACE, and Boost
find_package(appfwk REQUIRED)
find_package(HighFive REQUIRED)

##############################################################################
daq_point_build_to( src )

add_library(ddpdemo SHARED src/StorageKey.cpp)
target_link_libraries(ddpdemo ers::ers)
###add_library(ddpdemo SHARED src/StorageKey.cpp src/HDF5DataStore.cpp)
###target_link_libraries(ddpdemo ers::ers HighFive)

add_library(ddpdemo_SimpleDiskWriter_duneDAQModule src/SimpleDiskWriter.cpp)
target_link_libraries(ddpdemo_SimpleDiskWriter_duneDAQModule appfwk::appfwk ddpdemo HighFive)

add_library(ddpdemo_BinaryWriter_duneDAQModule src/BinaryWriter.cpp)
target_link_libraries(ddpdemo_BinaryWriter_duneDAQModule appfwk::appfwk ddpdemo HighFive)

add_library(ddpdemo_SimpleDiskReader_duneDAQModule src/SimpleDiskReader.cpp)
target_link_libraries(ddpdemo_SimpleDiskReader_duneDAQModule appfwk::appfwk ddpdemo HighFive)

add_library(ddpdemo_GetAllKeysTest_duneDAQModule src/GetAllKeysTest.cpp)
target_link_libraries(ddpdemo_GetAllKeysTest_duneDAQModule appfwk::appfwk ddpdemo HighFive stdc++fs)

##############################################################################
daq_point_build_to( test )

file(COPY test/disk_writer_demo.json DESTINATION test)
file(COPY test/binary_writer_demo.json DESTINATION test)
file(COPY test/disk_reader_demo.json DESTINATION test)
file(COPY test/get_all_keys_test.json DESTINATION test)

##############################################################################
daq_point_build_to( unittest )
daq_add_unit_test(HDF5FileUtils_test       appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)
daq_add_unit_test(HDF5KeyTranslator_test   appfwk::appfwk ddpdemo ers::ers)
daq_add_unit_test(StorageKey_test          ddpdemo ers::ers)

##############################################################################

daq_install(TARGETS ddpdemo
                    ddpdemo_SimpleDiskWriter_duneDAQModule
                    ddpdemo_BinaryWriter_duneDAQModule
                    ddpdemo_SimpleDiskReader_duneDAQModule
                    ddpdemo_GetAllKeysTest_duneDAQModule
)
