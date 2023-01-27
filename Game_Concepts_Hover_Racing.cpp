// Game_Concepts_Hover_Racing.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

using namespace tle;
using namespace std;


//Constants


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


	IMesh* skyMesh = myEngine->LoadMesh("Skybox 07.x");
	IModel* skyModel = skyMesh->CreateModel(0, -960, 0);

	IMesh* archMesh = myEngine->LoadMesh("Checkpoint.x");
	IModel* archModel = archMesh->CreateModel(0, 0, 0);


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
