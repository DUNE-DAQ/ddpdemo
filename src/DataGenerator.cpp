/**
 * @file DataGenerator.cpp DataGenerator class implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "DataGenerator.hpp"
#include "ddpdemo/HDF5DataStore.hpp"
#include "ddpdemo/KeyedDataBlock.hpp"

#include <TRACE/trace.h>
#include <ers/ers.h>

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "DataGenerator" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10 // NOLINT
#define TLVL_WORK_STEPS 15         // NOLINT

namespace dunedaq {
namespace ddpdemo {

DataGenerator::DataGenerator(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&DataGenerator::do_work, this, std::placeholders::_1))
{
  register_command("configure", &DataGenerator::do_configure);
  register_command("start", &DataGenerator::do_start);
  register_command("stop", &DataGenerator::do_stop);
  register_command("unconfigure", &DataGenerator::do_unconfigure);
}

void
DataGenerator::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
DataGenerator::do_configure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
  nGeoLoc_ = get_config().value<size_t>("nGeoLoc", static_cast<size_t>(REASONABLE_DEFAULT_GEOLOC));
  io_size_ = get_config().value<size_t>("io_size", static_cast<size_t>(REASONABLE_IO_SIZE_BYTES));
  sleepMsecWhileRunning_ =
    get_config().value<size_t>("sleepMsecWhileRunning", static_cast<size_t>(REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING));

  std::string directoryPath = get_config()["data_store_parameters"]["directory_path"].get<std::string>();
  std::string filenamePrefix = get_config()["data_store_parameters"]["filename_prefix"].get<std::string>();
  std::string operationMode = get_config()["data_store_parameters"]["mode"].get<std::string>();

  // Create the HDF5DataStore instance
  dataWriter_.reset(new HDF5DataStore("tempWriter", directoryPath, filenamePrefix, operationMode));

  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
}

void
DataGenerator::do_start(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
DataGenerator::do_stop(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

void
DataGenerator::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
  nGeoLoc_ = REASONABLE_DEFAULT_GEOLOC;
  io_size_ = REASONABLE_IO_SIZE_BYTES;
  sleepMsecWhileRunning_ = REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING;
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
}

void
DataGenerator::do_work(std::atomic<bool>& running_flag)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
  size_t writtenCount = 0;

  // ensure that we have a valid dataWriter instance
  if (dataWriter_.get() == nullptr) {
    throw InvalidDataWriterError(ERS_HERE, get_name());
  }

  // create a memory buffer
  void* membuffer = malloc(io_size_);
  memset(membuffer, 'X', io_size_);

  TLOG(TLVL_WORK_STEPS) << get_name() << ": Generating data ";

  int eventID = 1;
  while (running_flag.load()) {
    for (size_t geoID = 0; geoID < nGeoLoc_; ++geoID) {
      // AAA: Component ID is fixed, to be changed later
      StorageKey dataKey(eventID, "FELIX", geoID);
      KeyedDataBlock dataBlock(dataKey);
      dataBlock.data_size = io_size_;

      // Set the dataBlock pointer to the start of the constant memory buffer
      dataBlock.unowned_data_start = membuffer;

      dataWriter_->write(dataBlock);
      ++writtenCount;
    }
    ++eventID;

    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of sleep between sends";
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepMsecWhileRunning_));
    TLOG(TLVL_WORK_STEPS) << get_name() << ": End of do_work loop";
  }
  free(membuffer);

  std::ostringstream oss_summ;
  oss_summ << ": Exiting the do_work() method, wrote " << writtenCount << " fragments associated with " << (eventID - 1)
           << " fake events. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::DataGenerator)
