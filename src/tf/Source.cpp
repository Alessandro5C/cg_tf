#include <stdlib.h> // aleatorios
#include <time.h> // aleatorios

#include "files.h"
#include "model.h"
#include "cam.h"

#include <shader.h>
 
// settings
const i32 SCR_WIDTH = 1280;
const i32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

// camera 
Cam* cam;

// timing
f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;
bool wireframe = false;

// lighting

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		deltaTime *= 10;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		cam->processKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		cam->processKeyboard(DOWN, deltaTime);
	}
}
void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	}
	else {
		cam->stopPov();
	}	
}
void scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

f32 matrix[101][101];

/***********************************************/
f32 noise(f32 precision = 100.0f) {
	return (rand() % (int)precision + 1.0f) / precision;
}
i32 rescale(f32 num, f32 min, f32 max) {
	return (int)((max - min) * (num) / ((float)(1.0f)) + min);
}
void bilinear_interpolation(u32& n, u32 offset) {
	int xNearest, zNearest;
	float nearest, farest;
	//interpolamos valores en horizontal
	for (int x = 0; x < n; ++x) {
		if (x % offset == 0) {
			for (int z = 0; z < n - 1; ++z) {
				if (z % offset == 0) {
					zNearest = z;
					nearest = matrix[x][z];
					farest = matrix[x][z + offset];
					continue;
				}
				float point = z - zNearest;
				matrix[x][z] =
					(nearest * (offset - point) / (float)offset) +
					(farest * (point) / (float)offset);
			}
		}
	}
	//interpolamos valores en vertical
	for (int z = 0; z < n; ++z) {
		if (true) {
			for (int x = 0; x < n - 1; ++x) {
				if (x % offset == 0) {
					xNearest = x;
					nearest = matrix[x][z];
					farest = matrix[x + offset][z];
					continue;
				}
				float point = x - xNearest;
				matrix[x][z] =
					(nearest * (offset - point) / (float)offset) +
					(farest * (point) / (float)offset);
			}
		}
	}
}
void generate_random(u32& n, i32 min, i32 max, u32 offset) {
	//Solo generamos n�meros randoms, cada cierto intervalo
	//Aquellos que no se encuentren en el intervalo, quedan vacios
	float precision = 100.0f;
	for (int x = 0; x < n; ++x)
		for (int z = 0; z < n; ++z) {
			matrix[x][z] = 0.0f;
			if (x % offset == 0 && z % offset == 0)
				matrix[x][z] = noise();
		}

	bilinear_interpolation(n, offset);

	//Como este no es un espacio continuo, hay que truncar los valores
	for (int x = 0; x < n; ++x)
		for (int z = 0; z < n; ++z)
			matrix[x][z] = rescale(matrix[x][z], min, max);

	/*for (u32 x = 0; x < n; ++x) {
		for (u32 z = 0; z < n; ++z) {
			//std::cout << matrix[x][z] << "\t";
		}
		//std::cout << "\n";
	}
	for (u32 x = 0; x < n; ++x)
		for (u32 z = 0; z < n; ++z)
			matrix[x][z] = (int)(matrix[x][z]);
			*/
}
void load_positions(u32& n, std::vector<glm::vec3>& positions) {
	for (u32 x = 0; x < n; ++x) {
		for (u32 z = 0; z < n; ++z) {
			positions[x * n + z] = glm::vec3(x, matrix[x][z], z);
		}
	}
}
/***********************************************/


i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Generacion procedural de terrenos");
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glEnable(GL_DEPTH_TEST);

	cam = new Cam();

	srand(time(NULL));
	u32 n = 101;
	std::vector<glm::vec3> positions(n * n);
	generate_random(n, -5, 15, 20);
	load_positions(n, positions);

	glm::vec3 lightPos = glm::vec3(1.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

	Files* files = new Files();
	/*Files* files = new Files("bin", "resources/textures", "resources/objects");
	Files* files = new Files("src/tf",
		"/home/dwels/Repositorio/CG_TF/resources/textures",
		"/home/dwels/Repositorio/CG_TF/resources/objects");
*/
	Shader* mapaShader = new Shader("mapa.vert", "mapa.frag");
	Shader* modelShader = new Shader("model.vert", "model.frag");

	Model* agua = new Model(files, "piedra/piedra.obj");
	Model* tierra = new Model(files, "slime/slime.obj");
	Model* arena = new Model(files, "dirt/dirt.obj");


	Model* vaca = new Model(files, "cow/cow.obj");
	Model* slime = new Model(files, "robot/robot.obj");
	Model* sol = new Model(files, "planet/planet.obj");


	// cargar la matrices de posiciones
	std::vector<glm::mat4> modelMatricesagua;
	std::vector<glm::mat4> modelMatricestierra;
	std::vector<glm::mat4> modelMatricesarena;

	glm::mat4 modelaux;
	for (ui32 i = 0; i < n; i++) {
		for (ui32 j = 0; j < n; j++) {
			modelaux = glm::mat4(1.0f);
			modelaux = glm::translate(modelaux, 2.0f*positions[i*n+j]);

			if (positions[i * n + j].y < 0.0f) {
				modelMatricesagua.push_back(modelaux);
			}
			else if (positions[i * n + j].y < 5.0f) {
				modelMatricestierra.push_back(modelaux);
			}
			else {
				modelMatricesarena.push_back(modelaux);
			}
		}
	}
	
	cout << "total size agua " << modelMatricesagua.size() << endl;
	cout << "total size tierra " << modelMatricestierra.size() << endl;
	cout << "total size arena " << modelMatricesarena.size() << endl;


	glm::mat4* matricesagua = new glm::mat4[modelMatricesagua.size()];
	glm::mat4* matricestierra = new glm::mat4[modelMatricestierra.size()];
	glm::mat4* matricesarena = new glm::mat4[modelMatricesarena.size()];

	for (ui32 i = 0; i < modelMatricesagua.size(); i++) {
		matricesagua[i] = modelMatricesagua[i];
	}
	for (ui32 i = 0; i < modelMatricestierra.size(); i++) {
		matricestierra[i] = modelMatricestierra[i];
	}
	for (ui32 i = 0; i < modelMatricesarena.size(); i++) {
		matricesarena[i] = modelMatricesarena[i];
	}
	
	for (unsigned int i = 0; i < agua->meshes.size(); i++)
	{
		unsigned int VAO = agua->meshes[i].Vao;
		glBindVertexArray(VAO);

		// configure instanced array agua
		unsigned int bufferagua;
		glGenBuffers(1, &bufferagua);
		glBindBuffer(GL_ARRAY_BUFFER, bufferagua);
		glBufferData(GL_ARRAY_BUFFER, modelMatricesagua.size() * sizeof(glm::mat4), &matricesagua[0], GL_STATIC_DRAW);


		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	for (unsigned int i = 0; i < tierra->meshes.size(); i++)
	{
		unsigned int VAO = tierra->meshes[i].Vao;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)

		// configure instanced array tierra
		unsigned int buffertierra;
		glGenBuffers(1, &buffertierra);
		glBindBuffer(GL_ARRAY_BUFFER, buffertierra);
		glBufferData(GL_ARRAY_BUFFER, modelMatricestierra.size() * sizeof(glm::mat4), &matricestierra[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	for (unsigned int i = 0; i < arena->meshes.size(); i++)
	{
		unsigned int VAO = arena->meshes[i].Vao;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)

		// configure instanced array arena
		unsigned int bufferarena;
		glGenBuffers(1, &bufferarena);
		glBindBuffer(GL_ARRAY_BUFFER, bufferarena);
		glBufferData(GL_ARRAY_BUFFER, modelMatricesarena.size() * sizeof(glm::mat4), &matricesarena[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	
	while (!glfwWindowShouldClose(window)) {
		// per-frame logic
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// render
		glClearColor(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);


		// configurando transformation matrices
		glm::mat4 projection = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		glm::vec3 viewPos = cam->pos;
		glm::mat4 view = cam->getViewM4();

		mapaShader->useProgram();
		mapaShader->setVec3("pointLight.position", { 0.0, 0.0, 0.0 });
		mapaShader->setVec3("pointLight.ambient", 0.1f, 0.1f, 0.1f);
		mapaShader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
		mapaShader->setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		mapaShader->setF32("pointLight.constant", 1.0f); // attenuation (distance = 50)
		mapaShader->setF32("pointLight.linear", 0.09f);
		mapaShader->setF32("pointLight.quadratic", 0.032f);


		mapaShader->setMat4("proj", projection);
		mapaShader->setMat4("view", view);
		mapaShader->setVec3("viewPos", cam->pos);

		modelShader->useProgram();
		modelShader->setMat4("proj", projection);
		modelShader->setMat4("view", view);
		modelShader->setVec3("lightPos", lightPos);
		modelShader->setVec3("lightColor", lightColor);
		modelShader->setVec3("viewPos", cam->pos);

		// dibujar la vaca
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(n, matrix[n / 2][n / 2] + 5.0f, n));
		model = glm::scale(model, {0.5f, 0.5f, 0.5f});
		modelShader->setMat4("model", model);
		vaca->Draw(modelShader);
		
		
		// dibujar el slime
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		modelShader->setMat4("model", model);
		slime->Draw(modelShader);
		
		// dibujar el sol
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(n, (n * 2.0f) / 3.0f, n));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		modelShader->setMat4("model", model);
		sol->Draw(modelShader);

		// mapa
		mapaShader->useProgram();
		mapaShader->setF32("material.shininess", 32.0f);
		mapaShader->setF32("material.specular", 0.5f);
		mapaShader->setI32("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);

		// dibujar agua
		glBindTexture(GL_TEXTURE_2D, agua->textures_loaded[0].id);
		for (unsigned int i = 0; i < agua->meshes.size(); i++){
			glBindVertexArray(agua->meshes[i].Vao);
			glDrawElementsInstanced(GL_TRIANGLES, agua->meshes[i].indices.size(),
				GL_UNSIGNED_INT, 0, modelMatricesagua.size());
			glBindVertexArray(0);
		}
		// dibujar tierra
		
		glBindTexture(GL_TEXTURE_2D, tierra->textures_loaded[0].id);
		for (unsigned int i = 0; i < tierra->meshes.size(); i++) {
			glBindVertexArray(tierra->meshes[i].Vao);
			glDrawElementsInstanced(GL_TRIANGLES, tierra->meshes[i].indices.size(),
				GL_UNSIGNED_INT, 0, modelMatricestierra.size());
			glBindVertexArray(0);
		}

		// dibujar arena
		glBindTexture(GL_TEXTURE_2D, arena->textures_loaded[0].id);
		for (unsigned int i = 0; i < arena->meshes.size(); i++) {
			glBindVertexArray(arena->meshes[i].Vao);
			glDrawElementsInstanced(GL_TRIANGLES, arena->meshes[i].indices.size(),
				GL_UNSIGNED_INT, 0, modelMatricesarena.size());
			glBindVertexArray(0);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de-allocate all resources once they've outlived their purpose
	delete cam;
	delete mapaShader;
	delete agua;
	delete tierra;
	delete arena;

	delete vaca;
	delete slime;
	delete modelShader;

	return 0;
}
