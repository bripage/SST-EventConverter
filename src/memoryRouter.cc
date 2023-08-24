//
// _eventConverter_cc_
//

#include "memoryRouter.h"

using namespace SST;
using namespace SST::memRouter;

memoryRouter::memoryRouter(ComponentId_t id, SST::Params& params)
        : Component(id) {
    // Create a new SST output object
    out = new Output("", 1, 0, Output::STDOUT);

    // Load the subcomponent
    memSubComp = loadUserSubComponent<baseSubComponent>("memory");
    rtrSubComp = loadUserSubComponent<baseSubComponent>("router");

    memSubComp->setAdjacentSubComp(rtrSubComp);
    rtrSubComp->setAdjacentSubComp(memSubComp);
}

memoryRouter::~memoryRouter(){
    delete out;
}

void memoryRouter::init(unsigned int phase){
    memSubComp->init(phase);
    //rtrSubComp->init(phase);
}