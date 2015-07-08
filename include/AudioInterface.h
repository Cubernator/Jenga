#pragma once

#include <Audio.h>

using namespace DirectX;

class AudioInterface
{
private:
	std::unique_ptr<AudioEngine> m_engine;

	AudioListener m_listener;

public:
	AudioInterface();
	~AudioInterface();

	void update();

	const AudioListener& getListener() const;

	SoundEffect * loadSoundEffect(const std::wstring& fileName);

	void suspend();
	void resume();
};

extern AudioInterface * audio;