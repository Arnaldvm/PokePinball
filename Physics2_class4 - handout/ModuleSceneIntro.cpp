#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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
	box = App->textures->Load("pinball/crate.png");
	//rick = App->textures->Load("pinball/rick_head.png");
	backgroundmap = App->textures->Load("pinball/pinball.png");
	frontground = App->textures->Load("pinball/frontground.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	circles.add(App->physics->CreateCircle(350, 470, 6));
	//circles.getLast()->data->listener = this;
	ejector1 = App->physics->CreateRectangle(345, 575, 18, 35,b2_dynamicBody);
	ejector2 = App->physics->CreateRectangle(335, 500, 2, 2, b2_staticBody);
	boxes.add(ejector1);
	boxes.add(ejector2);
	App->physics->CreateLineJoint(ejector1, ejector2, 0, 0, 0, 0, 20.0f, 1.0f);
	ejector_force = 0;
	lifes = 3;

	int polygon1[16] = {
		122, 550,
		121, 543,
		125, 539,
		132, 540,
		164, 562,
		164, 567,
		159, 568,
		130, 555
	};

	int polygon2[16] = {
		226, 555,
		226, 550,
		223, 546,
		217, 546,
		183, 569,
		183, 574,
		187, 575,
		218, 562
	};

	polygons.add(App->physics->CreatePolygon(PIXEL_TO_METERS(121), PIXEL_TO_METERS(140), polygon1, 16));
	polygons.add(App->physics->CreatePolygon(PIXEL_TO_METERS(220), PIXEL_TO_METERS(558), polygon2, 16));


	//Background Chains
	int borders[324] = {
		144, 600,
		144, 589,
		57, 542,
		57, 560,
		34, 560,
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
		360, 610,
		339, 610,
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
		311, 560,
		290, 560,
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
		111, 531,
		100, 544,
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
		224, 529,
		227, 544,
		238, 538,
		256, 526,
		272, 516
	};

	background.add(App->physics->CreateChain(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0), borders, 324));
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

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 6));
		circles.getLast()->data->listener = this;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {

		ejector_force += 100.0f;
		ejector1->body->ApplyForceToCenter(b2Vec2(0, ejector_force), true);
	}

	/*
	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}
	*/
	


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

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		
		if (y > 610 && lifes > 0) {

			circles.del(c);
			circles.add(App->physics->CreateCircle(350, 470, 6));
			lifes--;

		}
		c = c->next;
		
	}

	
	/*
	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}
	*/
	//Background --------------------------

	
	App->renderer->Blit(frontground, PIXEL_TO_METERS(0), PIXEL_TO_METERS(0), NULL, 0.1f, NULL);

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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
