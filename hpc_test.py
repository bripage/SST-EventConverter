import os
import sst
import argparse

DEBUG_L1 = 1
DEBUG_MEM = 10
DEBUG_LEVEL = 10
MEM_SIZE = 1024*1024*1024-1

parser = argparse.ArgumentParser()
parser.add_argument("-b", "--binary", type=string, help="Binary to execute on the RISC-V cores")
parser.add_argument("-n", "--nodes", type=int, help="Number of nodes to simulate")
parser.add_argument("-s", "--sockets_per_node", type=int, help="Number of CPU sockets per node")
parser.add_argument("-t", "--tiles_per_socket", type=int, help="Number of tiles per node socket")
parser.add_argument("-c", "--cores_per_tile",type=int, help="Number of inividual cores per tile")
parser.add_argument("-v", "--verbosity", type=int, help="increase output verbosity")
parser.add_argument("-tl", "--tile_to_tile_latency", type=int, help="Set the latency  for tile-to-tile communication within a socket ")
parser.add_argument("-sl", "--socket_to_socket_latency", type=int, help="Set the latency for socket-to-socket communication within a node")
args = parser.parse_args()

BINARY = args.binary
VERBOSE = args.verbosity
NODES = args.nodes
SOCKETS_PER_NODE = args.sockets_per_node
TILES_PER_SOCKET = args.tiles_per_socket
CORES_PER_TILE = args.cores_per_tile
TILE_TO_TILE_LATENCY = args.tile_to_tile_latency
SOCKET_TO_SOCKET_LATENCY = args.socket_to_socket_latency

cores = []
core_counter = 0
l1caches = []
buses = []
bus_counter = 0
memctrls = []
links = []
link_counter = 0

for N in range(NODES):
  for S in range(SOCKETS_PER_NODE):
    for T in range(TILES_PER_SOCKET):
      bus = sst.Component("bus_n" + str(N) + "s" + str(S) + "t" + str(T), "memHierarchy.Bus")
      bus.addParams({
        "bus_frequency" : "2GHz"
      })
      buses.append(bus)

      for C in range(CORES_PER_TILE):
        core = sst.Component("cpu_n" + str(N) + "s" + str(S) + "t" + str(T) + "c" + str(C), "revcpu.RevCPU")
        core.addParams({
          "verbose" : 6,                                # Verbosity
          "numCores" : 1,                               # Number of cores
          "clock" : "2.0GHz",                           # Clock
          "memSize" : MEM_SIZE,                         # Memory size in bytes
          "machine" : "[0:RV64G]",                      # Core:Config; RV64G for core 0
          "startAddr" : "[0:0x00000000]",               # Starting address for core 0
          "memCost" : "[0:1:10]",                       # Memory loads required 1-10 cycles
          "program" : os.getenv("REV_EXE", BINARY),  # Target executable
          "enable_memH" : 1,                            # Enable memHierarchy support
          "splash" : 1                                  # Display the splash message
        })
        core.enableAllStatistics()

        # Create the RevMemCtrl subcomponent
        comp_lsq = core.setSubComponent("memory", "revcpu.RevBasicMemCtrl");
        comp_lsq.addParams({
          "verbose"         : "10",
          "clock"           : "2.0Ghz",
          "max_loads"       : 64,
          "max_stores"      : 64,
          "max_flush"       : 64,
          "max_llsc"        : 64,
          "max_readlock"    : 64,
          "max_writeunlock" : 64,
          "max_custom"      : 64,
          "ops_per_cycle"   : 64
        })
        comp_lsq.enableAllStatistics({"type":"sst.AccumulatorStatistic"})
        cores.append(core)

        iface = comp_lsq.setSubComponent("memIface", "memHierarchy.standardInterface")
        iface.addParams({
          "verbose" : VERBOSE
        })

        l1cache = sst.Component("l1cache_n" + str(N) + "s" + str(S) + "t" + str(T) + "c" + str(C), "memHierarchy.Cache")
        l1cache.addParams({
          "access_latency_cycles" : "4",
          "cache_frequency" : "2 Ghz",
          "replacement_policy" : "lru",
          "coherence_protocol" : "MESI",
          "associativity" : "4",
          "cache_line_size" : "64",
          "debug" : 1,
          "debug_level" : DEBUG_LEVEL,
          "verbose" : VERBOSE,
          "L1" : "1",
          "cache_size" : "16KiB"
        })
        l1caches.append(l1cache)

        link = sst.Link("link_L1_" + "cpun" + str(N) + "s" + str(S) + "t" + str(T) + "c" + str(C))
        link.connect( (iface, "port", "1ns"), (l1cache, "high_network_0", "1ns") )
        links.append(link)

        link = sst.Link("link_cpun" + str(N) + "s" + str(S) + "t" + str(T) + "c" + str(C) + "_L1_" + "busn" + str(N) + "s" + str(S) + "t" + str(T))
        link.connect( (l1cache, "low_network_0", "1ns"), (bus, "high_network_" + str(C), "1ns") )
        links.append(link)

        cores.append(core)
        l1caches.append(l1cache)
      #end CORE loop
      buses.append(bus)

      memctrl = sst.Component("memctrl_" + str(N) + "s" + str(S) + "t" + str(T), "memHierarchy.MemController")
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

      link = sst.Link("link_bus" + str(N) + "s" + str(S) + "t" + str(T) + "_memctrl" + str(N) + "s" + str(S) + "t" + str(T))
      link.connect( (bus, "low_network_0", "1ns"), (memctrl, "direct_link", "1ns") )
      links.append(link)

      memctrls.append(memctrl)
    # end TILE loop

  #end SOCKET loop

#end NODE loop