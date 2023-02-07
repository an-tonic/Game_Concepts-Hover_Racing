// Game_Concepts_Hover_Racing.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <vector>
#include <fstream>

using namespace tle;
using namespace std;


//Constants
float frameTime = 0.0f;
float kRacerSpeed = 0.0f;
float kRacerBackSpeed = 0.0f;
float kMaxSpeed = 50.f;
float kAcceleration = 70.0f;
float kAirDrag = 1.0f;
float kRacerRotateSpeed = 30.0f;
//Functions

bool isNumber(string* line) {

	for (auto ch : *line) {
		if (isdigit(ch)) {
			return true;
		}

	}
	return false;
}

//Structures
typedef struct Racer {
	IModel* model;
	float matrix[4][4];

	Racer(I3DEngine* myEngine, float x = 0.0f, float y = 0.0f, float z = 0.0f) {

		model = myEngine->LoadMesh("Racer.x")->CreateModel(x, y, z);
		model->GetMatrix(&matrix[0][0]);
	}

	void moveRight(bool move) {
		if (move) {
			//Rotate right and up. Slowly return back
			model->RotateLocalZ(-kRacerRotateSpeed * frameTime);
			model->RotateY(kRacerRotateSpeed * frameTime);
			cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << endl;
			cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << endl;
			cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << endl;
			cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << endl << endl;

		}
	}
	void moveLeft(bool move) {
		if (move) {
			model->RotateLocalZ(kRacerRotateSpeed * frameTime);
			model->RotateY(-kRacerRotateSpeed * frameTime);
		}
	}
	void moveForward(bool move) {
		if (move) {
			matrix[3][2] += kRacerSpeed * frameTime;
			if (kRacerSpeed < kMaxSpeed) {
				kRacerSpeed += kAcceleration * frameTime;
			}
			model->SetMatrix(&matrix[0][0]);
			cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << endl;
		}
		else {
			if (kRacerSpeed > 0) {
				matrix[3][2] += kRacerSpeed * frameTime;
				kRacerSpeed -= kAcceleration * frameTime * kAirDrag;
				model->SetMatrix(&matrix[0][0]);
			}
		}
	}
	void moveBackward(bool move) {
		if (move) {
			matrix[3][2] -= kRacerBackSpeed * frameTime;
			if (kRacerBackSpeed < kMaxSpeed / 2) {
				kRacerBackSpeed += kAcceleration * frameTime;
			}
			model->SetMatrix(&matrix[0][0]);
		}
		else {
			if (kRacerBackSpeed > 0) {
				matrix[3][2] -= kRacerBackSpeed * frameTime;
				kRacerBackSpeed -= kAcceleration * frameTime * kAirDrag;
				model->SetMatrix(&matrix[0][0]);
			}
		}
	}

	void Collide() {

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
	ICamera* myCamera = myEngine->CreateCamera(kManual);


	IMesh* skyMesh = myEngine->LoadMesh("Skybox.x");
	IModel* skyModel = skyMesh->CreateModel(0, -960, 0);

	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0, 0, 0);



	vector<IModel*> collidableObjects;
	
	//Loading models from file
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

			collidableObjects.push_back(someMesh->CreateModel(x, y, z));			
		}
	}

	
	Racer player(myEngine);

	

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

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
