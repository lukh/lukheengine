/**
  ******************************************************************************
  * @file    engine.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    06/11/2014
  * @brief   Engine is the core of the audio processing
  * It contains all the user objects, AudioComponent (abstracted class)
  *
  @verbatim
  ==============================================================================
                        ##### Engine #####
  ==============================================================================

  The Engine::process method calls process method of each AudioComponent,
  given in and out interleaved stereo buffer. These buffers are returned dependending on
  AudioComponent::mInStereoCh, AudioComponent::mOutStereoCh

  @endverbatim
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */


#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "const.hpp"
#include "audiocomponent.hpp"

class AbstractAudioDriver;

#define ENGINE_MAXAUDIOCOMPONENTS 16

/*
 * Engine
 */
class Engine{
	public:
        Engine(AbstractAudioDriver *driver);
        ~Engine();

        void process();
        void update();

        uint8_t setComponent(uint8_t id, AudioComponent *component);

	private:
        /*
         * Relative to Engine
         */
        AbstractAudioDriver *mDriver;

		/*
		 * Relative to Tracks
		 */
        AudioComponent *mAudioComponents[ENGINE_MAXAUDIOCOMPONENTS];
};




#endif /* ABSTRACT_ENGINE_HPP_ */
