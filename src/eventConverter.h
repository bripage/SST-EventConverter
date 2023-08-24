//
// _eventConverter_h_
//

#ifndef _EVENT_CONVERTER_H_
#define _EVENT_CONVERTER_H_

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>

namespace SST {
    namespace eventConverter {

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

            ImplementSerializable(SST::eventConverter::cloneableEvent);

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

            ImplementSerializable(SST::eventConverter::endpointDiscoveryEvent);

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
            SST_ELI_REGISTER_SUBCOMPONENT_API(SST::eventConverter::baseSubComponent)

            baseSubComponent(SST::ComponentId_t id, SST::Params& params)
                    : SubComponent(id),adjacentSubComp(nullptr),endpointType(0), verbose(0) { }

            // virtual methods
            virtual ~baseSubComponent() { }
            virtual void send(SST::Event *) = 0;
            virtual void init(unsigned int) = 0 ;
            virtual void passOffInitEvents(SST::Event* ev) = 0;

            // base methods
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
        class memRtrConverter : public SST::Component {
        public:
            // Register the component with the SST element library
            SST_ELI_REGISTER_COMPONENT(
                    memRtrConverter, // Component class
            "eventConverter", // Component library
            "memRtrConverter", // Component name
            SST_ELI_ELEMENT_VERSION(1,0,0), // Version of the component
            "memRtrConverter: Converts different one event type to another based on subcomponents in use", // Description of the component
            COMPONENT_CATEGORY_UNCATEGORIZED // Component category
            )

            SST_ELI_DOCUMENT_PARAMS()
            SST_ELI_DOCUMENT_PORTS()
            SST_ELI_DOCUMENT_STATISTICS()

            SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
            {"memory", "Handles memory event traffic",
                        "SST::eventConverter::memory"},
            {"router", "Handles router event traffic",
                        "SST::eventConverter::router"}
            )

            memRtrConverter(SST::ComponentId_t id, SST::Params& params);
            ~memRtrConverter();
            void init(unsigned int phase);

        private:
            SST::Output* out;       // SST Output object for printing, messaging, etc

            baseSubComponent* memSubComp; // baseSubComponent
            baseSubComponent* rtrSubComp; // baseSubComponent
        };  // class memRtrConverter
    }   // namespace BasicEventConverter
}   // namespace SST

#endif