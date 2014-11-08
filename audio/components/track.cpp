/*
 * track.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: Vivien
 */

#include "track.hpp"

Track::Track(uint8_t inStCh, uint8_t outStCh) :
    AudioComponent(inStCh, outStCh)
{
    uint8_t i;

    for(i = 0; i < ENGINE_MAXAUDIOCOMPONENTS; i ++)
        mAudioComponents[i] = (AudioComponent *)NULLPTR;
}

Track::~Track(){

}

void Track::process(void *in, void *out, uint32_t framesPerBuffer){
    (void *)in;
    (void *)out;
    framesPerBuffer;
}

void Track::update(){

}


