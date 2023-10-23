//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include "router.h"

using namespace SST;
using namespace SST::memRouter;

router::router(ComponentId_t id, Params& params)
        : baseSubComponent(id, params) {
    printf("%s Created!\n", getName().c_str());

    const int Verbosity = params.find<int>("verbose", 0);
    out = new Output("", Verbosity, 0, Output::STDOUT);

    const std::string nicClock = params.find<std::string>("clock", "1GHz");
    registerClock(nicClock, new Clock::Handler<router>(this,&router::clockTick));

    // load the SimpleNetwork interfaces
    iFace = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("iface", ComponentInfo::SHARE_NONE, 1);
    if( !iFace ){
        // load the anonymous nic
        Params netparams;
        netparams.insert("port_name", params.find<std::string>("port", "network"));
        netparams.insert("in_buf_size", "256B");
        netparams.insert("out_buf_size", "256B");
        netparams.insert("link_bw", "40GiB/s");
        iFace = loadAnonymousSubComponent<SST::Interfaces::SimpleNetwork>("merlin.linkcontrol",
                                                                          "iface",
                                                                          0,
                                                                          ComponentInfo::SHARE_PORTS | ComponentInfo::INSERT_STATS,
                                                                          netparams,
                                                                          1);
    }
    iFace->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<router>(this, &router::msgNotify));

    initBroadcastSent = false;
    numDest = 0;
    msgHandler = nullptr;
}

void router::setMsgHandler(Event::HandlerBase* handler){
    msgHandler = handler;
}


// memToRtr destructor
router::~router(){
    delete out;
}

void router::init(unsigned int phase){
    iFace->init(phase);

    if( iFace->isNetworkInitialized() ){
        if( !initBroadcastSent) {
            initBroadcastSent = true;
            rtrEvent *ev = new rtrEvent(getName());

            SST::Interfaces::SimpleNetwork::Request * req = new SST::Interfaces::SimpleNetwork::Request();
            req->dest = SST::Interfaces::SimpleNetwork::INIT_BROADCAST_ADDR;
            req->src = iFace->getEndpointID();
            req->givePayload(ev);
            iFace->sendInitData(req);
        }
    }

    while( SST::Interfaces::SimpleNetwork::Request * req = iFace->recvInitData() ) {
        rtrEvent *ev = static_cast<rtrEvent*>(req->takePayload());
        numDest++;
        out->verbose(CALL_INFO, 1, 0,
                        "%s received init message from %s\n",
                        getName().c_str(), ev->getSource().c_str());
    }
}

void router::setup(){
    if( msgHandler == nullptr ){
        out->fatal(CALL_INFO, -1,
                      "%s, Error: RevNIC implements a callback-based notification and parent has not registerd a callback function\n",
                      getName().c_str());
    }
}

bool router::msgNotify(int vn){
    SST::Interfaces::SimpleNetwork::Request* req = iFace->recv(0);
    if( req != nullptr ){
        if( req != nullptr ){
            SST::Event *ev = static_cast<SST::Event*>(req->takePayload());
            delete req;
            (*msgHandler)(ev);
        }
    }
    return true;
}

void router::send(SST::Event* event, int destination){
    rtrEvent* rev = dynamic_cast<rtrEvent*>(event);
    SST::Interfaces::SimpleNetwork::Request *req = new SST::Interfaces::SimpleNetwork::Request();
    req->dest = destination;
    req->src = iFace->getEndpointID();
    req->givePayload(rev);
    sendQ.push(req);
}

int router::getNumDestinations(){
    return numDest;
}

SST::Interfaces::SimpleNetwork::nid_t router::getAddress(){
    return iFace->getEndpointID();
}

bool router::clockTick(Cycle_t cycle){
    while( !sendQ.empty() ){
        if( iFace->spaceToSend(0,512) && iFace->send(sendQ.front(),0)) {
            sendQ.pop();
        }else{
            break;
        }
    }
    return false;
}


