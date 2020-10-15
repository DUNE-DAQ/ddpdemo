/**
 * @file HDF5WriteAndRead_test.cxx Application that tests and demonstrates
 * the write *and* read functionality of the HDF5DataStore class.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ddpdemo/HDF5DataStore.hpp"

#include "ers/ers.h"

#define BOOST_TEST_MODULE HDF5WriteAndRead_test // NOLINT

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

using namespace dunedaq::ddpdemo;

std::vector<std::string> getFilesMatchingPattern(const std::string& path, const std::string& pattern)
{
  std::regex regexSearchPattern(pattern);
  std::vector<std::string> fileList;
  for (const auto& entry : std::filesystem::directory_iterator(path))
  {
    if (std::regex_match(entry.path().filename().string(), regexSearchPattern))
    {
      fileList.push_back(entry.path());
    }
  }
  return fileList;
}

std::vector<std::string> deleteFilesMatchingPattern(const std::string& path, const std::string& pattern)
{
  std::regex regexSearchPattern(pattern);
  std::vector<std::string> fileList;
  for (const auto& entry : std::filesystem::directory_iterator(path))
  {
    if (std::regex_match(entry.path().filename().string(), regexSearchPattern))
    {
      if (std::filesystem::remove(entry.path()))
      {
        fileList.push_back(entry.path());
      }
    }
  }
  return fileList;
}

BOOST_AUTO_TEST_SUITE(HDF5WriteAndRead_test)

BOOST_AUTO_TEST_CASE(WriteAndReadFragmentFilesSeparateDataStores)
{
  std::string filePath(std::filesystem::temp_directory_path());;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = "demo.*event.*geoID.*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // desired Constructor arguments:
  // - name of the data store (just a descriptive name)
  // - directory path where the files should be stored and/or read from
  // - filename prefix
  // - operation mode
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore("tempWriter", filePath, "demo", "one-fragment-per-file"));


  std::vector<StorageKey> keyList; 

  // write several events, each with several fragments
  char dummyData[8];
  for (int eventID = 1; eventID <= 3; ++eventID)
  {
    for (int geoLoc = 0; geoLoc < 2; ++geoLoc)
    {
      StorageKey key(eventID, StorageKey::INVALID_DETECTORID, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.unowned_data_start = &dummyData[0];
      dataBlock.data_size = 8;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }
  dsPtr.reset();  // explicit destruction

  // check that the expected number of files was created
  std::string searchPattern = "demo.*event.*geoID.*.hdf5";
  std::vector<std::string> fileList = getFilesMatchingPattern(filePath, searchPattern);


  // create a new DataStore instance to read back the data that was written
  std::unique_ptr<HDF5DataStore> dsPtr2(new HDF5DataStore("tempReader", filePath, "demo", "one-fragment-per-file"));

  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx)
  {
    KeyedDataBlock dataBlock = dsPtr2->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.getDataSizeBytes(), 8);
  }
  dsPtr2.reset();  // explicit destruction


  // clean up the files that were created
  fileList = deleteFilesMatchingPattern(filePath, deletePattern);
}

BOOST_AUTO_TEST_CASE(WriteAndReadEventFilesSeparateDataStores)
{
  std::string filePath(std::filesystem::temp_directory_path());;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = "demo.*event.*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // desired Constructor arguments:
  // - name of the data store (just a descriptive name)
  // - directory path where the files should be stored and/or read from
  // - filename prefix
  // - file mode
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore("tempWriter", filePath, "demo", "one-event-per-file"));

  std::vector<StorageKey> keyList; 

  // write several events, each with several fragments
  char dummyData[8];
  for (int eventID = 1; eventID <= 3; ++eventID)
  {
    for (int geoLoc = 0; geoLoc < 2; ++geoLoc)
    {
      StorageKey key(eventID, StorageKey::INVALID_DETECTORID, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.unowned_data_start = &dummyData[0];
      dataBlock.data_size = 8;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }
  dsPtr.reset();  // explicit destruction

  // check that the expected number of files was created
  std::string searchPattern = "demo.*event.*.hdf5";
  std::vector<std::string> fileList = getFilesMatchingPattern(filePath, searchPattern);
  //BOOST_REQUIRE_EQUAL(fileList.size(), 6);

  // create a new DataStore instance to read back the data that was written
  std::unique_ptr<HDF5DataStore> dsPtr2(new HDF5DataStore("tempReader", filePath, "demo", "one-event-per-file"));


  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx)
  {
    KeyedDataBlock dataBlock = dsPtr2->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.getDataSizeBytes(), 8);
  }
  dsPtr2.reset();  // explicit destruction

  // clean up the files that were created
  fileList = deleteFilesMatchingPattern(filePath, deletePattern);
  BOOST_REQUIRE_EQUAL(fileList.size(), 3);
}

BOOST_AUTO_TEST_CASE(WriteAndReadFragmentFilesSameDataStore)
{
  std::string filePath(std::filesystem::temp_directory_path());;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = "demo.*event.*geoID.*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // desired Constructor arguments:
  // - name of the data store (just a descriptive name)
  // - directory path where the files should be stored and/or read from
  // - filename prefix
  // - file mode
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore("hdfStore", filePath, "demo", "one-fragment-per-file"));

  std::vector<StorageKey> keyList; 

  // write several events, each with several fragments
  char dummyData[8];
  for (int eventID = 1; eventID <= 3; ++eventID)
  {
    for (int geoLoc = 0; geoLoc < 2; ++geoLoc)
    {
      StorageKey key(eventID, StorageKey::INVALID_DETECTORID, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.unowned_data_start = &dummyData[0];
      dataBlock.data_size = 8;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }

  // check that the expected number of files was created
  std::string searchPattern = "demo.*event.*geoID.*.hdf5";
  std::vector<std::string> fileList = getFilesMatchingPattern(filePath, searchPattern);
  BOOST_REQUIRE_EQUAL(fileList.size(), 6);
  
  // Switch from writing to reading, continuing to use the same DataStore instance


  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx)
  {
    KeyedDataBlock dataBlock = dsPtr->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.getDataSizeBytes(), 8);
  }
  dsPtr.reset();  // explicit destruction

  // clean up the files that were created
  fileList = deleteFilesMatchingPattern(filePath, deletePattern);
  BOOST_REQUIRE_EQUAL(fileList.size(), 6);
}

BOOST_AUTO_TEST_CASE(WriteAndReadEventFilesSameDataStore)
{
  std::string filePath(std::filesystem::temp_directory_path());;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = "demo.*event.*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // desired Constructor arguments:
  // - name of the data store (just a descriptive name)
  // - directory path where the files should be stored and/or read from
  // - filename prefix
  // - file mode
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore("hdfStore", filePath, "demo", "one-event-per-file"));

  std::vector<StorageKey> keyList; 

  // write several events, each with several fragments
  char dummyData[10];
  for (int eventID = 1; eventID <= 3; ++eventID)
  {
    for (int geoLoc = 0; geoLoc < 2; ++geoLoc)
    {
      StorageKey key(eventID, StorageKey::INVALID_DETECTORID, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.unowned_data_start = &dummyData[0];
      dataBlock.data_size = 10;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }


  // check that the expected number of files was created
  std::string searchPattern = "demo.*event.*.hdf5";
  std::vector<std::string> fileList = getFilesMatchingPattern(filePath, searchPattern);
  //BOOST_REQUIRE_EQUAL(fileList.size(), 6);

  // Switch from writing to reading, continuing to use the same DataStore instance

  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx)
  {
    KeyedDataBlock dataBlock = dsPtr->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.getDataSizeBytes(), 10);
  }
  dsPtr.reset();  // explicit destruction

  
  // clean up the files that were created
  fileList = deleteFilesMatchingPattern(filePath, deletePattern);
  BOOST_REQUIRE_EQUAL(fileList.size(), 3);
}



BOOST_AUTO_TEST_SUITE_END()
