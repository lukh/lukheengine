/**
  ******************************************************************************
  * @file    AudioComponent.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    06/11/2014
  * @brief   This is the abstraction Class for all users audio components
  *
  @verbatim
  ==============================================================================
                        ##### AudioComponent #####
  ==============================================================================

  User has to implement process, the audio stream processing method
  and update should be implemented to update component parameters, decide by the user.


  @endverbatim
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */


#ifndef AUDIOCOMPONENT_HPP_
#define AUDIOCOMPONENT_HPP_

#include "const.hpp"

class AudioComponent{
	public:
        AudioComponent(uint8_t inBuffId, uint8_t outBuffId);
        //AudioComponent(AudioComponent &ac);
        virtual ~AudioComponent() {}

        virtual void process(void *in, uint8_t inNumCh, void *out, uint8_t outNumCh, uint32_t framesPerBuffer) = 0;
		virtual void update() = 0;

        inline uint8_t getInBufferId() const { return mInBufferId; }
        inline uint8_t getOutBufferId() const { return mOutBufferId; }

    private:
        uint8_t mInBufferId;
        uint8_t mInFrameChannels[4];

        uint8_t mOutBufferId;
        uint8_t mOutFrameChannels[4];
};



#endif /* AUDIOCOMPONENT_HPP_ */
