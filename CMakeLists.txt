cmake_minimum_required(VERSION 3.12)
project(ddpdemo VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

# 08-Sep-2020, KAB: the inclusion of the appfwk package brings in several other packages,
# include ERS, TRACE, and Boost
find_package(appfwk REQUIRED) 
find_package(ers REQUIRED)

##############################################################################
daq_point_build_to( src )

add_library(ddpdemo_SimpleDiskWriter_duneDAQModule src/SimpleDiskWriter.cpp)
target_link_libraries(ddpdemo_SimpleDiskWriter_duneDAQModule appfwk::appfwk)

##############################################################################
daq_point_build_to( test )

file(COPY test/disk_writer_demo.json DESTINATION test)

##############################################################################
daq_point_build_to( unittest )
daq_add_unit_test(StorageKey_test          ers::ers)
 
##############################################################################


daq_install(TARGETS ddpdemo_SimpleDiskWriter_duneDAQModule )

