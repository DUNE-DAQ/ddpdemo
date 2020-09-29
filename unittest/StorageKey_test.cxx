/**
 * @file StorageKey_test.cxx StorageKey class Unit Tests
 *
 * The tests here primarily confirm that StorageKey stored key components 
 * are what we expect them to be
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ddpdemo/StorageKey.hpp"

#define BOOST_TEST_MODULE StorageKey_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>

namespace {

  constexpr uint32_t eventID=111 ; 
  const  std::string detectorID("FELIX") ; 
  constexpr uint32_t geoLocation =101 ;
  dunedaq::ddpdemo::StorageKey stk(eventID, detectorID, geoLocation) ; ///< StorageKey instance for the test

} // namespace ""


BOOST_AUTO_TEST_SUITE(StorageKey_test)

BOOST_AUTO_TEST_CASE(sanity_checks)
{

  uint32_t m_eventID=-999 ;
  std::string m_detectorID="XXXXX" ;
  uint32_t m_geoLocation=-999 ;
  BOOST_TEST_MESSAGE("Attempted StorageKey sanity checks for Key::eventID,detectorID,geoLocation") ;
  m_eventID = stk.getEventID() ;
  m_detectorID = stk.getDetectorID() ;
  m_geoLocation = stk.getGeoLocation() ;

  BOOST_CHECK_EQUAL(m_eventID, eventID ) ;
  BOOST_CHECK_EQUAL(m_detectorID, detectorID ) ;
  BOOST_CHECK_EQUAL(m_geoLocation, geoLocation ) ;
}

BOOST_AUTO_TEST_SUITE_END()

