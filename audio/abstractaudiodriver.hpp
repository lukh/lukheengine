/**
  ******************************************************************************
  * @file    abstractaudiodriver.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    06/11/2014
  * @brief   AbstractAudioDriver is the abstraction of the driver,
  * to be able to load the code on different targets, such computers or embedded
  *
  @verbatim
  ==============================================================================
                        ##### AbstractAudioDriver #####
  ==============================================================================
In order to load LuKHeEngine on a specific target, it is mandatory to create his own driver, and implement virtual methods :

  configure() : init peripherals, memory needed (buffers) depending on the target
  terminate() : stop properly the driver, and free memory, peripherals used
  start() : start processing
  stop() : stop processing

  The user has to add methods depending on the target, like a callback.
  This callback should access to the depending hardware buffer(s), reorganized if needed and copy samples in the mIn/OutBuffers
  Then, it has to call mDriver->process() to start signal processing.
  @endverbatim



  getBuffer(): return in out, and FramePerBuffer interleaved stereo buffer depending on the channels given.
  this method should be used in Engine::process, to be able to give to the AudioComponent::process the right stereo buffer.

  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */



#ifndef ABSTRACTAUDIDRIVER_HPP_
#define ABSTRACTAUDIDRIVER_HPP_

#include "const.hpp"

#define AUDIOCONF_MAX_OUTPUT_CHANNELS 4
#define AUDIOCONF_MAX_INPUT_CHANNELS 4


#include "engine.hpp"


/**
 * @brief The SampleRate enum
 */
enum SampleRate{
	SR44100 = 44100,
    SR96000 = 96000,
    SR192000 = 192000
};


/**
 * @brief The AudioDriverState enum
 */
enum AudioDriverState{
	NonInitialized,
	Initialized,
	Running
};



/**
 * @brief The AbstractAudioDriver class
 * An Abstract Class to generalize the driver
 */
class AbstractAudioDriver{
	public:

    /**
         * @brief AbstractAudioDriver
         * Create a new AbstractAudioDriver with SampleRate and FramePerBuffer
         * @param sr
         * @param fpb
         */
        AbstractAudioDriver(SampleRate sr, uint32_t fpb);

        /**
         * @brief ~AbstractAudioDriver
         */
        virtual ~AbstractAudioDriver() {}

        /**
         * @brief configure
         * Target depending, pure virtual method
         * It should :
         * (#) : configure the number of stereo input, output
         * (#) : set these numbers in the mNumStereoIn, mNumStereoOut
         * (#) : check and/or change the framePerBuffer if needed by the target/API/hardware
         * (#) : initialize the API/hardware
         * (#) : set mState at Initialized if success
         * @retval LE_OK if success, otherwise LE_ERROR
         */
        virtual uint8_t configure() = 0;

        /**
         * @brief terminate
         * Target depending, pure virtual method
         * It should
         * (#) de-init the hardware/API
         * (#) set the mState at NonInitialized
         * @retval LE_OK if success, otherwise LE_ERROR
         */
        virtual uint8_t terminate() = 0;


        /**
         * @brief start
         * Target depending, pure virtual method
         * It should
         * (#) start the API/hardware
         * (#) set the mState at Running
         * @retval LE_OK if success, otherwise LE_ERROR
         */
        virtual uint8_t start() = 0;



        /**
         * @brief stop
         * Target depending, pure virtual method
         * It should
         * (#) stop the API/hardware
         * (#) set the mState at Initialized
         * @retval LE_OK if success, otherwise LE_ERROR
         */
        virtual uint8_t stop() = 0;



        /**
         * @brief getBuffer
         * Allow acquierement of the buffers asked, with there ids, and the acquierement of the length of the frame
         * @param inStCh : the id of the stereo input buffer wanted
         * @param inBuffer : a pointer on the return input buffer pointer
         * @param outStCh : the id of the stereo output buffer wanted
         * @param outBuffer : a pointer on the return output buffer pointer
         * @param nBufferFrames : a pointer to return the effective number of frames
         * @retval LE_OK if the driver is able to provide the buffers, LE_ERROR
         */
        virtual uint8_t getBuffer(uint8_t inStCh, Sample **inBuffer, uint8_t outStCh, Sample**outBuffer, uint32_t *nBufferFrames);

        /**
         * @brief process
         * This process should be called by the implementation of the driver (in the callback for instance)
         * Its purpose is just to lauch the Engine::process
         */
        virtual void process() { mEngine->process(); }


        /**
         * @brief getState
         * @retval the state of the driver
         */
        inline AudioDriverState getState() const { return mState; }


        /**
         * @brief setEngine
         * @param eng. Should be already created
         * This method is called in the Engine Constructor
         * That mean you have to create the driver, then create the Engine with a pointer to the driver, and that's it !
         */
        void setEngine(Engine *eng);


        /**
         * @brief getFramesPerBuffer
         * @return
         */
        inline uint32_t getFramesPerBuffer() const {	return mFramesPerBuffer;}

        /**
         * @brief getSampleRate
         * @return
         */
        inline SampleRate getSampleRate() const { return mSampleRate;}


        /**
         * @brief getNumStereoIn
         * @return
         */
        inline uint16_t getNumStereoIn() const { return mNumStereoIn;}


        /**
         * @brief getNumStereoOut
         * @return
         */
        inline uint16_t getNumStereoOut() const {   return mNumStereoOut;}


        /** @addtogroup ImplementedDriverReserved
         * dedicaded for the implemented callback, engine and the rest of the framework should'nt used it
         * @{
         */

        /**
         * @brief setInBufferAddr
         * Set a new adress for the buffer mInBuffers[ id ]
         * Used by the implemented driver, specially the callback
         * to set the buffer which should be processed
         * @param id : the id of the mInBuffers
         * @param buff : the adress of the buffer to link
         */
        inline void setInBufferAddr(uint8_t id, Sample *buff) { mInBuffers[id] = buff; }


        /**
         * @brief setOutBufferAddr
         * Set a new adress for the buffer mOutBuffers[ id ]
         * Used by the implemented driver, specially the callback
         * to set the buffer which should be processed
         * @param id : the id of the OutnBuffers
         * @param buff : the adress of the buffer to link
         */
        inline void setOutBufferAddr(uint8_t id, Sample *buff) { mOutBuffers[id] = buff; }


        /**
         * @brief setMFramesPerBuffer
         * Set the effective number of frames acquiered
         * This should be used only by the implemented driver during the callback
         * to set the right number of frames
         * @param fpb
         */
        inline void setMFramesPerBuffer(uint32_t fpb) { mFramesPerBuffer = fpb; }

    protected:
        /**
         * @brief setNumStereoIn
         * @param numStereoIn
         */
        void setNumStereoIn(uint16_t numStereoIn);

        /**
         * @brief setNumStereoOut
         * @param numStereoOut
         */
        void setNumStereoOut(uint16_t numStereoOut);

        /**
          }
          */

    protected:  
        /**
         * @brief mSampleRate
         */
		SampleRate mSampleRate;

        /**
         * @brief mFramesPerBuffer
         */
        uint32_t mFramesPerBuffer;


        /**
         * @brief mNumStereoIn
         */
        uint16_t mNumStereoIn;

        /**
         * @brief mNumStereoOut
         */
        uint16_t mNumStereoOut;

        /**
         * @brief mState
         */
		AudioDriverState mState;

        /**
         * @brief mEngine
         */
        Engine *mEngine;

        /**
         * @brief mInBuffers
         */
        Sample *mInBuffers[AUDIOCONF_MAX_INPUT_CHANNELS];

        /**
         * @brief mOutBuffers
         */
        Sample *mOutBuffers[AUDIOCONF_MAX_OUTPUT_CHANNELS];
};


#endif /* AUDIOCONF_HPP_ */
