#pragma once

#include <chrono>
#include <thread>

/* OpenAL headers */
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

class AudioObject
{
public:
	AudioObject() {}
	//~AudioObject();
    unsigned int GetID { return id; }
private:
    unsigned int id;
};

