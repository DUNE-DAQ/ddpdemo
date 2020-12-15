local moo = import "moo.jsonnet";
local cmd = import "appfwk-cmd-make.jsonnet";

local ftde = {
    config_params_function(sleepms=1000) :: {
        sleep_msec_while_running: sleepms
    },
};

# clearly, we'll want to use some sort of "for" loop to handle the N Queues into
# and out of the N FakeDataProducers...

local qlist = {
    trigdec_from_ds: cmd.qspec("trigger_decision_from_data_selection", "StdDeQueue", 2),
    data_requests_1: cmd.qspec("data_requests_1", "StdDeQueue", 2),
    data_requests_2: cmd.qspec("data_requests_2", "StdDeQueue", 2),
    data_requests_3: cmd.qspec("data_requests_3", "StdDeQueue", 2),
    internal_trigdec_copy: cmd.qspec("trigger_decision_copy_for_bookkeeping", "StdDeQueue", 2),
    data_fragments_1: cmd.qspec("data_fragments_1", "StdDeQueue", 2),
    data_fragments_2: cmd.qspec("data_fragments_2", "StdDeQueue", 2),
    data_fragments_3: cmd.qspec("data_fragments_3", "StdDeQueue", 2),
    trigger_records: cmd.qspec("trigger_records", "StdDeQueue", 2),
};

[
    cmd.init([qlist.trigdec_from_ds, qlist.internal_trigdec_copy, qlist.trigger_records,
              qlist.data_requests_1, qlist.data_requests_2, qlist.data_requests_3, 
              qlist.data_fragments_1, qlist.data_fragments_2, qlist.data_fragments_3],
             [cmd.mspec("ftde", "FakeTrigDecEmu", [
                  cmd.qinfo("trigger_decision_output_queue", qlist.trigdec_from_ds.inst, "output")]),
              cmd.mspec("frg", "FakeReqGen", [
                  cmd.qinfo("trigger_decision_input_queue", qlist.trigdec_from_ds.inst, "input"),
                  cmd.qinfo("trigger_decision_output_queue", qlist.internal_trigdec_copy.inst, "output"),
                  cmd.qinfo("data_request_1_output_queue", qlist.data_requests_1.inst, "output"),
                  cmd.qinfo("data_request_2_output_queue", qlist.data_requests_2.inst, "output"),
                  cmd.qinfo("data_request_3_output_queue", qlist.data_requests_3.inst, "output")]),
              cmd.mspec("fdp1", "FakeDataProd", [
                  cmd.qinfo("data_request_input_queue", qlist.data_requests_1.inst, "input"),
                  cmd.qinfo("data_fragment_output_queue", qlist.data_fragments_1.inst, "output")]),
              cmd.mspec("fdp2", "FakeDataProd", [
                  cmd.qinfo("data_request_input_queue", qlist.data_requests_2.inst, "input"),
                  cmd.qinfo("data_fragment_output_queue", qlist.data_fragments_2.inst, "output")]),
              cmd.mspec("fdp3", "FakeDataProd", [
                  cmd.qinfo("data_request_input_queue", qlist.data_requests_3.inst, "input"),
                  cmd.qinfo("data_fragment_output_queue", qlist.data_fragments_3.inst, "output")]),
              cmd.mspec("ffr", "FakeFragRec", [
                  cmd.qinfo("trigger_decision_input_queue", qlist.internal_trigdec_copy.inst, "input"),
                  cmd.qinfo("data_fragment_1_input_queue", qlist.data_fragments_1.inst, "input"),
                  cmd.qinfo("data_fragment_2_input_queue", qlist.data_fragments_2.inst, "input"),
                  cmd.qinfo("data_fragment_3_input_queue", qlist.data_fragments_3.inst, "input"),
                  cmd.qinfo("trigger_record_output_queue", qlist.trigger_records.inst, "output")]),
              cmd.mspec("fdw", "FakeDataWriter", [
                  cmd.qinfo("trigger_record_input_queue", qlist.trigger_records.inst, "input")])]) { waitms: 1000 },

    cmd.conf([cmd.mcmd("ftde", ftde.config_params_function(1000))]) { waitms: 1000 },

    cmd.start(42) { waitms: 1000 },

    cmd.stop() { waitms: 1000 },
]
