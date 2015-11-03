#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleWindow.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/pokeball8.png"); 
	backgroundmap = App->textures->Load("pinball/pinball.png");
	frontground = App->textures->Load("pinball/frontground.png");
	left_flipper_img = App->textures->Load("pinball/left_trigger.png");
	right_flipper_img = App->textures->Load("pinball/right_trigger.png");
	spoink = App->textures->Load("pinball/spoink.png");

	//---- Sensors
	big_bonus = App->textures->Load("pinball/big_bonus.png");
	small_bonus = App->textures->Load("pinball/small_bonus.png");
	pokeball = App->textures->Load("pinball/pokeball_img.png");
	shroomish = App->textures->Load("pinball/shroomish_img.png");
	left_triangle = App->textures->Load("pinball/left_triangle.png");
	right_triangle = App->textures->Load("pinball/right_triangle.png");

	//---- Audios
	bonus_fx = App->audio->LoadFx("pinball/audio_bonus.wav");
	shroomish_fx = App->audio->LoadFx("pinball/audio_sroomish.wav");;
	eject1_fx = App->audio->LoadFx("pinball/audio_ejector_load.wav");;
	eject2_fx = App->audio->LoadFx("pinball/audio_ejector_eject.wav");;
	flipper_fx = App->audio->LoadFx("pinball/audio_flipper.wav");;
	triangle_fx = App->audio->LoadFx("pinball/audio_bouncer.wav");;
	button_fx = App->audio->LoadFx("pinball/audio_button.wav");;
	deadball_fx = App->audio->LoadFx("pinball/audio_dead_ball.wav");
	losing_fx = App->audio->LoadFx("pinball/audio_losing.wav");


	//------ Sensors
	//4 bottom
	sensors.add(Sensor(this, 39, 471, SensorType::big_bonus));
	sensors.add(Sensor(this, 74, 471, SensorType::big_bonus));
	sensors.add(Sensor(this, 255, 471, SensorType::big_bonus));
	sensors.add(Sensor(this, 290, 471, SensorType::big_bonus));
		
	//3 top
	sensors.add(Sensor(this, 121, 128, SensorType::small_bonus));
	sensors.add(Sensor(this, 151, 128, SensorType::small_bonus));
	sensors.add(Sensor(this, 181, 128, SensorType::small_bonus));
	//wailmer + shop
	sensors.add(Sensor(this, 273, 248, SensorType::small_bonus));
	sensors.add(Sensor(this, 77, 238, SensorType::small_bonus));

	//pokeballs
	sensors.add(Sensor(this, 142, 465, SensorType::pokeball));
	sensors.add(Sensor(this, 165, 465, SensorType::pokeball));
	sensors.add(Sensor(this, 188, 465, SensorType::pokeball));

	//Shroomishs
	sensors.add(Sensor(this, 159, 215, SensorType::shroomish));
	sensors.add(Sensor(this, 136, 175, SensorType::shroomish));
	sensors.add(Sensor(this, 185, 175, SensorType::shroomish));
	
	//Buttons
	sensors.add(Sensor(this, 59, 421, SensorType::button));
	sensors.add(Sensor(this, 275, 421, SensorType::button));

	//Triangles bouncers
	sensors.add(Sensor(this, 112, 490, SensorType::triangle_left));
	sensors.add(Sensor(this, 234, 490, SensorType::triangle_right));

	

	//First ball
	circles.add(App->physics->CreateCircle(350, 470, 6, b2_dynamicBody, 0.5f, false));
	circles.getLast()->data->listener = this;

	//Small ball below flippers
	App->physics->CreateCircle(173, 595, 3, b2_staticBody, 0.5f, false);


	//------Ejector
	ejector1 = App->physics->CreateRectangle(345, 575, 18, 35,b2_dynamicBody, 0.5f, 0.0f);
	ejector2 = App->physics->CreateRectangle(335, 500, 2, 2, b2_staticBody, 0.5f, 0.0f);
	boxes.add(ejector1);
	boxes.add(ejector2);
	App->physics->CreateLineJoint(ejector1, ejector2, 0, 0, 0, 0, 25.0f, 0.8f);
	ejector_force = 0;

	// Variables
	lifes = 3;
	score = 0;
	last_score = 0;

	//------Flippers
	left_ball = App->physics->CreateCircle(125, 552, 1, b2_staticBody, 1.0f, false);
	right_ball = App->physics->CreateCircle(219, 552, 1, b2_staticBody, 1.0f, false);

	int polygon1[16] = {
		122, 540,
		121, 533,
		125, 529,
		132, 530,
		164, 552,
		164, 557,
		159, 558,
		130, 545
	};

	int polygon2[16] = {
		226, 545,
		226, 540,
		223, 536,
		217, 536,
		183, 559,
		183, 564,
		187, 565,
		218, 552
	};

	left_flipper = App->physics->CreatePolygon(PIXEL_TO_METERS(121), PIXEL_TO_METERS(558), polygon1, 16, 0.5f);
	right_flipper = App->physics->CreatePolygon(PIXEL_TO_METERS(220), PIXEL_TO_METERS(500), polygon2, 16, 0.5f);
	polygons.add(left_flipper);
	polygons.add(right_flipper);
	App->physics->CreateRevoluteJoint(left_flipper, left_ball, 126, 535, 0, 0, 70, 10);
	App->physics->CreateRevoluteJoint(right_flipper, right_ball, 220, 542, 0, 0, -10, -70);

	//-----------Bouncers
	//Shroomishs
	App->physics->CreateCircle(166, 222, 8, b2_staticBody, 1.25f, false);
	App->physics->CreateCircle(143, 182, 8, b2_staticBody, 1.25f, false);
	App->physics->CreateCircle(192, 182, 8, b2_staticBody, 1.25f, false);
	//Buttons
	App->physics->CreateCircle(65, 427, 8, b2_staticBody, 1.25f, false);
	App->physics->CreateCircle(281, 427, 8, b2_staticBody, 1.25f, false);
	//Triangles bouncers
	App->physics->CreateRectangle(112, 490, 6, 50, b2_staticBody, 1.25f, 325 * DEGTORAD);
	App->physics->CreateRectangle(234, 490, 6, 50, b2_staticBody, 1.25f, 35 * DEGTORAD);


	//Background Chains
	int borders[320] = {
		144, 600,
		144, 589,
		57, 542,
		35, 527,
		34, 465,
		35, 457,
		38, 451,
		45, 446,
		49, 445,
		56, 445,
		61, 445,
		66, 443,
		69, 438,
		69, 426,
		68, 400,
		62, 393,
		55, 385,
		48, 375,
		41, 365,
		35, 354,
		29, 342,
		25, 330,
		21, 316,
		18, 303,
		16, 289,
		15, 276,
		15, 262,
		15, 245,
		16, 226,
		17, 217,
		16, 204,
		15, 193,
		16, 180,
		17, 168,
		19, 148,
		21, 136,
		23, 126,
		28, 114,
		36, 101,
		44, 90,
		55, 76,
		65, 66,
		78, 58,
		90, 51,
		104, 45,
		120, 39,
		144, 31,
		165, 25,
		183, 20,
		202, 17,
		222, 14,
		243, 11,
		277, 7,
		299, 7,
		312, 8,
		321, 11,
		334, 17,
		345, 25,
		353, 36,
		358, 47,
		359, 58,
		359, 74,
		359, 98,
		359, 114,
		359, 142,
		360, 348,
		360, 439,
		360, 620,
		339, 620,
		339, 510,
		339, 394,
		340, 192,
		333, 186,
		333, 138,
		333, 59,
		331, 50,
		325, 41,
		315, 36,
		304, 32,
		286, 31,
		275, 31,
		261, 33,
		252, 34,
		237, 37,
		222, 40,
		168, 53,
		135, 64,
		113, 70,
		89, 82,
		81, 89,
		69, 102,
		60, 112,
		52, 125,
		48, 134,
		45, 144,
		44, 153,
		44, 164,
		50, 149,
		56, 139,
		61, 132,
		67, 125,
		75, 118,
		86, 111,
		96, 105,
		108, 100,
		121, 95,
		137, 91,
		150, 89,
		164, 88,
		179, 88,
		190, 88,
		201, 89,
		213, 92,
		224, 96,
		234, 100,
		246, 105,
		260, 113,
		273, 121,
		283, 130,
		293, 140,
		302, 152,
		308, 162,
		312, 170,
		318, 186,
		322, 200,
		326, 215,
		327, 229,
		328, 240,
		329, 268,
		329, 278,
		327, 294,
		324, 306,
		320, 323,
		313, 343,
		306, 361,
		296, 377,
		286, 390,
		277, 397,
		277, 436,
		277, 442,
		280, 444,
		285, 446,
		290, 446,
		299, 446,
		304, 449,
		309, 453,
		311, 464,
		311, 473,
		311, 490,
		311, 511,
		311, 530,
		290, 541,
		203, 588,
		203, 600,
		369, 600,
		369, 0,
		0, 0,
		0, 600,
		110, 600,
		132, 600
	};
	// Left triangle ---------------------------------------------------------------
	int leftri[22] = {
		94, 463,
		94, 494,
		96, 496,
		99, 498,
		108, 504,
		118, 510,
		122, 510,
		126, 507,
		126, 499,
		100, 462,
		97, 461
	};
	// Right triangle ---------------------------------------------------------------
	int rightri[24] = {
		214, 508,
		214, 500,
		239, 463,
		242, 461,
		245, 461,
		247, 464,
		247, 490,
		245, 495,
		239, 499,
		231, 504,
		224, 508,
		218, 510
	};
	// Left structure
	int leftstructure[74] = {
		115, 166,
		115, 130,
		103, 135,
		93, 142,
		84, 152,
		75, 164,
		61, 188,
		49, 209,
		46, 216,
		47, 229,
		55, 252,
		62, 267,
		70, 279,
		76, 285,
		82, 283,
		88, 282,
		92, 277,
		90, 268,
		83, 260,
		80, 252,
		71, 236,
		83, 227,
		102, 242,
		111, 250,
		120, 257,
		126, 255,
		138, 248,
		138, 242,
		133, 237,
		125, 231,
		118, 223,
		115, 214,
		113, 205,
		112, 195,
		107, 186,
		102, 177,
		108, 171
	};
	//Right structure
	int rightstructure[144] = {
		203, 152,
		197, 144,
		196, 135,
		197, 128,
		203, 123,
		212, 123,
		222, 124,
		228, 127,
		237, 129,
		245, 132,
		255, 140,
		261, 146,
		269, 155,
		277, 165,
		282, 176,
		285, 185,
		291, 196,
		300, 212,
		304, 224,
		307, 236,
		307, 248,
		307, 257,
		306, 271,
		302, 280,
		297, 286,
		288, 290,
		276, 290,
		269, 287,
		266, 282,
		267, 277,
		275, 269,
		284, 259,
		288, 255,
		289, 248,
		286, 242,
		278, 238,
		271, 239,
		263, 241,
		255, 246,
		252, 250,
		251, 257,
		249, 264,
		245, 270,
		242, 276,
		236, 282,
		234, 287,
		229, 290,
		222, 289,
		221, 279,
		224, 267,
		227, 263,
		234, 261,
		236, 246,
		230, 241,
		219, 236,
		215, 233,
		211, 237,
		205, 241,
		202, 235,
		203, 220,
		205, 212,
		210, 205,
		214, 200,
		222, 193,
		236, 184,
		246, 178,
		248, 173,
		245, 168,
		238, 164,
		230, 159,
		222, 156,
		213, 153
	};
	// Left block
	int leftblock[14] = {
		135, 149,
		135, 121,
		145, 121,
		145, 141,
		145, 149,
		143, 155,
		137, 155
	};
	// Right block
	int rightblock[14] = {
		165, 119,
		175, 119,
		175, 146,
		173, 152,
		167, 152,
		165, 146,
		165, 128
	};
	// Left barrier
	int leftbarrier[26] = {
		52, 511,
		52, 457,
		55, 455,
		59, 455,
		61, 459,
		61, 497,
		64, 503,
		82, 514,
		113, 532,
		101, 545,
		89, 537,
		72, 526,
		57, 515
	};
	// Right barrier
	int rightbarrier[26] = {
		278, 510,
		278, 458,
		276, 455,
		272, 455,
		270, 459,
		270, 499,
		266, 503,
		250, 514,
		217, 534,
		227, 544,
		238, 538,
		256, 526,
		272, 516
	};

	background.add(App->physics->CreateChain(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0), borders, 320));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(93), PIXEL_TO_METERS(465), leftri, 22));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(219), PIXEL_TO_METERS(464), rightri, 24));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(49), PIXEL_TO_METERS(133), leftstructure, 74));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(196), PIXEL_TO_METERS(122), rightstructure, 144));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(137), PIXEL_TO_METERS(122), leftblock, 14));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(166), PIXEL_TO_METERS(120), rightblock, 14));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(552), PIXEL_TO_METERS(465), leftbarrier, 26));
	background.add(App->physics->CreateChain(PIXEL_TO_METERS(213), PIXEL_TO_METERS(465), rightbarrier, 26));

	return ret;
	//--------------------------------------------------

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	/*if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
	ray_on = !ray_on;
	ray.x = App->input->GetMouseX();
	ray.y = App->input->GetMouseY();
	}*/

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 6, b2_dynamicBody, 0.5f, false));
		circles.getLast()->data->listener = this;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {

		ejector_force += 50.0f;
		ejector1->body->ApplyForceToCenter(b2Vec2(0, ejector_force), true);
		App->audio->PlayFx(eject1_fx);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP) {
		ejector_force = 0.0f;
		App->audio->PlayFx(eject2_fx);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT){

		left_flipper->body->ApplyAngularImpulse(DEGTORAD * -90, true);
	}
	

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT){

		right_flipper->body->ApplyAngularImpulse(DEGTORAD * 90, true);
		
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
		App->audio->PlayFx(flipper_fx);

	// Prepare for raycast ------------------------------------------------------

	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	p2List_item<PhysBody*>* c = background.getFirst();

	while (c != NULL){
		App->renderer->Blit(backgroundmap, PIXEL_TO_METERS(0), PIXEL_TO_METERS(0), NULL, 0.1f, NULL);
		c = c->next;
	}


	c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());

		if (y > 610 && lifes >= 0 && game_over == false) {

			circles.del(c);
			circles.add(App->physics->CreateCircle(350, 470, 6, b2_dynamicBody, 0.5f, false));
			circles.getLast()->data->listener = this;
			lifes--;
			App->audio->PlayFx(deadball_fx);

		}
		c = c->next;

	} 



	App->renderer->Blit(left_flipper_img, 117, 545, NULL, 1.0f, RADTODEG * left_flipper->body->GetAngle(), 7, 7);
	App->renderer->Blit(right_flipper_img, 180, 544, NULL, 1.0f, RADTODEG * right_flipper->body->GetAngle(), 38, 10);

	

	c = boxes.getFirst();

	while (c != NULL)
	{ 
		int x, y;
		c->data->GetPosition(x, y);
		if (c->data->body->GetType() != b2_staticBody)
			App->renderer->Blit(spoink, x-3, y-4, NULL, 1.0f, c->data->GetRotation());

		c = c->next;
	}

	//Background --------------------------

	App->renderer->Blit(frontground, PIXEL_TO_METERS(0), PIXEL_TO_METERS(0), NULL, 0.1f, NULL);

	p2List_item<Sensor>* s = sensors.getFirst();

	while (s != NULL){
		if (s->data.image == true){
			if (s->data.type == SensorType::small_bonus || s->data.type == SensorType::big_bonus) {
				if (237 > s->data.y || s->data.y > 249 )
					App->renderer->Blit(s->data.texture, s->data.x + 2, s->data.y + 1);
			}
			if (s->data.type == SensorType::shroomish) {
				App->renderer->Blit(s->data.texture, s->data.x-12, s->data.y - 8);
				s->data.image = false;
			}
			if (s->data.type == SensorType::triangle_left) {
				App->renderer->Blit(s->data.texture, s->data.x -15, s->data.y-15);
				s->data.image = false;
			}
			if (s->data.type == SensorType::triangle_right) {
				App->renderer->Blit(s->data.texture, s->data.x - 15, s->data.y - 15);
				s->data.image = false;
			}
			if (s->data.type == SensorType::pokeball)
				App->renderer->Blit(s->data.texture, s->data.x, s->data.y);
			
		}
	
		s = s->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	char title[50];
	sprintf_s(title, "Balls: %d -- Score: %d -- Last score: %d", lifes, score, last_score);
	App->window->SetTitle(title);

	if (lifes == 1)
		game_over == true;

	if (lifes == -1){

		App->audio->PlayFx(losing_fx);
		lifes = 3;
		last_score = score;
		score = 0;
		p2List_item<Sensor>* s = sensors.getFirst();
		while (s != NULL){
			if (s->data.image == true)
				s->data.image = false;
			s = s->next;
		}

	}
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	p2List_item<Sensor>* c = sensors.getFirst();
	while (c != NULL)
	{
		if (bodyA == c->data.body){
			App->audio->PlayFx(c->data.sound);

			c->data.image = true;
			
			switch (c->data.type){
			case SensorType::big_bonus:
				score += 20;
				break;
			case SensorType::small_bonus:
				score += 10;
				break;
			case SensorType::pokeball:
				score += 5;
				break;
			case SensorType::triangle_left:
				score +=5;
				break;
			case SensorType::triangle_right:
				score += 5;
				break;
			case SensorType::button:
				score +=5;
				break;
			case SensorType::shroomish:
				score +=25;
				break;

			}
		}
		c = c->next;
	}



}
Sensor::Sensor(ModuleSceneIntro* scene, int x, int y, SensorType type)
{
	this->x = x;
	this->y = y;
	this->type = type;

	image = false;
	collision = false;
	int radius = 0;

	switch (type)
	{
	case big_bonus:
		radius = 8;
		texture = scene->big_bonus;
		sound = scene->bonus_fx;

		body = scene->App->physics->CreateCircle(x + radius, y + radius, radius, b2_staticBody, 0.0f, true);
		body->listener = scene;
		break;

	case small_bonus:
		radius = 6;
		texture = scene->small_bonus;
		sound = scene->bonus_fx;

		body = scene->App->physics->CreateCircle(x + radius, y + radius, radius, b2_staticBody, 0.0f, true);
		body->listener = scene;
		break;

	case pokeball:
		radius = 8;
		texture = scene->pokeball;
		sound = scene->bonus_fx;

		body = scene->App->physics->CreateCircle(x + radius, y + radius, radius, b2_staticBody, 0.0f, true);
		body->listener = scene;
		break;

	case shroomish:
		radius = 10;
		texture = scene->shroomish;
		sound = scene->shroomish_fx;

		body = scene->App->physics->CreateCircle(x + radius, y + radius, radius, b2_staticBody, 0.0f, true);
		body->listener = scene;
		break;
	case triangle_left:
		texture = scene->left_triangle;
		sound = scene->triangle_fx;
		body = scene->App->physics->CreateRectangle(x, y, 6, 50, b2_staticBody, 1.25f, 325 * DEGTORAD);
		body->listener = scene;
		break;
	case triangle_right:
		texture = scene->right_triangle;
		sound = scene->triangle_fx;
		body = scene->App->physics->CreateRectangle(x, y, 6, 50, b2_staticBody, 1.25f, 35 * DEGTORAD);
		body->listener = scene;
		break;
	case button:
		radius = 6;
		texture = NULL;
		sound = scene->button_fx;
		body = scene->App->physics->CreateCircle(x + radius, y + radius, radius, b2_staticBody, 0.0f, true);
		body->listener = scene;
		break;


	}

}


