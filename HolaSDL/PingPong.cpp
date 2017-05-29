#include "PingPong.h"
#include "BounceOnBorders.h"
#include "RectRender.h"
#include "ImageRendered.h"
#include "PaddleKeyboardInputComp.h"
#include "PaddleMouseInputComp.h"
#include "StopOnBordersPhysics.h"
#include "PaddleAIPhysics.h"


PingPong::PingPong() :
		SDLGame("Ping Pong", _WINDOW_WIDTH_, _WINDOW_HEIGHT_) {
	initGame();
	exit_ = false;
}

PingPong::~PingPong() {
	closeGame();
}

void PingPong::initGame() {

	// hide cursor
	SDL_ShowCursor(0);

	SDL_Color color = { 255, 255, 255, 255 };

	rectangleRenderer_ = new RectRender(color);
	imageRenderer_ = new ImageRendered( getResources()->getTexture(SDLGame::TennisBall) );
	keyIcon_ = new ImageRendered(getResources()->getTexture(SDLGame::KeyBoardIcon));
	mouseIcon_ = new ImageRendered(getResources()->getTexture(SDLGame::MouseIcon));
	AIIcon_ = new ImageRendered(getResources()->getTexture(SDLGame::AIIcon));
	//bounceOnBorderPhysics_ = new BounceOnBorders(true, true, true, true);
	
	
	// ball
	ball_ = new GameComponent(this);
	ball_->setWidth(10);
	ball_->setHeight(10);
	ball_->setDirection((rand() % 5) + 3, (rand() % 5) + 3); // change to  (0,0) at the end
	ball_->setPosition(
		ball_->getGame()->getWindowWidth() / 2 - ball_->getWidth() / 2,
		ball_->getGame()->getWindowHeight() / 2 - ball_->getHeight() / 2);
	ball_->setRenderComponent(rectangleRenderer_);
	//ball_->setPhysicsComponent(bounceOnBorderPhysics_);
	// use the following for an image of a tennis ball
	// ball_->setRenderComponent(imageRenderer_);


	// left paddle
	left_paddle_ = new GameComponent(this);
	left_paddle_->setWidth(10);
	left_paddle_->setHeight(60);
	left_paddle_->setPosition(30,
			(getWindowHeight() - left_paddle_->getHeight()) / 2);
	left_paddle_->setDirection(0, 0);
	left_paddle_->setRenderComponent(rectangleRenderer_);

	leftKeyInput_ = new PaddleKeyboardInputComp(SDLK_UP, SDLK_DOWN, SDLK_SPACE, 5);
	leftMouseInput_ = new PaddleMouseInputComp();
	leftPaddlePhysics_ = new StopOnBordersPhysics(false, false, true, true);
	leftPaddleAI_ = new PaddleAIPhysics(ball_);
	SwitchLeftPaddle_ = new ComponentSwitcher(this, left_paddle_, SDLK_1);
	
	left_paddle_->setInputComponent(leftKeyInput_);
	//left_paddle_->setInputComponent(leftMouseInput_);
	left_paddle_->setPhysicsComponent(leftPaddlePhysics_);
	//left_paddle_->setPhysicsComponent(leftPaddleAI_);

	SwitchLeftPaddle_->setPosition(10, 10);
	//SwitchLeftPaddle_->setDirection(0, 0);
	SwitchLeftPaddle_->setWidth(50);
	SwitchLeftPaddle_->setHeight(50);
	SwitchLeftPaddle_->addMode(leftKeyInput_, leftPaddlePhysics_, rectangleRenderer_, keyIcon_);
	SwitchLeftPaddle_->addMode(leftMouseInput_, leftPaddlePhysics_, rectangleRenderer_, mouseIcon_);
	SwitchLeftPaddle_->addMode(nullptr, leftPaddleAI_, rectangleRenderer_, AIIcon_);
	SwitchLeftPaddle_->setMode(0);

	// right paddle
	right_paddle_ = new GameComponent(this);
	right_paddle_->setWidth(10);
	right_paddle_->setHeight(60);
	right_paddle_->setPosition(getWindowWidth() - 40,
			(getWindowHeight() - right_paddle_->getHeight()) / 2);
	right_paddle_->setDirection(0, 0);
	right_paddle_->setRenderComponent(rectangleRenderer_);

	rightKeyInput_ = new PaddleKeyboardInputComp(SDLK_a, SDLK_z, SDLK_s, 5);
	rightMouseInput_ = new PaddleMouseInputComp();
	rightPaddlePhysics_ = new StopOnBordersPhysics(false, false, true, true);
	rightPaddleAI_ = new PaddleAIPhysics(ball_);
	SwitchRightPaddle_ = new ComponentSwitcher(this, right_paddle_, SDLK_2);
	
	//right_paddle_->setInputComponent(rightKeyInput_);
	//right_paddle_->setInputComponent(rightMouseInput_);
	//right_paddle_->setPhysicsComponent(rightPaddlePhysics_);
	//right_paddle_->setPhysicsComponent(rightPaddleAI_);

	// switcher
	SwitchRightPaddle_->setPosition(getWindowWidth() - 60, 10);
	//SwitchRightPaddle_->setDirection(0, 0);
	SwitchRightPaddle_->setWidth(50);
	SwitchRightPaddle_->setHeight(50);
	SwitchRightPaddle_->addMode(rightKeyInput_, rightPaddlePhysics_, rectangleRenderer_, keyIcon_);
	SwitchRightPaddle_->addMode(rightMouseInput_, rightPaddlePhysics_, rectangleRenderer_, mouseIcon_);
	SwitchRightPaddle_->addMode(nullptr, rightPaddleAI_, rectangleRenderer_, AIIcon_);
	SwitchRightPaddle_->setMode(2);
	
	
	// game manager
	gameManager_ = new GameManager(this, left_paddle_, right_paddle_);
	
	pingPongPhysics_ = new PingPongPhysics(left_paddle_, right_paddle_);
	ball_->setPhysicsComponent(pingPongPhysics_);
	gameManager_->registerGameStateObserver(pingPongPhysics_);
	pingPongPhysics_->resgisterBallObserver(gameManager_);
	
	// obstacle
	TimedObstacle_ = new TimedObstacle(this, 500, 2500, ball_);
	//TimedObstacle_->setRenderComponent(rectangleRenderer_);
	gameManager_->registerGameStateObserver(TimedObstacle_);
	TimedObstacle_->addObserver(gameManager_);
	
	actors_.push_back(left_paddle_);
	actors_.push_back(right_paddle_);
	actors_.push_back(ball_);
	actors_.push_back(gameManager_);
	actors_.push_back(SwitchLeftPaddle_);
	actors_.push_back(SwitchRightPaddle_);
	actors_.push_back(TimedObstacle_);
	
}

void PingPong::closeGame() {
	delete right_paddle_;
	delete left_paddle_;
	delete ball_;
	delete rectangleRenderer_;
	delete imageRenderer_;
//	delete bounceOnBorderPhysics_;
}

void PingPong::start() {
	exit_ = false;
	while (!exit_) {
		handleInput();
		update();
		render();
		SDL_Delay(10);
	}
}

void PingPong::stop() {
	exit_ = true;
}

void PingPong::update() {
	for (unsigned int i = 0; i < actors_.size(); i++) {
		actors_[i]->update();
	}
}

void PingPong::handleInput() {
	
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit_ = true;
				break;
				// Pressing f to toggle fullscreen.
			case SDLK_f:
				int flags = SDL_GetWindowFlags(window_);
				if (flags & SDL_WINDOW_FULLSCREEN) {
					SDL_SetWindowFullscreen(window_, 0);
				} else {
					SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);
				}
				break;
			}
		}
		for (unsigned int i = 0; i < actors_.size(); i++) {
			actors_[i]->handleInput(event);
		}

	}
}

void PingPong::render() {
	SDL_SetRenderDrawColor(getRenderer(), 0, 100, 100, 255);  // Dark grey.
	SDL_RenderClear(getRenderer());

	for (unsigned int i = 0; i < actors_.size(); i++) {
		actors_[i]->render();
	}

	SDL_RenderPresent(getRenderer());
}

