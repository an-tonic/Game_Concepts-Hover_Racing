// Game_Concepts_Hover_Racing.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace tle;
using namespace std;
using namespace std::chrono;

//Constants
float frameTime = 0.0f;
float kMaxSpeed = 100.f;
float kAcceleration = 90;
float kAirDrag = 1.0f;
float kRacerRotateSpeed = 90.0f;
float kCameraSpeed = 190.0f;
float kNewModelSpeed = 1.0f;
IModel* newModel = nullptr;
IMesh* newMesh = nullptr;
string newModelName = "";
string inputfile = "input.txt";
int kCameraOffset = 22;
int winWidth;
int winHeight;
int borderForText = 10;

//Variables
unsigned long long frameCount = 0;
float timeCounter = 4;
string bigText = to_string(timeCounter);
float pV1[3];
int currentStage = 0;
bool warningMessage = false;

//Game enums
enum eGameState {Demo, CountDown, Stage, RaceComplete, Developer, Paused};

eGameState gameState = Stage;
eGameState previousGameState;

enum eCameraState {Free, ThirdPerson, FirstPerson, Surveillance};
eCameraState cameraState = ThirdPerson;

float vectorLen(IModel* a, IModel* b) {
	float vectorX = b->GetX() - a->GetX();
	float vectorY = b->GetY() - a->GetY();
	float vectorZ = b->GetZ() - a->GetZ();

	return sqrt(vectorX * vectorX + vectorY * vectorY + vectorZ * vectorZ);
}

struct Vector2 {
	float bounds[4] = { -10000, 10000,-10000, 10000 };
};

//Structures
typedef struct Racer {
	IModel* model;
	float kRacerSpeed = 0.0f;
	float kRacerBackSpeed = 0.0f;
	float racerMatrix[4][4];
	float objMatrix[4][4];
	bool collided = false;
	float vecToObjX;
	float vecToObjZ;
	float* boundsChecking;
	Vector2 racerBounds = { 2.2308, -2.2308, 6.46015, -6.46015 };

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
			
			if (kRacerSpeed < kMaxSpeed) {
				kRacerSpeed += kAcceleration * frameTime;
			}
			else {
				kRacerSpeed = kMaxSpeed;
			}
		}
		else {
			if (kRacerSpeed > 0) {			
				kRacerSpeed -= kAcceleration * frameTime * kAirDrag;
			}
			else {
				kRacerSpeed = 0;
			}
		}
		model->MoveLocalZ(kRacerSpeed * frameTime);
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
	void Collide(vector<IModel*>* staticObj, vector<Vector2>* staticObjBounds) {

		if (!collided) {
			int index = 0;
			
			for (auto obj : *staticObj) {
				
				if (vectorLen(model, obj) < 20) {
					obj->GetMatrix(&racerMatrix[0][0]);
					model->GetMatrix(&objMatrix[0][0]);

					boundsChecking = staticObjBounds->at(index).bounds;
					
					for (int i = 0; i <= 1; i++) {
						for (int j = 2; j <= 3; j++) {
							vecToObjX = racerMatrix[3][0] + racerMatrix[0][0] * boundsChecking[i] + racerMatrix[2][0] * boundsChecking[j] - x();
							vecToObjZ = racerMatrix[3][2] + racerMatrix[0][2] * boundsChecking[i] + racerMatrix[2][2] * boundsChecking[j] - z();

							if (vecToObjX < racerBounds.bounds[0] && vecToObjX > racerBounds.bounds[1] && vecToObjZ < racerBounds.bounds[2] && vecToObjZ > racerBounds.bounds[3]) {

								i = 4;
								kRacerBackSpeed = kRacerSpeed;
								kRacerSpeed = 0;
								collided = true;
								break;
							}
							
							
							vecToObjX = objMatrix[3][0] + objMatrix[0][0] * racerBounds.bounds[i] + objMatrix[2][0] * racerBounds.bounds[j] - obj->GetX();
							vecToObjZ = objMatrix[3][2] + objMatrix[0][2] * racerBounds.bounds[i] + objMatrix[2][2] * racerBounds.bounds[j] - obj->GetZ();

							if (vecToObjX < racerBounds.bounds[0] && vecToObjX > racerBounds.bounds[1] && vecToObjZ < racerBounds.bounds[2] && vecToObjZ > racerBounds.bounds[3]) {
								i = 4;
								kRacerBackSpeed = kRacerSpeed;
								kRacerSpeed = 0;
								collided = true;
								break;
							}
						}
					}
				}
				index++;
				
			}
		}
		if (kRacerBackSpeed <= 0) {
			collided = false;
		}

	}
	void checkStage(vector<IModel*>* checkpointObj, IFont* myFont) {
		if (warningMessage) {
			myFont->Draw("Stage " + to_string(currentStage + 1) + " is incomplete!", winWidth * 0.5, winHeight * 0.5, kRed, kCentre);
		}
		for (int i = currentStage; i < checkpointObj->size(); i++) {
			if (vectorLen(model, checkpointObj->at(i)) < 10) {
				if (currentStage == i) {
					currentStage++;
					warningMessage = false;
					break;
				}
				else {
					warningMessage = true;
					break;
				}
			}
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

string stringifyEnum(enum eGameState someEnum)
{
	switch (someEnum) {
		case Demo: return "Demo";
		case CountDown: return "Count Down";
		case Stage: return "Stage " + to_string(currentStage) + " Complete";
		case RaceComplete: return "Race Complete";
		case Developer: return "Developer";
		case Paused: return "Paused";

	}
}

void findBounds(IMesh* someMesh, Vector2* maxPoint) {
	someMesh->BeginEnumVertices();
	while (someMesh->GetVertex(pV1))
	{
		if (pV1[0] > maxPoint->bounds[0]) {
			maxPoint->bounds[0] = pV1[0];
		}
		if (pV1[2] > maxPoint->bounds[2]) {
			maxPoint->bounds[2] = pV1[2];
		}
		if (pV1[0] < maxPoint->bounds[1]) {
			maxPoint->bounds[1] = pV1[0];
		}
		if (pV1[2] < maxPoint->bounds[3]) {
			maxPoint->bounds[3] = pV1[2];
		}
	}
	someMesh->EndEnumVertices();
}

void loadModelsFromFile(vector<IModel*> &array, vector<Vector2> &arrayBounds, vector<IModel*>& arrayUncolliadable, string filename, I3DEngine* myEngine) {
	ifstream file(filename);

	string meshName;
	string meshCoords;
	IMesh* someMesh{};
	while (file.good())
	{
		getline(file, meshName);
		someMesh = myEngine->LoadMesh(meshName);

		getline(file, meshCoords);
		float x = stof(meshCoords);
		getline(file, meshCoords);
		float y = stof(meshCoords);
		getline(file, meshCoords);
		float z = stof(meshCoords);
		getline(file, meshCoords);
		float rotate = stof(meshCoords);

		if (meshName == "Checkpoint.x") {
			arrayUncolliadable.push_back(someMesh->CreateModel(x, y, z));
			arrayUncolliadable.back()->RotateLocalY(rotate);
		}
		else {
			array.push_back(someMesh->CreateModel(x, y, z));
			array.back()->RotateLocalY(rotate);
			
			Vector2 point;
			findBounds(someMesh, &point);
			arrayBounds.push_back(point);
		}	
	}
}

void changeCamera(I3DEngine* myEngine, ICamera* myCamera, Racer* thePlayer) {
	//GAME CAMERA STATES
	//FREE
	if (cameraState == Free) {

		myCamera->RotateY(myEngine->GetMouseMovementX() * frameTime * kCameraSpeed);
		myCamera->RotateLocalX(myEngine->GetMouseMovementY()* frameTime * kCameraSpeed);
		float calcCameraSpeed = kCameraSpeed * frameTime * myCamera->GetY() * 0.01;

		if (myEngine->KeyHeld(Key_Right)) {
			myCamera->MoveLocalX(calcCameraSpeed);
		}
		else if (myEngine->KeyHeld(Key_Left)) {
			myCamera->MoveLocalX(-calcCameraSpeed);
		}
		else if (myEngine->KeyHeld(Key_Up)) {
			myCamera->MoveLocalZ(calcCameraSpeed);
		}
		else if (myEngine->KeyHeld(Key_Down)) {
			myCamera->MoveLocalZ(-calcCameraSpeed);
		}
		else if (myEngine->KeyHit(Key_C)) {
			myCamera->ResetOrientation();
			myCamera->SetPosition(200, 200, 200);
			myCamera->RotateY(-90);
			myCamera->RotateLocalX(45);
		}

	} 
	else if (cameraState == FirstPerson) {
		myCamera->SetPosition(thePlayer->x(), thePlayer->y() + 5, thePlayer->z());
		thePlayer->model->GetMatrix(&thePlayer->racerMatrix[0][0]);
		myCamera->LookAt(thePlayer->x() + thePlayer->racerMatrix[2][0], thePlayer->y() + 5, thePlayer->z() + thePlayer->racerMatrix[2][2]);

	}
	//THIRD
	else if (cameraState == ThirdPerson) {
			
		thePlayer->model->GetMatrix(&thePlayer->racerMatrix[0][0]);
		myCamera->SetPosition(thePlayer->x() - thePlayer->racerMatrix[2][0] * kCameraOffset, thePlayer->y() - thePlayer->racerMatrix[2][1] * kCameraOffset + 15, thePlayer->z() - thePlayer->racerMatrix[2][2] * kCameraOffset);
		myCamera->LookAt(thePlayer->x(), thePlayer->y() + 10, thePlayer->z());

	}

	//SURVEILLANCE
	else if (cameraState == Surveillance) {
		//TODO - Position it somewhere
		myCamera->SetPosition(10, 10, 10);
		myCamera->LookAt(thePlayer->model);
	}
	
}

void displayCountDown(IFont* text, I3DEngine* myEngine) {

	timeCounter -= frameTime;

	if (timeCounter > 1) {
		text->Draw(to_string(timeCounter).substr(0, 1), winWidth / 2, winHeight / 2, kRed, kCentre);
	}
	else {
		bigText = "Go, go, go!";
		text->Draw(bigText, winWidth / 2 - text->MeasureTextWidth(bigText) / 2, winHeight / 2);
		if (timeCounter < 0) {
			gameState = Stage;
		}
	}

}

void startGame(IFont* myFont, I3DEngine* myEngine) {
	string text = "Press Space to Start";
	myFont->Draw(text, winWidth / 2 - myFont->MeasureTextWidth(text) / 2, winHeight / 2);

	if (myEngine->KeyHit(Key_Space)) {
		gameState = CountDown;
	}
}

IModel* loadModel(I3DEngine* myEngine, ICamera* myCamera, IModel* model) {
	
	
	
	if (model == nullptr) {
		if (myEngine->KeyHit(Key_1)) {
			newModelName = "Checkpoint.x";
		}
		else if (myEngine->KeyHit(Key_2)) {
			newModelName = "IsleCorner.x";
		}
		else if (myEngine->KeyHit(Key_3)) {
			newModelName = "IsleCross.x";
		}
		else if (myEngine->KeyHit(Key_4)) {
			newModelName = "IsleStraight.x";
		}
		else if (myEngine->KeyHit(Key_5)) {
			newModelName = "IsleTee.x";
		}
		else if (myEngine->KeyHit(Key_6)) {
			newModelName = "IsleCorner.x";
		}
		else if (myEngine->KeyHit(Key_7)) {
			newModelName = "Wall.x";
		}
		if (newModelName != "") {
			newMesh = myEngine->LoadMesh(newModelName);
			model = newMesh->CreateModel(myCamera->GetX(), 0, myCamera->GetZ());
		}
	}
	
	return model;
}

void moveNewModel(I3DEngine* myEngine, ICamera* myCamera) {

	
	if (newMesh != nullptr) {
		if (myEngine->KeyHeld(Key_D)) {
			newModel->MoveX(kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_A)) {
			newModel->MoveX(-kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_W)) {
			newModel->MoveZ(kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_S)) {
			newModel->MoveZ(-kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_E)) {
			newModel->RotateLocalY(kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_E)) {
			newModel->RotateLocalY(kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHeld(Key_Q)) {
			newModel->RotateLocalY(-kNewModelSpeed * frameTime * myCamera->GetY());
		}
		else if (myEngine->KeyHit(Key_Space)) {
			ofstream outfile;
			outfile.open(inputfile, ios::app);
			outfile << "\n" << newModelName << "\n";
			outfile << newModel->GetX() << "\n" << newModel->GetY() << "\n" << newModel->GetZ() << "\n";
			//TODO write rotate
			float matrix[4][4];
			newModel->GetMatrix(&matrix[0][0]);

			outfile << to_string(90 - atan2(matrix[2][2], matrix[2][0]) * (180 / 3.14159265));
			newModel = nullptr;
		}
		else if (myEngine->KeyHit(Key_Back)) {
			newModelName = "";
			newMesh->RemoveModel(newModel);
			newModel = nullptr;
		}
	}
}

void main()
{

	
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	winWidth = myEngine->GetWidth();
	winHeight = myEngine->GetHeight();

	// Add default folder for meshes and other media
	//myEngine->AddMediaFolder( "C:\\Users\\an-tonic\\source\\repos\\Game_Concepts-Hover_Racing\\Assesment 2 Resources" );
	myEngine->AddMediaFolder("./Resources");

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kManual);

	IMesh* skyMesh = myEngine->LoadMesh("Skybox.x");
	IModel* skyModel = skyMesh->CreateModel(0, -1000, 0);

	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0, 0, 0);
	
	IFont* myFont = myEngine->LoadFont("Amasis MT Pro Black", 36);
	IFont* myBigFont = myEngine->LoadFont("Amasis MT Pro Black", 76);

	ISprite* backdrop_console = myEngine->CreateSprite("ui_backdrop_console.tga", 0, 0);
	


	//Loading unmovable,collidable and uncollidable models from file
	vector<IModel*> staticObjects;
	vector<Vector2> staticObjectsBounds;

	vector<IModel*> staticNonCollidableObjects;
	loadModelsFromFile(staticObjects, staticObjectsBounds, staticNonCollidableObjects, "input.txt", myEngine);

	//Adding moving players
	vector<IModel*> dynamicObjects;
	Racer* player = &Racer(myEngine);
	

	dynamicObjects.push_back(player->model);
	myEngine->StopMouseCapture();

	ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.tga", 0, 0);

	high_resolution_clock::time_point start, finish;
	duration<double> duration;
	start = high_resolution_clock::now();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{	
		

		frameCount++;
		
		frameTime = myEngine->Timer();
		

		
		
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		myFont->Draw(to_string(1 / frameTime), borderForText, borderForText);
		string gameStateString = stringifyEnum(gameState);

		myFont->Draw(gameStateString, winWidth - myFont->MeasureTextWidth(gameStateString) - borderForText, borderForText, kCyan);

		//Major game states
		//DEMO
		if (gameState == Demo) {
			
			myFont->Draw("$ > Press SPACE to START_", winWidth * 0.02, winHeight * 0.925, kCyan);
			if (myEngine->KeyHit(Key_Space)) {
				gameState = CountDown;
				backdrop_console->~ISprite();
				ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.tga", 0, 0);
			}

		}
		//COUNT_DOWN
		else if (gameState == CountDown) {
			displayCountDown(myBigFont, myEngine);
		}
		//STAGE
		else if (gameState == Stage) {
			player->model->MoveLocalY(sin((frameCount % 500) * 0.01257)*0.003);
			myBigFont->Draw(to_string((int)player->kRacerSpeed), winWidth * 0.9, winHeight * 0.845, kCyan, kCentre);
			player->moveRight(myEngine->KeyHeld(Key_D));
			player->moveLeft(myEngine->KeyHeld(Key_A));
			player->moveForward(myEngine->KeyHeld(Key_W));
			player->moveBackward(myEngine->KeyHeld(Key_S));
			player->Collide(&staticObjects, &staticObjectsBounds);	
			player->checkStage(&staticNonCollidableObjects, myFont);
			
			
			
		}
		//RACE_COMPLETE
		else if (gameState == RaceComplete) {


		}
		//DEVELOPER MODE
		else if (gameState == Developer) {
			
			if (myEngine->AnyKeyHit()) {			
				newModel = loadModel(myEngine, myCamera, newModel);
			}
			if (newModel != nullptr) {
				moveNewModel(myEngine, myCamera);
			}		
		} 
		//PAUSED MODE
		else if (gameState == Paused) {
			//Do nothing
		}
		
		//Allowing camera to change when in any other state rather than Demo
		if (gameState != Demo) {
			//Change camera
			
			changeCamera(myEngine, myCamera, player);
			//GAME UTILS

			//FIRST
			if (myEngine->KeyHit(Key_1)) {
				cameraState = FirstPerson;
				myCamera->AttachToParent(player->model);

				
			}
			//FREE
			else if (myEngine->KeyHit(Key_2)) {
				cameraState = Free;
				myCamera->DetachFromParent();
				myCamera->SetPosition(200, 200, 200);
				myCamera->RotateY(-90);
				myCamera->RotateLocalX(45);
			}
			//THIRD
			else if (myEngine->KeyHit(Key_3)) {
				myCamera->AttachToParent(player->model);
				cameraState = ThirdPerson;
				
			}
			//Survaliance
			else if (myEngine->KeyHit(Key_4)) {
				cameraState = Surveillance;
				myCamera->DetachFromParent();
			}
			//Developer mode
			else if (myEngine->KeyHit(Key_F9)) {
				gameState = Developer;
				cameraState = Free;
				myCamera->DetachFromParent();
				myCamera->SetPosition(0, 300, 0);
				myCamera->ResetOrientation();
				myCamera->RotateX(90);
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
			
		}
		//Game Exit
		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}

		
		
	}
	
	finish = high_resolution_clock::now();
	duration = finish - start;

	cout << duration.count() << " seconds\n";
	cout << frameCount << " frames\n";
	cout << (frameCount/duration.count()) << " FPS\n";

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();


}
