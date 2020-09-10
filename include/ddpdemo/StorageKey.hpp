/**
 * @file StorageKey.hpp
 *
 * StorageKey class used to identify a given block of data
 * 
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DDPDEMO_SRC_STORAGEKEY_HPP_
#define DDPDEMO_SRC_STORAGEKEY_HPP_

#include <ers/ers.h>
#include <string>


namespace dunedaq {
namespace ddpdemo {
/**
 * @brief The StorageKey class defines the container class that will give us a way 
 * to group all of the parameters that identify a given block of data 
 * 
 */
struct Key {
  Key(uint32_t eventID, std::string detectorID, uint32_t geoLocation):
     m_event_id(eventID),
     m_detector_id(detectorID),
     m_geoLocation(geoLocation){}

  uint32_t m_event_id;
  std::string m_detector_id;
  uint32_t m_geoLocation;
};

class StorageKey {

public:
  StorageKey(uint32_t eventID, std::string detectorID, uint32_t geoLocation): 
     m_key(eventID, detectorID, geoLocation){}
  ~StorageKey() {}
  

  uint32_t getEventID() const {
    return m_key.m_event_id;
  }

  std::string getDetectorID() const {
    return m_key.m_detector_id;
  }

  uint32_t getGeoLocation() const {
    return m_key.m_geoLocation;
  }

private:
  Key m_key;


};




} // namespace dddpdemo
} // namespace dunedaq

#endif // DDPDEMO_SRC_STORAGEKEY_HPP_



