// Game_Concepts_Hover_Racing.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <vector>
#include <fstream>

using namespace tle;
using namespace std;


//Constants
float frameTime = 0.0f;
float kMaxSpeed = 50.f;
float kAcceleration = 90.0f;
float kAirDrag = 1.0f;
float kRacerRotateSpeed = 50.0f;
//Functions

bool isNumber(string* line) {

	for (auto ch : *line) {
		if (isdigit(ch)) {
			return true;
		}
	}
	return false;
}


float vectorLen(IModel* a, IModel* b) {
	float vectorX = b->GetX() - a->GetX();
	float vectorY = b->GetY() - a->GetY();
	float vectorZ = b->GetZ() - a->GetZ();

	return sqrt(vectorX * vectorX + vectorY * vectorY + vectorZ * vectorZ);
}

//Structures
typedef struct Racer {
	IModel* model;
	float kRacerSpeed = 0.0f;
	float kRacerBackSpeed = 0.0f;
	bool collided = false;

	Racer(I3DEngine* myEngine, float x = 0.0f, float y = 0.0f, float z = -20.0f) {
		model = myEngine->LoadMesh("Racer.x")->CreateModel(x, y, z);
	}

	void moveRight(bool move) {
		if (move) {
			//Rotate right and up. Slowly return back
			//model->RotateLocalZ(-kRacerRotateSpeed * frameTime);
			model->RotateY(kRacerRotateSpeed * frameTime);
		}
	}
	void moveLeft(bool move) {
		if (move) {
			//model->RotateLocalZ(kRacerRotateSpeed * frameTime);

			model->RotateY(-kRacerRotateSpeed * frameTime);
		}
	}
	void moveForward(bool move) {
		
		if (move) {
			model->MoveLocalZ(kRacerSpeed * frameTime);

			if (kRacerSpeed < kMaxSpeed) {
				kRacerSpeed += kAcceleration * frameTime;
			}
		}
		else {
			if (kRacerSpeed > 0) {
				model->MoveLocalZ(kRacerSpeed * frameTime);
				kRacerSpeed -= kAcceleration * frameTime * kAirDrag;
			}
		}
		
	}
	void moveBackward(bool move) {
		if (move) {
			model->MoveLocalZ(-kRacerBackSpeed * frameTime);

			if (kRacerBackSpeed < kMaxSpeed / 2) {
				kRacerBackSpeed += kAcceleration * frameTime;
			}
		}
		else {
			if (kRacerBackSpeed > 0) {
				model->MoveLocalZ(-kRacerBackSpeed * frameTime);
				kRacerBackSpeed -= kAcceleration * frameTime * kAirDrag;
			}
		}
	}
	void Collide(vector<IModel*>* allobjects) {
		
		if (!collided) {
			for (auto obj : *allobjects) {
				if (vectorLen(model, obj) < 10) {
					kRacerBackSpeed = kRacerSpeed;
					kRacerSpeed = 0;
					collided = true;
					break;
				}
			}
		}
		if (kRacerBackSpeed <= 0) {
			collided = false;
		}

	}
};

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\aserdyukov\\source\\repos\\Game_Concepts_Hover_Racing\\Assesment 2 Resources" );
	//myEngine->AddMediaFolder("./Assessment 2 Media");

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kFPS);


	IMesh* skyMesh = myEngine->LoadMesh("Skybox.x");
	IModel* skyModel = skyMesh->CreateModel(0, -960, 0);

	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0, 0, 0);


	//Loading unmovable models from file
	vector<IModel*> staticObjects;
	ifstream file("input.txt");
	string newLine;
	IMesh* someMesh{};
	while (file.good())
	{		
		getline(file, newLine);
		if (!isNumber(&newLine)) {
			someMesh = myEngine->LoadMesh(newLine);
		}
		else {
			float x = stof(newLine);
			getline(file, newLine);
			float y = stof(newLine);
			getline(file, newLine);
			float z = stof(newLine);

			staticObjects.push_back(someMesh->CreateModel(x, y, z));			
		}
	}

	//Adding moving players
	vector<IModel*> dynamicObjects;
	Racer player(myEngine);

	dynamicObjects.push_back(player.model);

	myEngine->StopMouseCapture();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{	
		
		frameTime = myEngine->Timer();
		// Draw the scene
		myEngine->DrawScene();
		
		/**** Update your scene each frame here ****/

		player.moveRight(myEngine->KeyHeld(Key_D));
		player.moveLeft(myEngine->KeyHeld(Key_A));
		player.moveForward(myEngine->KeyHeld(Key_W));
		player.moveBackward(myEngine->KeyHeld(Key_S));
		player.Collide(&staticObjects);
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
