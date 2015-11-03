#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
class ModuleSceneIntro;

enum SensorType
{
	big_bonus,
	small_bonus,
	pokeball,
	shroomish,
	button,
	triangle_left,
	triangle_right
};

struct Sensor
{
	Sensor() : body(NULL), texture(NULL), sound(0), image(false), collision(false)
	{}

	Sensor(ModuleSceneIntro* physics, int x, int y, SensorType type);

	PhysBody* body;
	int x;
	int y;

	uint sound;
	bool image;
	bool collision;

	SensorType type;
	SDL_Texture* texture;

};

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
	p2List<Sensor> sensors;
	

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
	
	SDL_Texture* small_bonus;
	SDL_Texture* big_bonus;
	SDL_Texture* pokeball;
	SDL_Texture* shroomish;
	SDL_Texture* left_triangle;
	SDL_Texture* right_triangle;
	uint bonus_fx;
	uint shroomish_fx;
	uint eject1_fx;
	uint eject2_fx;
	uint flipper_fx;
	uint triangle_fx;
	uint button_fx;
	uint deadball_fx;
	uint losing_fx;

	//SDL_Texture* rick;
	SDL_Texture* backgroundmap;
	SDL_Texture* frontground;
	p2Point<int> ray;
	bool ray_on;
	float ejector_force;
	uint lifes;
	uint score;
	uint last_score;
	bool game_over = false;
};
