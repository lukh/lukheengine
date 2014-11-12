#include "randomcomponent.hpp"

RandomComponent::RandomComponent(uint8_t inStCh, uint8_t outStCh) :
    AudioComponent(inStCh, outStCh)
{
}

void RandomComponent::process(Sample *in, Sample *out, uint32_t framesPerBuffer){
    //Testing Track
    uint32_t i;
    for(i = 0; i < framesPerBuffer; i ++){
        *out++ = *(++in);
        *out++ = *in++;
    }
}

