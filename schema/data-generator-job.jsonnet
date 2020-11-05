// Example commands for a ddpdemo job.
//
local moo = import "moo.jsonnet";
local cmd = import "appfwk-cmd-make.jsonnet";
local datagen = import "data-generator-make.jsonnet";

[
    cmd.init([], [cmd.mspec("datagen", "DataGenerator")]) { waitms: 1000 },

    cmd.conf([cmd.mcmd("datagen", datagen.conf(10,1048576,1000,".","demo_run20201104","one-fragment-per-file"))]) { waitms: 1000 },

    cmd.start(42) { waitms: 1000 },

    cmd.stop() { waitms: 1000 },
]
