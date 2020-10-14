/**
 * @file DataGenerator.hpp
 *
 * DataGenerator is a simple DAQModule implementation that
 * writes data to HDF5 file(s) on disk.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_SRC_DATAGENERATOR_HPP_
#define DDPDEMO_SRC_DATAGENERATOR_HPP_

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
 * @brief DataGenerator creates fake events writes
 * them to an HDF5 file..
 */
class DataGenerator : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief DataGenerator Constructor
   * @param name Instance name for this DataGenerator instance
   */
  explicit DataGenerator(const std::string& name);

  DataGenerator(const DataGenerator&) =
    delete; ///< DataGenerator is not copy-constructible
  DataGenerator& operator=(const DataGenerator&) =
    delete; ///< DataGenerator is not copy-assignable
  DataGenerator(DataGenerator&&) =
    delete; ///< DataGenerator is not move-constructible
  DataGenerator& operator=(DataGenerator&&) =
    delete; ///< DataGenerator is not move-assignable

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
  const size_t REASONABLE_DEFAULT_GEOLOC = 5;
  const size_t REASONABLE_DEFAULT_MSECBETWEENSENDS = 1000;
  const size_t REASONABLE_IO_SIZE_BYTES = 1024;

  const std::string DEFAULT_PATH = ".";
  const std::string DEFAULT_FILENAME = "demo";
  const std::string DEFAULT_MODE = "one-event-per-file";

  // Configuration
  size_t nGeoLoc_ = REASONABLE_DEFAULT_GEOLOC;
  size_t waitBetweenSendsMsec_ = REASONABLE_DEFAULT_MSECBETWEENSENDS;
  size_t io_size_ = REASONABLE_IO_SIZE_BYTES;

  std::string directory_path_ = DEFAULT_PATH;
  std::string filename_pattern_ = DEFAULT_FILENAME;
  std::string operation_mode_ = DEFAULT_MODE;

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

#endif // DDPDEMO_SRC_DATAGENERATOR_HPP_
