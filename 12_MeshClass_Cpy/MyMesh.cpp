#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color){
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i) {
		m_lVertexCol.push_back(a_v3Color);
	}
}

void MyMesh::AddVertexPosition(vector3 a_v3Input){
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}

void MyMesh::AddVertexColor(vector3 a_v3Input){
	m_lVertexCol.push_back(a_v3Input);
}

void MyMesh::CompileOpenGL3X(void){
	if (m_bBinded)
		return;
	if (m_uVertexCount == 0)
		return;

	//CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++) {
		m_lVertex.push_back(m_lVertexPos[i]);
		m_lVertex.push_back(m_lVertexCol[i]);
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	//all prev code is done
	m_bBinded = true;

	glBindVertexArray(0);
}

void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel){
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader);

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));

	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}

void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft){
	
}

void AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight){
	
}
#pragma region 3D Primitives

void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color){

}

void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color){

}

void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color){

}

void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color){

}

void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color){

}

void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionHeight, int a_nSubdivisionAxis, vector3 a_v3Color){

}

void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color){

}
#pragma endregion