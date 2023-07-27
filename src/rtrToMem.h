//
// Created by tokth on 7/11/23.
//

#ifndef _RTREVENT_CONVERTER_H_
#define _RTREVENT_CONVERTER_H_

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include "/scratch/shared/apps/sst/13.0.0/elements/include/sst/elements/merlin/router.h"
#include <sst/core/sst_types.h>

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

        SST_ELI_DOCUMENT_PARAMS()

        SST_ELI_DOCUMENT_PORTS(
            {"rtrPort", "Link to another component which uses SST::Interfaces::SimpleNetwork.", {"simpleNetworkExample.nicEvent"} }
        )

        SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
            {"iface", "SimpleNetwork interface to a network", "SST::Interfaces::SimpleNetwork"}
        )

        rtrToMem(ComponentId_t id, Params& params);
        virtual ~rtrToMem();
        virtual void send(nid_t dest, size_t size_in_bits, SST::Event* ev);
        bool handleEvent();

    private:
        // Params
        SST::Output* out;           // SST Output object for printing, messaging, etc
        SST::Interfaces::SimpleNetwork * iFace; // SST network interface
};

#endif