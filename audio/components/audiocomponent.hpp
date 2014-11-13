/**
  ******************************************************************************
  * \file    AudioComponent.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    06/11/2014
  *

  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */


#ifndef AUDIOCOMPONENT_HPP_
#define AUDIOCOMPONENT_HPP_

/**
  \addtogroup AudioComponents
  @{
  */

#include "const.hpp"

/**
 * \brief   This is the abstraction Class for all users audio components
  \verbatim

  This abstraction class should be used for each audio component the user want to implement
  It handles a 2 channels (stereo) stream, with one input and one output buffer.
  Each buffer is interleaved (framesize = 2) and contain L/R samples

  User has to implement process, the audio stream processing method
  and update should be implemented to update component parameters, decide by the user.
  The process method will be call by the Engine each time it's needed by the Driver.

  */
class AudioComponent{
	public:
        /**
          * \param inStCh : uint8_t value, define the Id of the stereo input buffer
          * \param outStCh : uint8_t value, define the Id of the stereo output buffer
          */
        AudioComponent(uint8_t inStCh, uint8_t outStCh);


        virtual ~AudioComponent() {}


        /**
          * \brief process method (pure virtual) of AudioComponent. Handle audios buffers for processing
          * Process is called by the Engine::process or by its AudioComponent parent
          * \param in : pointer on a Sample input array
          * \param out : pointer on a Sample output array
          * \param framesPerBuffer : uint32_t value, give the number of frames to be processed
          * \note Must be implemented in the inherited class
          */
        virtual void process(Sample *in, Sample *out, uint32_t framesPerBuffer) = 0;

        /**
          * \brief Update a componenent parameters (pure virtual)
          * Update a component could be useful, to change somes parameters belonging to the component
          * (Filter Coefficients, Impulse response)
          * Or to ask him to do some tasks
          * (like, update data which be sent outside (a full buffer in order to make a FFT, waveforme...
          * Its purpose is to manage all the non critical and/or non related audio process to the Component
          * \note Call by the Engine::update or its parent.
          *
          */
        virtual void update() {}


        /**
          * \brief Get the Id of the Input stereo channel
          * \retval uint8_t mInStereoCh
          */
        inline const uint8_t getInStereoCh() const { return mInStereoCh; }


        /**
          * \brief Get the Id of the Output stereo channel
          * \retval uint8_t mOutStereoCh
          */
        inline const uint8_t getOutStereoCh() const { return mOutStereoCh; }


        /**
          * \brief set the Id of the Input stereo channel
          * should be used to update the component when loaded in another component (like a Track)
          * \param uint8_t ch, the new channel
          */
        void setInStereoCh(uint8_t ch);

        /**
          * \brief set the Id of the Output stereo channel
          * should be used to update the component when loaded in another component (like a Track)
          * \param uint8_t ch, the new channel
          */
        void setOutStereoCh(uint8_t ch);

    protected:
        /**
         * \brief the Id of the Input Channel
         * will be ignored if the AudioComponent is include in another (ex : track)
        */
        uint8_t mInStereoCh;

        /**
         * \brief the Id of the Output Channel
         * will be ignored if the AudioComponent is include in another (ex : track)
        */
        uint8_t mOutStereoCh;
};


/**
  }
  */

#endif /* AUDIOCOMPONENT_HPP_ */
