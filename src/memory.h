//
// Created by tokth on 7/11/23.
//

#ifndef _MEMEVENT_CONVERTER_H_
#define _MEMEVENT_CONVERTER_H_

#include "memoryRouter.h"

#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/elements/memHierarchy/memEvent.h>
#include <sst/elements/memHierarchy/memEventBase.h>
#include <sst/core/interfaces/stdMem.h>

using namespace SST;
using namespace SST::memRouter;

class memory : public baseSubComponent {
public:
    SST_ELI_REGISTER_SUBCOMPONENT(
            memory,
    "memoryRouter",
    "memory",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "memory : accepts StdMem events and sends them to the router subcomponent for conversion to RtrEvent",
    SST::memRouter::baseSubComponent
    )

    SST_ELI_DOCUMENT_PARAMS(
    {"type", "MemHiearchy endpoint type: 0 = cache, 1 = memory controller","0"},
    {"verbose", "Verbosity level","0"}
    )

    SST_ELI_DOCUMENT_PORTS()

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    { "iface", "Set the interface to memory", "SST::Interfaces::StandardMem" }
    )

    memory(ComponentId_t id, Params& params);
    ~memory() override;
    virtual void send(SST::Event* ev) override;
    virtual void init(unsigned int phase);
    void handleEvent(SST::Interfaces::StandardMem::Request* req);


private:
    // Params
    SST::Output* out;           // SST Output object for printing, messaging, etc
    SST::Link* memLink;
    SST::Interfaces::StandardMem* iface;                  ///< StandardMem memory interface
};

#endif