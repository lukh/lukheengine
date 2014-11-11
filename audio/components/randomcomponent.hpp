#ifndef RANDOMCOMPONENT_H
#define RANDOMCOMPONENT_H

#include "audiocomponent.hpp"

class RandomComponent : public AudioComponent
{
    public:
        RandomComponent(uint8_t inStCh, uint8_t outStCh); //In the case of using in a track, inand out Ch are not used
        ~RandomComponent() {}

        virtual void process(Sample *in, Sample *out, uint32_t framesPerBuffer);
        virtual void update();
};

#endif // RANDOMCOMPONENT_H
