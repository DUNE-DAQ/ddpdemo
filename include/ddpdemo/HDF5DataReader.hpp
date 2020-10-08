#ifndef DDPDEMO_INCLUDE_DDPDEMO_HDF5DATAREADER_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_HDF5DATAREADER_HPP_

/**
 * @file HDF5DataReader.hpp
 *
 * An implementation of the HDF5DataReader class that simply reads
 * data but it does not store it.  Obviously, this class is just
 * for demonstration and testing purposes.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ddpdemo/DataStore.hpp"

#include <ers/Issue.h>
#include <TRACE/trace.h>


//#include "H5Cpp.h"
#include "highfive/H5File.hpp"
#include "highfive/H5DataSet.hpp"
#include "highfive/H5Easy.hpp"


namespace dunedaq {
namespace ddpdemo {


class HDF5DataReader : public DataStore {

public:
  explicit HDF5DataReader(const std::string name, const std::string& path, const std::string& fileName) : DataStore(name) {

  ERS_INFO("Directory path: " << path );
  ERS_INFO("Filename: " << fileName);

  filePtr = new HighFive::File(path + fileName, HighFive::File::ReadOnly);
  ERS_INFO("Opened HDF5 file in ReadOnly mode");


  }
  virtual void setup(const std::string path, const std::string fileName) {
    ERS_INFO("Setting up ... " << path + fileName);
  }



  virtual void write(const KeyedDataBlock& dataBlock) { // this is here just as a place holder and in order to use DataStore as a class 
    ERS_INFO("Screen print data from event ID " << dataBlock.data_key.getEventID() <<
             ", which has size of " << dataBlock.data_size );
  }


  virtual KeyedDataBlock read(const StorageKey& key) {
    ERS_INFO("reading data block from event ID " << key.getEventID() <<
             ", detector ID " << key.getDetectorID() << ", geoLocation "<< key.getGeoLocation());
  
    const std::string groupName = std::to_string(key.getEventID());
    const std::string detectorID = key.getDetectorID();
    const std::string datasetName = std::to_string(key.getGeoLocation());
    const std::string keyFragment = groupName+':'+detectorID+':'+datasetName;
    KeyedDataBlock dataBlock(key) ; 
    if(!filePtr->exist(groupName)) {
       throw InvalidDataReaderError(ERS_HERE, get_name());
    } else {
      HighFive::Group theGroup = filePtr->getGroup(groupName);
       if (!theGroup.isValid()) {
          throw InvalidDataReaderError(ERS_HERE, get_name());
       } else {
          try {  // to determine if the dataset exists in the group
             HighFive::DataSet theDataSet = theGroup.getDataSet( datasetName );
             ERS_INFO("reading fragment " <<keyFragment<<  ", with storage size "<<theDataSet.getStorageSize());
             dataBlock.data_size = theDataSet.getStorageSize();
             //             dataBlock.owned_data_start = theDataSet.getStorageSize();
             
          }
          catch( HighFive::DataSetException const& ) {
             ERS_INFO("HDF5DataSet "<< datasetName << " not found.");
          }
       }
    }
    return dataBlock;
  }




private: 
  HDF5DataReader(const HDF5DataReader&) = delete;
  HDF5DataReader& operator=(const HDF5DataReader&) = delete;
  HDF5DataReader(HDF5DataReader&&) = delete;
  HDF5DataReader& operator=(HDF5DataReader&&) = delete;

  HighFive::File* filePtr; 

};


} // namespace ddpdemo
} // namespace dunedaq
  // Operator function

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

