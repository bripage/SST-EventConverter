//
// _basicComponent_cc_
//

#include "memToRtr.h"

using namespace SST;
using namespace SST::eventConverter;

memToRtr::memToRtr(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {
    printf("getName().c_str() Created!\n");
    const int Verbosity = params.find<int>("verbose", 0);
    out = new Output("", Verbosity, 0, Output::STDOUT);

    memLink = configureLink("memPort", new Event::Handler<memToRtr>(this, &memToRtr::handleEvent));
    endpointType = params.find<bool>("type", 0);
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// handle the init calls
void memToRtr::init(unsigned int phase){
    out->output("%s begining init phase %d\n", getName().c_str(), phase);
    out->verbose(CALL_INFO, 1, 0, "%s begining init phase %d\n", getName().c_str(), phase);
    printf("%s begining init phase %d\n", getName().c_str(), phase);

    printf("%s ending init phase %d\n", getName().c_str(), phase);
    out->output("%s ending init phase %d\n", getName().c_str(), phase);
    out->verbose(CALL_INFO, 1, 0, "%s ending init phase %d\n", getName().c_str(), phase);
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




