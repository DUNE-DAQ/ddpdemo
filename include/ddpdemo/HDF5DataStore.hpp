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
#include "ddpdemo/HDF5FileUtils.hpp"
#include "ddpdemo/HDF5KeyTranslator.hpp"
#include "appfwk/DAQModule.hpp"

#include <ers/Issue.h>
#include <TRACE/trace.h>

#include "highfive/H5File.hpp"
#include <boost/lexical_cast.hpp>

#include <filesystem>
#include <memory>
#include <regex>

namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(ddpdemo,
                       InvalidHDF5Group,
                       appfwk::GeneralDAQModuleIssue,
                       "The HDF5 Group associated with name \"" << groupName << "\" is invalid. (file = " << filename << ")",
                       ((std::string)name),
                       ((std::string)groupName)((std::string)filename))

namespace ddpdemo {



class HDF5DataStore : public DataStore {

public:
  explicit HDF5DataStore(const std::string& name, const std::string& path, const std::string& fileName,  const std::string& operationMode) : DataStore(name) {
	
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
  
  virtual KeyedDataBlock read(const StorageKey& data_key) {
     ERS_INFO("reading data block from event ID " << data_key.getEventID() <<
             ", detector ID " << data_key.getDetectorID() << ", geoLocation "<< data_key.getGeoLocation());   
     KeyedDataBlock dataBlock(data_key);
     return  dataBlock; 
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
      throw InvalidHDF5Group(ERS_HERE, get_name(), datagroup_name, filePtr->getName());
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
  


  virtual std::vector<StorageKey> getAllExistingKeys() const
  {
    std::vector<StorageKey> keyList;
    std::vector<std::string> fileList = getAllFiles_();

    for (auto& filename : fileList)
    {
      std::unique_ptr<HighFive::File> localFilePtr(new HighFive::File(filename, HighFive::File::ReadOnly));
      TLOG(TLVL_DEBUG) << get_name() << ": Opened HDF5 file " << filename;

      std::vector<std::string> pathList = HDF5FileUtils::getAllDataSetPaths(*localFilePtr);
      ERS_INFO("Path list has element count: " << pathList.size());

      for (auto& path : pathList)
      {
        StorageKey thisKey(0, "", 0);
        thisKey = HDF5KeyTranslator::getKeyFromString(path);
        keyList.push_back(thisKey);
      }

      localFilePtr.reset();  // explicit destruction
    }

    return keyList;
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

  std::vector<std::string> getAllFiles_() const
  {
    // 05-Oct-2020, KAB: this method expects filename patterns of the following forms"
    // - "<constant_string>", e.g. "demo.hdf5:
    // - "<constant_string>%e<another_constant_string>", e.g. "demo_event%e.hdf5"
    // - "<constant_string>%e<another_constant_string>%g<another_constant_string>", e.g. "demo_event%e_geo%g.hdf5"

    // since this method is expected to find *all* files that match the expected pattern,
    // we can simply replace any "%e" and "%g" substrings with wildcards.
    std::string workString = fileName_;
    if (operation_mode_ == "one-event-per-file")
    {
      workString += "_event_.*.hdf5";  // fix this to use \d
    }
    else if (operation_mode_ == "one-fragment-per-file")
    {
      workString += "_event_.*_geoID_.*.hdf5";
    }
    else
    {
      workString += "_all_events.hdf5";
    }
    std::regex regexSearchPattern(workString);

    std::vector<std::string> fileList;
    for (const auto& entry : std::filesystem::directory_iterator(path_))
    {
      TLOG(TLVL_DEBUG) << "Directory element: " << entry.path().string();
      if (std::regex_match(entry.path().filename().string(), regexSearchPattern))
      {
        TLOG(TLVL_DEBUG) << "Matching directory element: " << entry.path().string();
        fileList.push_back(entry.path());
      }
    }
    return fileList;
  }

};


} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_HDF5DATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:

