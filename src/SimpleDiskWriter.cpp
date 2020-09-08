/**
 * @file SimpleDiskWriter.cpp SimpleDiskWriter class implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "SimpleDiskWriter.hpp"

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
  nIntsPerList_ = get_config().value<size_t>("nIntsPerList", static_cast<size_t>(REASONABLE_DEFAULT_INTSPERLIST));
  waitBetweenSendsMsec_ = get_config().value<size_t>("waitBetweenSendsMsec", static_cast<size_t>(REASONABLE_DEFAULT_MSECBETWEENSENDS));
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
  nIntsPerList_ = REASONABLE_DEFAULT_INTSPERLIST;
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
  int fakeDataValue = 1;

  while (running_flag.load()) {
    TLOG(TLVL_WORK_STEPS) << get_name() << ": Creating list of length " << nIntsPerList_;
    std::vector<int> theList(nIntsPerList_);

    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of fill loop";
    for (size_t idx = 0; idx < nIntsPerList_; ++idx)
    {
      theList[idx] = fakeDataValue++;
    }
    ++generatedCount;
    std::ostringstream oss_prog;
    oss_prog << "Generated list #" << generatedCount << " with contents " << theList
             << " and size " << theList.size() << ". ";
    ers::debug(ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));

    // Here is where we will eventually write the data out to disk
    // ++writtenCount;

    TLOG(TLVL_WORK_STEPS) << get_name() << ": Start of sleep between sends";
    std::this_thread::sleep_for(std::chrono::milliseconds(waitBetweenSendsMsec_));
    TLOG(TLVL_WORK_STEPS) << get_name() << ": End of do_work loop";
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting the do_work() method, generated " << generatedCount
           << " lists and successfully wrote " << writtenCount << " of them to disk. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace ddpdemo 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ddpdemo::SimpleDiskWriter)
