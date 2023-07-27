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

    iFace = loadUserSubComponent<SST::Interfaces::StandardMem>("iface", ComponentInfo::SHARE_NONE, 1);
    iFace->setNotifyOnReceive(new SST::Interfaces::StandardMem::Handler<memToRtr>(this, &memToRtr::handleEvent));
}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
virtual void send(SST::Interfaces::SimpleNetwork::Request** req, SST::Event** ev){
    SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(*ev);
    iFace->send(mev);
    delete ev;
    delete *mev;
    delete *req;
}

// memToRtr event handler
bool memToRtr::handleEvent(int vn){
    SST::Interfaces::StandardMem::Request* memReq = iFace->recv(0);
    if( memReq != nullptr ){
        SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(memReq->takePayload());

        SST::Interfaces::SimpleNetwork::nid_t src = memReq->src = iFace->getEndpointID();
        SST::Interfaces::SimpleNetwork::nid_t dest = mev->getDest();
        size_t size_in_bits = mev->getEventSize();

        adjacentSubComp->send(src, dest, size_in_bits, mev, memReq); // use memSubComponent's send method to hand off the memory event
    }
    return true;
}




