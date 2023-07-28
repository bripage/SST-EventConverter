//
// Created by tokth on 7/11/23.
//

#ifndef _MEMEVENT_CONVERTER_H_
#define _MEMEVENT_CONVERTER_H_

#include "eventConverter.h"
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include "/scratch/shared/apps/sst/13.0.0/elements/include/sst/elements/memHierarchy/memEvent.h"
#include "/scratch/shared/apps/sst/13.0.0/elements/include/sst/elements/memHierarchy/memEventBase.h"

using namespace SST;
using namespace SST::eventConverter;

class memToRtr : public baseSubComponent {
    public:
        SST_ELI_REGISTER_SUBCOMPONENT(
            memToRtr,
            "eventConverter",
            "memToRtr",
            SST_ELI_ELEMENT_VERSION(1,0,0),
            "memToRtr : accepts StdMem events and sends them to the router subcomponent for conversion to RtrEvent",
            SST::eventConverter::baseSubComponent
        )

        SST_ELI_DOCUMENT_PARAMS()

        SST_ELI_DOCUMENT_PORTS(
            {"memPort", "Link to another component which uses SST::StdMem.", {"StandardMem.Request"} }
        )

        SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
            {"iface", "SimpleNetwork interface to a network", "SST::Interfaces::StandardMem"}
        )

        memToRtr(ComponentId_t id, Params& params);
        ~memToRtr();
        void send(SST::Interfaces::SimpleNetwork::Request*&, SST::Event*&);
        bool handleEvent(int vn);

    private:
        // Params
        SST::Output* out;           // SST Output object for printing, messaging, etc
        SST::Interfaces::StandardMem* iFace; // SST network interface
};

#endif