cmake_minimum_required(VERSION 3.12)
project(ddpdemo VERSION 0.2.1)

daq_setup_environment()

# 08-Sep-2020, KAB: the inclusion of the appfwk package brings in several other packages,
# include ERS, TRACE, and Boost
find_package(appfwk REQUIRED)
find_package(HighFive REQUIRED)


##############################################################################
daq_add_library( StorageKey.cpp LINK_LIBRARIES ers::ers ) 

daq_add_plugin( HDF5DataStore      duneDataStore LINK_LIBRARIES ers::ers appfwk HighFive )

daq_add_plugin( DataGenerator      duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ) 
daq_add_plugin( DataTransferModule duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ) 

#		 DataGenerator.cpp
#                 DataTransferModule.cpp 
#		 GetAllKeysTest.cpp
#                 SimpleDiskReader.cpp 
#                 SimpleDiskWriter.cpp 
#		 HDF5DataStore.cpp    ## the new CMake style don't like this to be in the include directory

#		 LINK_LIBRARIES 
#		 ers::ers 
#		 appfwk::appfwk 
#		 HighFive )

###add_library(ddpdemo SHARED src/StorageKey.cpp src/HDF5DataStore.cpp)
###target_link_libraries(ddpdemo ers::ers HighFive)

##############################################################################
#daq_point_build_to( test )

#file(COPY test/data_combiner_demo.json DESTINATION test)
#file(COPY test/data_generator_demo.json DESTINATION test)
#file(COPY test/disk_reader_demo.json DESTINATION test)
#file(COPY test/disk_writer_demo.json DESTINATION test)
#file(COPY test/get_all_keys_test.json DESTINATION test)

##############################################################################
#daq_point_build_to( unittest )
#daq_add_unit_test(StorageKey_test          ddpdemo ers::ers)
#daq_add_unit_test(KeyedDataBlock_test      appfwk::appfwk ddpdemo ers::ers)
#daq_add_unit_test(HDF5KeyTranslator_test   appfwk::appfwk ddpdemo ers::ers)
#daq_add_unit_test(HDF5FileUtils_test       appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)
#daq_add_unit_test(HDF5Write_test           appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)
#daq_add_unit_test(HDF5Read_test            appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)
#daq_add_unit_test(HDF5GetAllKeys_test      appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)
#daq_add_unit_test(HDF5Combiner_test        appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)

##############################################################################

#daq_install(TARGETS ddpdemo
#                    ddpdemo_DataGenerator_duneDAQModule
#                    ddpdemo_DataTransferModule_duneDAQModule
#                    ddpdemo_GetAllKeysTest_duneDAQModule
#                    ddpdemo_SimpleDiskReader_duneDAQModule
#                    ddpdemo_SimpleDiskWriter_duneDAQModule
# )
