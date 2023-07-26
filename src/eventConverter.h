//
// _eventConverter_h_
//

#ifndef _EVENT_CONVERTER_H_
#define _EVENT_CONVERTER_H_

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include <sst/dbg/SSTDebug.h>

namespace SST {
    namespace BasicEventConverter {

        class baseSubComponent : public SST::SubComponent {
        public:
            SST_ELI_REGISTER_SUBCOMPONENT_API(SST::BasicEventConverter::baseSubComponent)

            baseSubComponent(SST::ComponentID_t id, SST::Params& params)
                    : SubComponent(id) { }

            virtual ~baseSubComponent() { }

            virtual void send(SST::Event) = 0;
        };

        class eventConverter : public SST::Component {
            public:
                // Register the component with the SST element library
                SST_ELI_REGISTER_COMPONENT(
                        eventConverter, // Component class
                        "BasicEventConverter", // Component library
                        "eventConverter", // Component name
                        SST_ELI_ELEMENT_VERSION(1,0,0), // Version of the component
                        "eventConverter: Converts different one event type to another based on subcomponents in use", // Description of the component
                        COMPONENT_CATEGORY_UNCATEGORIZED // Component category
                )

                SST_ELI_DOCUMENT_PARAMS()
                SST_ELI_DOCUMENT_PORTS()
                SST_ELI_DOCUMENT_STATISTICS()

                SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
                        {"memory", "Handles memory event traffic",
                         "SST::eventConverter::memToRtr"},
                        {"router", "Handles router event traffic",
                         "SST::eventConverter::rtrToMem"}
                )
                // Class members

                // Constructor: Components receive a unique ID and the set of parameters
                //              that were assigned in the simulation configuration script
                eventConverter(SST::ComponentId_t id, SST::Params& params);

                // Destructor
                ~eventConverter();

            private:
                // Params
                SST::Output* out;       // SST Output object for printing, messaging, etc

                // Subcomponents
                baseSubComponent *memory; // baseSubComponent
                baseSubComponent *router; // baseSubComponent

            // Debug
            #ifdef ENABLE_SSTDBG
                SSTDebug *Dbg;
            #endif
        };  // class eventConverter
    }   // namespace BasicEventConverter
}   // namespace SST

#endif
