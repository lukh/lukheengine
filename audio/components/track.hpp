/*
 * track.hpp
 *
 *  Created on: 25 mai 2014
 *      Author: Vivien
 */

#ifndef TRACK_HPP_
#define TRACK_HPP_

#include "audiocomponent.hpp"

/*enum TrackBroadcastType{
	Mono,
	Stereo
};*/



class Track : AudioComponent{
	public:
        Track(UInt8_t inStCh, UInt8_t outStCh);
		~Track();

        virtual void process(void *in, void *out, UInt32_t framesPerBuffer);
		virtual void update();

	private:


};


#endif /* TRACK_HPP_ */
