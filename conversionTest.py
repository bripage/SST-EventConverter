#
# conversionTest.py
#

import sst
from sst.merlin import *
#from sst.memHierarchy import *

DEBUG_L1 = 0
DEBUG_MEM = 0
DEBUG_LEVEL = 10
VERBOSE = 10
MEM_SIZE = 1024*1024*1024-1

sst.setProgramOption("timebase", "1 ps")

#-- set the `stop-at` option
sst.setProgramOption("stop-at", "10s")

#
# Create CPU and Memory Traffic Generator
#
cpu = sst.Component("cpu", "miranda.BaseCPU")
cpu.addParams({
  "verbose" : 0,
  "clock" : "1GHz",
  "printStats" : 1
})
gen = cpu.setSubComponent("generator", "miranda.Stencil3DBenchGenerator")
gen.addParams({
  "verbose" : 0,
  "nx" : 30,
  "ny" : 20,
  "nz" : 10
})

iface = cpu.setSubComponent("memory", "memHierarchy.standardInterface")

#
# Create MemHierarchy Components
#
l1_cache = sst.Component("l1_cache", "memHierarchy.Cache")
l1_cache.addParams({
  "access_latency_cycles" : "2",
  "cache_frequency" : "2GHz",
  "replacement_policy" : "lru",
  "coherence_protocol" : "MESI",
  "associativity" : "4",
  "cache_line_size" : "64",
  "prefetcher" : "cassini.StridePrefetcher",
  "debug" : "1",
  "l1" : "1",
  "cache_size" : "16KB"
})
l1_cache.enableAllStatistics({"type":"sst.AccumulatorStatistic"})


#
# Create Event Converters
#
eventConverter1 = sst.Component("eventConverter1", "eventConverter.eventConverter")

eventConverter1_rtrif = eventConverter1.setSubComponent("rtriface", "merlin.linkcontrol")
eventConverter1_rtrif.addParams({
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "1GB/s"
})

#
# Create  Routers
#
router1 = sst.Component("router1", "merlin.hr_router")
router1.setSubComponent("topology", "merlin.mesh")
router1.addParams({
  "id" : 0,
  "xbar_bw" : "10GB/s",
  "flit_size" : "512B",
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "1GB/s",
  "num_ports" : 4,
  "mesh.local_ports" : 2,
  "mesh.shape" : "1",
  "mesh.width" : "1",
  "debug" : 1
})


#
# Create Links
#
# CPU --- L1Cache --- Converter --- Rtr1 --- Rtr2 --- Converter --- Bus --- MemCtrl
#
cpu_l1_link = sst.Link("cpu_l1_link")
cpu_l1_link.connect((iface, "port", "10ps"),(l1_cache, "high_network_0", "10ps"))



link_eConv_rtr1 = sst.Link("link_eConv_rtr1")
link_eConv_rtr1.connect((eventConverter1_rtrif, "rtr_port", "1ps"),(router1, "port3", "1ps"))


sst.setStatisticLoadLevel(10)
sst.setStatisticOutput("sst.statOutputConsole")
sst.enableAllStatisticsForComponentType("merlin.hr_router")