#pragma once

#include "PhysicsScene.h"

#include "Tower.h"
#include "Background.h"
#include "Ground.h"
#include "Camera.h"
#include "DebugArrow.h"
#include "DebugPlane.h"
#include "GUIElement.h"
#include "ResultsMenu.h"
#include "PauseMenu.h"
#include "MainMenu.h"
#include "ScoreCounter.h"

#include <vector>
#include <memory>

class MainScene : public PhysicsScene
{
private:
	bool m_specialMode;

	unsigned int m_seed;

	std::unique_ptr<Background> m_background0, m_background1, m_background2, m_background3;
	std::unique_ptr<Ground> m_ground;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Shader> m_groundShader, m_backgroundShader, m_brickShader, m_debugShader;
	std::unique_ptr<IndexBuffer> m_brickIndices;

	std::unique_ptr<Tower> m_tower;

	px_ptr<PxMaterial> m_wallMat;
	px_ptr<PxRigidStatic> m_wall1, m_wall2, m_wall3, m_wall4, m_ceiling;

	std::unique_ptr<DebugArrow> m_springVisualizer;
	std::unique_ptr<DebugPlane> m_planeVisualizer;

	px_ptr<PxD6Joint> m_spring;

	PxVec3 m_planeOrigin, m_springPos;
	const float m_maxSpringDist;
	bool m_controlMode, m_showDebug;

	Brick *m_pickedBrick;

	bool m_highlight;

	float m_camX, m_camY, m_camDist, m_camYAngle;
	const float m_camYANormal, m_camYASteep;
	const float m_xSens, m_ySens;

	bool m_togglePause, m_restart, m_backToMain;

	bool m_paused, m_canPause, m_roundOver;

	std::unique_ptr<PowerupManager> m_powerupManager;
	std::unique_ptr<ScoreCounter> m_scoreCounter;

	ComPtr<ID2D1Bitmap> m_pauseSymbol, m_playSymbol;
	std::unique_ptr<GUIButton> m_pauseButton;
	std::unique_ptr<PauseMenu> m_pauseMenu;

	std::unique_ptr<ResultsMenu> m_resultsMenu;

	std::unique_ptr<SeedPrompt> m_seedPrompt;

	std::unique_ptr<MainMenu> m_mainMenu;

	void createInvWalls();

	void update() final;

	void tryPickBrick();
	void updateSpringPos();
	void updateCamPos();
	void releaseBrick();
	void setCamPos();

	void stop();
	void togglePauseInt();

public:
	MainScene(PxSceneDesc desc);
	~MainScene();

	void startRound(bool specialMode, unsigned int seed);

	PowerupManager * getPowerupManager();
	ScoreCounter * getScoreCounter();

	Tower * getTower();

	Brick * getSelectedBrick();
	void quickPlace();

	void activateHighlight();

	unsigned int getSeed() const;
	bool getSpecialMode() const;

	void brickFaulted();

	void setCanPause(bool canPause);
	void setPaused(bool paused);
	void restart();
	void backToMainMenu();
	void togglePause();
};

