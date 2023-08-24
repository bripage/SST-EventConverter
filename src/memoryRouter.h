//
// memoryRouter_h
//

#ifndef _MEM_ROUTER_H_
#define _MEM_ROUTER_H_

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>

namespace SST {
    namespace memRouter {

        // This is literally an SST::Event but with the ability to clone it like nearly all other events
        // P.S. I cant understand why this isnt a thing
        class cloneableEvent : public SST::Event {
        public:
            // Basic Constructor : inherits SST::Event()
            cloneableEvent() : SST::Event() { }
            // Overloaded Constructor

            void serialize_order(SST::Core::Serialization::serializer &ser)
            override {
                Event::serialize_order(ser);
                ser & payload;
            }
            virtual cloneableEvent* clone(void) override {
                return new cloneableEvent(*this);
            }

            ImplementSerializable(SST::memRouter::cloneableEvent);

        private:
            // Message payload
            uint32_t src;
            bool payload;
        };

        // ---------------------------------------------------------------------------------------------------
        //  endpointDiscoveryEvent Event handler
        //
        //  Note: This event type is ONLY used during the initialization phase of model execution. It allows
        //  for the conversion components to send the discovery events to all other converter's. This allows
        //  converts connected to MemHiearchy components such as memHierarchy.Cache to know what the
        //  component id for memHierarchy.MemController, etc., for routing purposes.
        //
        // ---------------------------------------------------------------------------------------------------
        class endpointDiscoveryEvent : public SST::Event {
        public:
            // Basic Constructor : inherits SST::Event()
            endpointDiscoveryEvent() : SST::Event() { }
            // Overloaded Constructor
            endpointDiscoveryEvent(bool endpointType) : SST::Event(), payload(endpointType) { }

            void serialize_order(SST::Core::Serialization::serializer &ser)
            override {
                Event::serialize_order(ser);
                ser & src;
                ser & payload;
            }

            void setSrc(uint32_t id){src = id;}
            uint32_t getSrc(){return src;}
            bool getPayload() { return payload; }
            virtual endpointDiscoveryEvent* clone(void) override {
                return new endpointDiscoveryEvent(*this);
            }

            ImplementSerializable(SST::memRouter::endpointDiscoveryEvent);

        private:
            // Message payload
            uint32_t src;
            bool payload;
        };

        // ----------------------------------------------------------
        // Base Sub-Component Definition for various endpoint types
        // ----------------------------------------------------------
        class baseSubComponent : public SST::SubComponent {
        public:
            SST_ELI_REGISTER_SUBCOMPONENT_API(SST::memRouter::baseSubComponent)

            baseSubComponent(SST::ComponentId_t id, SST::Params& params)
                    : SubComponent(id),adjacentSubComp(nullptr),endpointType(0), verbose(0) { }

            // virtual methods
            virtual ~baseSubComponent() { }
            virtual void setMsgHandler(Event::HandlerBase* handler) = 0;
            virtual void init(unsigned int phase) = 0;
            virtual void setup() { }
            virtual void send(SST::Event *ev, int dest) = 0;
            //virtual int getNumDestinations() = 0;
            //virtual SST::Interfaces::SimpleNetwork::nid_t getAddress() = 0;

            bool getEndpointType() { return endpointType; }
            void setAdjacentSubComp(baseSubComponent* comp){adjacentSubComp = comp;}

            baseSubComponent* adjacentSubComp;
            bool endpointType;
            uint32_t verbose;
        };

        // ---------------------------------------------------------------------------------------------------
        // Top Level Event Conversion Component Class Definition
        //
        //  Note: The top level component has no ports or parameters of its own as all functionality
        //  with respect to specialized component type interaction is handled by specialized subcomponents.
        //  Currently there are only 2 subcomponents however, this could be expanded to allow for different
        //  endpoint types, and or multiple endpoint connections.
        //
        // ---------------------------------------------------------------------------------------------------
        class memoryRouter : public SST::Component {
        public:
            // Register the component with the SST element library
            SST_ELI_REGISTER_COMPONENT(
                    memoryRouter, // Component class
            "memoryRouter", // Component library
            "memoryRouter", // Component name
            SST_ELI_ELEMENT_VERSION(1,0,0), // Version of the component
            "memoryRouter Converts different one event type to another based on subcomponents in use", // Description of the component
            COMPONENT_CATEGORY_UNCATEGORIZED // Component category
            )

            SST_ELI_DOCUMENT_PARAMS()
            SST_ELI_DOCUMENT_PORTS()
            SST_ELI_DOCUMENT_STATISTICS()

            SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
            {"memory", "Handles memory event traffic",
                        "SST::memoryRouter::memory"},
            {"router", "Handles router event traffic",
                        "SST::memoryRouter::router"}
            )

            memoryRouter(SST::ComponentId_t id, SST::Params& params);
            ~memoryRouter();
            void init(unsigned int phase);

        private:
            SST::Output* out;       // SST Output object for printing, messaging, etc

            baseSubComponent* memSubComp; // baseSubComponent
            baseSubComponent* rtrSubComp; // baseSubComponent
        };  // class memoryRouter
    }   // namespace memRouter
}   // namespace SST

#endif