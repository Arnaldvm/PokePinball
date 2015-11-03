#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> polygons;
	//p2List<PhysBody*> ricks;
	p2List<PhysBody*> background;
	p2List<PhysBody*> sensors_bonus;
	p2List<PhysBody*> sensors_pokeballs;
	p2List<PhysBody*> bouncers;
	p2List<PhysBody*> buttons;
	

	PhysBody* sensor;
	PhysBody* ejector1;
	PhysBody* ejector2;

	PhysBody* left_ball;
	PhysBody* left_flipper;
	PhysBody* right_ball;
	PhysBody* right_flipper;

	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* left_flipper_img;
	SDL_Texture* right_flipper_img;
	SDL_Texture* spoink;
	//SDL_Texture* rick;
	SDL_Texture* backgroundmap;
	SDL_Texture* frontground;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
	float ejector_force;
	uint lifes;
	uint score;
	uint last_score;
};
