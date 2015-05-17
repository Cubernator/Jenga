#pragma once

class GameObject;

class Component
{
private:
	GameObject * const m_parent;
	bool m_enabled;

public:
	Component(GameObject * parent);
	virtual ~Component() = 0;

	GameObject * getParent();
	const GameObject * getParent() const;

	bool getEnabled() const;
	void setEnabled(bool e);
};

