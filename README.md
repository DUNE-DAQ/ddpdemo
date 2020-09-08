# Dispatcher Demo (ddpdemo)

### Hints for building and running (KAB, 08-Sep-2020):

* create a software work area
  * see https://github.com/DUNE-DAQ/appfwk/wiki/Compiling-and-running
* clone this repo
  * `cd <work_area>`
  * `git clone https://github.com/DUNE-DAQ/ddpdemo.git`)
* build the software
  * `. ./setup_build_environment`
  * `./build_daq_software.sh --pkgname ddpdemo`
* run the demo
  * `. ./setup_runtime_environment`
  * `daq_application -c QueryResponseCommandFacility -j build/ddpdemo/test/disk_writer_demo.json`
    * use commands line `configure`, `start`, `stop`, and `quit`
