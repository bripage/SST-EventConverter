//
// _eventConverter_h_
//

#ifndef _EVENT_CONVERTER_H_
#define _EVENT_CONVERTER_H_

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/elements/merlin/router.h>
#include <sst/elements/memHierarchy/memEvent.h>
#include <sst/elements/memHierarchy/memEventBase.h>

namespace SST {
    namespace Converter {

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

            ImplementSerializable(SST::Converter::cloneableEvent);

        private:
            // Message payload
            uint32_t src;
            bool payload;
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
        class eventConverter : public SST::Component {
            public:
                // Register the component with the SST element library
                SST_ELI_REGISTER_COMPONENT(
                        eventConverter, // Component class
                        "eventConverter", // Component library
                        "eventConverter", // Component name
                        SST_ELI_ELEMENT_VERSION(1,0,0), // Version of the component
                        "eventConverter: Converts different one event type to another based on subcomponents in use", // Description of the component
                        COMPONENT_CATEGORY_UNCATEGORIZED // Component category
                )

                SST_ELI_DOCUMENT_PARAMS(
                        {"verbose", "Verbosity level","0"}
                )

                SST_ELI_DOCUMENT_PORTS()
                SST_ELI_DOCUMENT_STATISTICS()

                SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
                        {"rtriface", "SimpleNetwork interface to a network",
                         "SST::Interfaces::SimpleNetwork"},
                        {"memiface", "Set the interface to memory",
                         "SST::Interfaces::StandardMem" },
                )

                eventConverter(SST::ComponentId_t id, SST::Params& params);
                ~eventConverter();
                void init(unsigned int phase);
                void eventConverter::memToRtr(SST::Interfaces::StandardMem::Request* ev);
                bool rtrToMem(int vn);

            private:
                SST::Output* out;       // SST Output object for printing, messaging, etc
                bool initBroadcastSent; // has the init bcast been sent?

                SST::Interfaces::SimpleNetwork* rtriface; // SST network interface
                SST::Interfaces::StandardMem* memiface; // SST network interface
        };  // class memRtrConverter
    }   // namespace BasicEventConverter
}   // namespace SST

#endif
