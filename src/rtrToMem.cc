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
    linkHandler = configureLink("memPort", new Event::Handler<memToRtr>(this, &SST::Merlin::RtrEvent));
}

// memToRtr destructor
rtrToMem::~rtrToMem(){
    delete out;
}

// receive memory event data to make a RtrEvent from the memory side
void rtrToMem::send(std::string& src, std::string& dest, size_t size_in_bits, SST::Event* ev){
    SST::Interfaces::SimpleNetwork::Request req(dest, src, size_in_bits, 0, 0, ev);
    SST::Merlin::RtrEvent* rev = SST::Merlin::RtrEvent(req, src, 0);

    linkhandler->send(rev);

    delete ev;
    delete rev;
}

// memToRtr event handler
void rtrToMem::handleEvent(SST::Event *ev){
    SST::Merlin::RtrEvent* rev = dynamic_cast<SST::Merlin::RtrEvent*>(ev);
    delete ev;

    if( rev ){
        SST::Interfaces::SimpleNetwork::Request* req = rev->takeRequest();
        SST::Event* mev = dynamic_cast<SST::Event*>(req->takePayload());

        memSubComp->send(mev);

        delete rev;
        delete mev;
    }else{
        out->fatal(CALL_INFO, -1, "Error: Bad event type received!\n" );
    }
}





