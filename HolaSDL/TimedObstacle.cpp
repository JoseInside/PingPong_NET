#include "TimedObstacle.h"
#include "RectRender.h"


TimedObstacle::TimedObstacle(SDLGame* game, int pTime, int dTime, GameObject* ball) : GameObject(game)
{
	timePassed = 0;
	lastTick = 0;
	game_ = game;
	pTime_ = pTime;
	dTime_ = dTime;
	ball_ = ball;
	newGame = false;
	hit = false;
	
	SDL_Color color = { 0, 255, 255, 255 };
	renderComp_ = new RectRender(color);
	setActive(false);
}


TimedObstacle::~TimedObstacle()
{
}

void TimedObstacle::addObserver(TimedObstacleObserver* o) {
	observadores_.push_back(o);

}

void TimedObstacle::update() {
	
	if (newGame) {
		lastTick = SDL_GetTicks() - timePassed;
		if (lastTick > pTime_ && !isActive()) {
			if (rand() % 2 == 0) {
				setWidth(10);
				setHeight(150 + rand() % 50);
				setPositionX(game_->getWindowWidth() / 2 - 100 + rand() % 200);
				setPositionY(rand() % (game_->getWindowHeight() - getHeight()));
				setActive(true);
				obstacleActivated();
			}
				timePassed += lastTick;
		}
		else if (isActive()) {
			
			if (lastTick > dTime_) {
				setActive(false);
				obstacleActivated();
				timePassed += lastTick;
			}

			else if (!hit) {
				//hit = true;
				onCollision();
				//timePassed += lastTick;
			}
			if(hit) {
				setActive(false);
				hit = false;
				if (lastTick > dTime_) {	
					obstacleActivated();
				}
				//timePassed += lastTick;
			}			
		}
	
	}
	
}

void TimedObstacle::onCollision() {

	if (ball_->getPosition().getX() < this->getPosition().getX() + this->getWidth() &&
		ball_->getPosition().getX() > this->getPosition().getX() &&
		ball_->getPosition().getY() < this->getPosition().getY() + this->getHeight() &&
		ball_->getPosition().getY() > this->getPosition().getY()) {
		
		hit = true;
		ball_->setDirectionX(ball_->getDirection().getX() * -1);
		for (auto o : observadores_)
			o->onObstacleCollision(this, ball_);
	}
	
}

void TimedObstacle::obstacleActivated() {
	
	for (auto o : observadores_)
		o->onObstacleStateChange(this, isActive());
}


void TimedObstacle::onGameStart() {
	newGame = true;
	setActive(false);
}
void TimedObstacle::onGameOver() {
	newGame = false;
	setActive(false);
	//visible = false;
}
void TimedObstacle::onRoundStart() {
	newGame = true;
	setActive(false);
}
void TimedObstacle::onRoundOver() {
	newGame = false;
	setActive(false);
	//visible = false;
}

void TimedObstacle::render() {
	if (isActive())
		renderComp_->render(this);
}