#pragma once
#include "GameObject.h"

class TimedObstacleObserver
{
public:

	TimedObstacleObserver() {};
	virtual ~TimedObstacleObserver() {};
	
	virtual void onObstacleStateChange(GameObject* obs, bool state) = 0;
	virtual void onObstacleCollision(GameObject* obs, GameObject* o) = 0;



};

