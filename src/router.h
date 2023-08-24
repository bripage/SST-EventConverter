//
// Created by tokth on 7/11/23.
//

#ifndef _RTREVENT_CONVERTER_H_
#define _RTREVENT_CONVERTER_H_

#include "memoryRouter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/elements/merlin/router.h>
#include "memory.h"

using namespace SST;
using namespace SST::memRouter;

class router : public baseSubComponent {
public:
    SST_ELI_REGISTER_SUBCOMPONENT(
            router,
    "memoryRouter",
    "router",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "router : accepts router events and passes them to the memory subcomponent",
    SST::memRouter::baseSubComponent
    )

    SST_ELI_DOCUMENT_PARAMS({
        {"verbose", "Verbosity level","0"}
    })

    SST_ELI_DOCUMENT_PORTS()

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    {"iface", "SimpleNetwork interface to a network", "SST::Interfaces::SimpleNetwork"}
    )

    router(ComponentId_t id, Params& params);
    ~router() override;
    void send(SST::Event*) override;
    void init(unsigned int phase) override;
    bool handleEvent(int vn);
    void passOffInitEvents(SST::Event* ev);

private:
    // Params
    SST::Output* out;   // SST Output object for printing, messaging, etc
    SST::Interfaces::SimpleNetwork* iface; // SST network interface
    SST::Interfaces::SimpleNetwork::nid_t memContCompID;
    bool initBroadcastSent; // has the init bcast been sent?
};

#endif