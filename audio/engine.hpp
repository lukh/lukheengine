/**
  ******************************************************************************
  * \file    engine.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    06/11/2014
  *
  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */


#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "const.hpp"
#include "audiocomponent.hpp"

class AbstractAudioDriver;

#define ENGINE_MAXAUDIOCOMPONENTS 16

/**
  \brief   The core of the audio processing
  It handles and ask components for processing, depending on their channels, and order.
  It controlled by the AudioDriver, through AbstractAudioDriver::process()

  The Engine::process method calls process methods of each AudioComponent,
  given in and out interleaved stereo buffer. These buffers are returned dependending on
  AudioComponent::mInStereoCh, AudioComponent::mOutStereoCh

  The Engine::process() method will, for each component :

  > select stereo buffers (in and out, 2ch interleaved) from the driver
 (throught AbstractAudioDriver::getBuffer)
  depending on the channels needed by the component (AudioComponent::getInStereoCh(), AudioComponent::getOutStereoCh() )

  > call the AudioComponent::process of the component, with the buffers given.

 */
class Engine{
	public:
        /**
         * Create a new Engine, connected to the AbstractAudioDriver
         * This driver is necessary to route and give the right buffers from the driver to the AudioComponent
         * \param driver it should be created
         */
        Engine(AbstractAudioDriver *driver);


        ~Engine();

        /**
         * The Engine::process() is called by the AbstractAudioDriver::process
         * For each valid AudioComponent, it called the process for this component after find the right buffer
         */
        void process();

        /**
         * Could be used to update parameters in the whole running engine. Handle by a callback ?
         */
        void update();


        /**
         * Add a new component to mAudioComponents regarding to the id
         * the component MUST be created before and the id MUST be valid (0 < ENGINE_MAXAUDIOCOMPONENTS)
         * \param id : the position in the list (0 < ENGINE_MAXAUDIOCOMPONENTS)
         * \param component
         * \retval LE_OK if success or LE_ERROR if not
         * \note : if the mAudioComponents[id] is not free, the component is not added
         */
        uint8_t setComponent(uint8_t id, AudioComponent *component);

	private:

        AbstractAudioDriver *mDriver;


        AudioComponent *mAudioComponents[ENGINE_MAXAUDIOCOMPONENTS];
};




#endif /* ABSTRACT_ENGINE_HPP_ */
