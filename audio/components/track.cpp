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


