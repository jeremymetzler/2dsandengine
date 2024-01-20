#include "header_stack.h"


void SoundBase::LoadSound(ALbyte* filename)
{

    char* alBuffer = nullptr;             //data for the buffer
    ALenum alFormatBuffer = ALenum();    //buffer format
    ALsizei   alFreqBuffer = ALsizei();       //frequency
    ALsizei       alBufferLen = ALsizei();        //bit depth
    ALboolean    alLoop = ALboolean();         //loop

    unsigned int alSampleSet = 0;

    //load the wave file
    alutLoadWAVFile(filename, &alFormatBuffer, (void**)&alBuffer, &alBufferLen, &alFreqBuffer, &alLoop);

    //create a source
    alGenSources(1, &Index[soundTotal]);

    //create  buffer
    alGenBuffers(1, &alSampleSet);

    //put the data into our sampleset buffer
    alBufferData(alSampleSet, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);

    //assign the buffer to this source
    alSourcei(Index[soundTotal], AL_BUFFER, alSampleSet);

    //release the data
    alutUnloadWAV(alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);

    soundTotal += 1;
}

void SoundBase::PlaySoundO(int id, int x, int y, bool lcoop)
{
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    const float globalRefDistance = 12000000005.0f;
    const float globalMaxDistance = 1250000.0f;
    alSourcef(Index[id], AL_REFERENCE_DISTANCE, globalRefDistance);
    alSourcef(Index[id], AL_MAX_DISTANCE, globalMaxDistance);
    ALfloat alPos[] = { (x - I::baseClass->cameraPos.x) * 100000.f, (y - I::baseClass->cameraPos.y) * 1000000.f, 1000.0f };
    if (lcoop)
        alSourcei(Index[id], AL_LOOPING, lcoop);
    alSourcePlay(Index[id]);
    alSourcefv(Index[id], AL_POSITION, alPos);
}

void SoundBase::EndLoop(int id)
{
    alSourceStop(Index[id]);
}

void SoundBase::MoveSound(int id, float x, float y, float z)
{
    ALfloat alPos[3] = { (x - I::baseClass->cameraPos.x), (y - I::baseClass->cameraPos.y), z };
    alSourcefv(Index[id], AL_POSITION, alPos);
}
