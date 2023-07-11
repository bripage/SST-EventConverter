//
// _basicComponent_h_
//

#ifndef _BASIC_COMPONENT_H_
#define _BASIC_COMPONENT_H_

#include <sst/core/component.h>

namespace SST {
namespace basicComponent {

class basicClock : public SST::Component
{
public:

  // Class members

  // Constructor: Components receive a unique ID and the set of parameters
  //              that were assigned in the simulation configuration script
  basicClock(SST::ComponentId_t id, SST::Params& params);

  // Destructor
  ~basicClock();

private:

  // Clock handler
  bool clock(SST::Cycle_t cycle);

  // Params
  SST::Output* out;       // SST Output object for printing, messaging, etc
  std::string clockFreq;  // Clock frequency
  Cycle_t cycleCount;     // Cycle counter
  Cycle_t printInterval;  // Interval at which to print to the console

};  // class basicClocks
}   // namespace basicComponent
}   // namespace SST

#endif
