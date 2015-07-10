#pragma once

#include <Audio.h>
#include "Component.h"

#include <unordered_map>

using namespace DirectX;

class Transform;

class AudioSource : public Component
{
private:
	AudioEmitter m_emitter;
	bool m_is3D;

	SoundEffect * m_clip;
	SoundEffectInstance * m_instance;

	std::unordered_map<SoundEffect*, std::unique_ptr<SoundEffectInstance>> m_cache;

	Transform * m_transform;

	void syncPos();

	friend class AudioInterface;

public:
	AudioSource(GameObject * parent, SoundEffect * clip = nullptr, bool is3D = true);
	~AudioSource();

	void setClip(SoundEffect * clip);

	SoundState getSoundState() const;

	void play(bool loop = false);
	void stop(bool immediate = true);
	void pause();
	void resume();
};