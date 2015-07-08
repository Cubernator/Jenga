#pragma once

#include <Audio.h>

using namespace DirectX;

class AudioInterface
{
private:
	std::unique_ptr<AudioEngine> m_engine;

public:
	AudioInterface();

	void update();
};

extern AudioInterface * audio;