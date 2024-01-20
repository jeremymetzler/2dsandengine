#pragma once

enum SoundID
{
	DefaultSong,
	Explosion,
	Max
};


class SoundBase
{
public:
	std::string fileNames[SoundID::Max] = { "slowrollfull.wav", "tntboom.wav" };
	unsigned int Index[SoundID::Max];
    ALCcontext* context;
    ALCdevice* device;
    int soundTotal = 0;

	SoundBase()
	{

        device = alcOpenDevice(NULL);
        if (device == NULL)
        {
            // Handle Exception
        }

        //Create a context
        context = alcCreateContext(device, NULL);

        //Set active context
        alcMakeContextCurrent(context);

        // Clear Error Code
        alGetError();

        std::string fileName = "";
        ALbyte* d = new ALbyte[254];

        for (int i = 0; i < SoundID::Max; i++)
        {
            fileName = fileNames[i];
            d[fileName.size()] = '\0';
            for (int i = 0; i < fileName.size(); i++)
                d[i] = fileName[i];

           LoadSound(d);
        }

	}

    ~SoundBase()
    {
        //to stop
        for (int i = 0; i < SoundID::Max; i++)
        {
            alSourceStop(Index[i]);
        }

        alDeleteSources(soundTotal, Index);

        //Release context(s)
        alcDestroyContext(context);

        //Close device
        alcCloseDevice(device);
    }

	void LoadSound(ALbyte* filename);
	void PlaySoundO(int id, int x, int y, bool lcoop = false);
	void EndLoop(int id);
	void MoveSound(int id, float x, float y, float z);


};