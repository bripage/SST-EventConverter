//
// Created by tokth on 7/11/23.
//

#ifndef EVENTCONVERTER_RTRCONV_H
#define EVENTCONVERTER_RTRCONV_H

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include "/scratch/shared/sst/13.0.0/elements/include/sst/elements/merlin/router.h"

using namespace SST;
using namespace SST::eventConverter;

class rtrToMem : public baseSubComponent {
    public:
        SST_ELI_REGISTER_SUBCOMPONENT(
                router,
            "eventConverter",
            "router",
            SST_ELI_ELEMENT_VERSION(1,0,0),
            "router : simple message handler subcomponent",
            SST::eventConverter::baseSubComponent
        )

        SST_ELI_DOCUMENT_PARAMS()

        SST_ELI_DOCUMENT_PORTS(
            {"rtrPort", "Link to another component which uses SST::Interfaces::SimpleNetwork.", {"merlin.RtrEvent"} }
        )

        rtrToMem(ComponentId_t id, Params& params);
        virtual ~rtrToMem();
        virtual void send(std::string& src, std::string& dest, size_t size_in_bits, SST::Event* ev);

    private:
        // Params
        SST::Output* out;           // SST Output object for printing, messaging, etc
        SST::Link *linkHandler;     // Link handler object
};

