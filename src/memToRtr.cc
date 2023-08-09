//
// _basicComponent_cc_
//

#include "memToRtr.h"

using namespace SST;
using namespace SST::eventConverter;

memToRtr::memToRtr(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {
    printf("%s Created!\n", getName().c_str());
    const int Verbosity = params.find<int>("verbose", 0);
    out = new Output("", Verbosity, 0, Output::STDOUT);

    memIface = loadUserSubComponent<Interfaces::StandardMem>(
            "memIface", ComponentInfo::SHARE_NONE, , new StandardMem::Handler<memToRtr>(this, &memToRtr::handleEvent));

    if( !memIface ){
        output->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    }

    endpointType = params.find<bool>("type", 0);
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
void memToRtr::send(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    memIface->send(mev->clone());
    delete mev;
    delete ev;
}

// memToRtr event handler
void memToRtr::handleEvent(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    adjacentSubComp->send(mev->clone()); // use rtrToMem's send method to hand off the memory event
}

void memToRtr::init(unsigned int phase){
    out->verbose(CALL_INFO, 9, 0, "%s begining init phase %d\n", getName().c_str(), phase);
    SST::Event *ev;

    while (ev = memIface->recvUntimedData()) {
        cloneableEvent* cev = dynamic_cast<cloneableEvent*>(ev);
        if (cev) {
            out->verbose(CALL_INFO, 9, 0, "%s sending init events to router side %d\n", getName().c_str(), phase);
            adjacentSubComp->passOffInitEvents(cev->clone());
        }
    }

    out->verbose(CALL_INFO, 9, 0, "%s ending init phase %d\n", getName().c_str(), phase);
}

void memToRtr::passOffInitEvents(SST::Event* ev){
    cloneableEvent* cev = dynamic_cast<cloneableEvent*>(ev);
    if (cev) {
        memIface->send(cev->clone());
    }
    delete cev;
}