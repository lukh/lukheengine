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
        Track(uint8_t inStCh, uint8_t outStCh);
		~Track();

        virtual void process(void *in, void *out, uint32_t framesPerBuffer);
		virtual void update();

	private:
        AudioComponent *mAudioComponents[TRACK_MAXAUDIOCOMPONENTS];

};


#endif /* TRACK_HPP_ */
