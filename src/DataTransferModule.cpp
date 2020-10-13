/**
 * @file DataTransferModule.cpp DataTransferModule class implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "DataTransferModule.hpp"
#include "ddpdemo/KeyedDataBlock.hpp"
#include "ddpdemo/HDF5DataStore.hpp"

#include <ers/ers.h>
#include <TRACE/trace.h>

#include <chrono>
#include <cstdlib>
#include <thread>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "DataTransferModule" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_WORK_STEPS 15

namespace dunedaq {
namespace ddpdemo {

DataTransferModule::DataTransferModule(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&DataTransferModule::do_work, this, std::placeholders::_1))
{
  register_command("configure", &DataTransferModule::do_configure);
  register_command("start",  &DataTransferModule::do_start);
  register_command("stop",  &DataTransferModule::do_stop);
  register_command("unconfigure",  &DataTransferModule::do_unconfigure);
}

void DataTransferModule::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
DataTransferModule::do_configure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
  sleepMsecWhileRunning_ = get_config().value<size_t>("sleepMsecWhileRunning", static_cast<size_t>(REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING));

  // need to fetch the input and output DataStore parameters from the JSON...

  inputDataStore_.reset(new HDF5DataStore("input", inpath_, input_filename_pattern, input_filemode));
  outputDataStore_.reset(new HDF5DataStore("output", out_path, output_filename_pattern, output_filemode));

  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
}

void
DataTransferModule::do_start(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
DataTransferModule::do_stop(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

void
DataTransferModule::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
  sleepMsecWhileRunning_ = REASONABLE_DEFAULT_SLEEPMSECWHILERUNNING;
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
}

void
DataTransferModule::do_work(std::atomic<bool>& running_flag)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";

  std::vector<StorageKey> keyList = inputDataStore_->getAllExistingKeys();
  for (auto& key : keyList)
  {
    KeyedDataBlock dataBlock = inputDataStore_->read(key);
    outputDataStore_->write(dataBlock);
  }

  while (running_flag.load()) {
    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of sleep while waiting for run Stop";
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepMsecWhileRunning_));
    TLOG(TLVL_WORK_STEPS) << get_name() << ": End of sleep while waiting for run Stop";
  }

  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::DataTransferModule)