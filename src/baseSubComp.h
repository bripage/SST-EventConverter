//
// _basicComponent_h_
//

#ifndef _EVENT_CONVERTER_H_
#define _EVENT_CONVERTER_H_

#include <sst/core/subcomponent.h>
#include <sst/core/event.h>

namespace SST {
    namespace eventConverter {
    class baseSubComponent : public SST::SubComponent {
            public:
                SST_ELI_REGISTER_SUBCOMPONENT_API(SST::eventConverter::baseSubComponent)

            baseSubComponent(SST::ComponentID_t id, SST::Params& params)
                : SubComponent(id) { }

            ~baseSubComponent() { }

            virtual void send(SST::Event) = 0;
        };
    }   // namespace basicComponent
}   // namespace SST

#endif
