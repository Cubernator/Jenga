#pragma once
class Scene
{
private:
	virtual void update() { };

	friend class Engine;

public:
	Scene() { };
	virtual ~Scene() = 0;
};

