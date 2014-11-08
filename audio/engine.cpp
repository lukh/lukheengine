/*
 * engine.cpp
 *
 *  Created on: 25 mai 2014
 *      Author: Vivien
 */

#include "engine.hpp"

#include "abstractaudiodriver.hpp"


Engine::Engine(AbstractAudioDriver *driver) :
    mDriver(driver)
{
    uint8_t i;

    for(i = 0; i < ENGINE_MAXAUDIOCOMPONENTS; i ++)
        mAudioComponents[i] = (AudioComponent *)NULLPTR;

    mDriver->setEngine(this);
}


void Engine::process(){
    uint8_t i;

    Sample *in, *out;
    uint8_t inNumCh, outNumCh;
    uint32_t bufferSize;

    for(i = 0; i < ENGINE_MAXAUDIOCOMPONENTS; i ++){
        if(mAudioComponents[i] != (AudioComponent *)NULLPTR){
            //get buffers (in and out, buffsize) informations from the driver
            if(mDriver->getBuffer(mAudioComponents[i]->getInBufferId(), &in, &inNumCh, mAudioComponents[i]->getOutBufferId(), &out, &outNumCh, &bufferSize) == LE_OK){
                //start processing..
                mAudioComponents[i]->process(in, inNumCh, out, outNumCh, bufferSize);
            }
        }
    }
}


