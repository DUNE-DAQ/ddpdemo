/**
 * @file BinaryWriter.cpp BinaryWriter class implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "BinaryWriter.hpp"
#include "ddpdemo/KeyedDataBlock.hpp"
#include "ddpdemo/HDF5DataStore.hpp"

#include <ers/ers.h>
#include <TRACE/trace.h>

#include <chrono>
#include <cstdlib>
#include <thread>
#include <string>
#include <vector>
/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "BinaryWriter" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10 // NOLINT
#define TLVL_WORK_STEPS 15 // NOLINT

namespace dunedaq {
namespace ddpdemo {

BinaryWriter::BinaryWriter(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&BinaryWriter::do_work, this, std::placeholders::_1))
{
  register_command("configure", &BinaryWriter::do_configure);
  register_command("start",  &BinaryWriter::do_start);
  register_command("stop",  &BinaryWriter::do_stop);
  register_command("unconfigure",  &BinaryWriter::do_unconfigure);
}

void BinaryWriter::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
BinaryWriter::do_configure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
  nGeoLoc_ = get_config().value<size_t>("nGeoLoc", static_cast<size_t>(REASONABLE_DEFAULT_GEOLOC));
  waitBetweenSendsMsec_ = get_config().value<size_t>("waitBetweenSendsMsec", static_cast<size_t>(REASONABLE_DEFAULT_MSECBETWEENSENDS));
  io_size_ = get_config().value<size_t>("io_size", static_cast<size_t>(REASONABLE_IO_SIZE_BYTES));

  directory_path_ = get_config()["data_store_parameters"]["directory_path"].get<std::string>();
  filename_pattern_ = get_config()["data_store_parameters"]["filename"].get<std::string>();
  operation_mode_ = get_config()["data_store_parameters"]["mode"].get<std::string>();



  // Creating empty HDF5 file
  dataWriter_.reset(new HDF5DataStore("tempWriter", directory_path_ , 
                                          filename_pattern_ , operation_mode_ ));  
        

  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
}

void
BinaryWriter::do_start(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
BinaryWriter::do_stop(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

void
BinaryWriter::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
  waitBetweenSendsMsec_ = REASONABLE_DEFAULT_MSECBETWEENSENDS;
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
}

/**
 * @brief Format a std::vector<int> to a stream
 * @param t ostream Instance
 * @param ints Vector to format
 * @return ostream Instance
 */
std::ostream&
operator<<(std::ostream& t, std::vector<int> ints)
{
  t << "{";
  bool first = true;
  for (auto& i : ints) {
    if (!first)
      t << ", ";
    first = false;
    t << i;
  }
  return t << "}";

}

void
BinaryWriter::do_work(std::atomic<bool>& running_flag)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";

  // create a memory buffer
  char* membuffer = static_cast<char*>(malloc(io_size_));
  memset(membuffer, 'X', io_size_);


  TLOG(TLVL_WORK_STEPS) << get_name() << ": Generating data ";


  
  size_t event = 0;
  while (running_flag.load()) {
    for (size_t geoID = 0; geoID < nGeoLoc_; ++geoID) {
      
      // AAA: Component ID is fixed, to be changed later
      StorageKey dataKey(event, "FELIX", geoID); 
      KeyedDataBlock dataBlock(dataKey);
      dataBlock.data_size = io_size_;

      // Copy the constant memory buffer into the dataBlock
      dataBlock.unowned_data_start = membuffer;
      dataWriter_->write(dataBlock);
     
    }
    event++;

    sleep(1);

  }
      

  std::ostringstream oss_summ;
  oss_summ << ": Exiting the do_work() method, generated " << event
           << " fake events and successfully wrote " << nGeoLoc_  << " fragments to each event. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::BinaryWriter)
