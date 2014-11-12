/**
  ******************************************************************************
  * \file    RandomComponent.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    06/11/2014
  * \brief   Just a useless component, working as a bypass to test the whole engine
  *
  \verbatim
  ==============================================================================
                        ##### RandomComponent #####
  ==============================================================================



  \endverbatim
  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */

#ifndef RANDOMCOMPONENT_H
#define RANDOMCOMPONENT_H

#include "audiocomponent.hpp"

/**
 * \brief The RandomComponent class
 */
class RandomComponent : public AudioComponent
{
    public:
        /**
          * \brief Constructor of RandomComponent
          * \param inStCh : uint8_t value, define the Id of the stereo input buffer
          * \param outStCh : uint8_t value, define the Id of the stereo output buffer
          */
        RandomComponent(uint8_t inStCh, uint8_t outStCh); //In the case of using in a track, inand out Ch are not used

        /**
          * \brief Destructor of RandomComponent
          */
        ~RandomComponent() {}

        /**
          * \brief Implementation of the process
          * \param in : pointer on a Sample input array
          * \param out : pointer on a Sample output array
          * \param framesPerBuffer : uint32_t value, give the number of frames to be processed
          * \note Process is called by the Engine::process or by its AudioComponent parent
          * \note Nothing special about the processing, it is just a bypass
          */
        virtual void process(Sample *in, Sample *out, uint32_t framesPerBuffer);
};

#endif // RANDOMCOMPONENT_H
