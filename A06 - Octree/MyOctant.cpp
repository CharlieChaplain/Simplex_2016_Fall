#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uMyOctantCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_pEntityMngr = EntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	m_uID = m_uMyOctantCount;
	m_uMyOctantCount++;

	m_v3Max = vector3(-10000000.0f);
	m_v3Min = vector3(10000000.0f);

	//finds min and max points by looking at the farthest entities in the scene
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		if (m_pEntityMngr->GetEntity(i)->GetPosition().x > m_v3Max.x)
			m_v3Max.x = m_pEntityMngr->GetEntity(i)->GetPosition().x;
		else if (m_pEntityMngr->GetEntity(i)->GetPosition().x < m_v3Min.x)
			m_v3Min.x = m_pEntityMngr->GetEntity(i)->GetPosition().x;

		if (m_pEntityMngr->GetEntity(i)->GetPosition().y > m_v3Max.y)
			m_v3Max.y = m_pEntityMngr->GetEntity(i)->GetPosition().y;
		else if (m_pEntityMngr->GetEntity(i)->GetPosition().y < m_v3Min.y)
			m_v3Min.y = m_pEntityMngr->GetEntity(i)->GetPosition().y;

		if (m_pEntityMngr->GetEntity(i)->GetPosition().z > m_v3Max.z)
			m_v3Max.z = m_pEntityMngr->GetEntity(i)->GetPosition().z;
		else if (m_pEntityMngr->GetEntity(i)->GetPosition().z < m_v3Min.z)
			m_v3Min.z = m_pEntityMngr->GetEntity(i)->GetPosition().z;
	}

	m_v3Center = m_v3Max - m_v3Min;
	m_fSize = abs(m_v3Max.x) + abs(m_v3Min.x);
	m_pRoot = this;

	ConstructTree(a_nMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_pEntityMngr = EntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uID = m_uMyOctantCount;
	m_uMyOctantCount++;

	m_v3Max = a_v3Center + vector3(a_fSize);
	m_v3Min = a_v3Center - vector3(a_fSize);
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;
	delete(m_pParent);
	m_pParent = nullptr;
	if (m_uChildren != 0) {
		for (int i = 0; i < m_lChild.size(); i++) {
			delete(m_pChild[i]);
			m_pChild[i] = nullptr;
			delete(m_lChild[i]);
			m_lChild[i] = nullptr;
			m_uChildren = 0;
		}
	}
	//delete(m_pRoot);
	m_pRoot = nullptr;
	
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	if((m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().x >= m_v3Min.x && m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().x < m_v3Max.x) &&
		(m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().y >= m_v3Min.y && m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().y < m_v3Max.y) &&
		(m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().z >= m_v3Min.z && m_pEntityMngr->GetEntity(a_uRBIndex)->GetPosition().z < m_v3Max.z))
		return true;
	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	m_pMeshMngr->AddCubeToRenderList(glm::translate(m_v3Center)* glm::scale(vector3(m_fSize)), a_v3Color, 2);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	m_pMeshMngr->AddCubeToRenderList(glm::translate(m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, 2);
	if (m_uChildren != 0) {
		for (int i = 0; i < 8; i++)
			m_pChild[i]->Display(C_YELLOW);
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	/*
		   3-----0
		  /     /|
		 2-----1 |
		 | 7   | 4
		 |/    |/
		 6-----5
	*/

	m_pChild[0] = new MyOctant(m_v3Max - vector3(m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[1] = new MyOctant(m_v3Max - vector3(m_fSize / 4.0f,m_fSize / 4.0f, 3.0f * m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[2] = new MyOctant(m_v3Max - vector3(3.0f * m_fSize / 4.0f, m_fSize / 4.0f, 3.0f * m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[3] = new MyOctant(m_v3Max - vector3(3.0f * m_fSize / 4.0f, m_fSize / 4.0f, m_fSize / 4.0f), m_fSize / 2.0f);

	m_pChild[4] = new MyOctant(m_v3Max - vector3(m_fSize / 4.0f, 3.0f * m_fSize / 4.0f, m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[5] = new MyOctant(m_v3Max - vector3(m_fSize / 4.0f, 3.0f * m_fSize / 4.0f, 3.0f * m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[6] = new MyOctant(m_v3Max - vector3(3.0f * m_fSize / 4.0f, 3.0f * m_fSize / 4.0f, 3.0f * m_fSize / 4.0f), m_fSize / 2.0f);
	m_pChild[7] = new MyOctant(m_v3Max - vector3(3.0f * m_fSize / 4.0f, 3.0f * m_fSize / 4.0f, m_fSize / 4.0f), m_fSize / 2.0f);

	for (int i = 0; i < 8; i++) {
		m_pChild[i]->m_pParent = this;
		m_uChildren++;
	}

}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return a_nEntities > m_EntityList.size();
}

void Simplex::MyOctant::KillBranches(void)
{
	MyOctant* ToDie;
	if (m_uChildren != 0) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				ToDie = m_pChild[i]->GetChild(j);
				SafeDelete(ToDie);
			}
			ToDie = m_pChild[i];
			SafeDelete(ToDie);
		}
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (a_nMaxLevel <= 1) {
		return;
	}
	Subdivide();
	ConstructTree(a_nMaxLevel - 1);
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
}

uint Simplex::MyOctant::GetMyOctantCount(void)
{
	return m_uMyOctantCount;
}

void Simplex::MyOctant::Release(void)
{
}

void Simplex::MyOctant::Init(void)
{
}

void Simplex::MyOctant::ConstructList(void)
{
}
