/**
 * @file DataTransferModule.hpp
 *
 * DataTransferModule is a simple module that transfers data from
 * one DataStore to another.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_SRC_DATATRANSFERMODULE_HPP_
#define DDPDEMO_SRC_DATATRANSFERMODULE_HPP_

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
 * @brief DataTransferModule transfers data from one DataStore to another.
 */
class DataTransferModule : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief DataTransferModule Constructor
   * @param name Instance name for this DataTransferModule instance
   */
  explicit DataTransferModule(const std::string& name);

  DataTransferModule(const DataTransferModule&) = delete;            ///< DataTransferModule is not copy-constructible
  DataTransferModule& operator=(const DataTransferModule&) = delete; ///< DataTransferModule is not copy-assignable
  DataTransferModule(DataTransferModule&&) = delete;                 ///< DataTransferModule is not move-constructible
  DataTransferModule& operator=(DataTransferModule&&) = delete;      ///< DataTransferModule is not move-assignable

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
  const size_t REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING = 1000;

  // Configuration
  size_t sleepMsecWhileRunning_ = REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING;

  // Workers
  std::unique_ptr<DataStore> inputDataStore_;
  std::unique_ptr<DataStore> outputDataStore_;
};
} // namespace ddpdemo

ERS_DECLARE_ISSUE_BASE(ddpdemo,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

} // namespace dunedaq

#endif // DDPDEMO_SRC_DATATRANSFERMODULE_HPP_
