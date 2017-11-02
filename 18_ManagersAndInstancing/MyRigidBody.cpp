#include "MyRigidBody.h"

Simplex::MyRigidBody::MyRigidBody(MyMesh* a_pMesh)
{
	m_pBS = new Mesh();

	if (a_pMesh == nullptr)
		return;
	
	std::vector<vector3> pointsList = a_pMesh->GetVertexList();
	if (pointsList.size() < 1)
		return;
	fRadius = 0.0f;
	v3Center;
	v3Min = pointsList[0];
	v3Max = pointsList[0];

	for (uint i = 1; i < pointsList.size(); i++)
	{
		// X
		if (pointsList[i].x < v3Min.x)
			v3Min.x = pointsList[i].x;
		else if (pointsList[i].x > v3Max.x)
			v3Max.x = pointsList[i].x;
		// Y
		if (pointsList[i].y < v3Min.y)
			v3Min.y = pointsList[i].y;
		else if (pointsList[i].x > v3Max.y)
			v3Max.y = pointsList[i].y;
		// Z
		if (pointsList[i].z < v3Min.z)
			v3Min.z = pointsList[i].z;
		else if (pointsList[i].z > v3Max.z)
			v3Max.z = pointsList[i].z;
	}

	v3Center = (v3Min + v3Max) / 2.0f;
	fRadius = glm::distance(v3Center, v3Max); //use min or max, doesn't matter

	m_pBS->GenerateIcoSphere(1.0f, 2, C_WHITE);

	m_pBB = new Mesh();
	m_pBB->GenerateCube(1.0f, C_WHITE);
}

void Simplex::MyRigidBody::Render(MyCamera* a_pCamera, matrix4 a_m4Transform)
{
	float* pTemp = new float[16];

	matrix4 m4Resize = glm::scale(a_m4Transform, vector3(fRadius));
	memcpy(pTemp, glm::value_ptr(m4Resize), 16 * sizeof(float));
	m_pBS->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), pTemp, 1);

	vector3 v3Size = v3Max - v3Min;
	m4Resize = glm::scale(a_m4Transform, v3Size);
	memcpy(pTemp, glm::value_ptr(m4Resize), 16 * sizeof(float));
	m_pBB->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), pTemp, 1);

	SafeDelete(pTemp);
}
