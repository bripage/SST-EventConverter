//
// Created by tokth on 7/11/23.
//

#ifndef _RTREVENT_CONVERTER_H_
#define _RTREVENT_CONVERTER_H_

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/elements/merlin/router.h>
#include "memToRtr.h"

using namespace SST;
using namespace SST::eventConverter;

class rtrToMem : public baseSubComponent {
public:
    SST_ELI_REGISTER_SUBCOMPONENT(
            rtrToMem,
    "eventConverter",
    "rtrToMem",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "rtrToMem : accepts router events and passes them to the memory subcomponent",
    SST::eventConverter::baseSubComponent
    )

    SST_ELI_DOCUMENT_PARAMS({
        {"verbose", "Verbosity level","0"}
    })

    SST_ELI_DOCUMENT_PORTS()

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    {"iface", "SimpleNetwork interface to a network", "SST::Interfaces::SimpleNetwork"}
    )

    rtrToMem(ComponentId_t id, Params& params);
    ~rtrToMem() override;
    void send(SST::Event*) override;
    void init(unsigned int phase) override;
    bool handleEvent(int vn);
    void passOffInitEvents(SST::Event* ev);

private:
    // Params
    SST::Output* out;   // SST Output object for printing, messaging, etc
    SST::Interfaces::SimpleNetwork* iFace; // SST network interface
    SST::Interfaces::SimpleNetwork::nid_t memContCompID;
    bool initBroadcastSent; // has the init bcast been sent?
};

#endif