#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)
	
	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	matrix4 m4Position = glm::translate(m_v3StevePos);
	matrix4 m4Rotation = glm::toMat4(m_qSteveQuat);
	m_pEntityMngr->SetModelMatrix(m4Position * m4Rotation);

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, -10.0f),			//Position
		m_v3StevePos + vector3(0.0f, 4.5f, 0.0f),	//Target
		AXIS_Y);							//Up


	uint uInstances = 50;
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	uInstances = nSquare * nSquare;
	uint uIndex = 0;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");
			vector3 v3Position = vector3(glm::sphericalRand(10.0f));
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
			//m_pEntityMngr->AddDimension(-1, uIndex);
			//++uIndex;

			if (v3Position.x < 0.0f)
			{
				if (v3Position.x < -17.0f)
					m_pEntityMngr->AddDimension(-1, 1);
				else
					m_pEntityMngr->AddDimension(-1, 2);
			}
			else if (v3Position.x > 0.0f)
			{
				if (v3Position.x > 17.0f)
					m_pEntityMngr->AddDimension(-1, 3);
				else
					m_pEntityMngr->AddDimension(-1, 4);
			}

		}
	}

	m_pEntityMngr->Update();
	//steve
	//m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3StevePos) * glm::toMat4(m_qSteveQuat), "Steve");

	m_pCameraMngr->SetPositionTargetAndUp(
		m_v3StevePos + (m_qSteveQuat * vector3(0.0f, 5.0f, -10.0f)),			//Position
		m_v3StevePos + vector3(0.0f, 4.5f, 0.0f),	//Target
		AXIS_Y);								//up
	
	//Update Entity Manager
	m_pEntityMngr->Update();
		
	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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