#include "RTAudioDriver.hpp"


RtAudioDriver::RtAudioDriver() :
    AbstractAudioDriver(sampleRate, framesPerBuffer),
    rt(), inParams(), outParams()
{
    // Determine the number of devices available
   unsigned int devices = 1;//rt.getDeviceCount();
   // Scan through devices for various capabilities
   RtAudio::DeviceInfo info;
   for ( unsigned int i=0; i<devices; i++ ) {
       info = rt.getDeviceInfo( i );
       if ( info.probed == true ) {
           // Print, for example, the maximum number of output channels for each device
           std::cout << "device = " << i << "\n";
           std::cout << ": maximum output channels = " << info.outputChannels << "\n";
           std::cout << ": maximum input  channels = " << info.inputChannels << "\n";
           std::cout << ": maximum duplex channels = " << info.duplexChannels << "\n";
       }
   }

    if(rt.getDeviceCount() > 0){
         // Set the same number of channels for both input and output.
        inParams.deviceId = 0; // first available device
        inParams.nChannels = 2;
        outParams.deviceId = 0; // first available device
        outParams.nChannels = 2;
    }


}

RtAudioDriver::~RtAudioDriver(){
    terminate();
}

int RtAudioDriver::callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *data ){
    // avoid warning
    Sample *out = (Sample *)outputBuffer;
    Sample *in = (Sample *)inputBuffer;

    //Convert Engine..
    //Engine *engine = (Engine *)data;


    //Convert Data from hardware and fill mXBuffers
    //...
    //...

    //For the moment, just trying to put the in out...
    uint32_t i;
    for(i = 0; i < nBufferFrames; i++){
        *out++ = *(++in);
        *out++ = *in++;
    }

    //Call the Engine::process
    //engine->process();


    return 0;
}

uint8_t RtAudioDriver::configure(){
    uint32_t realBufferFrames = 256;

    std::cout << "RtAudioDriver::configure()" << "\n";

    try {
    rt.openStream( &outParams, &inParams, RTAUDIO_SINT32, 44100, &realBufferFrames, RtAudioDriver::callback, (void *)mEngine );
    }
    catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
    }

    mFramesPerBuffer = realBufferFrames;

    return LE_OK;
}

uint8_t RtAudioDriver::terminate(){
    std::cout << "AudioDriver::terminate\n";
    rt.closeStream();

    return LE_OK;
}

uint8_t RtAudioDriver::start(){
    rt.startStream();
    return LE_OK;
}

uint8_t RtAudioDriver::stop(){
    rt.stopStream();
    return LE_OK;
}

