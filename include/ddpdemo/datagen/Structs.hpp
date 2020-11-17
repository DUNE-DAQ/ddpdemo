/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::ddpdemo::datagen.
 */
#ifndef DUNEDAQ_DDPDEMO_DATAGEN_STRUCTS_HPP
#define DUNEDAQ_DDPDEMO_DATAGEN_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::ddpdemo::datagen {

    // @brief A count of not too many things
    using Count = int32_t;

    // @brief A count of very many things
    using Size = uint64_t;

    // @brief String used to specify a directory path
    using DirectoryPath = std::string;

    // @brief String used to specify a filename prefix
    using FilenamePrefix = std::string;

    // @brief String used to specify a data storage operation mode
    using OperationMode = std::string;

    // @brief DataStore configuration
    struct DataStore {

        // @brief Path of directory where files are located
        DirectoryPath directory_path;

        // @brief Filename prefix for the files on disk
        FilenamePrefix filename_prefix;

        // @brief The operation mode that the DataStore should use when organizing the data into files
        OperationMode mode;
    };

    // @brief DataGenerator configuration
    struct Conf {

        // @brief Number of geographic subdivisions within the detector
        Count geo_location_count;

        // @brief Size of data buffer to generate (in bytes)
        Size io_size;

        // @brief Millisecs to sleep between generating data
        Count sleep_msec_while_running;

        // @brief Parameters that configure the DataStore instance
        DataStore data_store_parameters;
    };

} // namespace dunedaq::ddpdemo::datagen

#endif // DUNEDAQ_DDPDEMO_DATAGEN_STRUCTS_HPP