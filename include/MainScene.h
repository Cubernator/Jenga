#pragma once
#include "PhysicsScene.h"

#include "Tower.h"
#include "Ground.h"
#include "Camera.h"
#include "DebugArrow.h"
#include "DebugPlane.h"
#include "GUIElement.h"
#include "ResultsMenu.h"
#include "PauseMenu.h"

#include <vector>
#include <memory>

class MainScene : public PhysicsScene
{
private:
	std::unique_ptr<Ground> m_ground;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Shader> m_brickShader, m_groundShader, m_debugShader;
	std::unique_ptr<IndexBuffer> m_brickIndices;
	std::unique_ptr<Texture2D> m_brickTex;
	ID3D11SamplerState * m_samplerState;

	std::unique_ptr<Tower> m_tower;

	std::unique_ptr<DebugArrow> m_springVisualizer;
	std::unique_ptr<DebugPlane> m_planeVisualizer;

	px_ptr<PxD6Joint> m_spring;

	PxVec3 m_planeOrigin, m_springPos;
	float m_maxSpringDist;
	bool m_controlMode, m_showDebug;

	Brick * m_pickedBrick;

	float m_camX, m_camY, m_camDist, m_camYAngle;
	float m_camYANormal, m_camYASteep;
	float m_xSens, m_ySens;

	bool m_togglePause, m_restart, m_backToMain;

	bool m_paused, m_roundOver;

	ComPtr<ID2D1Bitmap> m_pauseSymbol, m_playSymbol;
	std::unique_ptr<GUIButton> m_pauseButton;
	std::unique_ptr<PauseMenu> m_pauseMenu;

	std::unique_ptr<ResultsMenu> m_resultsMenu;

	void update() final;

	void tryPickBrick();
	void updateSpringPos();
	void updateCamPos();
	void releaseBrick();
	void setCamPos();

public:
	MainScene(PxSceneDesc desc);
	~MainScene();

	void brickFaulted();

	void togglePause();
	void restart();
	void backToMainMenu();
};

