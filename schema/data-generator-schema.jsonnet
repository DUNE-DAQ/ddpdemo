local moo = import "moo.jsonnet";
local ns = "dunedaq.ddpdemo";
local s = moo.oschema.schema(ns);

local types = {
    size: s.number("Size", "u8",
                   doc="A count of very many things"),

    count : s.number("Count", "i4",
                     doc="A count of not too many things"),

    dirpath: s.string("DirectoryPath", doc="String used to specify a directory path"),

    fnprefix: s.string("FilenamePrefix", doc="String used to specify a filename prefix"),

    opmode: s.string("OperationMode", doc="String used to specify a data storage operation mode"),

#    store: s.record("DataStore", [
#        s.field("directory_path", s.string, ".",
#                doc="Path of directory where files are located"),
#        s.field("filename_prefix", s.string, "demo_run20201104",
#                doc="Filename prefix for the files on disk"),
#        s.field("mode", s.string, "one-fragment-per-file",
#                doc="The operation mode that the DataStore should use when organizing the data into files"),
#    ], doc="DataStore configuration"),

    conf: s.record("Conf", [
        s.field("geo_location_count", self.count, 4,
                doc="Number of geographic subdivisions within the detector"),
        s.field("io_size", self.size, 1048576,
                doc="Size of data buffer to generate (in bytes)"),
        s.field("sleep_msec_while_running", self.count, 1000,
                doc="Millisecs to sleep between generating data"),
        s.field("directory_path", self.dirpath, ".",
                doc="Path of directory where files are located"),
        s.field("filename_prefix", self.fnprefix, "demo_run20201104",
                doc="Filename prefix for the files on disk"),
        s.field("mode", self.opmode, "one-fragment-per-file",
                doc="The operation mode that the DataStore should use when organizing the data into files"),
#        s.field("data_store_parameters", self.store,
#                doc="Parameters that configure the DataStore instance"),
    ], doc="DataGenerator configuration"),

};

moo.oschema.sort_select(types, ns)
