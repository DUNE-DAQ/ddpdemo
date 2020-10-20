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
  const void* unowned_data_start;
  std::unique_ptr<char> owned_data_start;

  explicit KeyedDataBlock(StorageKey theKey): data_key(theKey) {}

  // 20-Oct-2020, KAB: some notes on my proposed way of handling ownership of
  // memory in this class and the code that uses it.
  //
  // When user code writes a block of data to a DataStore, my sense is that we need
  // to provide a way for it to specify a bare pointer to a block of memory.
  // This point of view affected the choice to make unowned_data_start a bare pointer.
  // Some comments on this:
  // - The reason that I believe that we need to allow for a bare pointer:
  //   It may not be possible for user code to wrap the pointer in a std::shared_ptr
  //   or a std::unique_ptr, because that could result in the memory being deleted
  //   when the smart pointer goes out of scope, or it could result in a transfer
  //   of ownership, when such a transfer is undesirable.
  // - I like the idea of making the unowned_data_start pointer point to a "const void"
  //   block of memory.  By making it "const", then this class assures users that it
  //   won't touch the data within the data block.
  // - We may want to add constructors that allow users to pass whatever they want into
  //   an instance of this class.  For example, one constructor could take a bare pointer,
  //   another take a std::unique_ptr, and a third take a shared_ptr;
  //
  // When user code reads a block of data from a DataStore, the read() method will
  // allocate the memory to hold the data.  It makes sense for the read() method to pass
  // ownership of that memory to an instance of this class so that ownership can be
  // passed back to the user code (if that is what the user code wants) or the memory
  // automatically gets cleaned up (when the KeyedDataBlock instances goes out of scope,
  // if that is what the user code wants).
  // The nicest way for this class to take ownership of the memory is for it to store
  // it in a unique_ptr.  In that way, user code could take ownership, if it wanted.
  // Unfortunately, we will need to provide a custom deleter function when we switch
  // to std::unique_ptr<void>.

  // possible future enhancments:
  // constructor that takes Key, start address, size, and flag that says whether to take ownership or not
  // constructor that takes Key, unique_ptr to start address, and size
  // constructor that takes Key, shared_ptr to start address, and size
  // method that returns a bare pointer to the start of the data block (user code does *not* take ownership)
  // method that returns a unique_ptr to the data block (usr code *does* take ownership)
  //  -> this method would need to throw an exception if called on a instance that doesn't have a unique_ptr
  //     internally
  // maybe the way that we implement all of this is through inheritance.  The base class could provide the
  //   simple (bare-pointer) access, and child classes could add ownership handling

  const void* getDataStart() const
  {
    if (owned_data_start.get() != nullptr)
    {
      return static_cast<const void*>(owned_data_start.get());
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
