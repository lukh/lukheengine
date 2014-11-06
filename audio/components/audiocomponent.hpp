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
        AudioComponent(UInt8_t inStCh, UInt8_t outStCh);
        virtual ~AudioComponent() {}

        virtual void process(void *in, void *out, UInt32_t framesPerBuffer) = 0;
		virtual void update() = 0;

        inline UInt8_t getInStereoCh() const { return mInStereoCh; }
        inline UInt8_t getOutStereoCh() const { return mOutStereoCh; }

    private:
        UInt8_t mInStereoCh;
        UInt8_t mOutStereoCh;
};



#endif /* AUDIOCOMPONENT_HPP_ */
