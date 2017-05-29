#include "ComponentSwitcher.h"

ComponentSwitcher::ComponentSwitcher(SDLGame* game, GameComponent* actor,
		SDL_Keycode ctrlKey) :
		GameObject(game), actor_(actor), ctrlKey_(ctrlKey) {
}

ComponentSwitcher::~ComponentSwitcher() {
}

void ComponentSwitcher::update() {
	render();
}

void ComponentSwitcher::handleInput(const SDL_Event& event) {

	if (event.type == SDL_KEYDOWN) {
		
		if (event.key.keysym.sym == ctrlKey_) {
			switchToNext();
		}
	}
}

void ComponentSwitcher::addMode(InputComponent* inputComp,
		PhysicsComponent* physicsComp, RenderComponent* renderComp,
		RenderComponent* modeRendered) {

	inputComp_.push_back(inputComp);
	physicsComp_.push_back(physicsComp);
	renderComp_.push_back(renderComp);
	modeRendered_.push_back(modeRendered);
}

void ComponentSwitcher::setMode(int mode) {
	actor_->setInputComponent(inputComp_[mode]);
	actor_->setPhysicsComponent(physicsComp_[mode]);
	actor_->setRenderComponent(renderComp_[mode]);
	modeRendered_[mode]->init(this);
	
	modeActual_ = mode;
}

void ComponentSwitcher::switchToNext() {
	modeActual_++;
	if (modeActual_ == inputComp_.size())
		modeActual_ = 0;
	
	actor_->setInputComponent(inputComp_[modeActual_]);
	actor_->setPhysicsComponent(physicsComp_[modeActual_]);
	actor_->setRenderComponent(renderComp_[modeActual_]);
	modeRendered_[modeActual_]->init(this);
}

void ComponentSwitcher::render() {
	modeRendered_[modeActual_]->render(this);
}
