//
// _eventConverter_cc_
//

#include "eventConverter.h"

using namespace SST;
using namespace SST::Converter;

eventConverter::eventConverter(ComponentId_t id, SST::Params& params)
: Component(id) {
    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    rtriface = loadUserSubComponent<SST::Interfaces::SimpleNetwork>("inFromRtr", ComponentInfo::SHARE_NONE, 1);
    rtriface->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<eventConverter>(this, &eventConverter::rtrToMem));

    if(nullptr == rtriface){
        out->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    } else {
        out->verbose(CALL_INFO, 9, 0, "%s successfully created rtriface\n", getName().c_str());
    }

    memiface = loadUserSubComponent<SST::Interfaces::StandardMem>(
            "memiface", ComponentInfo::SHARE_NONE, getTimeConverter("1GHz"),
            new SST::Interfaces::StandardMem::Handler<eventConverter>(
                    this, &eventConverter::memToRtr));

    if(nullptr == memiface){
        out->fatal(CALL_INFO, -1, "Error : memory interface is null\n");
    } else {
        out->verbose(CALL_INFO, 9, 0, "%s successfully created memiface\n", getName().c_str());
    }

    initBroadcastSent = false;
}

eventConverter::~eventConverter(){
    delete out;
}

void eventConverter::init(unsigned int phase){
    out->verbose(CALL_INFO, 9, 0, "%s begining init phase %d\n", getName().c_str(), phase);

    memiface->init(phase);

    if( rtriface->isNetworkInitialized() ){
        if( rtriface->isNetworkInitialized() ) {
            rtriface->init(phase);
        }
        //rtriface->sendUntimedData(req);
    }

    out->verbose(CALL_INFO, 9, 0, "%s ending init phase %d\n", getName().c_str(), phase);
}

void eventConverter::memToRtr(SST::Interfaces::StandardMem::Request* ev){

}

bool eventConverter::rtrToMem(int vn){

}