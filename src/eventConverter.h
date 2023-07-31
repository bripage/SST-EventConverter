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

        class baseSubComponent : public SST::SubComponent {
        public:
            SST_ELI_REGISTER_SUBCOMPONENT_API(SST::eventConverter::baseSubComponent)

            baseSubComponent(SST::ComponentId_t id, SST::Params& params)
                    : SubComponent(id) { }

            virtual ~baseSubComponent() { }
            virtual void send(SST::Event *){ };
            void setAdjacentSubComp(baseSubComponent* comp){adjacentSubComp = comp;}

            baseSubComponent* adjacentSubComp;
        };

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
                         "SST::eventConverter::memToRtr"},
                        {"router", "Handles router event traffic",
                         "SST::eventConverter::rtrToMem"}
                )
                // Class members

                // Constructor: Components receive a unique ID and the set of parameters
                //              that were assigned in the simulation configuration script
                memRtrConverter(SST::ComponentId_t id, SST::Params& params);

                // Destructor
                ~memRtrConverter();

            private:
                // Params
                SST::Output* out;       // SST Output object for printing, messaging, etc

                // Subcomponents
                baseSubComponent* memSubComp; // baseSubComponent
                baseSubComponent* rtrSubComp; // baseSubComponent

        };  // class eventConverter
    }   // namespace BasicEventConverter
}   // namespace SST

#endif
