/**
  ******************************************************************************
  * \file    Track.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    06/11/2014
  * \brief   Track implement a simple way to handle and route audio stream.
  *
  \verbatim
  ==============================================================================
                        ##### Track #####
  ==============================================================================

  Track inherits of AudioComponent, and is called by a process.

  It has stereo input and output, defined by id (like an audio component)

  It contains an array of AudioComponent*, which define the Track.

  The first AudioComponent will handle the input stream and the output stream
  The others components will handle the last output as an input

  Then, it is possible to create a Real Track, with a (for instance) sound generator (without using in buffer) and FX
  or only FX

  An AudioComponent in a Track doesn't care about its mInStereoCh and mOutStereoCh, because the
  Track::process() give directly the right buffer.


  \endverbatim
  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */

#ifndef TRACK_HPP_
#define TRACK_HPP_

#include "audiocomponent.hpp"

#define TRACK_MAXAUDIOCOMPONENTS 16

/**
 * \brief The TrackBroadcastType enum
 */
enum TrackBroadcastType{
	Mono,
	Stereo
};


/**
 * \brief The Track class
 */
class Track : public AudioComponent{
	public:
        /**
         * \brief Track constructor
         * Give its parameters to the AudioComponent consructor
         * \param inStCh
         * \param outStCh
         */
        Track(uint8_t inStCh, uint8_t outStCh);

        /**
          * \brief Track destructor
          */
		~Track();

        /**
         * \brief process
         * Process is called by the Engine::process or by its AudioComponent parent
         * It calls all the initialized components from mAudioComponents
         * the first component called, if exist, take the input buffer as input, and a output buffer as output
         * the others take the output buffer from the last component as input (serial processing)
         * \param in, Sample* pointer on the in buffer
         * \param out Sample*, pointer on the out buffer
         * \param framesPerBuffer
         */
        virtual void process(Sample *in, Sample *out, uint32_t framesPerBuffer);

        /**
         * \brief update implementation
         * Will be call by its parent's update (engine or another AudioComponent
         */
		virtual void update();


        /**
         * \brief setComponent
         * Add a component to the list regarding the id
         * The Component MUST be created and initialized, and the position must be valid (0 < TRACK_MAXAUDIOCOMPONENTS)
         * \param id : uint8_t, the position in the chain. This position MUST be (0 < TRACK_MAXAUDIOCOMPONENTS)
         * \note if the position is zero, the component will be the "sound generator" but it can handle the input as well
         * \param component*, a pointer on the component.
         * \retval LE_OK if success or LE_ERROR.
         * \note this method changes the channels of the component loaded, to be synchronise on the tracks channels (it's just to keep a logical structure !)
         * \note if the mAudioComponents[id] is not free, the component is not added
         */
        uint8_t setComponent(uint8_t id, AudioComponent *component);

	private:
        /**
         * \brief mAudioComponents
         * The list of the AudioComponent pointers
         */
        AudioComponent *mAudioComponents[TRACK_MAXAUDIOCOMPONENTS];

};


#endif /* TRACK_HPP_ */
