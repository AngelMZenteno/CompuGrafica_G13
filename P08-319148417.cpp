/*
Ej 8: Iluminación 2 
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

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture llantaTexture;
Texture capoTexture;
Texture estructuraTexture;
Texture dadoTexture;
Texture Mushroom;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Model Carro_Est;
Model C_Capo;
Model CLlantaID;
Model CLlantaIT;
Model CLlantaDD;
Model CLlantaDT;
Model Lampara;
Model Hongo;

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

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
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
	llantaTexture = Texture("Textures/llanta-completa.tga");
	llantaTexture.LoadTextureA();
	capoTexture = Texture("Textures/capo.tga");
	capoTexture.LoadTextureA();
	estructuraTexture = Texture("Textures/estructura.tga");
	estructuraTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado-octaedro.jpg");
	dadoTexture.LoadTextureA();
	Mushroom = Texture("Textures/Mushroom.png");
	Mushroom.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	
	Carro_Est = Model();
	C_Capo = Model();
	CLlantaID = Model();
	CLlantaIT = Model();
	CLlantaDD = Model();
	CLlantaDT = Model();
	Lampara = Model();
	Hongo= Model();
	Carro_Est.LoadModel("Models/estructura_coche.obj");
	C_Capo.LoadModel("Models/Capo_coche.obj");
	CLlantaID.LoadModel("Models/llanta_izqDel.obj");
	CLlantaIT.LoadModel("Models/llanta_izqTra.obj");
	CLlantaDD.LoadModel("Models/llanta_derDel.obj");
	CLlantaDT.LoadModel("Models/llanta_derTra.obj");
	Lampara.LoadModel("Models/Lamp.obj");
	Hongo.LoadModel("Models/honguitoTeemo.obj");

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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.01f, 0.1f,
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

		//helicoptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getarticulacion11(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		//luz helicoptero, esto se calcula cada frame por lo que va dentro del while
		glm::vec3 heliLightPos = glm::vec3(model * glm::vec4(lightOffsetLocal, 1.0f));
		glm::vec3 heliDownDir = glm::normalize(glm::vec3(model * glm::vec4(lightDirLocal, 0.0f)));
		spotLights[3].SetPos(heliLightPos);
		spotLights[3].SetFlash(heliLightPos, heliDownDir);

		//Pared
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 10.0));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.33f,1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Hongo Teemo
		model = glm::mat4(1.0);
		model = glm::translate(model, { 15.0f, -1.0f, -8.0f });
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
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
