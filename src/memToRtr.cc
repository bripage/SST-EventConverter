//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include "memToRtr.h"

using namespace SST;
using namespace SST::eventConverter;

memToRtr::memToRtr(ComponentId_t id, Params& params)
  : baseSubComponent(id, params) {

    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    // Read the parameters
    count = params.find<unsigned>("count",5);
    clockFreq  = params.find<std::string>("clockFreq", "1GHz");

    // Register the clock handler
    registerClock(clockFreq, new Clock::Handler<basicMsg>(this, &basicMsg::clock));
    out->output("Registering subcomponent clock with frequency=%s\n", clockFreq.c_str());

    // Configure the links
    linkHandler = configureLink("memPort", new Event::Handler<memToRtr>(this, &memToRtr::handleEvent));

}

// memToRtr destructor
memToRtr::~memToRtr(){
    delete out;
}

// receive memory event from router side
void memToRtr::send(SST::Event* ev){
    SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(ev);
    linkhandler->send(mev);
    delete ev;
    delete mev;
}

// memToRtr event handler
void memToRtr::handleEvent(SST::Event *ev){
    SST::memHierarchy::MemEventBase* mev = dynamic_cast<SST::memHierarchy::MemEventBase*>(ev);
    delete ev;

    if( mev ) {
        std::string& src = mev->getSrc();
        std::string& dest = mev->getDest();
        size_t size_in_bits = mev->getEventSize();

        rtrSubComp->send(src, dest, size_in_bits, mev);
        delete mev;
    } else {
        out->fatal(CALL_INFO, -1, "Error: Bad event type received!\n" );
    }
}




