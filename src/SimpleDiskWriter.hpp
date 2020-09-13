/**
 * @file SimpleDiskWriter.hpp
 *
 * SimpleDiskWriter is a simple DAQModule implementation that
 * periodically writes data to an HDF5 file on disk.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_SRC_SIMPLEDISKWRITER_HPP_
#define DDPDEMO_SRC_SIMPLEDISKWRITER_HPP_

#include "ddpdemo/DataStore.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace ddpdemo {

/**
 * @brief SimpleDiskWriter creates fake events writes
 * them to an HDF5 file..
 */
class SimpleDiskWriter : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief SimpleDiskWriter Constructor
   * @param name Instance name for this SimpleDiskWriter instance
   */
  explicit SimpleDiskWriter(const std::string& name);

  SimpleDiskWriter(const SimpleDiskWriter&) =
    delete; ///< SimpleDiskWriter is not copy-constructible
  SimpleDiskWriter& operator=(const SimpleDiskWriter&) =
    delete; ///< SimpleDiskWriter is not copy-assignable
  SimpleDiskWriter(SimpleDiskWriter&&) =
    delete; ///< SimpleDiskWriter is not move-constructible
  SimpleDiskWriter& operator=(SimpleDiskWriter&&) =
    delete; ///< SimpleDiskWriter is not move-assignable

  void init() override;

private:
  // Commands
  void do_configure(const std::vector<std::string>& args);
  void do_start(const std::vector<std::string>& args);
  void do_stop(const std::vector<std::string>& args);
  void do_unconfigure(const std::vector<std::string>& args);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  void do_work(std::atomic<bool>&);

  // Configuration defaults
  const size_t REASONABLE_DEFAULT_INTSPERFAKEEVENT = 4;
  const size_t REASONABLE_DEFAULT_MSECBETWEENSENDS = 1000;

  // Configuration
  size_t nIntsPerFakeEvent_ = REASONABLE_DEFAULT_INTSPERFAKEEVENT;
  size_t waitBetweenSendsMsec_ = REASONABLE_DEFAULT_MSECBETWEENSENDS;

  // Workers
  std::unique_ptr<DataStore> dataWriter_;
};
} // namespace ddpdemo

ERS_DECLARE_ISSUE_BASE(ddpdemo,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

} // namespace dunedaq

#endif // DDPDEMO_SRC_SIMPLEDISKWRITER_HPP_
