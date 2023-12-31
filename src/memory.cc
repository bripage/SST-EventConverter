//
// _basicComponent_cc_
//

#include "memory.h"

using namespace SST;
using namespace SST::memRouter;

memory::memory(ComponentId_t id, Params& params)
        : baseSubComponent(id, params) {
    printf("%s Created!\n", getName().c_str());
    const int Verbosity = params.find<int>("verbose", 0);
    out = new Output("", Verbosity, 0, Output::STDOUT);

    iface = loadUserSubComponent<SST::Interfaces::StandardMem>(
            "iface", ComponentInfo::SHARE_NONE, getTimeConverter("1GHz"),
            new SST::Interfaces::StandardMem::Handler<memory>(this, &memory::handleEvent));

    if( !iface ){
        out->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    }

    endpointType = params.find<bool>("type", 0);
}

// memory destructor
memory::~memory(){
    delete out;
}

// receive memory event from router side
void memory::send(SST::Event* ev, int dest){
    SST::MemHierarchy::MemEventBase* mev = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    //memIFace->send(mev->clone());
    delete mev;
    delete ev;
}

// memory event handler
void memory::handleEvent(SST::Interfaces::StandardMem::Request* req){

}

void memory::init(unsigned int phase){
    iface->init(phase);
    out->verbose(CALL_INFO, 9, 0, "%s ending init phase %d\n", getName().c_str(), phase);
}

void memory::setMsgHandler(Event::HandlerBase* handler){

}