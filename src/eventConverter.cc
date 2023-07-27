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

    // Load the subcomponent
    memSubComp = loadUserSubComponent<baseSubComponent>("memory");
    rtrSubComp = loadUserSubComponent<baseSubComponent>("router");
}

eventConverter::~eventConverter(){
    delete out;
}

