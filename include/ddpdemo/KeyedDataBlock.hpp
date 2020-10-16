/**
 * @file KeyedDataBlock.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_INCLUDE_DDPDEMO_KEYEDDATABLOCK_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_KEYEDDATABLOCK_HPP_

#include "ddpdemo/StorageKey.hpp"
#include <cstdint>
#include <memory>

namespace dunedaq {
namespace ddpdemo {

/**
 * @brief comment
 */
struct KeyedDataBlock
{
public:

  // These data members will be made private, at some point in time.
  StorageKey data_key;
  size_t data_size;
  const char* unowned_data_start;
  std::unique_ptr<char> owned_data_start;

  explicit KeyedDataBlock(StorageKey theKey): data_key(theKey) {}

  const char* getDataStart() const
  {
    if (owned_data_start.get() != nullptr)
    {
      return owned_data_start.get();
    }
    else
    {
      return unowned_data_start;
    }
  }

  size_t getDataSizeBytes() const
  {
    return data_size;
  }
};

} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_KEYEDDATABLOCK_HPP_
