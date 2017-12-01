#include "MyOctant.h"
using namespace Simplex;

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	m_uID = m_uMyOctantCount;
	m_uMyOctantCount++;

	m_v3Max = vector3(std::numeric_limits<float>::min());
	m_v3Min = vector3(std::numeric_limits<float>::max());

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

	m_pRoot = this;
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_uMyOctantCount++;
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	// TODO: insert return statement here
}

Simplex::MyOctant::~MyOctant(void)
{
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
	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
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
