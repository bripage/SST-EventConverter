//
// Created by tokth on 7/11/23.
//

#ifndef EVENTCONVERTER_MEMCONV_H
#define EVENTCONVERTER_MEMCONV_H

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/elements/memHierarchy/memEvent.h>
#include <sst/elements/memHierarchy/memEventBase.h>

using namespace SST;
using namespace SST::eventConverter;

class memToRtr : public baseSubComponent {
    public:
        SST_ELI_REGISTER_SUBCOMPONENT(
                memory,
            "eventConverter”,
            "memory",
            SST_ELI_ELEMENT_VERSION(1,0,0),
            "memory : simple message handler subcomponent",
            SST::eventConverter::baseSubComponent
        )

        SST_ELI_DOCUMENT_PARAMS()

        SST_ELI_DOCUMENT_PORTS(
            {"memPort", "Link to another component which uses SST::StdMem.", {"memHierarchy.MemEventBase"} }
        )
        memConv(ComponentId_t id, Params& params);
        virtual ~memToRtr();
        virtual void send(SST::Event);

    private:
        // Params
        SST::Output* out;           // SST Output object for printing, messaging, etc
        SST::Link *linkHandler;     // Link handler object

};

