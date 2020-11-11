cmake_minimum_required(VERSION 3.12)
project(ddpdemo VERSION 0.2.1)

daq_setup_environment()

# 08-Sep-2020, KAB: the inclusion of the appfwk package brings in several other packages,
# include ERS, TRACE, and Boost
find_package(appfwk REQUIRED)
find_package(HighFive REQUIRED)


##############################################################################
daq_add_library( StorageKey.cpp 
                 LINK_LIBRARIES 
		 ers::ers HighFive appfwk stdc++fs ) 

daq_add_plugin( HDF5DataStore      duneDataStore LINK_LIBRARIES ers::ers appfwk HighFive )
daq_add_plugin( TrashCanDataStore  duneDataStore LINK_LIBRARIES ers::ers appfwk  )

daq_add_plugin( DataGenerator      duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ddpdemo )  
daq_add_plugin( DataTransferModule duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ddpdemo ) 
daq_add_plugin( GetAllKeysTest     duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ddpdemo ) 
daq_add_plugin( SimpleDiskReader   duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ddpdemo ) 
daq_add_plugin( SimpleDiskWriter   duneDAQModule LINK_LIBRARIES ers::ers appfwk HighFive ddpdemo ) 


##############################################################################
#daq_add_unit_test( StorageKey_test          LINK_LIBRARIES ddpdemo ers::ers )
#daq_add_unit_test( KeyedDataBlock_test      LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers )
#daq_add_unit_test( HDF5KeyTranslator_test   LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers )
#daq_add_unit_test( HDF5FileUtils_test       LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs )
#daq_add_unit_test( HDF5Write_test           LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs )
#daq_add_unit_test( HDF5Read_test            LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs )
#daq_add_unit_test( HDF5GetAllKeys_test      LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs )
#daq_add_unit_test( HDF5Combiner_test        LINK_LIBRARIES appfwk::appfwk ddpdemo ers::ers HighFive stdc++fs)

##############################################################################

daq_install()
