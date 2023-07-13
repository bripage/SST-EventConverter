//
// _basicComponent_h_
//

#ifndef _EVENT_CONVERTER_H_
#define _EVENT_CONVERTER_H_

#include <sst/core/component.h>

namespace SST {
    namespace eventConverter {

        class eventConverter : public SST::Component {
        public:
            // Register the component with the SST element library
            SST_ELI_REGISTER_COMPONENT(
                    eventConverter, // Component class
                    "eventConverter", // Component library
                    "eventConverter", // Component name
                    SST_ELI_ELEMENT_VERSION(1,0,0), // Version of the component
                    "eventConverter: Converts different one event type to another based on the subcomponents in use", // Description of the component
                    COMPONENT_CATEGORY_UNCATEGORIZED // Component category
            )

            // Create a new SST output object
            out = new Output("", 1, 0, Output::STDOUT);

            // Document the parameters that this component accepts
            // { "parameter_name", "description", "default value or NULL if required" }
            SST_ELI_DOCUMENT_PARAMS( )
            // [Optional] Document the ports: we do not define any
            SST_ELI_DOCUMENT_PORTS()
            // [Optional] Document the statisitcs: we do not define any
            SST_ELI_DOCUMENT_STATISTICS()
            // [Optional] Document the subcomponents: we do not define any
            SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
                    {"memory", "Handles memory event traffic", "SST::eventConverter::memConv"},
                    {"router", "Handles router event traffic", "SST::eventConverter::rtrConv"}
            )
          // Class members

          // Constructor: Components receive a unique ID and the set of parameters
          //              that were assigned in the simulation configuration script
          eventConverter(SST::ComponentId_t id, SST::Params& params);

          // Destructor
          ~eventConverter();

        private:

          // Clock handler
          bool clock(SST::Cycle_t cycle);

          // Params
          SST::Output* out;       // SST Output object for printing, messaging, etc
          std::string clockFreq;  // Clock frequency
          Cycle_t cycleCount;     // Cycle counter
          Cycle_t printInterval;  // Interval at which to print to the console

        };  // class basicClocks
    }   // namespace basicComponent
}   // namespace SST

#endif
