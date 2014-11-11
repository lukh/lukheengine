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
    for(i = 0; i < TRACK_MAXAUDIOCOMPONENTS; i ++)
        mAudioComponents[i] = (AudioComponent *)NULLPTR;
}

Track::~Track(){

}

void Track::process(Sample *in, Sample *out, uint32_t framesPerBuffer){
    //Testing Track
    /*uint32_t i;
    for(i = 0; i < framesPerBuffer; i ++){
        *out++ = *(++in);
        *out++ = *in++;
    }*/

    uint8_t i;

    //the first component handle the input, the others are Fx
    if(mAudioComponents[0] != NULLPTR)
         mAudioComponents[0]->process(in, out, framesPerBuffer);

    for(i = 1; i < TRACK_MAXAUDIOCOMPONENTS; i ++){
        if(mAudioComponents[i] != NULLPTR)
            mAudioComponents[i]->process(out, out, framesPerBuffer);
    }
}

void Track::update(){

}


uint8_t Track::setComponent(uint8_t id, AudioComponent *component){
    if(id > TRACK_MAXAUDIOCOMPONENTS) return LE_ERROR;

    mAudioComponents[id] = component;

    return LE_OK;
}

