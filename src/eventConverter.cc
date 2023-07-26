//
// _eventConverter_cc_
//

#include "eventConverter.h"

using namespace SST;
using namespace SST::BasicEventConverter;


eventConverter::eventConverter(ComponentId_t id, SST::Params& params)
: Component(id) {
    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    // Load the subcomponent
    memory = loadUserSubComponent<baseSubComponent>("memory");
    router = loadUserSubComponent<baseSubComponent>("router");

    #ifdef ENABLE_SSTDBG
        Dbg = new SSTDebug("eventConverter","./");
    #endif
}

eventConverter::~eventConverter(){
    delete out;
    #ifdef ENABLE_SSTDBG
        delete Dbg;
    #endif
}

