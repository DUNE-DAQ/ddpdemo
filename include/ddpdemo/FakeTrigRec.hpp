/**
 * @file FakeTrigRec.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_INCLUDE_DDPDEMO_FAKETRIGREC_HPP_
#define DDPDEMO_INCLUDE_DDPDEMO_FAKETRIGREC_HPP_

#include <cstdint>
#include <memory>

namespace dunedaq {
namespace ddpdemo {

/**
 * @brief comment
 */
struct FakeTrigRec
{
public:
  int32_t identifier;
};

} // namespace ddpdemo
} // namespace dunedaq

#endif // DDPDEMO_INCLUDE_DDPDEMO_FAKETRIGREC_HPP_
