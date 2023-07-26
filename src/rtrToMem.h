//
// Created by tokth on 7/11/23.
//

#ifndef EVENTCONVERTER_MEMCONV_H
#define EVENTCONVERTER_MEMCONV_H

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/elements/merlin/router.h>

#include <vector>
#include <queue>

using namespace SST;
using namespace SST::BasicEventConverter;

class rtrToMem : public baseSubComponent {
    public:
        SST_ELI_REGISTER_SUBCOMPONENT(
                router,
            "BasicEventConverter”,
            "router",
            SST_ELI_ELEMENT_VERSION(1,0,0),
            "router : simple message handler subcomponent",
            SST::BasicEventConverter::baseSubComponent
        )

        SST_ELI_DOCUMENT_PARAMS()

        SST_ELI_DOCUMENT_PORTS(
            {"rtrPort", "Link to another component which uses SST::Interfaces::SimpleNetwork.", {"merlin.RtrEvent"} }
        )

        memConv(ComponentId_t id, Params& params);
        virtual ~rtrToMem();
        virtual void send(std::string& src, std::string& dest, size_t size_in_bits, SST::Event* ev);

    private:
        // Params
        SST::Output* out;           // SST Output object for printing, messaging, etc
        SST::Link *linkHandler;     // Link handler object
};

