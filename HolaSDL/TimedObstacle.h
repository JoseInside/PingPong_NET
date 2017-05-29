#pragma once
#include "GameStateObserver.h"
#include "TimedObstacleObserver.h"
#include "GameObject.h"
#include "RenderComponent.h"
class TimedObstacle : public GameObject, public GameStateObserver
{
public:
	TimedObstacle(SDLGame* game, int pTime, int dTime, GameObject* ball);
	virtual ~TimedObstacle();
	virtual void addObserver(TimedObstacleObserver* o);

	virtual void update();
	virtual void render();
	virtual void onCollision();
	virtual void obstacleActivated();
	virtual void handleInput(const SDL_Event& event) {};

private:

	virtual void onGameStart();
	virtual void onGameOver();
	virtual void onRoundStart();
	virtual void onRoundOver();

	RenderComponent* renderComp_;
	int timePassed, lastTick;
	int pTime_, dTime_;
	SDLGame* game_;
	GameObject* ball_;
	bool newGame, hit;
	std::vector<TimedObstacleObserver*> observadores_;
};

