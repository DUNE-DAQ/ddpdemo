#ifndef DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

/**
 * @file TrashCanDataStore.hpp
 *
 * An implementation of the DataStore interface that simply throws
 * data away instead of storing it.  Obviously, this class is just
 * for demonstration and testing purposes.
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
  explicit HDF5DataStore(const std::string name, const std::string& path, const std::string& fileName) : DataStore(name) {

  ERS_INFO("Filename: " << fileName);
  ERS_INFO("Directory path: " << path );

  // Creating an empty HDF5 file
  filePtr = new HighFive::File(path + "/" + fileName + ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate);
  ERS_INFO("Created HDF5 file.");
 

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


  virtual void write(const KeyedDataBlock& dataBlock) {
    ERS_INFO("Writing data from event ID " << dataBlock.data_key.getEventID() <<
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

};


} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

