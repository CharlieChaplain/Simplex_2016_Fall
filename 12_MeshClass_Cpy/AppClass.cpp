#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	m_pMesh = new MyMesh();

	m_pMesh->AddVertexPosition(vector3(0, 0, 0));
	m_pMesh->AddVertexPosition(vector3(1, 0, 0));
	m_pMesh->AddVertexPosition(vector3(0, 1, 0));

	m_pMesh->AddVertexColor(vector3(1, 0, 0));
	m_pMesh->AddVertexColor(vector3(0, 1, 0));
	m_pMesh->AddVertexColor(vector3(0, 0, 1));

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

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	matrix4 m4Proj = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Model = ToMatrix4(m_qArcBall);

	m_pMesh->Render(m4Proj, m4View, m4Model);
	
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
	if (m_pMesh != nullptr) {
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	//release GUI
	ShutdownGUI();
}