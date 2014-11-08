/*
 * audioconf.cpp
 *
 *  Created on: 28 juin 2014
 *      Author: Vivien
 */

#include "abstractaudiodriver.hpp"
#include "engine.hpp"

AbstractAudioDriver::AbstractAudioDriver(SampleRate sr, FramePerBuffer fpb) :
	mSampleRate(sr), mFramesPerBuffer(fpb),
    mNumInBuffers(0), mNumOutBuffers(0),
    mState(NonInitialized),
    mEngine((Engine*)NULLPTR)
{
    uint8_t i;
    for(i = 0; i < AUDIOCONF_MAX_INPUT_BUFFERS; i ++){
        mInNumBufferChannels[i] = 0;
        mInBuffers[i] = (Sample *)NULLPTR;
    }
    for(i = 0; i < AUDIOCONF_MAX_OUTPUT_BUFFERS; i ++){
        mOutNumBufferChannels[i] = 0;
        mOutBuffers[i] = (Sample *)NULLPTR;
    }
}


uint8_t AbstractAudioDriver::getBuffer(uint8_t inBufferId,Sample **inBuffer,uint8_t *inNumChannels, uint8_t outBufferId, Sample**outBuffer, uint8_t *outNumChannels, uint32_t *bufferSize){
    if(inBufferId >= AUDIOCONF_MAX_INPUT_BUFFERS || outBufferId >= AUDIOCONF_MAX_OUTPUT_BUFFERS)
        return LE_ERROR;

    *inBuffer = mInBuffers[inBufferId];
    *outBuffer = mOutBuffers[outBufferId];

    *inNumChannels = mInNumBufferChannels[inBufferId];
    *outNumChannels = mOutNumBufferChannels[outBufferId];

    *bufferSize = mFramesPerBuffer;

    return LE_OK;
}



void AbstractAudioDriver::setNumInBuffers(uint16_t numInBuffers){
    if(mState == Running) return;

    if(numInBuffers >= 0 && numInBuffers < AUDIOCONF_MAX_INPUT_BUFFERS)
    {
        mNumInBuffers = numInBuffers;
    }
}

void AbstractAudioDriver::setNumOutBuffers(uint16_t numOutBuffers){
    if(mState == Running) return;

    if(numOutBuffers >= 0 && numOutBuffers < AUDIOCONF_MAX_OUTPUT_BUFFERS)
    {
        mNumOutBuffers = numOutBuffers;
    }
}


void AbstractAudioDriver::setEngine(Engine *eng){
    if(eng == NULLPTR) return;

    mEngine = eng;
}



