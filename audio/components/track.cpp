/*
 * track.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: Vivien
 */

#include "track.hpp"

Track::Track(UInt8_t inStCh, UInt8_t outStCh) :
    AudioComponent(inStCh, outStCh)
{

}

Track::~Track(){

}

void Track::process(void *in, void *out, UInt32_t framesPerBuffer){
    (void *)in;
    (void *)out;
    framesPerBuffer;
}

void Track::update(){

}


