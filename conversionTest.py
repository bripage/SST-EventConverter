#
# conversionTest.py
#

import os
import sst
from sst.merlin import *
from sst.memHierarchy import *

DEBUG_L1 = 0
DEBUG_MEM = 0
DEBUG_LEVEL = 10
VERBOSE = 2
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
l1_cache = sst.Component("l1_cache", "memHiearchy.Cache")
l1_cache.addParams({
  "access_latency_cycles" : "2",
  "cache_frequency" : "2GHz",
  "replacement_policy" : "lru",
  "coherence_protocol" : "MESI",
  "associativity" : "4",
  "cache_line_size" : "64",
  "prefetcher" : "cassini.StridePrefetcher",
  "debug" : "0",
  "l1" : "1",
  "cache_size" : "16KB"
})
l1_cache.enableAllStatistics({"type":"sst.AccumulatorStatistic"})

bus = sst.Component("bus", "memHiearchy.Bus")

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
cpu_evConv = sst.Component("cpu_evConv", "eventConverter.memRtrtConverter")
cpu_evConv_mem = cpu_evConv.setSubComponent("memory", "eventConverter.memToRtr")
cpu_evConv_rtr = cpu_evConv.setSubComponent("router", "eventConverter.rtrToMem")
cpu_evConv_rtr_iFace = cpu_evConv_rtr.setSubComponent("iface", "merlin.SimpleNetwork")

bus_evConv = sst.Component("bus_evConv", "eventConverter.memRtrtConverter")
bus_evConv_mem = bus_evConv.setSubComponent("memory", "eventConverter.memToRtr")
bus_evConv_rtr = bus_evConv.setSubComponent("router", "eventConverter.rtrToMem")
bus_evConv_rtr_iFace = bus_evConv_rtr.setSubComponent("iface", "merlin.SimpleNetwork")

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
  "num_ports" : 2,
  "mesh.local_ports" : 1,
  "mesh.shape" : "2x1",
  "mesh.width" : "1"
})

router2 = sst.Component("router2", "merlin.hr_router")
router2.setSubComponent("topology", "merlin.mesh")
router2.addParams({
  "id" : 0,
  "xbar_bw" : "10GB/s",
  "flit_size" : "512B",
  "input_buf_size" : "512B",
  "output_buf_size" : "512B",
  "link_bw" : "1GB/s",
  "num_ports" : 2,
  "mesh.local_ports" : 1,
  "mesh.shape" : "2x1",
  "mesh.width" : "1"
})


#
# Create Links
#
# CPU --- L1Cache --- Converter --- Rtr1 --- Rtr2 --- Converter --- Bus --- MemCtrl
#
cpu_l1_link = sst.Link("cpu_l1_link")
cpu_l1_link.connect((iface,"port", "10ps"),(l1_cache, "high_network_0", "10ps"))

link_cpu_evConv = sst.Link("link_cpu_evConv")
link_cpu_evConv.connect((l1_cache, "low_network_0", "0ps"),(cpu_evConv, "memPort", "0ps"))

link_cpuConv_rtr1 = sst.Link("link_cpuConv_rtr1")
link_cpuConv_rtr1.connect((cpu_evConv_rtr_iFace, "port", "0ps"),(router1, "port1", "0ps"))

link_routers = sst.Link("link_routers")
link_routers.connect((router1, "port0", "100ps"),(router2, "port0", "100ps"))

link_rtr2_bus = sst.Link("link_rtr2_bus")
link_rtr2_bus.connect((router2, "port1", "0ps"), (bus_evConv_rtr_iFace, "port", "0ps"),)

link_bus_evConv = sst.Link("link_bus_evConv")
link_bus_evConv.connect((bus, "high_network_0", "0ps"),(bus_evConv, "memPort", "0ps"))

link_bus_memctrl = sst.Link("link_bus_memctrl")
link_bus_memctrl.connect((bus, "low_network_0", "10ps"),(memctrl, "direct_link", "50ps"))
