#include <cmath>
#include <random>
#include "PingPongPhysics.h"

PingPongPhysics::PingPongPhysics(GameObject* left_paddle,
		GameObject* right_paddle) :
		left_paddle_(left_paddle), right_paddle_(right_paddle), hits_(
				0), speed_(0) {
}

PingPongPhysics::~PingPongPhysics() {
}

void PingPongPhysics::init(GameObject* o) {
	onGameStart();
}

void PingPongPhysics::update(GameObject* o) {

	if (!newGame) {
		//Reset 
		o->setPosition(
			o->getGame()->getWindowWidth() / 2 - o->getWidth() / 2,
			o->getGame()->getWindowHeight() / 2 - o->getHeight() / 2);
		
		int direction = 1 - 2 * (rand() % 2);
		int angle = 60 - (rand() % 121);
		int dx = direction * speed_ * std::cos(angle * M_PI / 180.0f);
		int dy = speed_ * std::sin(angle * M_PI / 180.0f);

		o->setDirection(dx, dy);
		//o->setDirection(0, 0);

		left_paddle_->setPosition(30,
			(left_paddle_->getGame()->getWindowHeight() - left_paddle_->getHeight()) / 2);
		left_paddle_->setDirection(0, 0);
		
		right_paddle_->setPosition(right_paddle_->getGame()->getWindowWidth() - 40,
			(right_paddle_->getGame()->getWindowHeight() - right_paddle_->getHeight()) / 2);
		right_paddle_->setDirection(0, 0);

		newGame = false;
	}
	else {
		
		//Next ball's position
		Vector2D<int> nextPos = o->getPosition() + o->getDirection();

		//COLLISION WITH BORDERS
		if (nextPos.getY() <= 0) {

			nextPos.setY(0);
			o->setPosition(nextPos);
			o->setDirectionY(-1 * o->getDirection().getY());

			for (auto i : observadores_) {
				i->onBorderExit(o, BallObserver::EXIT_SIDE::TOP);;
			}
		}
		else if (nextPos.getY()
			>= o->getGame()->getWindowHeight() - o->getHeight()) {

			nextPos.setY(o->getGame()->getWindowHeight() - o->getHeight());
			o->setDirectionY(-1 * o->getDirection().getY());

			for (auto i : observadores_) {
				i->onBorderExit(o, BallObserver::EXIT_SIDE::BOT);;
			}
		}
		else if (nextPos.getX() <= 0) {

			for (auto i : observadores_) {
				i->onBorderExit(o, BallObserver::EXIT_SIDE::LEFT);;
			}
		}
		else if (nextPos.getX()
			>= o->getGame()->getWindowWidth() - o->getWidth()) {

			for (auto i : observadores_) {
				i->onBorderExit(o, BallObserver::EXIT_SIDE::RIGHT);;
			}
		}

		//COLLISION WITH PADDLES 
		if (o->getPosition().getX() < left_paddle_->getPosition().getX() + left_paddle_->getWidth() &&
			o->getPosition().getX() > left_paddle_->getPosition().getX() &&
			o->getPosition().getY() < left_paddle_->getPosition().getY() + left_paddle_->getHeight() &&
			o->getPosition().getY() > left_paddle_->getPosition().getY()) {

			//Collision happened
			++hits_;
			if (hits_ == 5) {
				hits_ = 0;
				speed_++;
			}
			for (auto i : observadores_) {
				i->onCollision(o, left_paddle_);
			}
			int sign = o->getDirection().getX() < 0 ? 1 : -1;
			int rel_y = (o->getPosition().getY() - left_paddle_->getPosition().getY() + o->getHeight());
			float angle = (2.14f * rel_y - 75.0f);
			int dx = sign * speed_ * std::cos(angle * M_PI / 180.0f);
			int dy = speed_ * std::sin(angle * M_PI / 180.0f);

			o->setDirection(dx, dy);

		}
		else if (o->getPosition().getX() < right_paddle_->getPosition().getX() + right_paddle_->getWidth() &&
			o->getPosition().getX() > right_paddle_->getPosition().getX() &&
			o->getPosition().getY() < right_paddle_->getPosition().getY() + right_paddle_->getHeight() &&
			o->getPosition().getY() > right_paddle_->getPosition().getY()) {

			//Collision happened
			++hits_;
			if (hits_ == 5) {
				hits_ = 0;
				speed_++;
			}
			for (auto i : observadores_) {
				i->onCollision(o, right_paddle_);
			}

			int sign = o->getDirection().getX() < 0 ? 1 : -1;
			int rel_y = (o->getPosition().getY() - right_paddle_->getPosition().getY() + o->getHeight());
			float angle = (2.14f * rel_y - 75.0f);
			int dx = sign * speed_ * std::cos(angle * M_PI / 180.0f);
			int dy = speed_ * std::sin(angle * M_PI / 180.0f);

			o->setDirection(dx, dy);
		}

		nextPos = o->getPosition() + o->getDirection();

		o->setPosition(nextPos);
	}
}


void PingPongPhysics::resgisterBallObserver(BallObserver* o) {
	observadores_.push_back(o);
}


void PingPongPhysics::onGameStart() {
	hits_ = 0;
	speed_ = 8;

	newGame = false;
}

void PingPongPhysics::onGameOver() {
	newGame = false;
}

void PingPongPhysics::onRoundStart() {
	hits_ = 0;
	speed_ = 8;
	
	newGame = true;
}

void PingPongPhysics::onRoundOver() {
	newGame = false;
}

