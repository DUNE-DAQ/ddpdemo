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
#include "ddpdemo/HDF5KeyTranslator.hpp"

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


class HDF5DataReader : public DataStore {

public:
  explicit HDF5DataReader(const std::string name, const std::string& path, const std::string& fileName) : DataStore(name) {

  ERS_INFO("Directory path: " << path );
  ERS_INFO("Filename: " << fileName);

  filePtr = new HighFive::File(path + fileName, HighFive::File::ReadOnly);
  ERS_INFO("Opened HDF5 file in ReadOnly mode");


  }
  virtual void setup(const size_t eventId) {
    ERS_INFO("Setup ... " << eventId);
  }


  virtual void write(const KeyedDataBlock& dataBlock) { // this is here just as a place holder and in order to use DataStore as a class 
    ERS_INFO("Screen print data from event ID " << dataBlock.data_key.getEventID() <<
             ", which has size of " << dataBlock.data_size );
  }


  virtual KeyedDataBlock read(const StorageKey& key) {
    ERS_INFO("going to read data block from eventID/geoLocationID " << HDF5KeyTranslator::getPathString(key));
  
    const std::string groupName = std::to_string(key.getEventID());
    //    const std::string detectorID = key.getDetectorID();
    const std::string datasetName = std::to_string(key.getGeoLocation());
    KeyedDataBlock dataBlock(key) ; 

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

  virtual std::vector<StorageKey> getAllExistingKeys() const
  {
    std::vector<StorageKey> emptyList;
    return emptyList;
  }

private: 
  HDF5DataReader(const HDF5DataReader&) = delete;
  HDF5DataReader& operator=(const HDF5DataReader&) = delete;
  HDF5DataReader(HDF5DataReader&&) = delete;
  HDF5DataReader& operator=(HDF5DataReader&&) = delete;

  HighFive::File* filePtr; 
  std::unique_ptr<char> data_start;
};


} // namespace ddpdemo
} // namespace dunedaq
  // Operator function

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

