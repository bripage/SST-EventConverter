//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include "memToRtr.h"
#include <sst/core/sst_types.h>

using namespace SST;
using namespace SST::eventConverter;

memToRtr::memToRtr(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {
    out = new Output("", 1, 0, Output::STDOUT);
    memLink = configureLink("memPort", "1 ps", new Event::Handler<memToRtr>(this, &memToRtr::handleEvent));
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
void send(SST::Interfaces::SimpleNetwork::Request* req, SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);

    memLink->send(mev);

    delete ev;
    delete mev;
    delete req;
}

// memToRtr event handler
bool memToRtr::handleEvent(SST::Event* ev){
    adjacentSubComp->send(ev); // use rtrToMem's send method to hand off the memory event
    return true;
}




