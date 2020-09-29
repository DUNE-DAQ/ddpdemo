/**
 * @file KeyDataBlock_test.cxx KeyDataBlock class Unit Tests
 *
 * The tests here primarily confirm that KeyDataBlock stored key components 
 * are what we expect them to be
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "ddpdemo/KeyedDataBlock.hpp"

#define BOOST_TEST_MODULE KeyedDataBlock_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace {

  int generatedCount = 0 ;
  int fakeDataValue = 10 ;
  size_t nIntsPerFakeEvent = 4 ;
  std::vector<int> theFakeEvent(nIntsPerFakeEvent) ;
  //  std::unique_ptr<char> fakeData_start = std::make_unique<char>(&theFakeEvent[0]);  //< the unique ptr to the start of data address 

} // namespace ""


BOOST_AUTO_TEST_SUITE(KeyDataBlock_test)

BOOST_AUTO_TEST_CASE(sanity_checks)
{

    BOOST_TEST_MESSAGE("Attempted KeyDataBlock sanity checks for fake event data and key::eventID,detectorID,geoLocation a") ;
  
  ++generatedCount;

  for(size_t index = 0; index < nIntsPerFakeEvent; ++index)
    {
      theFakeEvent[index] = fakeDataValue++;
    }
  auto fakeData_start = std::make_unique<char>(theFakeEvent[0]);  //< the unique ptr to the start of data address 
  if(fakeData_start != nullptr)
    BOOST_TEST_MESSAGE( "fakeData_start != nullptr && theFakeEvent[0] = " << theFakeEvent[0] ) ;
  dunedaq::ddpdemo::StorageKey dataKey(generatedCount, "FELIX", 101);
  dunedaq::ddpdemo::KeyedDataBlock dataBlock(dataKey);
  dataBlock.data_size = theFakeEvent.size() * sizeof(int);
  dataBlock.owned_data_start = std::move(fakeData_start) ; //< the owned_data_start pointer of the dataBlock
   
  if(dataBlock.getDataStart() != nullptr)
    BOOST_TEST_MESSAGE( "dataBlock.getDataStart() != nullptr && (int)dataBlock.getDataStart()[0] = " << (int)dataBlock.getDataStart()[0] ) ;


  BOOST_CHECK_EQUAL((int)dataBlock.getDataStart()[0], theFakeEvent[0] ) ;

}

BOOST_AUTO_TEST_SUITE_END()

