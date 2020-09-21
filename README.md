# Data Dispatcher and Packager Demo (ddpdemo)

### Hints for building and running (KAB, 08-Sep-2020):

* create a software work area
  * see https://github.com/DUNE-DAQ/appfwk/wiki/Compiling-and-running
* clone this repo
  * `cd <MyTopDir>`
  * `git clone https://github.com/DUNE-DAQ/ddpdemo.git`)
* build the software
  * `. ./setup_build_environment`
  * `./build_daq_software.sh --pkgname ddpdemo`
* run the demo
  * `. ./setup_runtime_environment`
  * `daq_application -c QueryResponseCommandFacility -j ddpdemo/test/disk_writer_demo.json`
    * use commands line `configure`, `start`, `stop`, and `quit`

To enable and view TRACE messages:
* `export TRACE_FILE=<MyTopDir>/log/<username>_dunedaq.trace`
* run the application to get the TRACE names populated in the TRACE buffer, if needed
* `tlvls` to view the available TRACE names and their currently enabled levels
* `tonM -n <TRACE_NAME> <level>` to enable a desired level (e.g. `tonM -n SimpleDiskWriter 15`)
* re-run the application to get the TRACE messages output to the TRACE buffer
* `tshow | tdelta -ct 1` to view the TRACE messages (in reverse order, with human-readable timestamps)
