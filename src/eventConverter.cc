//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include "eventConverter.h"

using namespace SST;
using namespace SST::eventConverter;

// basicClock constructor
// - Read the parameters
// - Register the clock handler
basicClock::basicClock(ComponentId_t id, Params& params)
  : Component(id) {

  // Create a new SST output object
  out = new Output("", 1, 0, Output::STDOUT);

  // Retrieve the paramaters from the simulation script
  clockFreq  = params.find<std::string>("clockFreq", "1GHz");
  cycleCount = params.find<Cycle_t>("clockTicks", "500");

  // Tell the simulation not to end until we signal completion
  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();

  // Register the clock handler
  registerClock(clockFreq, new Clock::Handler<basicClock>(this, &basicClock::clock));

  out->output("Registering clock with frequency=%s\n", clockFreq.c_str());

  // This component prints the clock cycles & time every so often so calculate a print interval
  // based on simulation time
  printInterval = cycleCount / 10;
  if (printInterval < 1)
    printInterval = 1;
}

// basicClock destructor
basicClock::~basicClock(){
  delete out;
}

// main clock handler
bool basicClock::clock(Cycle_t cycles){

  // Print time in terms of clock cycles every `printInterval` clocks
  if( (cycles % printInterval) == 0 ){
    out->output("Clock cycles: %" PRIu64 ", SimulationCycles: %" PRIu64 ", Simulation ns: %" PRIu64 "\n",
                cycles, getCurrentSimCycle(), getCurrentSimTimeNano());
  }

  cycleCount--;

  // If the simulation exit condition has been met,
  // end the simulation
  if( cycleCount == 0 ){
    primaryComponentOKToEndSim();
    return true;
  }else{
    return false;
  }
}
