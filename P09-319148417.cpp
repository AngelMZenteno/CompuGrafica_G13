/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
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
//#include<assimp/Importer.hpp>

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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float dragonavance = 0.0f;

//puertas
float movPuertaIzq;     // Desplazamiento actual de la reja izquierda
float rotPuertaDer;     // Rotación actual de la reja derecha

float movPuertaMax;     // Cuánto se va a mover max
float rotPuertaMax;     // Cuánto va a rotar max

float movPuertaSpeed;   // Vel deslizamiento
float rotPuertaSpeed;   // Vel de rotación


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture GDiffuse;
Texture GNormal;
Texture PDiffuse;
Texture PNormal;
Texture LLowerNormal;
Texture LLowerDiffuse;
Texture LUpperDiffuse;
Texture LUpperNormal;
Texture FuenteLoL;

Model Kitt_M;
Model Llanta_M;
Model Dragon_M;
Model Tiamat_M;
Model RejaIzq;
Model RejaDer;
Model Arco;
Model Cartel;

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
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
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
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};
	unsigned int letreroIndices[] = {
		0, 3, 1, // Triángulo 1 (abajo-izq, arriba-izq, abajo-der)
		1, 3, 2  // Triángulo 2 (abajo-der, arriba-izq, arriba-der)
	};
	GLfloat pVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.40625f, 0.46094f,     0.0f, 0.0f, 1.0f, // Abajo Izq
		 0.5f, -0.5f, 0.0f,     0.50391f, 0.46094f,     0.0f, 0.0f, 1.0f, // Abajo Der
		 0.5f,  0.5f, 0.0f,     0.50391f, 0.59766f,     0.0f, 0.0f, 1.0f, // Arriba Der
		-0.5f,  0.5f, 0.0f,     0.40625f, 0.59766f,     0.0f, 0.0f, 1.0f  // Arriba Izq
	};

	// --- Mesh para 'R' (índice 8) ---
	GLfloat rVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.50781f, 0.46094f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.61328f, 0.46094f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.61328f, 0.59766f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.50781f, 0.59766f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'O' (índice 9) ---
	GLfloat oVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.12891f, 0.45312f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.25781f, 0.45312f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.25781f, 0.59375f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.12891f, 0.59375f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'Y' (índice 10) ---
	GLfloat yVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.66211f, 0.27539f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.77344f, 0.27539f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.77344f, 0.41797f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.66211f, 0.41797f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'E' (índice 11) ---
	GLfloat eVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.56641f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.65625f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.65625f, 0.96289f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.56641f, 0.96289f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'C' (índice 12) ---
	GLfloat cVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.32227f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.43359f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.43359f, 0.96484f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.32227f, 0.96484f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'T' (índice 13) ---
	GLfloat tVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.69336f, 0.46094f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.80273f, 0.46094f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.80273f, 0.59766f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.69336f, 0.59766f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'G' (índice 14) ---
	GLfloat gVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.75391f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.87305f, 0.81836f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.87305f, 0.96484f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.75391f, 0.96484f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'H' (índice 15) ---
	GLfloat hVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.10156f, 0.63672f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.20898f, 0.63672f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.20898f, 0.77930f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.10156f, 0.77930f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'I' (índice 16) ---
	GLfloat iVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.22266f, 0.63672f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.27344f, 0.63672f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.27344f, 0.77930f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.22266f, 0.77930f,     0.0f, 0.0f, 1.0f
	};

	// --- Mesh para 'Espacio' (índice 17) ---
	GLfloat espVertices[] = {
		-0.5f, -0.5f, 0.0f,     0.87891f, 0.48633f,     0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,     0.95703f, 0.48633f,     0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,     0.95703f, 0.60352f,     0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,     0.87891f, 0.60352f,     0.0f, 0.0f, 1.0f
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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

	Mesh* meshP = new Mesh();
	meshP->CreateMesh(pVertices, letreroIndices, 32, 6);
	meshList.push_back(meshP); // meshList[7]

	Mesh* meshR = new Mesh();
	meshR->CreateMesh(rVertices, letreroIndices, 32, 6);
	meshList.push_back(meshR); // meshList[8]

	Mesh* meshO = new Mesh();
	meshO->CreateMesh(oVertices, letreroIndices, 32, 6);
	meshList.push_back(meshO); // meshList[9]

	Mesh* meshY = new Mesh();
	meshY->CreateMesh(yVertices, letreroIndices, 32, 6);
	meshList.push_back(meshY); // meshList[10]

	Mesh* meshE = new Mesh();
	meshE->CreateMesh(eVertices, letreroIndices, 32, 6);
	meshList.push_back(meshE); // meshList[11]

	Mesh* meshC = new Mesh();
	meshC->CreateMesh(cVertices, letreroIndices, 32, 6);
	meshList.push_back(meshC); // meshList[12]

	Mesh* meshT = new Mesh();
	meshT->CreateMesh(tVertices, letreroIndices, 32, 6);
	meshList.push_back(meshT); // meshList[13]

	Mesh* meshG = new Mesh();
	meshG->CreateMesh(gVertices, letreroIndices, 32, 6);
	meshList.push_back(meshG); // meshList[14]

	Mesh* meshH = new Mesh();
	meshH->CreateMesh(hVertices, letreroIndices, 32, 6);
	meshList.push_back(meshH); // meshList[15]

	Mesh* meshI = new Mesh();
	meshI->CreateMesh(iVertices, letreroIndices, 32, 6);
	meshList.push_back(meshI); // meshList[16]

	Mesh* meshEsp = new Mesh();
	meshEsp->CreateMesh(espVertices, letreroIndices, 32, 6);
	meshList.push_back(meshEsp); // meshList[17]


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	GDiffuse = Texture("Textures/gate_diffuse.jpg");
	GDiffuse.LoadTextureA();
	GNormal = Texture("Textures/gate_normal.jpg");;
	GNormal.LoadTextureA();
	PDiffuse = Texture("Textures/pillar_diffuse.jpg");;
	PDiffuse.LoadTextureA();
	PNormal = Texture("Textures/pillar_normalmap.jpg");;
	PNormal.LoadTextureA();
	LLowerNormal = Texture("Textures/lightlower_normal.jpg");;
	LLowerNormal.LoadTextureA();
	LLowerDiffuse = Texture("Textures/lightlower_diffuse.jpg");;
	LLowerDiffuse.LoadTextureA();
	LUpperDiffuse = Texture("Textures/lightupper_diffuse.jpg");;
	LUpperDiffuse.LoadTextureA();
	LUpperNormal = Texture("Textures/lightupper_normal.jpg");;
	LUpperNormal.LoadTextureA();
	FuenteLoL = Texture("Textures/FuenteLoL.tga");
	FuenteLoL.LoadTextureA();



	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Dragon_M = Model();
	Dragon_M.LoadModel("Models/17174_Tiamat_new.obj");
	RejaDer = Model();
	RejaDer.LoadModel("Models/rejader.obj");	
	RejaIzq = Model();	
	RejaIzq.LoadModel("Models/rejaizq.obj");	
	Cartel = Model();
	Cartel.LoadModel("Models/cartel.obj");
	Arco = Model();
	Arco.LoadModel("Models/arco.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	// --- Inicializar variables de puertas ---
	movPuertaIzq = 0.0f;     // Posición inicial 
	rotPuertaDer = 0.0f;     // Ángulo inicial 

	movPuertaMax = 5.0f;     // Límite: Mover 2.5 unidades por movimiento
	rotPuertaMax = 90.0f;    // Límite: Rotar 90 grados

	movPuertaSpeed = 1.0f;   // 1.0 unidad por segundo
	rotPuertaSpeed = 45.0f;  // 45 grados por segundo



	int letreroMeshSequence[] = {
		7,  // P
		8,  // R
		9,  // O
		10, // Y
		11, // E
		12, // C
		13, // T
		9,  // O  
		17, // ' ' (espacio)
		12, // C 
		14, // G
		11, // E 
		16, // I
		15, // H
		12, // C  
		17  // ' ' (espacio)
	};

	int letreroLength = 16;

	float letreroCharWidth = 1.0f; // Ancho de cada letra en tu mundo
	float letreroTotalWidth = (float)letreroLength * letreroCharWidth;
	float letreroSpeed = 0.2f; 
	float letreroScroll = 0.0f;



	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.03f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f,0.0f,0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


	float tTimer = 0.0f;
	float fInterval = 0.5f; // Intervalo de tiempo para el cambio 
	bool cambioTextura = true;
	float tiempoActual = 0.0f;
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		tiempoActual = deltaTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

	

		//dragonavance
		/*	
		if (avanza)
		{
			if (movCoche > -250.0f)
			{
				movCoche -= movOffset * deltaTime;
				printf("avanza%f \n ",movCoche);
				rotllanta += rotllantaOffset * deltaTime;
			}
			else
			{
				avanza = !avanza;
			}
		}
		else
		{
			if (movCoche < 300.0f)
			{
				movCoche += movOffset * deltaTime;
				printf("avanza%f \n ",movCoche);
				rotllanta -= rotllantaOffset * deltaTime;
			}
			else
			{
				avanza = avanza;
			}
		}
		*/
		
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		
		
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		
		//Reinicializando variables cada ciclo de reloj
		model= glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		

		//Piso
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		


		//dragonavance para moverlo hacia adelante y atrás en X
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f+sin(glm::radians(angulovaria)), 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		/* color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); */
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dragon_M.RenderModel();

		// --- Logica de animacion de puertas ---
		if (mainWindow.getPuertasAbiertas()) // Si deben estar ABIERTAS
		{
			// --- ABRIR PUERTAS ---
			// Puerta Izquierda (Deslizamiento)
			if (movPuertaIzq < movPuertaMax)
			{
				movPuertaIzq += movPuertaSpeed * deltaTime;
				if (movPuertaIzq > movPuertaMax) movPuertaIzq = movPuertaMax;
			}

			// Puerta Derecha (Rotación)
			if (rotPuertaDer < rotPuertaMax)
			{
				rotPuertaDer += rotPuertaSpeed * deltaTime;
				if (rotPuertaDer > rotPuertaMax) rotPuertaDer = rotPuertaMax;
			}
		}
		else // Si deben estar CERRADAS
		{
			// --- CERRAR PUERTAS ---
			// Puerta Izquierda (Deslizamiento)
			if (movPuertaIzq > 0.0f)
			{
				movPuertaIzq -= movPuertaSpeed * deltaTime;
				if (movPuertaIzq < 0.0f) movPuertaIzq = 0.0f;
			}
			// Puerta Derecha (Rotación)
			if (rotPuertaDer > 0.0f)
			{
				rotPuertaDer -= rotPuertaSpeed * deltaTime;
				if (rotPuertaDer < 0.0f) rotPuertaDer = 0.0f;
			}
		}


		/* color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); */

		//arco
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.3, 0.3f, 0.3f)); // Ajusta tamaño
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Arco.RenderModel();

		//cartel
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.3, 0.3f, 0.3f)); // Ajusta tamaño
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cartel.RenderModel();

		//Reja derecha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -10.0f));
		model = glm::rotate(model, glm::radians(rotPuertaDer), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3, 0.3f, 0.3f)); // Ajusta tamaño
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RejaDer.RenderModel();

		//Reja izquierda (Deslizamiento por etapas)
		float x_desliz = 0.0f;
		float z_desliz = 0.0f;
		const float movPuertaAtrasMax = 2.5f; // Define el límite de la primera etapa (moverse atrás)

		if (movPuertaIzq <= movPuertaAtrasMax)
		{
			// --- ETAPA 1: Moverse solo hacia atrás ---
			// El deslizamiento en Z es el progreso actual.
			z_desliz = -movPuertaIzq;
			// Aún no hay deslizamiento en X.
			x_desliz = 0.0f;
		}
		else
		{
			// --- ETAPA 2: Moverse solo hacia la izquierda ---
			// El movimiento en Z ya terminó, así que se queda en el máximo.
			z_desliz = -movPuertaAtrasMax;

			// El movimiento en X es el progreso que "sobra" después de la primera etapa.
			x_desliz = -(movPuertaIzq - movPuertaAtrasMax);
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -10.0f));
		// Aplicar el deslizamiento calculado por etapas
		model = glm::translate(model, glm::vec3(x_desliz, 0.0f, z_desliz));
		model = glm::scale(model, glm::vec3(0.3, 0.3f, 0.3f)); // Escala
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RejaIzq.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		
		/*
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();


		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		*/

		//Cartel con tipografia de LoL
		letreroScroll += letreroSpeed * deltaTime;
		if (letreroScroll > letreroTotalWidth)
		{
			letreroScroll -= letreroTotalWidth;
		}

		FuenteLoL.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// Dibuja cada letra
		for (int i = 0; i < letreroLength; i++)
		{
			//Obtiene el índice del mesh del arreglo
			int meshIndex = letreroMeshSequence[i];

			//Calcula la posición X de esta letra
			float x_base = (float)i * letreroCharWidth;
			float x_final = x_base - letreroScroll;

			if (x_final < -letreroCharWidth)
			{
				x_final += letreroTotalWidth;
			}

			//Configura la Matriz 'model'
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-17.0f+x_final, 23.0f, -8.8f)); // Ajusta Y y Z
			model = glm::scale(model, glm::vec3(letreroCharWidth, 1.0f, 1.0f)); // Ajusta tamaño
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			
			//Renderiza el mesh específico para esa letra
			meshList[meshIndex]->RenderMesh();

		}

		

		/*

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));   //descomenté esto
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		for (int i = 1; i<4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25f;
			toffsetnumerov = 0.0f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();

		 }

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25f;
			toffsetnumerov = -0.33f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);

			
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
		}
 
		tTimer += tiempoActual;
		if (tTimer >= fInterval)
		{
			toffsetnumerocambiau += 0.25f;
			tTimer = 0.0f;

			if (toffsetnumerocambiau > 1.0)
				toffsetnumerocambiau = 0.0;
			cambioTextura = !cambioTextura; // Invierte el booleano para que cambie la textura
		}
		*/
		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		 

		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		*/
		//cambiar automáticamente entre textura número 1 y número 2
		
		/*
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		if (cambioTextura)
		{
			Numero1Texture.UseTexture();
		}
		else
		{
			Numero2Texture.UseTexture();
		}

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		*/

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
