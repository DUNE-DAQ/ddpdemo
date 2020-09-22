#ifndef DDPDEMO_INCLUDE_DDPDEMO_TRASHCANDATASTORE_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_TRASHCANDATASTORE_HPP_

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

#include <iomanip>

namespace dunedaq {
namespace ddpdemo {

class TrashCanDataStore : public DataStore
{
public:

  explicit TrashCanDataStore(const std::string& name)
    : DataStore(name)
  {}

  virtual void write(const KeyedDataBlock& dataBlock)
  {
    const char* dataPtr = dataBlock.getDataStart();
    ERS_INFO("Throwing away the data from event ID " << dataBlock.data_key.getEventID() <<
             ", which has size of " << dataBlock.data_size << " bytes, and the following data " <<
             "in the first few bytes: 0x" << std::hex << std::setfill('0') << std::setw(2) <<
             ((int)dataPtr[0]) << " 0x" << ((int)dataPtr[1]) << " 0x" << ((int)dataPtr[2]) <<
             " 0x" << ((int)dataPtr[3]) << std::dec);
  }

private:
  // Delete the copy and move operations
  TrashCanDataStore(const TrashCanDataStore&) = delete;
  TrashCanDataStore& operator=(const TrashCanDataStore&) = delete;
  TrashCanDataStore(TrashCanDataStore&&) = delete;
  TrashCanDataStore& operator=(TrashCanDataStore&&) = delete;
};

} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_TRASHCANDATASTORE_HPP_

// Local Variables:
// c-basic-offset: 2
// End:
