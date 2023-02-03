// Game_Concepts_Hover_Racing.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

using namespace tle;
using namespace std;


//Constants
float frameTime = 0.0f;

//Functions



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

	IMesh* archMesh = myEngine->LoadMesh("Checkpoint.x");
	IModel* archModel = archMesh->CreateModel(0, 0, 0);
	IModel* archModel1 = archMesh->CreateModel(0, 0, 150);

	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IModel* wallModel = wallMesh->CreateModel(-10.5, 0, 46);
	IModel* wallModel2 = wallMesh->CreateModel(9.5, 0, 46);

	IMesh* isleMesh = myEngine->LoadMesh("IsleStraight.x");
	IModel* isleModel = isleMesh->CreateModel(-10, 0, 40);
	IModel* isleModel2 = isleMesh->CreateModel(10, 0, 40);
	IModel* isleModel3 = isleMesh->CreateModel(-10, 0, 53);
	IModel* isleModel4 = isleMesh->CreateModel(10, 0, 53);

	
	typedef struct Racer {
		IModel* model;

		Racer(I3DEngine* myEngine, float x = 0.0f, float y = 10.0f, float z = 0.0f){
			
			model = myEngine->LoadMesh("Racer.x")->CreateModel(x, y, z);
		}
	};

	
	Racer player(myEngine);

	

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{	
		frameTime = myEngine->Timer();
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/



	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
