//
// _eventConverter_cc_
//

#include "eventConverter.h"

using namespace SST;
using namespace SST::eventConverter;

eventConverter::eventConverter(ComponentId_t id, SST::Params& params)
: Component(id) {
    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    rtriface = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("inFromRtr", ComponentInfo::SHARE_NONE, 1);
    rtriface->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<rtrToMem>(this, &eventConverter::rtrToMem));

    if(nullptr == rtriface){
        out->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    }

    memiface = loadUserSubComponent<SST::Interfaces::memiface>(
            "memiface", ComponentInfo::SHARE_NONE, getTimeConverter("1GHz"),
            new SST::Interfaces::StandardMem::Handler<memToRtr>(
                    this, &eventConverter::memToRtr));

    if(nullptr == memiface){
        out->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    }

    initBroadcastSent = false;

    // configure ports
    linkprefix = "port;
    linkname = linkprefix + "0";
    while (isPortConnected(linkname)) {
        link = configureLink(linkname, "1 ps",
                             new SST::Interfaces::StandardMem::Handler<memToRtr>(this, &eventConverter::memToRtr));
        if (!link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on port '%s'\n", getName().c_str(), linkname.c_str());
        lowNetPorts_.push_back(link);
        numLowNetPorts_++;
        linkname = linkprefix + std::to_string(numLowNetPorts_);
}

eventConverter::~eventConverter(){
    delete out;
}

void eventConverter::init(unsigned int phase){
    out->verbose(CALL_INFO, 9, 0, "%s begining init phase %d\n", getName().c_str(), phase);

    memiface->init(phase);

    if( iFace->isNetworkInitialized() ){
        if( rtriface->isNetworkInitialized() ) {
            rtriface->init(phase);
        }
        iFace->sendUntimedData(req);
    }

    out->verbose(CALL_INFO, 9, 0, "%s ending init phase %d\n", getName().c_str(), phase);
}

bool eventConverter::memToRtr(SST::Interfaces::SimpleNetwork::Request* req){

}

bool eventConverter::rtrToMem(int vn){

}