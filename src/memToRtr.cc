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

    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    // load the SimpleNetwork interfaces
    iFace = loadUserSubComponent<SST::Interfaces::StandardMem>("iface", ComponentInfo::SHARE_NONE, 1);
    iFace->setNotifyOnReceive(new SST::Interfaces::StandardMem::Handler<memToRtr>(this, &memToRtr::handleEvent));
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
void memToRtr::send(SST::Event* ev){
    SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(ev);
    iFace->send(mev);
    delete ev;
    delete mev;
}

// memToRtr event handler
bool memToRtr::handleEvent(){
    SST::Interfaces::SimpleNetwork::Request* req = iFace->recv(0);
    if( req != nullptr ){
        SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(req->takePayload());

        nid_t dest = mev->getDest();
        size_t size_in_bits = mev->getEventSize();

        rtrSubComp->send(dest, size_in_bits, mev); // use memSubComponent's send method to hand off the memory event
        delete req;
    }
    return true;
}




