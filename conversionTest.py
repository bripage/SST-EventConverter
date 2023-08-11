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

#bus = sst.Component("bus", "memHierarchy.Bus")
#bus.addParams({
#  "bus_frequency" : "2GHz"
#})

memctrl = sst.Component("memory", "memHierarchy.MemController")
memctrl.addParams({
  "debug" : DEBUG_MEM,
  "debug_level" : DEBUG_LEVEL,
  "clock" : "2GHz",
  "verbose" : VERBOSE,
  "addr_range_start" : 0,
  "addr_range_end" : MEM_SIZE,
  "backing" : "malloc"
})
memory = memctrl.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({
  "access_time" : "100ns",
  "mem_size" : "8GB"
})

#
# Create Event Converters
#
eventConverter1 = sst.Component("eventConverter1", "eventConverter.eventConverter")
eventConverter1_memif = eventConverter1.setSubComponent("memiface", "memHierarchy.standardInterface")
eventConverter1.addParams({
  "verbose": VERBOSE
})
eventConverter1_rtrif = eventConverter1.setSubComponent("rtriface", "merlin.linkcontrol")
eventConverter1_rtrif.addParams({
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "1GB/s"
})

eventConverter2 = sst.Component("eventConverter2", "eventConverter.eventConverter")
eventConverter2_memif = eventConverter2.setSubComponent("memiface", "memHierarchy.standardInterface")
eventConverter2.addParams({
  "verbose": VERBOSE
})
eventConverter2_rtrif = eventConverter2.setSubComponent("rtriface", "merlin.linkcontrol")
eventConverter2_rtrif.addParams({
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

router2 = sst.Component("router2", "merlin.hr_router")
router2.setSubComponent("topology", "merlin.mesh")
router2.addParams({
  "id" : 1,
  "xbar_bw" : "10GB/s",
  "flit_size" : "512B",
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "1GB/s",
  "num_ports" : 4,
  "mesh.local_ports" : 1,
  "mesh.shape" : "2",
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

eventConverter1 = sst.Link("link_cpu_evConv")
eventConverter1.connect((l1_cache, "low_network_0", "1ps"),(eventConverter1_memif, "port", "1ps"))

link_eConv_rtr1 = sst.Link("link_eConv_rtr1")
link_eConv_rtr1.connect((eventConverter1_rtrif, "rtr_port", "1ps"),(router1, "port3", "1ps"))

link_routers = sst.Link("link_routers")
link_routers.connect((router1, "port0", "100ps"),(router2, "port0", "100ps"))

link_rtr2_eConv = sst.Link("link_rtr2_eConv")
link_rtr2_eConv.connect((router2, "port3", "1ps"), (eventConverter2_rtrif, "rtr_port", "1ps"))

link_mem_evConv = sst.Link("link_mem_evConv")
link_mem_evConv.connect((memctrl, "direct_link", "1ps"),(eventConverter2_memif, "port", "1ps"))

sst.setStatisticLoadLevel(10)
sst.setStatisticOutput("sst.statOutputConsole")
sst.enableAllStatisticsForComponentType("merlin.hr_router")