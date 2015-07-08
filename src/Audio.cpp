#include "Audio_.h"

AudioInterface * audio;

AudioInterface::AudioInterface()
{
	audio = this;

	AUDIO_ENGINE_FLAGS f = AudioEngine_Default;

#ifdef _DEBUG
	f = f | AudioEngine_Debug;
#endif

	m_engine.reset(new AudioEngine(f));
}

void AudioInterface::update()
{
	m_engine->Update();
}