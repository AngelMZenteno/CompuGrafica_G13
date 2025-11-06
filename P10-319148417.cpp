/*
Proyecto Final
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>	
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
float guardoArchivo = 0, reinicioArchivo = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;
Texture Mushroom;
Texture CuerpoRakan;
Texture CapaRakan;
Texture Teemo;
Texture Braum;
Texture Gnar;
Texture Karma;
Texture MascotaAzul;

Model Blackhawk_M;
Model Lampara;
Model Hongo;
Model Rakan;
Model Rakan_PI;
Model Teemo_M;
Model Braum_M;
Model Gnar_M;
Model Karma_M;
Model MascotaAzul_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
// para declarar varias luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Spotlights para el faro del carro
SpotLight spotLightFaroDel;
SpotLight spotLightFaroTra;

//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	



	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int dado_indices[] = {
		0, 1, 2,       // Cara 1
		3, 4, 5,       // Cara 2
		6, 7, 8,       // Cara 3
		9, 10, 11,     // Cara 4
		12, 13, 14,    // Cara 5
		15, 16, 17,    // Cara 6
		18, 19, 20,    // Cara 7
		21, 22, 23     // Cara 8
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat dado_vertices[] = {
		// c1
		//x		y		z		S		T			NX		NY		NZ
		// Cara 1: Superior, Frontal, Derecha
		 0.0f,  0.5f,  0.0f,    0.644f,  0.715f,    -0.577f,  -0.577f, -0.577f,
		 0.0f,  0.0f,  0.5f,    0.373f,  0.873f,    -0.577f,  -0.577f, -0.577f,  //5
		 0.5f,  0.0f,  0.0f,    0.373f,  0.56f,     -0.577f,  -0.577f,  -0.577f,

		 // Cara 2: Superior, Trasera, Derecha
		0.0f,  0.5f,  0.0f,    0.658f,  0.706f,    -0.577f,  -0.577f, 0.577f,
		0.5f,  0.0f,  0.0f,    0.658f,  0.393f,     -0.577f,  -0.577f, 0.577f, //7
		0.0f,  0.0f, -0.5f,    0.933f,  0.551f,     -0.577f,  -0.577f, 0.577f,

		// Cara 3: Superior, Trasera, Izquierda
		0.0f,  0.5f,  0.0f,    0.651,	0.709,    0.577f,  -0.577f, 0.577f,
		0.0f,  0.0f, -0.5f,    0.378,	0.551,    0.577f,  -0.577f, 0.577f,   //6
		-0.5f,  0.0f,  0.0f,    0.651,	0.393,   0.577f,  -0.577f, 0.577f,

		// Cara 4: Superior, Frontal, Izquierda
		0.0f,  0.5f,  0.0f,    0.662f,  0.715f,    0.577f,  -0.577f,  -0.577f,
		-0.5f,  0.0f,  0.0f,    0.935f,  0.56f,   0.577f,  -0.577f,  -0.577f,  //8
		0.0f,  0.0f,  0.5f,    0.935f,  0.873f,   0.577f,  -0.577f,  -0.577f,

		// --- Pirámide Inferior ---

		// Cara 5: Inferior, Frontal, Derecha
		0.0f, -0.5f,  0.0f,    0.357f,  0.222f,     -0.577f, 0.577f,  -0.577f,
		0.5f,  0.0f,  0.0f,    0.087f,  0.378f,     -0.577f, 0.577f,  -0.577f,   //1
		0.0f,  0.0f,  0.5f,    0.087f,  0.064f,     -0.577f, 0.577f,  -0.577f,

		// Cara 6: Inferior, Trasera, Derecha
		0.0f, -0.5f,  0.0f,    0.373f,0.229f,     -0.577f, 0.577f, 0.577f,
		0.0f,  0.0f, -0.5f,    0.644f,0.384f,      -0.577f, 0.577f, 0.577f,			//3
		0.5f,  0.0f,  0.0f,    0.373f,0.542,      -0.577f, 0.577f, 0.577f,

		// Cara 7: Inferior, Trasera, Izquierda
		0.0f, -0.5f,  0.0f,     0.364f,  0.229f,   0.577f, 0.577f, 0.577f,
		-0.5f,  0.0f,  0.0f,    0.364f,0.542f,    0.577f, 0.577f, 0.577f,  //2
		0.0f,  0.0f, -0.5f,    0.091f,0.386f,    0.577f, 0.577f, 0.577f,

		// Cara 8: Inferior, Frontal, Izquierda
		0.0f, -0.5f,  0.0f,    0.378f, 0.22f,    0.577f, 0.577f,  -0.577f,
		0.0f,  0.0f,  0.5f,    0.649f,  0.064f,    0.577f, 0.577f,  -0.577f,		//4
		-0.5f,  0.0f,  0.0f,   0.649f,  0.378f,    0.577f, 0.577f,  -0.577f,
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(dado_vertices, dado_indices, 192, 36);
	meshList.push_back(dado);
	
}

void CrearCuboUnitario()
{
	// --- Definición de un Cubo Unitario (1x1x1) ---
	// *** NORMALES INVERTIDAS (apuntan hacia adentro) ***
	GLfloat cuboVertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Cara Frontal (Normal: 0,0,-1)
		-0.5f, -0.5f, 0.5f,	 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 0
		 0.5f, -0.5f, 0.5f,	 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 1
		 0.5f,  0.5f, 0.5f,	 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 2
		-0.5f,  0.5f, 0.5f,	 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 3

		// Cara Trasera (Normal: 0,0,1)
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 4
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 5
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 6
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 7

		// Cara Superior (Normal: 0,-1,0)
		-0.5f, 0.5f,  0.5f,	 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 8
		 0.5f, 0.5f,  0.5f,	 1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 9
		 0.5f, 0.5f, -0.5f,	 1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 10
		-0.5f, 0.5f, -0.5f,	 0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 11

		// Cara Inferior (Normal: 0,1,0)
		-0.5f, -0.5f,  0.5f,	 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 12
		 0.5f, -0.5f,  0.5f,	 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 13
		 0.5f, -0.5f, -0.5f,	 1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 14
		-0.5f, -0.5f, -0.5f,	 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 15

		// Cara Izquierda (Normal: 1,0,0)
		-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 16
		-0.5f, -0.5f,  0.5f,	 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 17
		-0.5f,  0.5f,  0.5f,	 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 18
		-0.5f,  0.5f, -0.5f,	 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 19

		// Cara Derecha (Normal: -1,0,0)
		 0.5f, -0.5f,  0.5f,	 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 20
		 0.5f, -0.5f, -0.5f,	 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 21
		 0.5f,  0.5f, -0.5f,	 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 22
		 0.5f,  0.5f,  0.5f,	 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f  // 23
	};

	// Índices ESTÁNDAR (CCW)
	unsigned int cuboIndices[] = {
		0, 1, 2,   2, 3, 0,		// Frontal
		4, 5, 6,   6, 7, 4,		// Trasera
		8, 9, 10,  10, 11, 8,	// Superior
		12, 13, 14, 14, 15, 12,	// Inferior
		16, 17, 18, 18, 19, 16,	// Izquierda
		20, 21, 22, 22, 23, 20	// Derecha
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cuboVertices, cuboIndices, 192, 36);
	meshList.push_back(cubo); // Se añadirá en meshList[5]
}

void CrearBaseTruncada()
{
	// --- Definición de una Pirámide MUY Truncada ---
	// Base de 1x1 (en y=-0.5), Top de 0.9x0.9 (en y=0.5), Altura 1.0
	// Centrado en el origen (va de -0.5 a 0.5 en Y)

	// Vértices base (y = -0.5)
	glm::vec3 p0 = glm::vec3(-0.5f, -0.5f, 0.5f);  // B0 (frontal-izq)
	glm::vec3 p1 = glm::vec3(0.5f, -0.5f, 0.5f);   // B1 (frontal-der)
	glm::vec3 p2 = glm::vec3(0.5f, -0.5f, -0.5f);  // B2 (trasera-der)
	glm::vec3 p3 = glm::vec3(-0.5f, -0.5f, -0.5f); // B3 (trasera-izq)
	// Vértices top (y = 0.5)
	glm::vec3 p4 = glm::vec3(-0.45f, 0.5f, 0.45f); // T4 (frontal-izq)
	glm::vec3 p5 = glm::vec3(0.45f, 0.5f, 0.45f);  // T5 (frontal-der)
	glm::vec3 p6 = glm::vec3(0.45f, 0.5f, -0.45f); // T6 (trasera-der)
	glm::vec3 p7 = glm::vec3(-0.45f, 0.5f, -0.45f); // T7 (trasera-izq)

	// *** CÁLCULO DE NORMALES (Apunta Hacia Adentro) ***
	// Se invierte el orden del cross product para voltear la normal
	// Normal para cara frontal (p0, p1, p5, p4)
	glm::vec3 edgeF1 = p1 - p0;
	glm::vec3 edgeF2 = p4 - p0;
	glm::vec3 normalFront = glm::normalize(glm::cross(edgeF2, edgeF1)); // Invertido

	// Normal para cara trasera (p2, p3, p7, p6)
	glm::vec3 edgeB1 = p3 - p2;
	glm::vec3 edgeB2 = p6 - p2;
	glm::vec3 normalBack = glm::normalize(glm::cross(edgeB2, edgeB1)); // Invertido

	// Normal para cara izquierda (p3, p0, p4, p7)
	glm::vec3 edgeL1 = p0 - p3;
	glm::vec3 edgeL2 = p7 - p3;
	glm::vec3 normalLeft = glm::normalize(glm::cross(edgeL2, edgeL1)); // Invertido

	// Normal para cara derecha (p1, p2, p6, p5)
	glm::vec3 edgeR1 = p2 - p1;
	glm::vec3 edgeR2 = p5 - p1;
	glm::vec3 normalRight = glm::normalize(glm::cross(edgeR2, edgeR1)); // Invertido


	// *** VÉRTICES CON NORMALES INVERTIDAS ***
	GLfloat frustumVertices[] = {
		// x, y, z, u, v, nx, ny, nz
		// Caras definidos en sentido ANTI-HORARIO (CCW) visto desde FUERA

		// Cara Superior (Normal: 0,-1,0)
		p7.x, p7.y, p7.z,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // T7 (0)
		p6.x, p6.y, p6.z,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // T6 (1)
		p5.x, p5.y, p5.z,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // T5 (2)
		p4.x, p4.y, p4.z,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // T4 (3)

		// Cara Inferior (Normal: 0,1,0)
		p3.x, p3.y, p3.z,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // B3 (4)
		p0.x, p0.y, p0.z,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // B0 (5)
		p1.x, p1.y, p1.z,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // B1 (6)
		p2.x, p2.y, p2.z,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // B2 (7)

		// Cara Frontal (Normal calculada e invertida)
		p0.x, p0.y, p0.z,  0.0f, 0.0f,  normalFront.x, normalFront.y, normalFront.z, // B0 (8)
		p1.x, p1.y, p1.z,  1.0f, 0.0f,  normalFront.x, normalFront.y, normalFront.z, // B1 (9)
		p5.x, p5.y, p5.z,  1.0f, 1.0f,  normalFront.x, normalFront.y, normalFront.z, // T5 (10)
		p4.x, p4.y, p4.z,  0.0f, 1.0f,  normalFront.x, normalFront.y, normalFront.z, // T4 (11)

		// Cara Trasera (Normal calculada e invertida)
		p2.x, p2.y, p2.z,  0.0f, 0.0f,  normalBack.x, normalBack.y, normalBack.z, // B2 (12)
		p3.x, p3.y, p3.z,  1.0f, 0.0f,  normalBack.x, normalBack.y, normalBack.z, // B3 (13)
		p7.x, p7.y, p7.z,  1.0f, 1.0f,  normalBack.x, normalBack.y, normalBack.z, // T7 (14)
		p6.x, p6.y, p6.z,  0.0f, 1.0f,  normalBack.x, normalBack.y, normalBack.z, // T6 (15)

		// Cara Izquierda (Normal calculada e invertida)
		p3.x, p3.y, p3.z,  0.0f, 0.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // B3 (16)
		p0.x, p0.y, p0.z,  1.0f, 0.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // B0 (17)
		p4.x, p4.y, p4.z,  1.0f, 1.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // T4 (18)
		p7.x, p7.y, p7.z,  0.0f, 1.0f,  normalLeft.x, normalLeft.y, normalLeft.z, // T19)

		// Cara Derecha (Normal calculada e invertida)
		p1.x, p1.y, p1.z,  0.0f, 0.0f,  normalRight.x, normalRight.y, normalRight.z, // B1 (20)
		p2.x, p2.y, p2.z,  1.0f, 0.0f,  normalRight.x, normalRight.y, normalRight.z, // B2 (21)
		p6.x, p6.y, p6.z,  1.0f, 1.0f,  normalRight.x, normalRight.y, normalRight.z, // T6 (22)
		p5.x, p5.y, p5.z,  0.0f, 1.0f,  normalRight.x, normalRight.y, normalRight.z  // T5 (23)
	};

	// *** ÍNDICES ESTÁNDAR (Winding order CCW - Anti-horario) ***
	unsigned int frustumIndices[] = {
		0, 1, 2,   2, 3, 0,		// Superior
		4, 5, 6,   6, 7, 4,		// Inferior
		8, 9, 10,  10, 11, 8,	// Frontal
		12, 13, 14,  14, 15, 12,	// Trasera
		16, 17, 18,  18, 19, 16,	// Izquierda
		20, 21, 22,  22, 23, 20	// Derecha
	};

	Mesh* base = new Mesh();
	base->CreateMesh(frustumVertices, frustumIndices, 192, 36);
	meshList.push_back(base); // Se añadirá en meshList[6]
}

//NEW// Keyframes				-150.0f, 0.5f, 120.0f
float posXMAzul = -70.0, posYMAzul =-1.0, posZMAzul = 170.0;
float movMAzul_x = 0.0f, movMAzul_y = 0.0f, movMAzul_z = 0.0f; 
float giroMAzul = 0;

#define MAX_FRAMES 144 //Número de cuadros máximos
int i_max_steps = 144; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movMAzul_x;		//Variable para PosicionX
	float movMAzul_y;		//Variable para PosicionY
	float movMAzul_z;		//Variable para PosicionZ 
	float movMAzul_xInc;		//Variable para IncrementoX
	float movMAzul_yInc;		//Variable para IncrementoY
	float movMAzul_zInc;		//Variable para IncrementoZ
	float giroMAzul;		//Variable para GiroAvion
	float giroMAzulInc;		//Variable para IncrementoGiroAvion
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movMAzul_x = movMAzul_x;
	KeyFrame[FrameIndex].movMAzul_y = movMAzul_y;
	KeyFrame[FrameIndex].movMAzul_z = movMAzul_z;
	KeyFrame[FrameIndex].giroMAzul = giroMAzul;
	//Se agregan nuevas líneas para guardar más variables si es necesario

	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movMAzul_x = KeyFrame[0].movMAzul_x;
	movMAzul_y = KeyFrame[0].movMAzul_y;
	movMAzul_z = KeyFrame[0].movMAzul_z;
	giroMAzul = KeyFrame[0].giroMAzul;
}

//Función para guardar los keyframes en un archivo de texto
void guardarFramesArchivo(void)
{
	std::ofstream archivo; // Crea un objeto de flujo de salida
	archivo.open("keyframes.txt"); // Abre o crea el archivo

	if (archivo.is_open())
	{
		printf("Guardando %d keyframes en 'keyframes.txt'...\n", FrameIndex);
		// Itera desde el frame 0 hasta el último guardado (FrameIndex)
		for (int i = 0; i < FrameIndex; i++)
		{
			// Escribe las variables de cada keyframe en una nueva línea
			archivo << "KeyFrame[" << i << "].movMAzul_x = " << KeyFrame[i].movMAzul_x << "f;\n";
			archivo << "KeyFrame[" << i << "].movMAzul_y = " << KeyFrame[i].movMAzul_y << "f;\n";
			archivo << "KeyFrame[" << i << "].movMAzul_z = " << KeyFrame[i].movMAzul_z << "f;\n";
			archivo << "KeyFrame[" << i << "].giroMAzul = " << KeyFrame[i].giroMAzul << ";\n\n";
		}
		archivo.close(); // Cierra el archivo
		printf("¡Keyframes guardados exitosamente!\n");
	}
	else
	{
		printf("Error: No se pudo abrir el archivo 'keyframes.txt' para guardar.\n");
	}
}

bool cargarFramesDesdeArchivo()
{
	std::ifstream archivo; // Objeto de flujo de ENTRADA
	archivo.open("keyframes.txt");

	if (!archivo.is_open())
	{
		printf("No se encontró 'keyframes.txt'. Se usarán los keyframes por defecto.\n");
		return false; // Indica que no se cargó nada
	}

	printf("Cargando keyframes guardados desde 'keyframes.txt'...\n");
	FrameIndex = 0; // Reiniciamos el contador global de frames
	std::string linea;
	int estado_carga = 0; // 0=buscando X, 1=buscando Y, 2=buscando Z, 3= buscando Giro

	// Leer el archivo línea por línea
	while (FrameIndex < MAX_FRAMES && std::getline(archivo, linea))
	{
		// Ignora líneas vacías 
		if (linea.empty()) {
			continue;
		}

		// Encontrar la posición del '=' y del ';'
		size_t pos_igual = linea.find("=");
		size_t pos_punto_coma = linea.find(";");

		// Si la línea no tiene el formato esperado, la ignoramos
		if (pos_igual == std::string::npos || pos_punto_coma == std::string::npos) {
			continue;
		}

		// Extraer la sub-cadena que contiene el valor (entre '=' y ';')
		std::string valor_str = linea.substr(pos_igual + 1, pos_punto_coma - (pos_igual + 1));

		float valor_float;
		try {
			// Convertir la cadena (que puede tener 'f' o espacios) a float
			valor_float = std::stof(valor_str);
		}
		catch (const std::exception& e) {
			printf("Error al leer valor en línea: %s\n", linea.c_str());
			continue; // Saltar esta línea si está mal formada
		}

		// Asignar el valor al keyframe actual basado en el orden (X, luego Y, luego Giro)
		switch (estado_carga)
		{
		case 0: // Esperando movMAzul_x
			if (linea.find("movMAzul_x") != std::string::npos) {
				KeyFrame[FrameIndex].movMAzul_x = valor_float;
				estado_carga = 1; // Ahora buscar Y
			}
			break;
		case 1: // Esperando movMAzul_y
			if (linea.find("movMAzul_y") != std::string::npos) {
				KeyFrame[FrameIndex].movMAzul_y = valor_float;
				estado_carga = 2; // Ahora buscar Giro
			}
			break;
		case 2: // Esperando movMAzul_z (NUEVO)
			if (linea.find("movMAzul_z") != std::string::npos) {
				KeyFrame[FrameIndex].movMAzul_z = valor_float;
				estado_carga = 3; // Ahora buscar Giro
			}
			break;
		case 3: // Esperando giroMAzul (ERA EL CASO 2)
			if (linea.find("giroMAzul") != std::string::npos) {
				KeyFrame[FrameIndex].giroMAzul = valor_float;
				estado_carga = 0; // Resetea para el próximo frame
				FrameIndex++;     // Frame completado, incrementar contador
			}
			break;
		}
	}

	archivo.close();

	if (FrameIndex == 0)
	{
		printf("El archivo 'keyframes.txt' estaba vacío o no se pudo leer. Se usarán los keyframes por defecto.\n");
		return false;
	}

	printf("¡Carga completa! Se leyeron %d keyframes desde el archivo.\n", FrameIndex);
	return true; // Indica que la carga fue exitosa
}

void interpolation(void)
{
	KeyFrame[playIndex].movMAzul_xInc = (KeyFrame[playIndex + 1].movMAzul_x - KeyFrame[playIndex].movMAzul_x) / i_max_steps;
	KeyFrame[playIndex].movMAzul_yInc = (KeyFrame[playIndex + 1].movMAzul_y - KeyFrame[playIndex].movMAzul_y) / i_max_steps;
	KeyFrame[playIndex].movMAzul_zInc = (KeyFrame[playIndex + 1].movMAzul_z - KeyFrame[playIndex].movMAzul_z) / i_max_steps;
	KeyFrame[playIndex].giroMAzulInc = (KeyFrame[playIndex + 1].giroMAzul - KeyFrame[playIndex].giroMAzul) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movMAzul_x += KeyFrame[playIndex].movMAzul_xInc;
			movMAzul_y += KeyFrame[playIndex].movMAzul_yInc;
			movMAzul_z += KeyFrame[playIndex].movMAzul_zInc;
			giroMAzul += KeyFrame[playIndex].giroMAzulInc;
			i_curr_steps++;
		}

	}
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearCuboUnitario();
	CrearBaseTruncada();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado-octaedro.jpg");
	dadoTexture.LoadTextureA();
	Mushroom = Texture("Textures/Mushroom.png");
	Mushroom.LoadTextureA();
	CuerpoRakan = Texture("Textures/Rakan_cuerpo.png");
	CuerpoRakan.LoadTextureA();
	CapaRakan = Texture("Textures/Rakan_capa.png");
	CapaRakan.LoadTextureA();
	Teemo = Texture("Textures/Teemo.png");
	Teemo.LoadTextureA();
	Braum = Texture("Textures/Braum.png");	
	Braum.LoadTextureA();	
	Gnar = Texture("Textures/Gnar.png");	
	Gnar.LoadTextureA();
	Karma = Texture("Textures/karma.png");
	Karma.LoadTextureA();
	MascotaAzul = Texture("Textures/MascotaAzul.png");
	MascotaAzul.LoadTextureA();

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/Lamp.obj");
	Hongo= Model();
	Hongo.LoadModel("Models/honguitoTeemo.obj");
	Rakan = Model();
	Rakan.LoadModel("Models/Rakan.obj");
	Rakan_PI = Model();	
	Rakan_PI.LoadModel("Models/Rakan_PI.obj");	
	Teemo_M = Model();
	Teemo_M.LoadModel("Models/Teemo.obj");	
	Braum_M = Model();
	Braum_M.LoadModel("Models/Braum.obj");
	Gnar_M = Model();
	Gnar_M.LoadModel("Models/Mega_Gnar.obj");
	Karma_M = Model();
	Karma_M.LoadModel("Models/Karma.obj");
	MascotaAzul_M = Model();
	MascotaAzul_M.LoadModel("Models/MascotaAzul.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Poste de Luz
	pointLights[1] = PointLight(
		1.0f, 1.0f, 1.0f,  // color RGB
		1.0f, 2.0f,        // intensidades ambiental y difusa
		-15.0f, 10.0f, -4.0f,  // posición x, y, z
		1.0f, 0.1f, 0.0f   // constante, lineal, exponente
	);
	pointLightCount++;

	// Luz del hongo
	pointLights[2] = PointLight(
		1.0f, 1.0f, 1.0f,  // color RGB
		1.0f, 1.0f,        // intensidades ambiental y difusa
		15.0f, 2.0f, -8.0f,  // posición x, y, z
		1.0f, 0.1f, 0.0f   // constante, lineal, exponente
	);
	pointLightCount++;

	const float lamp_aIntensity_ON = 1.0f;
	const float lamp_dIntensity_ON = 2.0f;

	const float hongo_aIntensity_ON = 1.0f;
	const float hongo_dIntensity_ON = 1.0f;


	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//Luz del cofre
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		-5.0f, 0.0f,  0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	
	// Faros del carro
	spotLightFaroDel = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 5.0f,  // Intensidad normal
		5.0f, 15.0f, 0.0f,
		-5.0f, 0.0f, 0.0f,
		1.0f, 0.01f, 0.001f, 
		45.0f);

	// Faros del carro (Plantilla APAGADA)
	spotLightFaroTra = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 5.0f,   //misma intensidad
		5.0f, 15.0f, 0.0f, 
		5.0f, 0.0f, 0.0f, 
		1.0f, 0.01f, 0.001f,
		45.0f);
	spotLights[2] = spotLightFaroDel;
	spotLightCount++;

	// Luz helicoptero
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		0.0f,0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		30.0f);
	spotLightCount++;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformColor = shaderList[0].getColorLocation();

	//información en el shader de intensidad especular y brillo
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();


	// Estas son constantes y solo necesitan declararse e inicializarse una vez.
	glm::vec3 lightOffsetLocal(0.0f, -1.5f, 0.0f);
	glm::vec3 lightDirLocal(0.0f, 0.0f, -1.0f);
	// Offset de los faros del carro
	glm::vec3 frontLightOffset = glm::vec3(0.0f, 0.0f, 4.3f); // Offset frontal
	glm::vec3 backLightOffset = glm::vec3(0.0f, 0.0f, -4.5f); // Offset trasero
	//Offset del la luz del cofre
	glm::vec3 luzCofreOffset = glm::vec3(0.0f, 0.2f, 2.0f);

	glm::vec3 posMAzul = glm::vec3(2.0f, 0.0f, 0.0f);

	//Cargar keyframes desde archivo
	if (!cargarFramesDesdeArchivo())
	{
		printf("Estableciendo Keyframes por defecto...\n");
		KeyFrame[0].movMAzul_x = 0.0f;
		KeyFrame[0].movMAzul_y = 0.0f;
		KeyFrame[0].movMAzul_z = -3.0f;
		KeyFrame[0].giroMAzul = 0;

		KeyFrame[1].movMAzul_x = 0.0f;
		KeyFrame[1].movMAzul_y = 0.0f;
		KeyFrame[1].movMAzul_z = -6.0f;
		KeyFrame[1].giroMAzul = 0;
		
		KeyFrame[2].movMAzul_x = 0.0f;
		KeyFrame[2].movMAzul_y = 0.0f;
		KeyFrame[2].movMAzul_z = -10.0f;
		KeyFrame[2].giroMAzul = 0;


		FrameIndex = 3;
	}
	//Se agregan nuevos frames 

	printf("\nTeclas para uso de Keyframes:\n");
	printf("Barra espaciadora: Reproducir animacion.\n");
	printf("0: Habilitar reproduccion de nuevo.\n");
	printf("L: Guardar frame actual en memoria.\n");
	printf("P: Habilitar guardar nuevo frame.\n");
	printf("G: Guardar TODOS los frames de memoria a 'keyframes.txt'.\n");
	printf("H: Habilitar guardado en archivo.\n");
	printf("1: Mover X+   | 2: Mover X-\n");
	printf("3: Mover Y+   | 4: Mover Y-\n");
	printf("5: Girar +    | 6: Girar -\n");
	printf("7: Mover Z+   | 8: Mover Z-\n"); 
	printf("9: Habilitar próximo movimiento.\n");



	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		inputKeyframes(mainWindow.getsKeys());
		animate();


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//If para el poste de luz
		if (mainWindow.getPosteDeLuzOn() < 0.5f) // Encendido 
		{
			// Usa la nueva función "setter"
			pointLights[1].SetIntensities(lamp_aIntensity_ON, lamp_dIntensity_ON);
		}
		else // Apagado
		{
			//Se usa el set para modificar las intensidades a 0
			pointLights[1].SetIntensities(0.0f, 0.0f);
		}
		//If para el hongo de luz
		if (mainWindow.getHongoOn()) // Encendido
		{
			pointLights[2].SetIntensities(hongo_aIntensity_ON, hongo_dIntensity_ON);
		}
		else // Apagado
		{	//Se usa el set para modificar las intensidades a 0
			pointLights[2].SetIntensities(0.0f, 0.0f);
		}

		shaderList[0].SetPointLights(pointLights, pointLightCount);
		
		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		//Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		
		//Rakan
		//la pierna solo puede rotar hasta 45 grados hacia atrás o si no se nota demasiado el corte en el modelo
		//pierna izquierda x=0, y=8.75346 m, z=103.16 m
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, -1.0f, 170.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CapaRakan.UseTexture();
		CuerpoRakan.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Rakan.RenderModel();
		
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CapaRakan.UseTexture();
		CuerpoRakan.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Rakan_PI.RenderModel();


		//Teemo para volador de Papantla
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 30.f, -170.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 0.0f, 1.0f });
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Teemo.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Teemo_M.RenderModel();

		//Braum
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 10.0f, 180.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 1.0f, 0.0f });
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Braum.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Braum_M.RenderModel();


		//MegaGnar
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 10.0f, 170.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 0.0f, 1.0f });
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gnar.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Gnar_M.RenderModel();


		//Karma
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 0.0f, 1.0f });
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco para que vea la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Karma.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Karma_M.RenderModel();






		/*
		//Instancia del coche 
		model = glm::mat4(1.0);
		//modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, mainWindow.getPosZ()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		estructuraTexture.UseTexture();
		Carro_Est.RenderModel();

		modelaux = model;
		glm::vec3 forwardDirection = glm::vec3(model[2]);
		glm::vec3 backwardDirection = forwardDirection * -1.0f;
		// Faro del coche, que se calcula su posición en cada frame por eso va dentro del while
		model = modelaux;
		glm::vec3 carPos = glm::vec3(model[3]);
		if (mainWindow.estaMoviendoAdelante())
		{
			// Hacia delante:
			spotLights[2] = spotLightFaroDel; // Carga el faro DELANTERO
			// Calcula posición y dirección DELANTERA
			glm::vec3 lightPos = carPos + frontLightOffset;
			spotLights[2].SetFlash(lightPos, forwardDirection);
		}
		else
		{
			// Hacia atrás:
			spotLights[2] = spotLightFaroTra; // Carga el faro TRASERO
			glm::vec3 lightPos = carPos + backLightOffset;
			spotLights[2].SetFlash(lightPos, backwardDirection);
		}

		//Capo
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.007604f, 0.557871f, 1.42633f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(-1.0f, 0.0f, 0.0f));
		//Spotlight del cofre
		glm::vec3 luzCofrePos = glm::vec3(model * glm::vec4(luzCofreOffset, 1.0f));
		spotLights[1].SetPos(luzCofrePos);
		// Se usa forwardDirection para apuntar el spot hacia delante del coche.
		glm::vec3 luzCofreDir = glm::normalize(glm::vec3(model[2]));
		spotLights[1].SetFlash(luzCofrePos, luzCofreDir);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		capoTexture.UseTexture();
		C_Capo.RenderModel();


		//llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.71273f, -0.903329f, 2.8152f));
		model = glm::rotate(model, glm::radians(mainWindow.getWheelRotation()), glm::vec3(1.0f, 0.0f, 0.0f)); //rotacion común
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(-1.0f, 0.0f, 0.0f));
		
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		CLlantaDD.RenderModel();

		// llanta delantera izquierda  x=1.80016 y= -0.879197  z= 2.82455
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.80016f, -0.879197f, 2.82455f));
		model = glm::rotate(model, glm::radians(mainWindow.getWheelRotation()), glm::vec3(1.0f, 0.0f, 0.0f)); //rotacion común
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(-1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		CLlantaID.RenderModel();

		//llanta trasera derecha   
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.71273f, -0.903329f, -2.89581f));
		model = glm::rotate(model, glm::radians(mainWindow.getWheelRotation()), glm::vec3(1.0f, 0.0f, 0.0f)); //rotacion común
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(-1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		CLlantaDT.RenderModel();

		//llanta trasera izquierda	x=1.80016	y= -0.879197 z= -2.88598 m
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.80016f, -0.879197f, -2.82455f));
		model = glm::rotate(model, glm::radians(mainWindow.getWheelRotation()), glm::vec3(1.0f, 0.0f, 0.0f)); //rotacion común
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(-1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		CLlantaIT.RenderModel();
		*/
		
		//Mascota Azul con keyframes
		model = glm::mat4(1.0);
		posMAzul = glm::vec3(posXMAzul + movMAzul_x, posYMAzul + movMAzul_y, posZMAzul + movMAzul_z);
		model = glm::translate(model, posMAzul);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, giroMAzul * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MascotaAzul_M.RenderModel();

		// BASES 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.5f, 120.0f));
		model = glm::scale(model, glm::vec3(140.0f, 3.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.5f, 220.0f));
		model = glm::scale(model, glm::vec3(140.0f, 3.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh(); // <-- Dibuja la malla 6 (Base)

		// Prismas cuadrangulares

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 15.5f, 120.0f));
		model = glm::scale(model, glm::vec3(100.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh(); // <-- Dibuja la malla 5 (Cubo)

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 15.5f, 220.0f));
		model = glm::scale(model, glm::vec3(100.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh(); // <-- Dibuja la malla 5 (Cubo)




		//Pared
		/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 10.0));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.33f,1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		*/
		//Hongo Teemo
		model = glm::mat4(1.0);
		model = glm::translate(model, { 15.0f, -1.0f, -8.0f });
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Hongo.RenderModel();

		//Poste de Luz
		model = glm::mat4(1.0);
		model = glm::translate(model, { -15.0f, -1.0f, -4.0f });
		model = glm::rotate(model, glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;

		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("FRAME %d GUARDADO\n", FrameIndex - 1);
			printf("movMAzul_x es: %f\n", movMAzul_x);
			printf("movMAzul_y es: %f\n", movMAzul_y);
			printf("movMAzul_z es: %f\n", movMAzul_z);
			printf("giroMAzul: %f\n", giroMAzul);
			printf("Presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			reinicioFrame++;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}

	//Teclas para guardar en archivo los keyframes
	if (keys[GLFW_KEY_G]) // Tecla 'G' para Guardar
	{
		if (guardoArchivo < 1)
		{
			guardarFramesArchivo(); // Llama a la nueva función
			printf("Presiona 'H' para habilitar guardar de nuevo en archivo'\n");
			guardoArchivo++;
			reinicioArchivo = 0;
		}
	}
	if (keys[GLFW_KEY_H]) // Tecla 'H' para Habilitar
	{
		if (reinicioArchivo < 1)
		{
			guardoArchivo = 0;
			reinicioArchivo++;
			printf("Ya puedes guardar de nuevo en archivo presionando la tecla G'\n");
		}
	}



	// Tecla 1: Mover en X+
	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			movMAzul_x += 20.0f; // Mueve 1 unidad en X positivo (derecha)
			printf("\n> movMAzul_x es ahora: %f", movMAzul_x);
			ciclo++; // Activa el seguro
			ciclo2 = 0; // Prepara el reseteo
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 2: Mover en X-
	if (keys[GLFW_KEY_2])
	{
		if (ciclo < 1)
		{
			movMAzul_x -= 20.0f; // Mueve 1 unidad en X negativo (izquierda)
			printf("\n> movMAzul_x es ahora: %f", movMAzul_x);
			ciclo++;
			ciclo2 = 0;
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 3: Mover en Y+
	if (keys[GLFW_KEY_3])
	{
		if (ciclo < 1)
		{
			movMAzul_y += 1.0f; // Mueve hacia arriba en Y
			printf("\n> movMAzul_y es ahora: %f", movMAzul_y);
			ciclo++;
			ciclo2 = 0;
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 4: Mover en Y-
	if (keys[GLFW_KEY_4])
	{
		if (ciclo < 1)
		{
			movMAzul_y -= 1.0f; // Mueve hacia abajo en Y
			printf("\n> movMAzul_y es ahora: %f", movMAzul_y);
			ciclo++;
			ciclo2 = 0;
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 5: Girar en + (sentido anti-horario)
	if (keys[GLFW_KEY_5])
	{
		if (ciclo < 1)
		{
			giroMAzul += 30.0f;
			printf("\n> giroMAzul es ahora: %f", giroMAzul);
			ciclo++;
			ciclo2 = 0;
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 6: Girar en - (sentido horario)
	if (keys[GLFW_KEY_6])
	{
		if (ciclo < 1)
		{
			giroMAzul -= 30.0f;
			printf("\n> giroMAzul es ahora: %f", giroMAzul);
			ciclo++;
			ciclo2 = 0;
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 7: Mover en Z+ (Adelante) (NUEVO)
	if (keys[GLFW_KEY_7])
	{
		if (ciclo < 1)
		{
			movMAzul_z += 10.0f; // Mueve 1 unidad en Z positivo
			printf("\n> movMAzul_z es ahora: %f", movMAzul_z);
			ciclo++; // Activa el seguro
			ciclo2 = 0; // Prepara el reseteo
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 8: Mover en Z- (Atrás) (NUEVO)
	if (keys[GLFW_KEY_8])
	{
		if (ciclo < 1)
		{
			movMAzul_z -= 10.0f; // Mueve 1 unidad en Z negativo
			printf("\n> movMAzul_z es ahora: %f", movMAzul_z);
			ciclo++; // Activa el seguro
			ciclo2 = 0; // Prepara el reseteo
			printf(" (Presiona 9 para habilitar otro movimiento)\n");
		}
	}
	// Tecla 9: Habilitar movimiento 
	if (keys[GLFW_KEY_9])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0; // Resetea el 'seguro' de movimiento
			ciclo2++;
			printf("\nYa puedes hacer otro movimiento (Teclas 1-8)\n");
		}
	}

}