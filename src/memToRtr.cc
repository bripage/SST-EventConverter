//
// _basicComponent_cc_
//

#include "memToRtr.h"

using namespace SST;
using namespace SST::eventConverter;

memToRtr::memToRtr(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {
    out = new Output("", 1, 0, Output::STDOUT);
    memLink = configureLink("memPort", new Event::Handler<memToRtr>(this, &memToRtr::handleEvent));
    endpointType = params.find<bool>("type", 0);
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
void memToRtr::send(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);

    memLink->send(mev->clone());

    delete mev;
    delete ev;
}

// memToRtr event handler
void memToRtr::handleEvent(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    adjacentSubComp->send(mev->clone()); // use rtrToMem's send method to hand off the memory event
}




