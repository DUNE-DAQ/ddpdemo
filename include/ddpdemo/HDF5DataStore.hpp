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
  explicit HDF5DataStore(const std::string name, const std::string& path, const std::string& fileName,  const std::string operationMode, size_t event_number, size_t fragment_number) : DataStore(name) {
	
  ERS_INFO("Filename prefix: " << fileName);
  ERS_INFO("Directory path: " << path );
  ERS_INFO("Operation mode: " << operationMode );


  fileName_ = fileName;
  path_ = path;
  event_number_ = event_number;
  fragment_number_ = fragment_number;
  operation_mode_ = operationMode;


  }

  virtual void setup(const size_t eventId) {
     ERS_INFO("Setup ... " << eventId);
  }
  
  virtual KeyedDataBlock read(const StorageKey& data_key) {
     ERS_INFO("reading data block from event ID " << data_key.getEventID() <<
             ", detector ID " << data_key.getDetectorID() << ", geoLocation "<< data_key.getGeoLocation());   
     KeyedDataBlock dataBlock(data_key);
     return  dataBlock; 
  }


  virtual void prepare_write() {

    dataWriterVec_.resize(event_number_);

    if (operation_mode_ == "one-event-per-file" ) {
      for (size_t idx = 0; idx < event_number_; ++idx) {

        // Creating empty HDF5 file
        dataWriterVec_[idx].push_back(new HighFive::File(path_ + "/" + fileName_ + "_event_" + std::to_string(idx) + ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate));

      }
    } else if (operation_mode_ == "one-fragment-per-file" ) {
      for (size_t idx=0; idx < event_number_; ++idx) {
        for (size_t geoID = 0; geoID < fragment_number_; ++geoID) {

          // Creating empty HDF5 file
          dataWriterVec_[idx].push_back(new HighFive::File(path_ + "/" + fileName_ + "_event_" + std::to_string(idx) + "_geoID_" + std::to_string(geoID) +  ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate));
        }
      }
    } else if (operation_mode_ == "all-per-file" ) {    
      // Creating empty HDF5 file
      dataWriterVec_[0].push_back(new HighFive::File(path_ + "/" + fileName_ + "_all_events" +  ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate));    
    }

    ERS_INFO("Created HDF5 file(s).");


  }


  virtual void write(const KeyedDataBlock& dataBlock) {
    ERS_INFO("Writing data with event ID " << dataBlock.data_key.getEventID() <<
             " and geolocation ID " << dataBlock.data_key.getGeoLocation());

    size_t idx = dataBlock.data_key.getEventID();
    size_t geoID =dataBlock.data_key.getGeoLocation();
     


    //if (operation_mode_ == "one-event-per-file" ) {
    //  geoID = 0;
    //} else if (operation_mode_ == "all-per-file") {
    //  geoID = 0;
    //  idx = 0; 
    //}
     

    const std::string datagroup_name = std::to_string(dataBlock.data_key.getGeoLocation());
   
    // Check if a HDF5 group exists and if not create one
    if (!dataWriterVec_[idx][geoID]->exist(datagroup_name)) {
      dataWriterVec_[idx][geoID]->createGroup(datagroup_name);
    }
    HighFive::Group theGroup = dataWriterVec_[idx][geoID]->getGroup(datagroup_name);
    if (!theGroup.isValid()) {
      throw InvalidHDF5Group(ERS_HERE, get_name());
    } else {
      const std::string dataset_name = std::to_string(dataBlock.data_key.getGeoLocation());
      HighFive::DataSpace theDataSpace = HighFive::DataSpace({ dataBlock.data_size, 1 });
      HighFive::DataSetCreateProps dataCProps_;
      HighFive::DataSetAccessProps dataAProps_;

      auto theDataSet = theGroup.createDataSet<char>(dataset_name, theDataSpace, dataCProps_, dataAProps_);

      theDataSet.write_raw(dataBlock.getDataStart());
      
    }

    // AAA: how often should we flush? After every write? 
    dataWriterVec_[idx][geoID]->flush();
         
  }
  





private: 
  HDF5DataStore(const HDF5DataStore&) = delete;
  HDF5DataStore& operator=(const HDF5DataStore&) = delete;
  HDF5DataStore(HDF5DataStore&&) = delete;
  HDF5DataStore& operator=(HDF5DataStore&&) = delete;

  HighFive::File* filePtr; 

  std::string path_;
  std::string fileName_;
  size_t event_number_;
  size_t fragment_number_;
  std::string operation_mode_;


  // 2-dimensional vector for storing both the event ID and the geo ID
  std::vector<std::vector<HighFive::File*>> dataWriterVec_;


};


} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

