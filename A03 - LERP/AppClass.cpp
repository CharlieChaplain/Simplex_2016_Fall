#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Stephen Callen - src3363@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		uint k = i - uSides; // counts from 0 to m_uOrbits
		std::vector<vector3> ring;
		float fAngle = 2 * glm::pi<float>() / (uSides + k); // the internal angle of each n-gon ring

		// this part populates stopsListList with stops for the spheres to visit as they LERP around
		for (uint j = 0; j < uSides + k; j++) {
			ring.push_back(vector3(fSize * cos(fAngle * j), fSize * sin(fAngle * j), 0.0f));
		}
		m_stopsListList.push_back(ring);


		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
	
	for (uint i = 0; i < m_uOrbits; ++i) {
		m_fPrevStopIndex.push_back(0);
		m_fNextStopIndex.push_back(1);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	// draw shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		// gets the necessary positions to LERP from via the list of stops. first number is which ring, second number is which stop of which ring
		vector3 v3PrevStop = m_stopsListList[i][m_fPrevStopIndex[i]];
		vector3 v3NextStop = m_stopsListList[i][m_fNextStopIndex[i]];

		float fAnimationLasts = 0.5f;
		float fPercent = MapValue(fTimer, 0.0f, fAnimationLasts, 0.0f, 1.0f);
		
		//calculate the current position
		vector3 v3CurrentPos = glm::lerp(v3PrevStop, v3NextStop, fPercent);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		// reassigns stop indices after they hit the next stop
		if (fPercent > 1.0) {
			for (int j = 0; j < m_uOrbits; j++) {
				if (m_fNextStopIndex[j] >= m_stopsListList[j].size() - 1)
					m_fNextStopIndex[j] = 0;
				else
					m_fNextStopIndex[j]++;

				if (m_fPrevStopIndex[j] >= m_stopsListList[j].size() - 1)
					m_fPrevStopIndex[j] = 0;
				else
					m_fPrevStopIndex[j]++;
			}
			
			fTimer = 0;
		}
		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}