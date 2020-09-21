/**
 * @file SimpleDiskWriter.cpp SimpleDiskWriter class implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "SimpleDiskWriter.hpp"
#include "ddpdemo/KeyedDataBlock.hpp"
//#include "ddpdemo/TrashCanDataStore.hpp"
#include "ddpdemo/HDF5DataStore.hpp"

#include <ers/ers.h>
#include <TRACE/trace.h>

#include <chrono>
#include <cstdlib>
#include <thread>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "SimpleDiskWriter" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_WORK_STEPS 15

namespace dunedaq {
namespace ddpdemo {

SimpleDiskWriter::SimpleDiskWriter(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&SimpleDiskWriter::do_work, this, std::placeholders::_1))
{
  register_command("configure", &SimpleDiskWriter::do_configure);
  register_command("start",  &SimpleDiskWriter::do_start);
  register_command("stop",  &SimpleDiskWriter::do_stop);
  register_command("unconfigure",  &SimpleDiskWriter::do_unconfigure);
}

void SimpleDiskWriter::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
SimpleDiskWriter::do_configure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
  nIntsPerFakeEvent_ = get_config().value<size_t>("nIntsPerFakeEvent", static_cast<size_t>(REASONABLE_DEFAULT_INTSPERFAKEEVENT));
  waitBetweenSendsMsec_ = get_config().value<size_t>("waitBetweenSendsMsec", static_cast<size_t>(REASONABLE_DEFAULT_MSECBETWEENSENDS));

  directory_path_ = get_config()["data_store_parameters"]["directory_path"].get<std::string>();
  filename_pattern_ = get_config()["data_store_parameters"]["filename_pattern"].get<std::string>();

  // Initializing the HDF5 DataStore constructor
  // Creating empty HDF5 file
  dataWriter_.reset(new HDF5DataStore("tempWriter", directory_path_ , filename_pattern_));


  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
}

void
SimpleDiskWriter::do_start(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
SimpleDiskWriter::do_stop(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

void
SimpleDiskWriter::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
  nIntsPerFakeEvent_ = REASONABLE_DEFAULT_INTSPERFAKEEVENT;
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
SimpleDiskWriter::do_work(std::atomic<bool>& running_flag)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
  size_t generatedCount = 0;
  size_t writtenCount = 0;

  // AAA: Fake event of 10 KiB 
  int io_size_bytes = 10240;
  char value = 'X';
  char* data = new char[io_size_bytes];

  while (running_flag.load()) {
    TLOG(TLVL_WORK_STEPS) << get_name() << ": Creating fakeEvent of length " << nIntsPerFakeEvent_;

    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of fill loop";
    for (size_t idx = 0; idx < nIntsPerFakeEvent_; ++idx)
    {
      std::fill(data, data + io_size_bytes, value);
    }
    ++generatedCount;
    std::ostringstream oss_prog;
    oss_prog << "Generated fake event #" << generatedCount << ". ";
    ers::debug(ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));

    // Here is where we will eventually write the data out to disk
    StorageKey dataKey(generatedCount, "FELIX", 101);
    KeyedDataBlock dataBlock(dataKey);
    dataBlock.data_size = io_size_bytes * generatedCount;
    //dataBlock.unowned_data_start = reinterpret_cast<uint8_t*>(&theFakeEvent[0]);
    dataBlock.unowned_data_start = data;
    TLOG(TLVL_WORK_STEPS) << get_name() << ": size of fake event number " << dataBlock.data_key.getEventID()
                          << " is " << dataBlock.data_size << " bytes.";
    //dataWriter_->write(dataBlock);
    ++writtenCount;

    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of sleep between sends";
    std::this_thread::sleep_for(std::chrono::milliseconds(waitBetweenSendsMsec_));
    TLOG(TLVL_WORK_STEPS) << get_name() << ": End of do_work loop";
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting the do_work() method, generated " << generatedCount
           << " fake events and successfully wrote " << writtenCount << " of them to disk. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::SimpleDiskWriter)
