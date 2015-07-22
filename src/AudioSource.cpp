#include "AudioSource.h"
#include "GameObject.h"
#include "Transform.h"
#include "AudioInterface.h"

AudioSource::AudioSource(GameObject * parent, SoundEffect * clip, bool is3D) : Component(parent), m_clip(nullptr), m_instance(nullptr)
{
	m_transform = getParent()->getTransform();
	m_is3D = is3D && m_transform; // if no transform is available, simply default to non-3D sound

	setClip(clip);
}

AudioSource::~AudioSource()
{
	stop();
}

SoundState AudioSource::getSoundState() const
{
	if (m_instance) {
		return m_instance->GetState();
	}

	return STOPPED;
}

void AudioSource::syncPos()
{
	if (m_is3D) {
		m_emitter.SetPosition((XMFLOAT3&)m_transform->getPosition());
		m_emitter.SetOrientationFromQuaternion(XMLoadFloat4((XMFLOAT4*)&m_transform->getRotation()));

		if (m_instance) m_instance->Apply3D(audio->getListener(), m_emitter);
	}
}

void AudioSource::setClip(SoundEffect * clip)
{
	if (clip != m_clip) {
		stop();

		m_clip = clip;

		if (m_clip) {
			std::unique_ptr<SoundEffectInstance>& inst = m_cache[m_clip];
			if (!inst) {
				SOUND_EFFECT_INSTANCE_FLAGS f = m_is3D ? SoundEffectInstance_Use3D : SoundEffectInstance_Default;
				inst = m_clip->CreateInstance(f);
			}
			m_instance = inst.get();
			syncPos();
		} else {
			m_instance = nullptr;
		}
	}
}

void AudioSource::play(bool loop)
{
	try {
		if (m_instance) m_instance->Play(loop);
	} catch (std::exception&) { }
}

void AudioSource::stop(bool immediate)
{
	if (m_instance) m_instance->Stop(immediate);
}

void AudioSource::pause()
{
	if (m_instance) m_instance->Pause();
}

void AudioSource::resume()
{
	if (m_instance) m_instance->Resume();
}

void AudioSource::setVolume(float volume)
{
	if (m_instance) m_instance->SetVolume(volume);
}
