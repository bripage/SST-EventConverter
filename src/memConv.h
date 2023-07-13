//
// Created by tokth on 7/11/23.
//

#ifndef EVENTCONVERTER_MEMCONV_H
#define EVENTCONVERTER_MEMCONV_H

using namespace SST;
using namespace SST::eventConverter;

class memConv : public baseSubComponent {
public:
    SST_ELI_REGISTER_SUBCOMPONENT(
            memConv, "eventConverter”,"memConv",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "memConv : simple message handler subcomponent",
    SST::basicComponent::basicSubcomponent
    )
    SST_ELI_DOCUMENT_PARAMS()
    SST_ELI_DOCUMENT_PORTS(

    )
    basicMsg(ComponentId_t id, Params& params);
    virtual ~basicMsg();
    virtual void send(int number);
    virtual bool clock(Cycle_t cycle);
private:
    // Params
    SST::Output* out; // SST Output object for printing, messaging, etc
    std::string clockFreq; // Clock frequency
    unsigned count; // Num of integers to store before flushing a msg
    std::vector<int> msgs; // Message storage
};
