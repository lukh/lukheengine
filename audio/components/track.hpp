/*
 * track.hpp
 *
 *  Created on: 25 mai 2014
 *      Author: Vivien
 */

#ifndef TRACK_HPP_
#define TRACK_HPP_

#include "audiocomponent.hpp"

#define TRACK_MAXAUDIOCOMPONENTS 16

enum TrackBroadcastType{
	Mono,
	Stereo
};



class Track : AudioComponent{
	public:
        Track(uint8_t inBufferId, uint8_t outBufferId);
		~Track();

        virtual void process(void *in, uint8_t inNumCh, void *out, uint8_t outNumCh, uint32_t framesPerBuffer);
		virtual void update();

	private:
        AudioComponent *mAudioComponents[TRACK_MAXAUDIOCOMPONENTS];

};


#endif /* TRACK_HPP_ */
