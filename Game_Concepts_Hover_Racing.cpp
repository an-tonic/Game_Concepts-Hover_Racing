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
float avgFrameTime = 0.0f;
long long frameCount = 0;
//Variables
float timeCounter = 4;
string bigText = to_string(timeCounter);

//Game enums
enum eGameState {Demo, Count_Down, Stage, RaceComplete, Developer, Paused};

eGameState gameState = Demo;
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
	float matrix[4][4];
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

		myCamera->RotateY(myEngine->GetMouseMovementX() * frameTime * kCameraSpeed);
		myCamera->RotateLocalX(myEngine->GetMouseMovementY()* frameTime * kCameraSpeed);

		if (myEngine->KeyHeld(Key_Right)) {
			myCamera->MoveLocalX(kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Left)) {
			myCamera->MoveLocalX(-kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Up)) {
			myCamera->MoveLocalZ(kCameraSpeed * frameTime);
		}
		else if (myEngine->KeyHeld(Key_Down)) {
			myCamera->MoveLocalZ(-kCameraSpeed * frameTime);
		}
	}
	
	if (myEngine->AnyKeyHit() || myEngine->AnyKeyHeld()) {
		
		//FIRST
		if (cameraState == FirstPerson) {
			myCamera->SetPosition(thePlayer->x(), thePlayer->y() + 5, thePlayer->z());

			thePlayer->model->GetMatrix(&thePlayer->matrix[0][0]);

			myCamera->LookAt(thePlayer->x() + thePlayer->matrix[2][0], thePlayer->y() + 5, thePlayer->z() + thePlayer->matrix[2][2]);

		}
		//THIRD
		else if (cameraState == ThirdPerson) {
			//
			thePlayer->model->GetMatrix(&thePlayer->matrix[0][0]);
			myCamera->SetPosition(thePlayer->x() - thePlayer->matrix[2][0] * 20, thePlayer->y() - thePlayer->matrix[2][1] * 20 + 10, thePlayer->z() - thePlayer->matrix[2][2] * 20);
			myCamera->LookAt(thePlayer->model);

		}

		//SURVEILLANCE
		else if (cameraState == Surveillance) {
			//TODO - Position it somewhere
			myCamera->SetPosition(10, 10, 10);
			myCamera->LookAt(thePlayer->model);
		}
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
		if (timeCounter < 0) {
			gameState = Stage;
		}
	}

}

void startGame(IFont* myFont, I3DEngine* myEngine) {
	string text = "Press Space to Start";
	myFont->Draw(text, myEngine->GetWidth() / 2 - myFont->MeasureTextWidth(text) / 2, myEngine->GetHeight() / 2);

	if (myEngine->KeyHit(Key_Space)) {
		gameState = Count_Down;
	}
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\an-tonic\\source\\repos\\Game_Concepts-Hover_Racing\\Assesment 2 Resources" );
	myEngine->AddMediaFolder("./Assessment 2 Media");

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

	

	myEngine->StartMouseCapture();
	
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{	
		
		
		myFont->Draw(to_string(1/frameTime), 0, 0);

		
		frameTime = myEngine->Timer();
		// Draw the scene
		myEngine->DrawScene();
		
		/**** Update your scene each frame here ****/
		
		
		//Change camera
		changeCamera(myEngine, myCamera, player);

		//Major game states
		//DEMO
		if (gameState == Demo) {
			displayCountDown(myFont, myEngine);
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
		//FIRST
		if (myEngine->KeyHit(Key_1)) {
			cameraState = FirstPerson;
			myCamera->AttachToParent(player->model);

		//FREE
		} else if (myEngine->KeyHit(Key_2)) {
			cameraState = Free;
			myCamera->DetachFromParent();

		//THIRD
		} else if(myEngine->KeyHit(Key_3)) {
			myCamera->AttachToParent(player->model);
			cameraState = ThirdPerson;
		
		//Survaliance
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
