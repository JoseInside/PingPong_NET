#include <string>
#include <cmath>
#include <random>

#include "GameManager.h"


GameManager::GameManager(SDLGame* game, GameObject* left_paddle, GameObject* right_paddle) :
		GameObject(game) {
	font_ = game_->getResources()->getFont(SDLGame::NESChimera16);
	color = { 255, 255, 255, 255 };
	startMsgTexture_.loadFromText(getGame()->getRenderer(),
			"Press Space to Start", *font_, color);

	wall_Hit = game->getResources()->getSoundEffect(SDLGame::Wall_Hit);
	paddle_Hit = game->getResources()->getSoundEffect(SDLGame::Paddle_Hit);
	points_LEFT = 0;
	points_RIGHT = 0;
	newGame = true;

	left_paddle_ = left_paddle;
	right_paddle_ = right_paddle;

}

GameManager::~GameManager() {
}

void GameManager::update() {

	pointsUI_.loadFromText(getGame()->getRenderer(),
		std::string(std::to_string(points_LEFT) + " ::: " + std::to_string(points_RIGHT)),
		*font_, color);


}

void GameManager::handleInput(const SDL_Event& event) {
	
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_SPACE) {
				if (newGame) {
					winnerText_.close();
					newGame = false;
					points_LEFT = 0;
					points_RIGHT = 0;

					for (auto i : observadores_) {
						i->onGameStart();
					}
					startMsgTexture_.close();
				}
				else {
					for (auto i : observadores_) {
						i->onRoundStart();
					}
					continueMsgTexture_.close();
				}
			}
		}
}

void GameManager::render() {

	// just an example, should be adjusted to the requirements!
	startMsgTexture_.render(getGame()->getRenderer(), (getGame()->getWindowWidth() - startMsgTexture_.getWidth()) / 2, getGame()->getWindowHeight() - 40);
	pointsUI_.render(getGame()->getRenderer(), (getGame()->getWindowWidth() - pointsUI_.getWidth()) / 2, getGame()->getWindowHeight() - 20);
	continueMsgTexture_.render(getGame()->getRenderer(), (getGame()->getWindowWidth()-continueMsgTexture_.getWidth())/ 2, getGame()->getWindowHeight()-40);
	winnerText_.render(getGame()->getRenderer(), (getGame()->getWindowWidth() - winnerText_.getWidth()) / 2, getGame()->getWindowHeight() - 40);
	bonusMsgTexture_.render(getGame()->getRenderer(), (getGame()->getWindowWidth() - bonusMsgTexture_.getWidth()) / 2, getGame()->getWindowHeight() - 40);
}

void GameManager::onCollision(GameObject* ball, GameObject* o) {
	last_paddle_hit_ = o;
	paddle_Hit->play();
}

void GameManager::onBorderExit(GameObject* ball, BallObserver::EXIT_SIDE side) {

	switch (side) {
	case TOP:
		wall_Hit->play();
		break;
	case BOT:
		wall_Hit->play();
		break;
	case LEFT:
		if (!leftObstacleActive) {
			points_RIGHT++;

			update();

			if (points_RIGHT == points_MAX) {
				winnerText_.loadFromText(game_->getRenderer(), "Right Player is the winner!", *font_, color);
				newGame = true;
				for (auto i : observadores_) {
					i->onGameOver();
				}
			}
			else {
				last_paddle_hit_ = nullptr;
				onObstacleStateChange(ball, false);
				for (auto i : observadores_) {
					i->onRoundOver();
				}
				continueMsgTexture_.loadFromText(getGame()->getRenderer(),
					"Press Space to Continue", *font_, color);
				render();
				//	handleInput();
			}
		}
		else {
			ball->setDirectionX(ball->getDirection().getX() * -1);
			wall_Hit->play();
		}
		break;
	case RIGHT:
		if (!rightObstacleActive) {
			points_LEFT++;

			update();

			if (points_LEFT == points_MAX) {
				winnerText_.loadFromText(game_->getRenderer(), "Left Player is the winner!", *font_, color);
				newGame = true;
				for (auto i : observadores_) {
					i->onGameOver();
				}
			}
			else {
				last_paddle_hit_ = nullptr;
				onObstacleStateChange(ball, false);
				for (auto i : observadores_) {
					i->onRoundOver();
				}
				continueMsgTexture_.loadFromText(getGame()->getRenderer(),
					"Press Space to Continue", *font_, color);
				render();
				//	handleInput(event)
			}
		}
		else {
			ball->setDirectionX(ball->getDirection().getX() * -1);
			wall_Hit->play();
		}
		break;
	}
}

void GameManager::registerGameStateObserver(GameStateObserver* o) {
	observadores_.push_back(o);
}

void GameManager::onObstacleCollision(GameObject* obs, GameObject* o) {
	
	if (last_paddle_hit_ == left_paddle_) {
		bonusMsgTexture_.loadFromText(getGame()->getRenderer(),
			"Left Player has a wall bonus!", *font_, color);
		leftObstacleActive = true;	
		rightObstacleActive = false;
	}
	if(last_paddle_hit_ == right_paddle_){
		bonusMsgTexture_.loadFromText(getGame()->getRenderer(),
			"Right Player has a wall bonus!", *font_, color);
		leftObstacleActive = false;
		rightObstacleActive = true;
	}
	wall_Hit->play();
}

void GameManager::onObstacleStateChange(GameObject* obs, bool state) {
	
	if (!state) {
		leftObstacleActive = false;
		rightObstacleActive = false;
		bonusMsgTexture_.close();
		//obs->setActive(false);
	}
}