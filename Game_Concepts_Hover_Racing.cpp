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
float kRacerRotateSpeed = 90.0f;
float kCameraSpeed = 90.0f;
//Variables
float timeCounter = 4;
string bigText = to_string(timeCounter);

//Game enums
enum eGameState {Demo, Count_Down, Stage, RaceComplete, Developer, Paused};

eGameState gameState = Stage;
eGameState previousGameState;

enum eCameraState {Free, ThirdPerson, FirstPerson, Surveillance};
eCameraState cameraState = Free;

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
	float x() {
		return model->GetX();
	}
	float y() {
		return model->GetY();
	}
	float z() {
		return model->GetZ();
	}
	void moveRight(bool move) {
		if (move) {
			//Rotate right and up. Slowly return back
			//model->RotateLocalZ(-kRacerRotateSpeed * frameTime);
			model->RotateLocalY(kRacerRotateSpeed * frameTime);
		}
	}
	void moveLeft(bool move) {
		if (move) {
			//model->RotateLocalZ(kRacerRotateSpeed * frameTime);
			model->RotateLocalY(-kRacerRotateSpeed * frameTime);
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
	void Collide(vector<IModel*>* array) {

		if (!collided) {
			for (auto obj : *array) {
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

//Functions

bool isNumber(string* line) {

	for (auto ch : *line) {
		if (isdigit(ch)) {
			return true;
		}
	}
	return false;
}

void loadModelsFromFile(vector<IModel*> &array, string filename, I3DEngine* myEngine) {
	ifstream file(filename);

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

			array.push_back(someMesh->CreateModel(x, y, z));
		}
	}
}

void changeCamera(I3DEngine* myEngine, ICamera* myCamera, Racer* thePlayer) {
	//GAME CAMERA STATES
	//FREE
	if (cameraState == Free) {
		if (myEngine->KeyHeld(Key_Right)) {
			myCamera->MoveLocalX(kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Left)) {
			myCamera->MoveLocalX(-kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Up)) {
			myCamera->MoveLocalZ(-kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Down)) {
			myCamera->MoveLocalZ(kCameraSpeed * frameTime);
		}

	//FIRST
	}
	else if (cameraState == FirstPerson) {
		myCamera->SetPosition(thePlayer->x(), thePlayer->y() + 5, thePlayer->z());
		//myCamera->LookAt(thePlayer->model);
		
	}
	//THIRD
	else if (cameraState == ThirdPerson) {
		//
		
		
		if (myEngine->KeyHeld(Key_A)) {
			myCamera->MoveLocalX(kCameraSpeed * frameTime);
			myCamera->LookAt(thePlayer->model);
		}
		else if (myEngine->KeyHeld(Key_D)) {
			myCamera->MoveLocalX(-kCameraSpeed * frameTime);
			myCamera->LookAt(thePlayer->model);
		}
		else if (myEngine->KeyHeld(Key_W)) {
			myCamera->MoveZ(thePlayer->kRacerSpeed * frameTime);
			myCamera->LookAt(thePlayer->model);
		}
		else if (myEngine->KeyHeld(Key_D)) {
			myCamera->MoveZ(thePlayer->kRacerSpeed * frameTime);
			myCamera->LookAt(thePlayer->model);
		}
	}

	//SURVEILLANCE
	else if (cameraState == Surveillance) {
		//TODO - Position it somewhere
		myCamera->SetPosition(10, 10, 10);
		myCamera->LookAt(thePlayer->x(), thePlayer->y() + 5, thePlayer->z());
	}
}

void displayCountDown(IFont* text, I3DEngine* myEngine) {
	timeCounter = timeCounter - frameTime;
	bigText = to_string(timeCounter);
	//Getting just the first number, i.e. integer
	bigText = bigText[0];
	if (timeCounter > 1) {
		text->Draw(bigText, myEngine->GetWidth() / 2 - text->MeasureTextWidth(bigText) / 2, myEngine->GetHeight() / 2);
	}
	else {
		bigText = "Go, go, go!";
		text->Draw(bigText, myEngine->GetWidth() / 2 - text->MeasureTextWidth(bigText) / 2, myEngine->GetHeight() / 2);
		if (timeCounter < 1) {
			gameState = Stage;
		}
	}

}


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
	
	IFont* myFont = myEngine->LoadFont("Times New Roman", 36);
	IFont* myBigRedFont = myEngine->LoadFont("Times New Roman", 56);

	//Loading unmovable models from file
	vector<IModel*> staticObjects;
	loadModelsFromFile(staticObjects, "input.txt", myEngine);

	//Adding moving players
	vector<IModel*> dynamicObjects;
	Racer* player = &Racer(myEngine);
	

	dynamicObjects.push_back(player->model);

	

	myEngine->StopMouseCapture();
	
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{	
		
		frameTime = myEngine->Timer();
		// Draw the scene
		myEngine->DrawScene();
		
		/**** Update your scene each frame here ****/
		
		
		//Change camera
		changeCamera(myEngine, myCamera, player);

		//Major game states
		//DEMO
		if (gameState == Demo) {
			string text = "Press Space to Start";
			myFont->Draw(text, myEngine->GetWidth()/2 - myFont->MeasureTextWidth(text)/2, myEngine->GetHeight()/2);

			if (myEngine->KeyHit(Key_Space)) {
				gameState = Count_Down;
			}
		}
		//COUNT_DOWN
		else if (gameState == Count_Down) {
			displayCountDown(myBigRedFont, myEngine);
		}
		//STAGE
		else if (gameState == Stage) {
			player->moveRight(myEngine->KeyHeld(Key_D));
			player->moveLeft(myEngine->KeyHeld(Key_A));
			player->moveForward(myEngine->KeyHeld(Key_W));
			player->moveBackward(myEngine->KeyHeld(Key_S));
			player->Collide(&staticObjects);
		}
		//RACE_COMPLETE
		else if (gameState == RaceComplete) {
			

		}

		//GAME UTILS
		 
		if (myEngine->KeyHit(Key_1)) {
			cameraState = FirstPerson;
			myCamera->AttachToParent(player->model);		
		} else if (myEngine->KeyHit(Key_2)) {
			cameraState = Free;
			myCamera->DetachFromParent();
		} else if(myEngine->KeyHit(Key_3)) {
			myCamera->SetLocalPosition(player->x(), player->y() + 15, player->z() - 15);
			cameraState = ThirdPerson;
			myCamera->RotateLocalX(45);
			//myCamera->AttachToParent(player->model);
		} else if(myEngine->KeyHit(Key_4)) {
			cameraState = Surveillance;
			myCamera->DetachFromParent();
		}

		//Game pause
		if (myEngine->KeyHit(Key_P)) {
			if (gameState != Paused) {
				previousGameState = gameState;
				gameState = Paused;
			}
			else {
				gameState = previousGameState;
			}
		}
		if (gameState == Paused) {
			myFont->Draw("PAUSED", 0, 0);
		}
		//Game Exit
		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}


	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
