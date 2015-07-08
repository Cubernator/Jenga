#pragma once

#include <Audio.h>
#include "Component.h"

using namespace DirectX;

class Transform;

class AudioSource : public Component
{
private:
	std::unique_ptr<SoundEffectInstance> m_instance;
	AudioEmitter m_emitter;
	bool m_is3D;

	SoundEffect * m_clip;

	Transform * m_transform;

	void syncPos();

	friend class AudioInterface;

public:
	AudioSource(GameObject * parent, SoundEffect * clip = nullptr, bool is3D = true);
	~AudioSource();

	void setClip(SoundEffect * clip);

	void play(bool loop = false);
	void stop(bool immediate = true);
	void pause();
	void resume();
};