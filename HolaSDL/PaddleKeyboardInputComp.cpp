#include "PaddleKeyboardInputComp.h"

PaddleKeyboardInputComp::PaddleKeyboardInputComp(SDL_Keycode up_key,
		SDL_Keycode down_key, SDL_Keycode stop_key, unsigned int velocity) :
		InputComponent() {
	up_key_ = up_key;
	down_key_ = down_key;
	stop_key_ = stop_key;
	velocity_ = velocity;
}

void PaddleKeyboardInputComp::init(GameObject* paddle) {
	paddle->setDirectionY(0);
}

void PaddleKeyboardInputComp::handleInput(SDL_Event event, GameObject* paddle) {
	
	if (event.key.keysym.sym == up_key_) {
		paddle->setDirectionY(-1 * velocity_);	
	}
	else if (event.key.keysym.sym == down_key_) {
		paddle->setDirectionY(velocity_);
	}
	else if(event.key.keysym.sym == stop_key_){
		paddle->setDirectionY(0);
	}
	
}

PaddleKeyboardInputComp::~PaddleKeyboardInputComp() {
}
