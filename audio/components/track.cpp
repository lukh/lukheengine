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
    //inputs param are ok
    if(id > TRACK_MAXAUDIOCOMPONENTS || component == (AudioComponent*)NULLPTR) return LE_ERROR;

    //slot is free
    if(mAudioComponents[id] != (AudioComponent *)NULLPTR) return LE_ERROR;

    //set the rights stereo channels, it is useless but just to keep something logical
    component->setInStereoCh(mInStereoCh);
    component->setOutStereoCh(mOutStereoCh);

    //add the component
    mAudioComponents[id] = component;

    return LE_OK;
}

