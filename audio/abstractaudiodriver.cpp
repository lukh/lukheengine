/*
 * audioconf.cpp
 *
 *  Created on: 28 juin 2014
 *      Author: Vivien
 */

#include "abstractaudiodriver.hpp"

AbstractAudioDriver::AbstractAudioDriver(SampleRate sr, uint32_t fpb) :
	mSampleRate(sr), mFramesPerBuffer(fpb),
    mNumStereoIn(0), mNumStereoOut(0),
    mState(NonInitialized),
    mEngine((Engine*)NULLPTR)
{
}


uint8_t AbstractAudioDriver::getBuffer(uint8_t inStCh, Sample **inBuffer, uint8_t outStCh, Sample**outBuffer, uint32_t *bufferSize){
    if(inStCh >= AUDIOCONF_MAX_INPUT_CHANNELS || outStCh >= AUDIOCONF_MAX_OUTPUT_CHANNELS)
        return LE_ERROR;

    *inBuffer = mInBuffers[inStCh];
    *outBuffer = mOutBuffers[outStCh];

    *bufferSize = mFramesPerBuffer;

    return LE_OK;
}



void AbstractAudioDriver::setNumStereoIn(uint16_t numStereoIn){
    if(mState == Running) return;

    if(numStereoIn >= 0 && numStereoIn < AUDIOCONF_MAX_INPUT_CHANNELS)
    {
        mNumStereoIn = numStereoIn;
    }
}

void AbstractAudioDriver::setNumStereoOut(uint16_t numStereoOut){
    if(mState == Running) return;

    if(numStereoOut >= 0 && numStereoOut < AUDIOCONF_MAX_OUTPUT_CHANNELS)
    {
        mNumStereoOut = numStereoOut;
    }
}


void AbstractAudioDriver::setEngine(Engine *eng){
    if(eng == NULLPTR) return;

    mEngine = eng;
}



