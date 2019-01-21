#include "ProceduralGrid.h"


ProceduralGrid::ProceduralGrid()
{
	init();
	createBuffers();
	setBuffers();
}
ProceduralGrid::~ProceduralGrid(void)
{
	deleteBuffers();
}
GLvoid ProceduralGrid::init() {
	vertexCount = 257;
	size = 3200;
	srand(time(NULL));
	seed = (GLfloat)rand();
	amplitude = 1000;
	count = vertexCount * vertexCount;
	verticesCount = count * 3;
	textureCoordsCount = count * 2;
	indicesCount = 6 * (vertexCount - 1) * (vertexCount - 1);
	noise = new FractalNoise(log2(vertexCount), seed, 0.50);
}
GLvoid ProceduralGrid::createBuffers()
{
	// Using heap memory due to the amount of memory required
	vertices = new GLfloat[verticesCount];
	normals = new GLfloat[verticesCount];
	textureCoords = new GLfloat[textureCoordsCount];
	indices = new GLuint[indicesCount];
	normalVectors = new glm::vec3[count * 2];

}
GLvoid ProceduralGrid::deleteBuffers()
{
	delete noise;
	delete[] vertices;
	delete[] normals;
	delete[] textureCoords;
	delete[] indices;
	delete[] normalVectors;
}
GLvoid ProceduralGrid::setBuffers() {
	GLuint i, j;
	//std::cout << "Bone count: " << m_iWidth << std::endl;
	GLuint vertexLocation = 0;
	for (GLuint i = 0; i < vertexCount; i++)
	{

		for (GLuint j = 0; j < vertexCount; j++)
		{

			// Vertices
			// x 
			vertices[vertexLocation * 3] = (GLfloat)j / ((GLfloat)vertexCount - 1) * size;

			// y 
			GLfloat height;
			if (i == vertexCount - 1 || i == 0 || j == 0 || j == vertexCount - 1)
			{
				height = 0;
				vertices[vertexLocation * 3 + 1] = 0.0f;
				glm::vec2 key = glm::vec2(j, i);
			}
			else
			{
				GLfloat value = noise->getAltitude(j, i);
				//GLfloat value = fractal.getAltitude(j, i);
				value *= amplitude;
				height = value;
				vertices[vertexLocation * 3 + 1] = value;
			}


			// z
			vertices[vertexLocation * 3 + 2] = (float)i / ((float)vertexCount - 1) * size;


			// Generate normals
			//glm::vec3 normal = generateNormal(j, i);
			normals[vertexLocation * 3] = 0.0f;
			normals[vertexLocation * 3 + 1] = 1.0f;
			normals[vertexLocation * 3 + 2] = 0.0f;

			// texture coords
			textureCoords[vertexLocation * 2] = (float)j / ((float)vertexCount - 1);
			textureCoords[vertexLocation * 2 + 1] = (float)i / ((float)vertexCount - 1);

			vertexLocation++;
		}
	}
	m_fMedianHeight = 0.0f;
	CalculateMedianHeight();
	//generating normals
	GLuint pointer = 0;
	GLuint triangleIndex = 0;
	for (GLuint zIncrement = 0; zIncrement < vertexCount - 1; zIncrement++)
	{
		for (GLuint xIncrement = 0; xIncrement < vertexCount - 1; xIncrement++)
		{
			//4 points of a quad
			//GLuint bottomLeft = (zIncrement * (vertexCount)) + xIncrement;
			//GLuint bottomRight = bottomLeft + 1;
			//GLuint topLeft = ((zIncrement + 1) * (vertexCount)) + xIncrement;
			//GLuint topRight = topLeft + 1;
			GLuint bottomRight = (zIncrement * (vertexCount)) + xIncrement;
			GLuint bottomLeft = bottomRight + 1;
			GLuint topRight = ((zIncrement + 1) * (vertexCount)) + xIncrement;
			GLuint topLeft = topRight + 1;
			/*
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;
			*/
			indices[pointer++] = bottomRight;
			indices[pointer++] = topRight;
			indices[pointer++] = topLeft;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomRight;
			indices[pointer++] = bottomLeft;

			//printf("********************************   QUAD %d   ""********************************\n\n\n", ((zIncrement* xIncrement) + xIncrement));

			//bottomLeft
			//glm::vec3 p1 = glm::vec3(vertices[bottomLeft*3], vertices[bottomLeft*3+1], vertices[bottomLeft*3+2]);
			//bottomRight
			glm::vec3 p1 = glm::vec3(vertices[bottomRight * 3], vertices[bottomRight * 3 + 1], vertices[bottomRight * 3 + 2]);

			//topLeft
			//glm::vec3 p2 = glm::vec3(vertices[topLeft*3], vertices[topLeft*3+1], vertices[topLeft*3+2]);
			//topRight
			glm::vec3 p2 = glm::vec3(vertices[topRight * 3], vertices[topRight * 3 + 1], vertices[topRight * 3 + 2]);

			//topRight
			//glm::vec3 p3 = glm::vec3(vertices[topRight * 3], vertices[topRight * 3 + 1], vertices[topRight * 3 + 2]);
			//topLeft
			glm::vec3 p3 = glm::vec3(vertices[topLeft * 3], vertices[topLeft * 3 + 1], vertices[topLeft * 3 + 2]);

			//bottomRight
			//glm::vec3 p4 = glm::vec3(vertices[bottomRight * 3], vertices[bottomRight * 3 + 1], vertices[bottomRight * 3 + 2]);
			//bottomLeft
			glm::vec3 p4 = glm::vec3(vertices[bottomLeft * 3], vertices[bottomLeft * 3 + 1], vertices[bottomLeft * 3 + 2]);

			//TRIANGLE 1 NORMALS
			normalVectors[triangleIndex] = generateNormal(p1, p2, p3);
			//triangleCentroidCoordinates[triangleIndex] = generateTriangleCentroid(p1,p2,p3);
			triangleIndex++;

			//TRIANGLE 2 NORMALS
			normalVectors[triangleIndex] = generateNormal(p3, p4, p1);
			//triangleCentroidCoordinates[triangleIndex] = generateTriangleCentroid(p3, p4, p1);
			triangleIndex++;
		}
	}
}
glm::vec3 ProceduralGrid::generateNormal(glm::vec3 trianglePoint1, glm::vec3 trianglePoint2, glm::vec3 trianglePoint3)
{
	// point 2 - point 1
	glm::vec3 side1 = trianglePoint2 - trianglePoint1;

	// point 3 - point 1 
	glm::vec3 side2 = trianglePoint3 - trianglePoint1;

	//calculating the cross product
	glm::vec3 normalVector;
	normalVector.x = (side1.y * side2.z) - (side1.z * side2.y);
	normalVector.y = (side1.z * side2.x) - (side1.x * side2.z);
	normalVector.z = (side1.x * side2.y) - (side1.y * side2.x);

	//normalizing so all x,y and z only add up to 1.0 and it doesn't increase the force's magnitude
	glm::vec3 scaledNormalVector;
	scaledNormalVector.x = normalVector.x / (fabs(normalVector.x) + fabs(normalVector.y) + fabs(normalVector.z));
	scaledNormalVector.y = normalVector.y / (fabs(normalVector.x) + fabs(normalVector.y) + fabs(normalVector.z));
	scaledNormalVector.z = normalVector.z / (fabs(normalVector.x) + fabs(normalVector.y) + fabs(normalVector.z));

	return scaledNormalVector;
}
void ProceduralGrid::CalculateMedianHeight(void)
{
	GLuint i, j;
	//std::cout << "Bone count: " << m_iWidth << std::endl;
	GLuint vertexLocation = 0;
	for (GLuint i = 0; i < vertexCount; i++)
	{
		for (GLuint j = 0; j < vertexCount; j++)
		{
			m_fMedianHeight += vertices[vertexLocation * 3 + 1];
		}

	}
	m_fMedianHeight = m_fMedianHeight / float(m_uiNumVertices);
}
//Returns the grid's total width and height as the x and z components of a vector
//glm::vec3 ProceduralGrid::GetGridSize(void) const
//{
//	glm::vec4 size(m_fHsize, 0.0f, m_fVsize, 0.0f);
//	return size;
//}
//
//float ProceduralGrid::GetMedianHeight(void) const
//{
//	return m_fMedianHeight;
//}
//
////xpos and ypos are in the grid's model space: it may be necessary to convert.
//float ProceduralGrid::GetPointHeight(float xpos, float zpos) const
//{
//	glm::vec3 tmp1, tmp2;
//	GLuint i, j;
//	float w;
//	float xposition = xpos;
//	float zposition = zpos;
//	float tmpy1, tmpy2, yposition;
//
//	xposition += 0.5f*float(m_iWidth - 1)*m_fTileSize;
//	zposition -= 0.5f*float(m_iHeight - 1)*m_fTileSize;
//	i = GLuint(xposition / m_fTileSize);
//	j = GLuint(0.0f - zposition / m_fTileSize);
//	if (i >= m_iWidth)
//	{
//		return 0.0f;
//	}
//	if (j >= m_iHeight)
//	{
//		return 0.0f;
//	}
//
//	GetVertexAttrib(POSITION, i, j, tmp1);
//	GetVertexAttrib(POSITION, i, j + 1, tmp2);
//	w = ((tmp1.z - zpos) / m_fTileSize);
//
//	tmpy1 = (1.0f - w)*tmp1.y + w*tmp2.y;
//
//	GetVertexAttrib(POSITION, i + 1, j, tmp1);
//	GetVertexAttrib(POSITION, i + 1, j + 1, tmp2);
//
//	tmpy2 = (1.0f - w)*tmp1.y + w*tmp2.y;
//
//	GetVertexAttrib(POSITION, i, j, tmp1);
//	GetVertexAttrib(POSITION, i + 1, j, tmp2);
//
//	w = ((xpos - tmp1.x) / m_fTileSize);
//	yposition = (1.0f - w)*tmpy1 + w*tmpy2;
//
//	return yposition;
//}
//
//glm::vec3 ProceduralGrid::GetPointNormal(float xpos, float zpos) const
//{
//	glm::vec3 postmp, tmp1, tmp2;
//	GLuint i, j;
//	float w;
//	float xposition = xpos;
//	float zposition = zpos;
//	glm::vec3 tmpn1, tmpn2, Normal;
//	Normal = glm::vec3(0.0f, 1.0f, 0.0f);
//
//	xposition += 0.5f*float(m_iWidth - 1)*m_fTileSize;
//	zposition -= 0.5f*float(m_iHeight - 1)*m_fTileSize;
//	i = GLuint(xposition / m_fTileSize);
//	j = GLuint(0.0f - zposition / m_fTileSize);
//	if (i >= m_iWidth)
//	{
//		return Normal;
//	}
//	if (j >= m_iHeight)
//	{
//		return Normal;
//	}
//
//	GetVertexAttrib(POSITION, i, j, tmp1);
//	w = ((tmp1.z - zpos) / m_fTileSize);
//
//	GetVertexAttrib(NORMAL, i, j, tmp1);
//	GetVertexAttrib(NORMAL, i, j + 1, tmp2);
//
//	tmpn1 = (1.0f - w)*tmp1 + w*tmp2;
//	glm::normalize(tmpn1);
//
//	GetVertexAttrib(NORMAL, i + 1, j, tmp1);
//	GetVertexAttrib(NORMAL, i + 1, j + 1, tmp2);
//
//	tmpn2 = (1.0f - w)*tmp1 + w*tmp2;
//	glm::normalize(tmpn2);
//
//	GetVertexAttrib(POSITION, i, j, tmp1);
//	w = ((xpos - tmp1.x) / m_fTileSize);
//
//	Normal = (1.0f - w)*tmpn1 + w*tmpn2;
//	glm::normalize(Normal);
//
//	return Normal;
//}
//
//void ProceduralGrid::Reset(float height)
//{
//	GLuint i, j;
//	for (i = 0; i<m_iHeight; i++)
//	{
//		for (j = 0; j<m_iWidth; j++)
//		{
//			vertices[(j + i*m_iWidth)] = (float(j) - (float(m_iHeight - 1)*0.5f))*m_fTileSize;
//			vertices[(j + i*m_iWidth)+1] = height;
//			vertices[(j + i*m_iWidth)+2] = ((float(m_iHeight - 1)*0.5f) - float(i))*m_fTileSize;
//			//m_pVertices[j + i*m_iWidth].Pos.w = 1.0f;
//		}
//	}
//	NormalGen();
//	m_fMedianHeight = height;
//}

void ProceduralGrid::RandomNoise(float magnitude)
{
	GLuint i, j;
	GLuint vertexLocation = 0;
	for (i = 0; i<vertexCount; i++)
	{
		for (j = 0; j<vertexCount; j++)
		{
			vertices[vertexLocation * 3+1] += magnitude*((float(rand()) / float(RAND_MAX)) - 0.5f);
			vertexLocation++;
		}
	}
	//NormalGen();
	CalculateMedianHeight();
}

void ProceduralGrid::Fault(GLuint iterations, float initdisplacement, float dampening)
{
	GLuint it, i, j;
	float disp = initdisplacement;
	glm::vec3 p1, p2, fault;
	glm::vec3 proj;
	for (it = 0; it<iterations; it++)
	{
		p1.x = m_fTileSize * float(vertexCount) * ((float(rand()) / float(RAND_MAX)) - 0.5f);
		p1.y = 0.0f;
		p1.z = m_fTileSize * float(m_iWidth) * (0.5f - (float(rand()) / float(RAND_MAX)));
		p2.x = m_fTileSize * float(m_iWidth) * ((float(rand()) / float(RAND_MAX)) - 0.5f);
		p2.y = 0.0f;
		p2.z = m_fTileSize * float(m_iWidth) * (0.5f - (float(rand()) / float(RAND_MAX)));
		fault = p2 - p1;
		//fault.w = 0.0f;
		GLuint vertexLocation = 0;
		for (i = 0; i<vertexCount; i++)
		{
			for (j = 0; j<vertexCount; j++)
			{
				proj = glm::vec3(vertices[vertexLocation * 3], vertices[vertexLocation * 3 + 1], vertices[vertexLocation * 3 + 2]);
				proj.y = 0.0f;
				proj = proj - p1;
				proj = glm::cross(proj, fault);
				glm::normalize(proj);
				vertices[vertexLocation * 3 + 1] += disp * proj.y;
				vertexLocation++;
			}
		}
		if (dampening > 0.0f && dampening < 1.0f)
		{
			disp = disp * dampening;
		}
		else
		{
			disp = disp * (float(iterations - it) / float(iterations));
		}
	}
	//NormalGen();
	CalculateMedianHeight();
}

//bool ProceduralGrid::GetVertexAttrib(VertexAttribute attr, GLuint column, GLuint row, glm::vec3 &output) const
//{
//	bool succeded = false;
//	if (column < m_iWidth)
//	{
//		if (row < m_iHeight)
//		{
//			switch (attr)
//			{
//			case POSITION:
//				output = m_pVertices[column + (row)*m_iWidth].Pos;
//				succeded = true;
//				break;
//			case NORMAL:
//				output = m_pVertices[column + (row)*m_iWidth].Norm;
//				succeded = true;
//				break;
//			case TEXCOORDINATE:
//				/*output = m_pVertices[column + (row)*m_iWidth].Tex;
//				succeded = true;*/
//				break;
//			}
//		}
//	}
//	return succeded;
//}

//void ProceduralGrid::NormalGen(void)
//{
//	GLuint i, j;
//	glm::vec3 VertexPos, Neighbor1Pos, Neighbor2Pos, TotalNorm, Norm, v1, v2;
//	bool r1, r2;
//	for (i = 0; i<m_iHeight; i++)
//	{
//		for (j = 0; j<m_iWidth; j++)
//		{
//			TotalNorm.x = 0.0f;
//			TotalNorm.y = 0.0f;
//			TotalNorm.z = 0.0f;
//			//TotalNorm.w = 0.0f;
//			GetVertexAttrib(POSITION, j, i, VertexPos);
//			if ((i + j) % 2 == 0)
//			{
//				r1 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j + 1), (i - 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j + 1), (i - 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j + 1), (i + 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j + 1), (i + 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j - 1), (i + 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j - 1), (i + 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j - 1), (i - 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j - 1), (i - 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//			}
//			else
//			{
//				r1 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//				r1 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor1Pos);
//				r2 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor2Pos);
//				if (r1 && r2)
//				{
//					v1 = Neighbor1Pos - VertexPos;
//					v2 = Neighbor2Pos - VertexPos;
//					Norm = glm::cross(v1, v2);
//					glm::normalize(Norm);
//					TotalNorm += Norm;
//				}
//			}
//			glm::normalize(TotalNorm);
//			m_pVertices[j + i*m_iWidth].Norm = TotalNorm;
//		}
//	}
//	GenerateVertexBuffer();
//}

//void ProceduralGrid::CalculateMedianHeight(void)
//{
//	GLuint i;
//	m_fMedianHeight = 0.0f;
//	for (i = 0; i<m_uiNumVertices; i++)
//	{
//		m_fMedianHeight += m_pVertices[i].Pos.y;
//	}
//	m_fMedianHeight = m_fMedianHeight / float(m_uiNumVertices);
//}

//void ProceduralGrid::GenerateVertexBuffer(void)
//{
//	glDeleteBuffers(1, &m_hVBOVertexBuffer);
//	glGenBuffers(1, &m_hVBOVertexBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, m_hVBOVertexBuffer);
//	glBufferData(GL_ARRAY_BUFFER, m_uiNumVertices * sizeof(Vertex), m_pVertices, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
//void ProceduralGrid::GenerateIndicesBuffer(void)
//{
//	glDeleteBuffers(1, &m_hVBOIndexBuffer);
//	glGenBuffers(1, &m_hVBOIndexBuffer);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hVBOIndexBuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * NumTriangles * sizeof(GLuint), pIndices, GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//}
//GLvoid ProceduralGrid::addVertexArrayAttrib(GLuint vao, GLuint vbo, GLint attribute, GLint size, GLint dataLength, GLint offset)
//{
//	glBindVertexArray(vao);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, dataLength * 4, (GLvoid*)(offset * 4));
//	glVertexAttribDivisor(attribute, 1);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
