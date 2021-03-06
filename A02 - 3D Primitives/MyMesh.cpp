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
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
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
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float fHalfHeight = a_fHeight / 2.0f;
	float fNumDegrees = 6.2832f / a_nSubdivisions; // number of radians per subdivision

	vector3 point0(0, fHalfHeight, 0); // top point
	vector3 point1(0, -fHalfHeight, 0); // bottom middle point
	vector3* bottomPoints = new vector3[a_nSubdivisions]; // contains the bottom ring of remaining points

	// poulates bottomPoints with the bottom ring of points
	for (int i = 0; i < a_nSubdivisions; i++) {
		bottomPoints[i] = vector3(a_fRadius * glm::cos(i * fNumDegrees), -fHalfHeight, a_fRadius * glm::sin(i * fNumDegrees));
	}

	// makes all the tris
	for (int i = 0; i < a_nSubdivisions; i++) {
		// if the index is at the end
		if (i == a_nSubdivisions - 1) {
			AddTri(bottomPoints[0], bottomPoints[i], point0);
			AddTri(bottomPoints[i], bottomPoints[0], point1);
		}
		else {
			AddTri(bottomPoints[i + 1], bottomPoints[i], point0);
			AddTri(bottomPoints[i], bottomPoints[i + 1], point1);
		}
	}
	
	delete[] bottomPoints;
	bottomPoints = nullptr;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float fHalfHeight = a_fHeight / 2.0f;
	float fNumDegrees = 6.2832f / a_nSubdivisions; // number of radians per subdivision

	vector3 point0(0, fHalfHeight, 0); // top middle point
	vector3 point1(0, -fHalfHeight, 0); // bottom middle point
	vector3* topPoints = new vector3[a_nSubdivisions]; // contains the top ring of points
	vector3* bottomPoints = new vector3[a_nSubdivisions]; // contains the bottom ring of points

	// poulates both arrays with points
	for (int i = 0; i < a_nSubdivisions; i++) {
		float x = glm::cos(i * fNumDegrees);
		float z = glm::sin(i * fNumDegrees);

		topPoints[i] = vector3(a_fRadius * x, fHalfHeight, a_fRadius * z);
		bottomPoints[i] = vector3(a_fRadius * x, -fHalfHeight, a_fRadius * z);
	}

	// makes all faces
	for (int i = 0; i < a_nSubdivisions; i++) {
		// if index is at the end
		if (i == a_nSubdivisions - 1) {
			AddTri(topPoints[0], topPoints[i], point0);								// makes top tri
			AddTri(bottomPoints[i], bottomPoints[0], point1);						// makes bottom tri
			AddQuad(bottomPoints[0], bottomPoints[i], topPoints[0], topPoints[i]);	// makes length face
		}
		else {
			AddTri(topPoints[i + 1], topPoints[i], point0);									// makes top tri
			AddTri(bottomPoints[i], bottomPoints[i + 1], point1);							// makes bottom tri
			AddQuad(bottomPoints[i + 1], bottomPoints[i], topPoints[i + 1], topPoints[i]);	// makes length face
		}
	}

	delete[] topPoints;
	delete[] bottomPoints;
	topPoints = nullptr;
	bottomPoints = nullptr;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	float fHalfHeight = a_fHeight / 2.0f;
	float fNumDegrees = 6.2832f / a_nSubdivisions; // number of radians per subdivision

	vector3* topPointsOut = new vector3[a_nSubdivisions]; // contains the top outer ring of points
	vector3* bottomPointsOut = new vector3[a_nSubdivisions]; // contains the bottom outer ring of points
	vector3* topPointsIn = new vector3[a_nSubdivisions]; // contains the top inner ring of points
	vector3* bottomPointsIn = new vector3[a_nSubdivisions]; // contains the bottom inner ring of points

	// poulates all 4 arrays with their respective points
	for (int i = 0; i < a_nSubdivisions; i++) {
		float x = glm::cos(i * fNumDegrees);
		float z = glm::sin(i * fNumDegrees);

		topPointsOut[i] = vector3(a_fOuterRadius * x, fHalfHeight, a_fOuterRadius * z);
		bottomPointsOut[i] = vector3(a_fOuterRadius * x, -fHalfHeight, a_fOuterRadius * z);
		topPointsIn[i] = vector3(a_fInnerRadius * x, fHalfHeight, a_fInnerRadius * z);
		bottomPointsIn[i] = vector3(a_fInnerRadius * x, -fHalfHeight, a_fInnerRadius * z);
	}

	// makes all faces
	for (int i = 0; i < a_nSubdivisions; i++) {
		// if index is at the end
		if (i == a_nSubdivisions - 1) {
			AddQuad(bottomPointsOut[0], bottomPointsOut[i], topPointsOut[0], topPointsOut[i]);		// makes outside face
			AddQuad(bottomPointsIn[i], bottomPointsIn[0], topPointsIn[i], topPointsIn[0]);			// makes inside face
			AddQuad(topPointsOut[0], topPointsOut[i], topPointsIn[0], topPointsIn[i]);				// makes top face
			AddQuad(bottomPointsOut[i], bottomPointsOut[0], bottomPointsIn[i], bottomPointsIn[0]);	// makes bottom face
		}
		else {
			AddQuad(bottomPointsOut[i + 1], bottomPointsOut[i], topPointsOut[i + 1], topPointsOut[i]);		// makes outside face
			AddQuad(bottomPointsIn[i], bottomPointsIn[i + 1], topPointsIn[i], topPointsIn[i + 1]);			// makes inside face
			AddQuad(topPointsOut[i + 1], topPointsOut[i], topPointsIn[i + 1], topPointsIn[i]);				// makes top face
			AddQuad(bottomPointsOut[i], bottomPointsOut[i + 1], bottomPointsIn[i], bottomPointsIn[i + 1]);	// makes bottom face
		}
	}

	delete[] topPointsOut;
	delete[] bottomPointsOut;
	delete[] topPointsIn;
	delete[] bottomPointsIn;
	topPointsOut = nullptr;
	bottomPointsOut = nullptr;
	topPointsIn = nullptr;
	bottomPointsIn = nullptr;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	float fTubeRadius = a_fOuterRadius - a_fInnerRadius;
	float fAngleA = 2 * glm::pi<float>() / a_nSubdivisionsA;
	float fAngleB = 2 * glm::pi<float>() / a_nSubdivisionsB;

	/*
		
		x = (c + a cos(v)) cos(u)
		z = a sin(v)
		z = (c + a cos(v)) sin(u)

		c = inner radius =	a_fInnerRadius
		a = tube radius =	fTubeRadius
		u, v = 0 to 2pi =	fAngleA, fAngleB respectively

	*/

	// vPoints holds all sphere points aside from the very top and bottom.
	std::vector<std::vector<vector3>> lPoints(a_nSubdivisionsA);

	// populates vPoints
	for (int y = 0; y < a_nSubdivisionsB; y++) {
		for (int x = 0; x < a_nSubdivisionsA; x++) {
			lPoints[a_nSubdivisionsB - 1 - y].push_back(vector3((a_fInnerRadius + (fTubeRadius * cos(y * fAngleB))) * cos(x * fAngleA),
																fTubeRadius * sin(y * fAngleB),
																(a_fInnerRadius + (fTubeRadius * cos(y * fAngleB))) * sin(x * fAngleA)));
		}
	}

	for (int x = 0; x < a_nSubdivisionsA; x++) {
		for (int y = 0; y < a_nSubdivisionsB; y++) {
			// if x is at the end
			if (x == a_nSubdivisionsA - 1) {
				//if y is at the end
				if (y == a_nSubdivisionsB - 1) {
					AddQuad(lPoints[0][0], lPoints[0][x], lPoints[y][0], lPoints[y][x]);
				}
				else {
					AddQuad(lPoints[y + 1][0], lPoints[y + 1][x], lPoints[y][0], lPoints[y][x]);
				}
			}
			else {
				//if y is at the end
				if (y == a_nSubdivisionsB - 1) {
					AddQuad(lPoints[0][x + 1], lPoints[0][x], lPoints[y][x + 1], lPoints[y][x]);
				}
				else {
					AddQuad(lPoints[y + 1][x + 1], lPoints[y + 1][x], lPoints[y][x + 1], lPoints[y][x]);
				}
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 3)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 12)
		a_nSubdivisions = 12;

	Release();
	Init();

	float fHorzAngle = (float)(glm::pi<float>() * 2.0f / a_nSubdivisions);
	float fVertAngle = (float)(glm::pi<float>() / a_nSubdivisions);

	// vPoints holds all sphere points aside from the very top and bottom.
	std::vector<std::vector<vector3>> lPoints(a_nSubdivisions);
	// these two are the top and bottom points of the sphere
	vector3 v3TopPoint(0.0f, a_fRadius, 0.0f);
	vector3 v3BotPoint(0.0f, -a_fRadius, 0.0f);

	// populates vPoints
	for (int y = 0; y < a_nSubdivisions; y++) {
		for (int x = 0; x < a_nSubdivisions; x++) {
			lPoints[a_nSubdivisions - 1 - y].push_back(vector3(a_fRadius * sin(x * fHorzAngle) * (cos((y * fVertAngle) - glm::half_pi<float>())),
										 a_fRadius * (sin((y * fVertAngle) - glm::half_pi<float>())),
										 a_fRadius * cos(x * fHorzAngle) * (cos((y * fVertAngle) - glm::half_pi<float>()))));
		}
	}

	//makes all faces
	for (int i = 0; i < a_nSubdivisions; i++) {
		// if index is at the end
		if (i == a_nSubdivisions - 1) {
			AddTri(lPoints[0][i], lPoints[0][0], v3TopPoint);										// makes top tri
			AddTri(lPoints[a_nSubdivisions - 1][0], lPoints[a_nSubdivisions - 1][i], v3BotPoint);	// makes bottom tri
		}
		else {
			AddTri(lPoints[0][i], lPoints[0][i + 1], v3TopPoint);										// makes top tri
			AddTri(lPoints[a_nSubdivisions - 1][i + 1], lPoints[a_nSubdivisions - 1][i], v3BotPoint);	// makes bottom tri
		}

		//this for loop will loop through all vertical slices.
		//It stops before the last ring because all points in that ring are basically equal to v3BotPoint
		for (int j = 0; j < a_nSubdivisions - 1; j++) {
			// if index is at the end
			if (i == a_nSubdivisions - 1) {
				AddQuad(lPoints[j + 1][i], lPoints[j + 1][0], lPoints[j][i], lPoints[j][0]);
			}
			else {
				
				AddQuad(lPoints[j + 1][i], lPoints[j + 1][i + 1], lPoints[j][i], lPoints[j][i + 1]);
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}