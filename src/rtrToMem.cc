//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include "rtrToMem.h"

using namespace SST;
using namespace SST::eventConverter;

rtrToMem::rtrToMem(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {

    out = new Output("", 1, 0, Output::STDOUT);

    iFace = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("iface", ComponentInfo::SHARE_NONE, 1);
    iFace->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<rtrToMem>(this, &rtrToMem::handleEvent));
}

// memToRtr destructor
rtrToMem::~rtrToMem(){
    delete out;
}

// receive memory event data to make a RtrEvent
void rtrToMem::send(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);

    SST::Interfaces::SimpleNetwork::nid_t src = iFace->getEndpointID();
    SST::Interfaces::SimpleNetwork::nid_t dest = 1337;  // this needs to be learned during router network initialization
    size_t size_in_bits = mev->getEventSize();

    SST::Interfaces::SimpleNetwork::Request netReq = SST::Interfaces::SimpleNetwork::Request(dest, src, size_in_bits, 0, 0, mev);

    iFace->send(netReq.clone(), 0);

    delete mev;
    delete mev;
}

// memToRtr event handler
bool rtrToMem::handleEvent(int vn){
    SST::Interfaces::SimpleNetwork::Request* netReq = iFace->recv(0);
    if( netReq != nullptr ){
        SST::Event* mev = dynamic_cast<SST::Event*>(netReq->takePayload());
        //adjacentSubComp->send(netReq, mev); // use memToRtr's send method to hand off the memory event
    }
    return true;
}





