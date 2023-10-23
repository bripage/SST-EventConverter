//
// Created by tokth on 7/11/23.
//

#ifndef _RTREVENT_CONVERTER_H_
#define _RTREVENT_CONVERTER_H_

#include "memoryRouter.h"

// -- Standard Headers
#include <vector>
#include <queue>
#include <tuple>
#include <unistd.h>

// -- SST Headers
#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/timeConverter.h>
#include <sst/core/interfaces/simpleNetwork.h>

using namespace SST;
using namespace SST::memRouter;

class rtrEvent : public SST::Event {
public:
    /// rtrEvent: standard constructor
    rtrEvent(std::string name) : Event(), SrcName(name) { }

    /// rtrEvent: extended constructor
    rtrEvent(std::string name, std::vector<uint8_t> data)
            : Event(), SrcName(name), Data(data) { }

    /// rtrEvent: retrieve the source name
    std::string getSource() { return SrcName; }

    // rtrEvent: retrieve the data payload
    std::vector<uint8_t> getData() { return Data; }

    /// rtrEvent: virtual function to clone an event
    virtual Event* clone(void) override{
        rtrEvent* ev = new rtrEvent(*this);
        return ev;
    }

private:
    std::string SrcName;        ///< rtrEvent: Name of the sending device
    std::vector<uint8_t> Data;  ///< rtrEvent: Data payload

public:
    /// rtrEvent: secondary constructor
    rtrEvent() : Event() {}

    /// rtrEvent: event serializer
    void serialize_order(SST::Core::Serialization::serializer &ser) override{
        Event::serialize_order(ser);
        ser & SrcName;
        ser & Data;
    }

    /// rtrEvent: implements the NIC serialization
    ImplementSerializable(rtrEvent);
};  // end rtrEvent



/*
 * router: the router interface controller subcomponent
 */
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
        {"verbose", "Verbosity level", "0" },
        {"clock", "Clock frequency of the NIC", "1Ghz"},
        {"port", "Port to use, if loaded as an anonymous subcomponent", "network"}
    })

    SST_ELI_DOCUMENT_PORTS(
        {"network", "Port to network", {"simpleNetworkExample.rtrEvent"} }
    )

    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
        { "iface", "SimpleNetwork interface to a network", "SST::Interfaces::SimpleNetwork" }
    )

    router(ComponentId_t id, Params &params);

    ~router() override;

    // router: Callback to parent on received messages
    void setMsgHandler(Event::HandlerBase* handler);

    /// router: initialization function
    void init(unsigned int phase);

    /// router: setup function
    void setup();

    /// router: send event to the destination id
    void send(SST::Event*ev, int dest);

    /// router: retrieve the number of destinations
    int getNumDestinations();

    /// router: get the endpoint's network address
    SST::Interfaces::SimpleNetwork::nid_t getAddress();

    /// router: callback function for the SimpleNetwork interface
    bool msgNotify(int virtualNetwork);

    /// router: clock function
    bool clockTick(Cycle_t cycle);

    void handleEvent(SST::Interfaces::SimpleNetwork::Request* req);

private:
    // Params
    SST::Output *out;   // SST Output object for printing, messaging, etc
    SST::Interfaces::SimpleNetwork *iFace; // SST network interface
    SST::Interfaces::SimpleNetwork::nid_t memContCompID;
    int numDest;
    std::queue<SST::Interfaces::SimpleNetwork::Request*> sendQ;
    bool initBroadcastSent; // has the init bcast been sent?
}; // end memoryRouter::router
#endif