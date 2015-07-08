#include "AudioSource.h"
#include "GameObject.h"
#include "Transform.h"
#include "AudioInterface.h"

AudioSource::AudioSource(GameObject * parent, SoundEffect * clip, bool is3D) : Component(parent), m_clip(nullptr)
{
	m_transform = getParent()->getTransform();
	m_is3D = is3D && m_transform; // if no transform is available, simply default to non-3D sound

	setClip(clip);
}

AudioSource::~AudioSource()
{
	stop();
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
		m_clip = clip;

		if (m_clip) {
			SOUND_EFFECT_INSTANCE_FLAGS f = m_is3D ? SoundEffectInstance_Use3D : SoundEffectInstance_Default;
			m_instance = m_clip->CreateInstance(f);
			syncPos();
		} else {
			m_instance.reset();
		}
	}
}

void AudioSource::play(bool loop)
{
	if (m_instance) m_instance->Play(loop);
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
