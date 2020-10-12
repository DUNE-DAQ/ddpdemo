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

#include <ers/Issue.h>
#include <TRACE/trace.h>


#include "highfive/H5File.hpp"


namespace dunedaq {
namespace ddpdemo {



class HDF5DataStore : public DataStore {

public:
  explicit HDF5DataStore(const std::string name, const std::string& path, const std::string& fileName,  const std::string operationMode, std::string idx, std::string geoID) : DataStore(name) {

  ERS_INFO("Filename: " << fileName);
  ERS_INFO("Directory path: " << path );
  ERS_INFO("Operation mode: " << operationMode );

  operation_mode_ = operationMode;

  // Creating an empty HDF5 file
  if (operationMode == "one-event-per-file" ) {
    filePtr = new HighFive::File(path + "/" + fileName + "_event_" + idx + ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate);
  } else if (operationMode == "one-fragment-per-file" ) {
    filePtr = new HighFive::File(path + "/" + fileName + "_event_" + idx + "_geoID_" + geoID +  ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate);
  }


  ERS_INFO("Created HDF5 file(s).");
 

  }

  virtual void setup(const size_t eventId) {
     ERS_INFO("Setup ... " << eventId);
  }



  virtual void write(const KeyedDataBlock& dataBlock) {
    ERS_INFO("Writing data with event ID " << dataBlock.data_key.getEventID() <<
             " and geolocation ID " << dataBlock.data_key.getGeoLocation());
     
    const std::string datagroup_name = std::to_string(dataBlock.data_key.getEventID());
   
    // Check if a HDF5 group exists and if not create one
    if (!filePtr->exist(datagroup_name)) {
      filePtr->createGroup(datagroup_name);
    }
    HighFive::Group theGroup = filePtr->getGroup(datagroup_name);
    if (!theGroup.isValid()) {
      throw InvalidDataWriterError(ERS_HERE, get_name());
    } else {
      const std::string dataset_name = std::to_string(dataBlock.data_key.getGeoLocation());
      HighFive::DataSpace theDataSpace = HighFive::DataSpace({ dataBlock.data_size, 1 });
      HighFive::DataSetCreateProps dataCProps_;
      HighFive::DataSetAccessProps dataAProps_;

      auto theDataSet = theGroup.createDataSet<char>(dataset_name, theDataSpace, dataCProps_, dataAProps_);
      theDataSet.write_raw(dataBlock.getDataStart());
    }

    // AAA: how often should we flush? After every write? 
    filePtr->flush();

         
  }
  





private: 
  HDF5DataStore(const HDF5DataStore&) = delete;
  HDF5DataStore& operator=(const HDF5DataStore&) = delete;
  HDF5DataStore(HDF5DataStore&&) = delete;
  HDF5DataStore& operator=(HDF5DataStore&&) = delete;

  HighFive::File* filePtr; 
  std::string operation_mode_;

};


} // namespace ddpdemo



ERS_DECLARE_ISSUE_BASE(ddpdemo,
                       InvalidHDF5Group,
                       appfwk::GeneralDAQModuleIssue,
                       "Invalid HDF5 group.",
                       ((std::string)name),
                       ERS_EMPTY)




} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

