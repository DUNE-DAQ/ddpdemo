local moo = import "moo.jsonnet";
local cmd = import "appfwk-cmd-make.jsonnet";

local ftde = {
    config_params_function(sleepms=1000) :: {
        sleep_msec_while_running: sleepms
    },
};

local qlist = {
    trigdec_from_ds: cmd.qspec("trigger_decision_from_data_selection", "StdDeQueue", 2),
    data_requests: cmd.qspec("data_requests", "StdDeQueue", 2),
    internal_trigdec_copy: cmd.qspec("trigger_decision_copy_for_bookkeeping", "StdDeQueue", 2),
    data_fragments: cmd.qspec("data_fragments", "StdDeQueue", 2),
    trigger_records: cmd.qspec("trigger_records", "StdDeQueue", 2),
};

[
    cmd.init([qlist.trigdec_from_ds, qlist.data_requests, qlist.internal_trigdec_copy,
              qlist.data_fragments, qlist.trigger_records],
             [cmd.mspec("ftde", "FakeTrigDecEmu", [
                  cmd.qinfo("trigger_decision_output_queue", qlist.trigdec_from_ds.inst, "output")]),
              cmd.mspec("frg", "FakeReqGen", [
                  cmd.qinfo("trigger_decision_input_queue", qlist.trigdec_from_ds.inst, "input"),
                  cmd.qinfo("trigger_decision_output_queue", qlist.internal_trigdec_copy.inst, "output"),
                  cmd.qinfo("data_request_output_queue", qlist.data_requests.inst, "output")]),
              cmd.mspec("fdp", "FakeDataProd", [
                  cmd.qinfo("data_request_input_queue", qlist.data_requests.inst, "input"),
                  cmd.qinfo("data_fragment_output_queue", qlist.data_fragments.inst, "output")]),
              cmd.mspec("ffr", "FakeFragRec", [
                  cmd.qinfo("trigger_decision_input_queue", qlist.internal_trigdec_copy.inst, "input"),
                  cmd.qinfo("data_fragment_input_queue", qlist.data_fragments.inst, "input"),
                  cmd.qinfo("trigger_record_output_queue", qlist.trigger_records.inst, "output")]),
              cmd.mspec("fdw", "FakeDataWriter", [
                  cmd.qinfo("trigger_record_input_queue", qlist.trigger_records.inst, "input")])]) { waitms: 1000 },

    cmd.conf([cmd.mcmd("ftde", ftde.config_params_function(1000))]) { waitms: 1000 },

    cmd.start(42) { waitms: 1000 },

    cmd.stop() { waitms: 1000 },
]
