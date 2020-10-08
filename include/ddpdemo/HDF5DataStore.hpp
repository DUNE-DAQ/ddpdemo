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

  ERS_INFO("Directory path: " << path );
  ERS_INFO("Filename: " << fileName);

  // Creating an empty HDF5 file
  filePtr = new HighFive::File(path + fileName, HighFive::File::OpenOrCreate | HighFive::File::Truncate);
  ERS_INFO("Created HDF5 file.");
 

  }

  virtual void setup(const std::string path, const std::string fileName) {
     ERS_INFO("Setting up ... " << path + fileName);
  }
  
  virtual KeyedDataBlock read(const StorageKey& data_key) {
     ERS_INFO("reading data block from event ID " << data_key.getEventID() <<
             ", detector ID " << data_key.getDetectorID() << ", geoLocation "<< data_key.getGeoLocation());   
     KeyedDataBlock dataBlock(data_key);
     return  dataBlock; 
  }


  virtual void write(const KeyedDataBlock& dataBlock) {
    ERS_INFO("Writing data from event ID " << dataBlock.data_key.getEventID() <<
             ", which has size of " << dataBlock.data_size );
  
   // HighFive::DataSpace theDataSpace = HighFive::DataSpace::From(dataBlock.getDataStart());
    const std::string dataset_name = std::to_string(dataBlock.data_key.getEventID());
    HighFive::DataSpace theDataSpace = HighFive::DataSpace({ dataBlock.data_size, 1 });
    HighFive::DataSetCreateProps dataCProps_;
    HighFive::DataSetAccessProps dataAProps_;

    auto theDataSet = filePtr->createDataSet<char>(dataset_name, theDataSpace, dataCProps_, dataAProps_);
    theDataSet.write_raw(dataBlock.getDataStart());

    // AAA: how often should we flush? After every write? 
    //filePtr->flush();
    
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

