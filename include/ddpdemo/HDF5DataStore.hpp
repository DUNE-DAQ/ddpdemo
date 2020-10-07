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
#include "ddpdemo/HDF5FileUtils.hpp"

#include <ers/Issue.h>
#include <TRACE/trace.h>

#include "highfive/H5File.hpp"
#include <boost/lexical_cast.hpp>

#include <filesystem>
#include <memory>
#include <regex>

namespace dunedaq {
namespace ddpdemo {



class HDF5DataStore : public DataStore {

public:
  explicit HDF5DataStore(const std::string name, const std::string& path, const std::string& filePattern)
    : DataStore(name)
    , directoryPath_(path)
    , filePattern_(filePattern)
  {
    ERS_INFO("Directory path: " << path );
    ERS_INFO("Filename pattern: " << filePattern);

    // Creating an empty HDF5 file
    filePtr = new HighFive::File(path + "/" + filePattern + ".hdf5", HighFive::File::OpenOrCreate | HighFive::File::Truncate);
    ERS_INFO("Created HDF5 file.");
  }

  virtual void setup(const size_t eventId) {
     ERS_INFO("Setup ... " << eventId);
  }



  virtual void write(const KeyedDataBlock& dataBlock) {
    ERS_INFO("Writing data from event ID " << dataBlock.data_key.getEventID() <<
             " and geolocation ID " << dataBlock.data_key.getGeoLocation());
  

    const std::string datagroup_name = std::to_string(dataBlock.data_key.getEventID());
   
    // Check if a HDF5 group exists and if not create one
    if (!filePtr->exist(datagroup_name)) {
      HighFive::Group theGroup = filePtr->createGroup(datagroup_name);
    } else {
      HighFive::Group theGroup = filePtr->getGroup(datagroup_name);
      //if (!theGroup.isValid()) {
      //  throw InvalidDataWriterError(ERS_HERE, get_name());
      //} else {
        const std::string dataset_name = std::to_string(dataBlock.data_key.getGeoLocation());
        HighFive::DataSpace theDataSpace = HighFive::DataSpace({ dataBlock.data_size, 1 });
        HighFive::DataSetCreateProps dataCProps_;
        HighFive::DataSetAccessProps dataAProps_;

        auto theDataSet = theGroup.createDataSet<char>(dataset_name, theDataSpace, dataCProps_, dataAProps_);
        theDataSet.write_raw(dataBlock.getDataStart());

        //}
    }

    // AAA: how often should we flush? After every write? 
    filePtr->flush();

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

      std::vector<std::string> topLevelObjects = localFilePtr->listObjectNames();
      for (auto& topLevelObject : topLevelObjects)
      {
        int eventNumber = boost::lexical_cast<int>(topLevelObject);

        // to-do: create a getAllPaths method (to get all of the paths in an HDF5 file)

        // to-do: determine real geoLocation and pass it into the Key
        StorageKey thisKey(eventNumber, "none", 0);
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

  std::string directoryPath_;
  std::string filePattern_;

  std::vector<std::string> getAllFiles_() const
  {
    // 05-Oct-2020, KAB: this method expects filename patterns of the following forms"
    // - "<constant_string>", e.g. "demo.hdf5:
    // - "<constant_string>%e<another_constant_string>", e.g. "demo_event%e.hdf5"
    // - "<constant_string>%e<another_constant_string>%g<another_constant_string>", e.g. "demo_event%e_geo%g.hdf5"

    // since this method is expected to find *all* files that match the expected pattern,
    // we can simply replace any "%e" and "%g" substrings with wildcards.
    std::string workString = filePattern_ + ".hdf5";
    workString = std::regex_replace(workString, std::regex("\\%e"), ".*");
    workString = std::regex_replace(workString, std::regex("\\%g"), ".*");
    std::regex regexSearchPattern(workString);

    std::vector<std::string> fileList;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath_))
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

