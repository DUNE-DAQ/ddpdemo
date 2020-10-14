#ifndef DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

/**
 * @file HDF5DataStore.hpp
 *
 * An implementation of the DataStore interface that uses the 
 * highFive library to create objects in HDF5 Groups and datasets 
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ddpdemo/DataStore.hpp"
#include "ddpdemo/HDF5KeyTranslator.hpp"
#include "appfwk/DAQModule.hpp"

#include <ers/Issue.h>
#include <TRACE/trace.h>


#include "highfive/H5File.hpp"


namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(ddpdemo,
                       InvalidHDF5Group,
                       appfwk::GeneralDAQModuleIssue,
                       "Invalid HDF5 group.",
                       ((std::string)name),
                       ERS_EMPTY)

namespace ddpdemo {



class HDF5DataStore : public DataStore {

public:
  explicit HDF5DataStore(const std::string name, const std::string& path, const std::string& fileName,  const std::string& operationMode) : DataStore(name) {
	
  ERS_INFO("Filename prefix: " << fileName);
  ERS_INFO("Directory path: " << path );
  ERS_INFO("Operation mode: " << operationMode );


  fileName_ = fileName;
  path_ = path;
  operation_mode_ = operationMode;


  }

  virtual void setup(const size_t eventId) {
     ERS_INFO("Setup ... " << eventId);
  }
  
  virtual KeyedDataBlock read(const StorageKey& key) {
     ERS_INFO("going to read data block from eventID/geoLocationID " << HDF5KeyTranslator::getPathString(key));

     const std::string groupName = std::to_string(key.getEventID());
     //    const std::string detectorID = key.getDetectorID();
     const std::string datasetName = std::to_string(key.getGeoLocation());
     KeyedDataBlock dataBlock(key);

     if(!filePtr->exist(groupName)) {
        throw InvalidHDF5Group(ERS_HERE, get_name());
     } else {
        HighFive::Group theGroup = filePtr->getGroup(groupName);
        if (!theGroup.isValid()) {
           throw InvalidHDF5Group(ERS_HERE, get_name());
        } else {
           try {  // to determine if the dataset exists in the group and copy it to membuffer
              HighFive::DataSet theDataSet = theGroup.getDataSet( datasetName );
              dataBlock.data_size = theDataSet.getStorageSize();
              HighFive::DataSpace thedataSpace = theDataSet.getSpace();
              char* membuffer = (char*)malloc(dataBlock.data_size);
              theDataSet.read( membuffer);
              dataBlock.unowned_data_start = membuffer;
              ERS_INFO("finished reading eventID/geoLocationID " <<HDF5KeyTranslator::getPathString(key)<<  ", with storage size "<<theDataSet.getStorageSize());

           }
           catch( HighFive::DataSetException const& ) {
              ERS_INFO("HDF5DataSet "<< datasetName << " not found.");
           }
        }
     }
     return dataBlock;
  }






  virtual void write(const KeyedDataBlock& dataBlock) {

    size_t idx = dataBlock.data_key.getEventID();
    size_t geoID =dataBlock.data_key.getGeoLocation();
 

    // Creating empty HDF5 file
    // AAA: to be changed with open/read/write implementation from Carlos
    if (operation_mode_ == "one-event-per-file" ) {
      filePtr = new HighFive::File(path_ + "/" + fileName_ + "_event_" + std::to_string(idx) + ".hdf5", HighFive::File::OpenOrCreate);

    } else if (operation_mode_ == "one-fragment-per-file" ) {

      filePtr = new HighFive::File(path_ + "/" + fileName_ + "_event_" + std::to_string(idx) + "_geoID_" + std::to_string(geoID) +  ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate);

    } else if (operation_mode_ == "all-per-file" ) {    

      filePtr = new HighFive::File(path_ + "/" + fileName_ + "_all_events" +  ".hdf5", HighFive::File::OpenOrCreate);    
    }

    ERS_INFO("Created HDF5 file(s).");



    ERS_INFO("Writing data with event ID " << dataBlock.data_key.getEventID() <<
             " and geolocation ID " << dataBlock.data_key.getGeoLocation());
         

    const std::string datagroup_name = std::to_string(idx);


    // Check if a HDF5 group exists and if not create one
    if (!filePtr->exist(datagroup_name)) {
      filePtr->createGroup(datagroup_name);
    }
    HighFive::Group theGroup = filePtr->getGroup(datagroup_name);


    if (!theGroup.isValid()) {
      throw InvalidHDF5Group(ERS_HERE, get_name());
    } else {
      const std::string dataset_name = std::to_string(geoID);
      HighFive::DataSpace theDataSpace = HighFive::DataSpace({ dataBlock.data_size, 1 });
      HighFive::DataSetCreateProps dataCProps_;
      HighFive::DataSetAccessProps dataAProps_;

      auto theDataSet = theGroup.createDataSet<char>(dataset_name, theDataSpace, dataCProps_, dataAProps_);

      theDataSet.write_raw(dataBlock.getDataStart());
      
    }

    filePtr->flush();

    delete filePtr;
   
  }
  





private: 
  HDF5DataStore(const HDF5DataStore&) = delete;
  HDF5DataStore& operator=(const HDF5DataStore&) = delete;
  HDF5DataStore(HDF5DataStore&&) = delete;
  HDF5DataStore& operator=(HDF5DataStore&&) = delete;

  HighFive::File* filePtr; 

  std::string path_;
  std::string fileName_;
  std::string operation_mode_;




};


} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

