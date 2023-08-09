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
    printf("%s Created!\n", getName().c_str());
    const int Verbosity = params.find<int>("verbose", 0);
    out = new Output("", Verbosity, 0, Output::STDOUT);

    iFace = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("iface", ComponentInfo::SHARE_NONE, 1);
    iFace->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<rtrToMem>(this, &rtrToMem::handleEvent));

    initBroadcastSent = false;
}

// memToRtr destructor
rtrToMem::~rtrToMem(){
    delete out;
}

// receive memory event data to make a RtrEvent
void rtrToMem::send(SST::Event* ev){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    SST::Interfaces::SimpleNetwork::nid_t src = iFace->getEndpointID();
    SST::Interfaces::SimpleNetwork::nid_t dest = memContCompID;  // this needs to be learned during router network initialization
    size_t size_in_bits = mev->getEventSize();
    SST::Interfaces::SimpleNetwork::Request netReq = SST::Interfaces::SimpleNetwork::Request(dest, src, size_in_bits, 0, 0, mev);

    iFace->send(netReq.clone(), 0);

    delete mev;
    delete ev;
}

// memToRtr event handler
bool rtrToMem::handleEvent(int vn){
    out->verbose(CALL_INFO, 9, 0, "%s event handler was called!\n", getName().c_str());
    SST::Interfaces::SimpleNetwork::Request* netReq = iFace->recv(0);
    if( netReq != nullptr ){
        SST::Event* ev = dynamic_cast<SST::Event*>(netReq->takePayload());
        SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
        adjacentSubComp->send(mev->clone()); // use memToRtr's send method to hand off the memory event
    }
    delete netReq;

    return true;
}

void rtrToMem::init(unsigned int phase){
    iFace->init(phase);
    out->verbose(CALL_INFO, 9, 0, "%s begining init phase %d\n", getName().c_str(), phase);

    if( iFace->isNetworkInitialized() ){
        if(!initBroadcastSent) {
            initBroadcastSent = true;
            SST::Interfaces::SimpleNetwork::Request *req = new SST::Interfaces::SimpleNetwork::Request();
            req->dest = SST::Interfaces::SimpleNetwork::INIT_BROADCAST_ADDR;
            req->src = iFace->getEndpointID();

            endpointDiscoveryEvent *ev = new endpointDiscoveryEvent(adjacentSubComp->getEndpointType());
            ev->setSrc(iFace->getEndpointID());

            req->givePayload(ev);
            out->verbose(CALL_INFO, 2, 0, "%s (endpointType=%d) sending init message to %zu\n", getName().c_str(),
                         adjacentSubComp->getEndpointType(), SST::Interfaces::SimpleNetwork::INIT_BROADCAST_ADDR);

            //iFace->sendInitData(req);
            iFace->sendUntimedData(req);
        }
    }

    while( SST::Interfaces::SimpleNetwork::Request* req = iFace->recvUntimedData() ) {
        SST::Event* ev;
        out->verbose(CALL_INFO, 1, 0, "%s received a request during init()\n", getName().c_str());
        ev = req->takePayload();
        eventConverter::cloneableEvent* cev = dynamic_cast<eventConverter::cloneableEvent*>(ev);
        endpointDiscoveryEvent *epde = dynamic_cast<endpointDiscoveryEvent*>(cev);

        if (epde) {
            out->verbose(CALL_INFO, 1, 0, "%s received init message from %zu\n", getName().c_str(), epde->getSrc());

            bool remoteEndpointType = epde->getPayload();
            out->output("%s received init message from %d (endpointType = %d)\n", getName().c_str(), epde->getSrc(),
                        remoteEndpointType);

            if (remoteEndpointType) {
                memContCompID = static_cast<SST::Interfaces::SimpleNetwork::nid_t>(epde->getSrc());
            }
        } else {
            adjacentSubComp->passOffInitEvents(cev->clone());
        }
    }

    out->verbose(CALL_INFO, 9, 0, "%s ending init phase %d\n", getName().c_str(), phase);
}

void rtrToMem::passOffInitEvents(SST::Event* ev){
    SST::Interfaces::SimpleNetwork::Request *req = new SST::Interfaces::SimpleNetwork::Request();
    req->dest = SST::Interfaces::SimpleNetwork::INIT_BROADCAST_ADDR;
    req->src = iFace->getEndpointID();
    req->givePayload(ev);

    out->verbose(CALL_INFO, 2, 0, "%s (endpointType=%d) sending init message to %zu\n", getName().c_str(),
                 adjacentSubComp->getEndpointType(), SST::Interfaces::SimpleNetwork::INIT_BROADCAST_ADDR);

    iFace->sendUntimedData(req);
}