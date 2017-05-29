#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include "SDLGame.h"
#include "GameObject.h"
#include "BallObserver.h"
#include "GameStateObserver.h"
#include "Font.h"
#include "Texture.h"
#include "TimedObstacleObserver.h"
class GameManager: public GameObject, public BallObserver, public TimedObstacleObserver {

public:
	GameManager(SDLGame* game, GameObject* left_paddle, GameObject* right_paddle);
	virtual ~GameManager();

	virtual void registerGameStateObserver(GameStateObserver* o);

	// from GameObject
	virtual void handleInput(const SDL_Event& e);
	virtual void update();
	virtual void render();
//	virtual void render(Texture type);

	// from BallObserver
	virtual void onCollision(GameObject* ball, GameObject* o);
	virtual void onBorderExit(GameObject* ball, BallObserver::EXIT_SIDE side);

	void onObstacleCollision(GameObject* obs, GameObject* o);
	void onObstacleStateChange(GameObject* obs, bool state);

private:
	Font* font_;
	Texture startMsgTexture_;
	Texture continueMsgTexture_;
	Texture bonusMsgTexture_;
	Texture winnerText_;
	Texture pointsUI_;
	SDL_Color color;

	bool newGame, leftObstacleActive, rightObstacleActive;

	int points_LEFT;
	int points_RIGHT;
	const int points_MAX = 5;

	GameObject* left_paddle_;
	GameObject* right_paddle_;
	GameObject* last_paddle_hit_;

	SoundEffect* wall_Hit;
	SoundEffect* paddle_Hit;
	std::vector<GameStateObserver*> observadores_;

};

#endif /* SCOREBOARD_H_ */
