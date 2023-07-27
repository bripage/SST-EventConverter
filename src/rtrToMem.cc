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

    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    // Configure the links
    //linkHandler = configureLink("rtrPort", new Event::Handler<rtrToMem>(this, &SST::Merlin::RtrEvent));

    // load the SimpleNetwork interfaces
    iFace = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("iface", ComponentInfo::SHARE_NONE, 1);
    iFace->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<rtrToMem>(this, &rtrToMem::handleEvent));
}

// memToRtr destructor
rtrToMem::~rtrToMem(){
    delete out;
}

// receive memory event data to make a RtrEvent
void rtrToMem::send(nid_t dest, size_t size_in_bits, SST::Event* ev){
    SST::Interfaces::SimpleNetwork::Request req = SST::Interfaces::SimpleNetwork::Request(dest, src, size_in_bits, 0, 0, ev);
    SST::Merlin::RtrEvent* rev = SST::Merlin::RtrEvent(req, src, 0);

    iFace->send(rev);

    delete ev;
    delete rev;
}

// memToRtr event handler
bool rtrToMem::handleEvent(){
    SST::Interfaces::SimpleNetwork::Request* req = iFace->recv(0);
    if( req != nullptr ){
        SST::Event* mev = dynamic_cast<SST::Event*>(req->takePayload());
        memSubComp->send(mev); // use memSubComponent's send method to hand off the memory event
        delete req;
    }
    return true;
}





