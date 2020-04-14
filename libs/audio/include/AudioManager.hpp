#pragma once

#include <chrono>
#include <thread>

/* OpenAL headers */
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>    /* for sqrtf() */
#include <time.h>    /* for time(), to seed srand() */

#include "AssetManager.hpp"

class AudioManager
{
public:
	AudioManager(std::shared_ptr<AssetManager> assetManager);
	~AudioManager();
	int demo();

private:
	std::shared_ptr<AssetManager> assetManager = nullptr;
};

