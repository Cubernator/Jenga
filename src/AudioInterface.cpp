#include "AudioInterface.h"
#include "Graphics.h"
#include "Camera.h"
#include "Objects.h"
#include "AudioSource.h"

AudioInterface * audio;

AudioInterface::AudioInterface()
{
	audio = this;

	AUDIO_ENGINE_FLAGS f = AudioEngine_UseMasteringLimiter;

#ifdef _DEBUG
	f = f | AudioEngine_Debug;
#endif

	m_engine.reset(new AudioEngine(f));
}

AudioInterface::~AudioInterface()
{
	m_engine->Suspend();
}

void AudioInterface::update()
{
	Camera * cam = graphics->getCamera();

	if (cam) {
		Transform * camTrans = cam->getTransform();
		if (camTrans) {
			m_listener.SetPosition((XMFLOAT3&)camTrans->getPosition());
			m_listener.SetOrientationFromQuaternion(XMLoadFloat4((XMFLOAT4*)&camTrans->getRotation()));
		}
	}

	for (GameObject * obj : objects->m_objects) {
		AudioSource * s = obj->getAudioSource();
		if (s) {
			s->syncPos();
		}
	}

	m_engine->Update();
}

void AudioInterface::suspend()
{
	m_engine->Suspend();
}

void AudioInterface::resume()
{
	m_engine->Resume();
}

void AudioInterface::setMasterVolume(float volume)
{
	m_engine->SetMasterVolume(volume);
}

SoundEffect * AudioInterface::loadSoundEffect(const std::wstring& fileName)
{
	return new SoundEffect(m_engine.get(), fileName.c_str());
}

const AudioListener& AudioInterface::getListener() const
{
	return m_listener;
}