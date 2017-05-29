#include "PaddleMouseInputComp.h"

PaddleMouseInputComp::PaddleMouseInputComp() {
}

PaddleMouseInputComp::~PaddleMouseInputComp() {

}

void PaddleMouseInputComp::init(GameObject* paddle) {
	paddle->setDirectionY(0);
}

void PaddleMouseInputComp::handleInput(SDL_Event event, GameObject* paddle) {
	
	//MOUSE_POSITION_
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (event.button.type == SDL_MOUSEBUTTONUP) {
		if (paddle->getPosition().getY() > y) {
			paddle->setDirectionY(-1 * 5);
		}
		else if (paddle->getPosition().getY() < y) {
			paddle->setDirectionY(5);
		}
		else {
			paddle->setDirectionY(0);
		}
	}


}
