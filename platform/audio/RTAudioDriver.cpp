#include "RTAudioDriver.hpp"


RtAudioDriver::RtAudioDriver() :
    AbstractAudioDriver(sampleRate, framesPerBuffer)
{

    if(rt.getDeviceCount() > 0){
         // Set the same number of channels for both input and output.
        inParams.deviceId = 0 ; // first available device
        inParams.nChannels = 2;
        outParams.deviceId = 0;
        outParams.nChannels = 2; // first available device
    }


}

RtAudioDriver::~RtAudioDriver(){

}

int RtAudioDriver::callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *data ){
    // avoid warning
    (void *)outputBuffer;
    (void *)inputBuffer;
    nBufferFrames;
    streamTime;
    status;

    //Convert Engine..
    Engine *engine = (Engine *)data;



    //Convert Data from hardware and fill mXBuffers
    //...
    //...

    //Call the Engine::process
    engine->process();


    return 0;
}

uint8_t RtAudioDriver::configure(){
    uint32_t realBufferFrames;

    try {
    rt.openStream( &outParams, &inParams, RTAUDIO_FLOAT64, SR44100, &realBufferFrames, RtAudioDriver::callback, (void *)mEngine );
    }
    catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
    }

    mFramesPerBuffer = realBufferFrames;

    return LE_OK;
}

uint8_t RtAudioDriver::terminate(){
    return LE_OK;
}

uint8_t RtAudioDriver::start(){
    return LE_OK;
}

uint8_t RtAudioDriver::stop(){
    return LE_OK;
}

