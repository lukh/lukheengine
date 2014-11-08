/*
 * track.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: Vivien
 */

#include "track.hpp"

Track::Track(uint8_t inBufferId, uint8_t outBufferId) :
    AudioComponent(inBufferId, outBufferId)
{
    uint8_t i;

    for(i = 0; i < TRACK_MAXAUDIOCOMPONENTS; i ++)
        mAudioComponents[i] = (AudioComponent *)NULLPTR;
}

Track::~Track(){

}

void Track::process(void *in, uint8_t inNumCh, void *out, uint8_t outNumCh, uint32_t framesPerBuffer){
    uint8_t i;
    for(i = 0; i < TRACK_MAXAUDIOCOMPONENTS; i ++){
        if(mAudioComponents[i] != (AudioComponent *)NULLPTR){
            mAudioComponents[i]->process(in, inNumCh, out, outNumCh, framesPerBuffer);
        }
    }
}

void Track::update(){

}


