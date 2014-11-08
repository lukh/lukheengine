/*
 * audiocomponent.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: Vivien
 */

#include "audiocomponent.hpp"

AudioComponent::AudioComponent(uint8_t inBuffId, uint8_t outBuffId) :
    mInBufferId(inBuffId), mOutBufferId(outBuffId)
{
    uint8_t i;
    for(i = 0; i < 4 ; i++){
        mInFrameChannels[i] = 0;
        mOutFrameChannels[i] = 0;
    }
}
