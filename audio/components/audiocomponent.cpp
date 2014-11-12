/*
 * audiocomponent.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: Vivien
 */

#include "audiocomponent.hpp"

AudioComponent::AudioComponent(uint8_t inStCh, uint8_t outStCh) :
    mInStereoCh(inStCh), mOutStereoCh(outStCh)
{
}

void AudioComponent::setInStereoCh(uint8_t ch){
    //TODO : Add somes asserts on ch : ch < AUDIOCONF_MAXCHANNELS
    mInStereoCh = ch;
}

void AudioComponent::setOutStereoCh(uint8_t ch){
    //TODO : Add somes asserts on ch : ch < AUDIOCONF_MAXCHANNELS
    mOutStereoCh = ch;
}
