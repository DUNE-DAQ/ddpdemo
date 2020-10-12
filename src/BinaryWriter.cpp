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

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "BinaryWriter" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_WORK_STEPS 15

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
  nFakeEvent_ = get_config().value<size_t>("nFakeEvent", static_cast<size_t>(REASONABLE_DEFAULT_FAKEEVENT));
  nGeoLoc_ = get_config().value<size_t>("nGeoLoc", static_cast<size_t>(REASONABLE_DEFAULT_GEOLOC));
  waitBetweenSendsMsec_ = get_config().value<size_t>("waitBetweenSendsMsec", static_cast<size_t>(REASONABLE_DEFAULT_MSECBETWEENSENDS));
  io_size_ = get_config().value<size_t>("io_size", static_cast<size_t>(REASONABLE_IO_SIZE_BYTES));

  directory_path_ = get_config()["data_store_parameters"]["directory_path"].get<std::string>();
  filename_pattern_ = get_config()["data_store_parameters"]["filename"].get<std::string>();
  operation_mode_ = get_config()["data_store_parameters"]["mode"].get<std::string>();


  dataWriterVec_.resize(nFakeEvent_);

  // Initializing the HDF5 DataStore constructor
  if (operation_mode_ == "one-event-per-file" ) {
    for (size_t idx = 0; idx < nFakeEvent_; ++idx) {

      // Creating empty HDF5 file
      dataWriter_.reset(new HDF5DataStore("tempWriter", directory_path_ , 
                                          filename_pattern_ , operation_mode_,
                                          std::to_string(idx), "" ));  
      dataWriterVec_[idx].push_back(std::move(dataWriter_));
    }
  } else if (operation_mode_ == "one-fragment-per-file" ) {
    for (size_t idx=0; idx < nFakeEvent_; ++idx) {
      for (size_t geoID = 0; geoID < nGeoLoc_; ++geoID) {
        // Creating empty HDF5 file
        dataWriter_.reset(new HDF5DataStore("tempWriter", directory_path_ , 
                                            filename_pattern_ , operation_mode_,
                                            std::to_string(idx), std::to_string(geoID) ));
        dataWriterVec_[idx].push_back(std::move(dataWriter_));
      }
    }
  }
           

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
  nFakeEvent_ = REASONABLE_DEFAULT_FAKEEVENT;
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
  char* membuffer = (char*)malloc(io_size_);
  memset(membuffer, 'X', io_size_);


  TLOG(TLVL_WORK_STEPS) << get_name() << ": Generating data ";



  for (size_t idx = 0; idx < nFakeEvent_; ++idx) {
    for (size_t geoID = 0; geoID < nGeoLoc_; ++geoID) {
      // AAA: Component ID is fixed, to be changed later
      StorageKey dataKey(idx, "FELIX", geoID); 
      KeyedDataBlock dataBlock(dataKey);
      dataBlock.data_size = io_size_;

      // Copy the constant memory buffer into the dataBlock
      dataBlock.unowned_data_start = membuffer;
      
      if (operation_mode_ == "one-event-per-file" ) {
        dataWriterVec_[idx][0]->write(dataBlock);
      } else if (operation_mode_ == "one-fragment-per-file") {
        dataWriterVec_[idx][geoID]->write(dataBlock);
      }
    }
  }    




  while (running_flag.load()) {
    sleep(1);

    // AAA: for now we generate a fixed number of events and then stop
    // in the future delete the following line and add the HDF5 file 
    // generation in this section
    //running_flag = false;

  }
      

  std::ostringstream oss_summ;
  oss_summ << ": Exiting the do_work() method, generated " << nFakeEvent_
           << " fake events and successfully wrote " << nGeoLoc_  << " fragments to each event. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::BinaryWriter)
