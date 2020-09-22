/**
 * @file DataStore.hpp
 *
 * This is the interface for storing and retrieving data from
 * various storage systems.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

// 09-Sep-2020, KAB: the initial version of this class was based on the
// Queue interface from the appfwk repo.

#ifndef DDPDEMO_INCLUDE_DDPDEMO_DATASTORE_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_DATASTORE_HPP_

#include "ddpdemo/KeyedDataBlock.hpp"
#include "appfwk/NamedObject.hpp"

#include <ers/Issue.h>

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace ddpdemo {

/**
 * @brief comment
 */
class DataStore : public appfwk::NamedObject
{
public:

  /**
   * @brief DataStore Constructor
   * @param name Name of the DataStore instance
   */
  explicit DataStore(const std::string& name)
    : appfwk::NamedObject(name)
  {}

  /**
   * @brief Writes the specified data payload into the DataStore.
   * @param dataBlock Data block to write.
   */
  virtual void write(const KeyedDataBlock& dataBlock) = 0;

  // Ideas for future work...
  //virtual void write(const std::vector<KeyedDataBlock>& dataBlockList) = 0;
  //virtual KeyedDataBlock read(const StorageKey& key) = 0;
  //virtual std::vector<KeyedDataBlock> read(const std::vector<StorageKey>& key) = 0;

private:
  DataStore(const DataStore&) = delete;
  DataStore& operator=(const DataStore&) = delete;
  DataStore(DataStore&&) = default;
  DataStore& operator=(DataStore&&) = default;
};

} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_DATASTORE_HPP_
