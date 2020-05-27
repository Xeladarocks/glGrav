#pragma once
#include <iostream>
#include <vector>
#include <time.h>
#include <iomanip>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include "perlin.hpp"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);

#define PI 3.14159
#define SCALE 200.0f

#include "Shader.h"
#include "Camera.hpp"
#include "util.hpp"
#include "gl_util.hpp"
#include "Body.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

/** Buffer Arrays **/
// VAO
std::vector<float> vertices;
std::vector<unsigned int> indices;
std::vector<float> colors;

// VAO2
std::vector< std::vector<glm::vec3> > lineVertices;
std::vector< std::vector<glm::vec3> > lineColors;
/** ------------- **/

struct cnfg {
	int iterations = 1; // per frame

	bool show_trails = true;
	float trail_length = 1; // s
	float trail_time_interval = 0.01; // s   (-1 for no gap)

	float destroy_distance = 500*SCALE;

} config;

// camera
Camera camera(glm::vec3(0.0f, 10.f, 10.0f)*SCALE);
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// bodies
std::vector<Body> bodies;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// perlin
siv::PerlinNoise perlin(time(0));
const int frequency = 64;
const int octaves = 1;

float getNoise(const int &x, const int &z) {
	return perlin.accumulatedOctaveNoise2D_0_1(x / (float)frequency, z / (float)frequency, octaves);
}
void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 c) {
	push_vec3(vertices, v1);
	push_vec3(vertices, v2);
	push_vec3(vertices, v3);
	push_vec3(vertices, v4);
	if (indices.size() != 0) {
		unsigned int idx = indices[indices.size() - 1];
		push_ivec3(indices, glm::vec3(idx + 1, idx + 2, idx + 3));
		push_ivec3(indices, glm::vec3(idx + 1, idx + 3, idx + 4));
	}
	else indices = { 0, 1, 2,   0, 2, 3 };
	push_vec3(colors, c, 4);
}
void addCube(glm::vec3 v1, glm::vec3 v2, glm::vec3 c) {
	addQuad(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v1.x, v1.y, v2.z), glm::vec3(v2.x, v1.y, v2.z), glm::vec3(v2.x, v1.y, v1.z), c); // bottom
	addQuad(glm::vec3(v1.x, v2.y, v1.z), glm::vec3(v1.x, v2.y, v2.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v2.x, v2.y, v1.z), c); // top
	addQuad(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v1.x, v2.y, v1.z), glm::vec3(v2.x, v2.y, v1.z), glm::vec3(v2.x, v1.y, v1.z), c); // left
	addQuad(glm::vec3(v1.x, v1.y, v2.z), glm::vec3(v1.x, v2.y, v2.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v2.x, v1.y, v2.z), c); // right
	addQuad(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v1.x, v1.y, v2.z), glm::vec3(v1.x, v2.y, v2.z), glm::vec3(v1.x, v2.y, v1.z), c); // across
	addQuad(glm::vec3(v2.x, v1.y, v1.z), glm::vec3(v2.x, v1.y, v2.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v2.x, v2.y, v1.z), c); // behind
}
void initWorld() {

	// Orbitals
	//bodies.push_back(Body(glm::vec3(2, -2, 0), glm::vec3(0, 0, 6), 0.1, 0.05));
	bodies.push_back(Body(glm::vec3(-2, -2, 0), glm::vec3(0, 0, -6), 0.1, 0.05));
	//bodies.push_back(Body(glm::vec3(0, 2, 2), glm::vec3(6, 0, 0), 0.1, 0.05));
	bodies.push_back(Body(glm::vec3(0, 2, -2), glm::vec3(-6, 0, 0), 0.1, 0.05));

	// Sun
	bodies.push_back(Body(glm::vec3(0), glm::vec3(0, 0, 0), 10000, glm::vec3(1, 1, 0)));

	for (int i = 0; i < bodies.size(); i++) {
		lineVertices.push_back(std::vector<glm::vec3>());
		lineColors.push_back(std::vector<glm::vec3>());
	}
}
float last_trail_time = 0.f;
void tickWorld(float totalTime, float dt) {
	const float time_step = 0.0001;
	bool should_draw_trail = false;
	
	for (int b = 0; b < bodies.size(); b++) {
		Body& body = bodies[b];

		if (config.show_trails) {
			if (last_trail_time >= config.trail_time_interval) {
				last_trail_time = 0;
				should_draw_trail = true;

				if (lineVertices[b].size() != 0) {
					lineVertices[b].push_back(lineVertices[b][lineVertices[b].size()-1]);
					lineColors[b].emplace_back(glm::vec3(0.7));
				} else { // first vertex
					lineVertices[b].push_back(body.pos * SCALE);
					lineColors[b].push_back(glm::vec3(0.7));
				}
			}
			else last_trail_time += dt;
		}
		
		for (int s = 0; s < config.iterations; s++) {
			for (int o = 0; o < bodies.size(); o++) {
				if (b == o) continue;
				body.tick(bodies[o], time_step);
			}
		}
		if(glm::distance(camera.Position, body.pos) >= config.destroy_distance) bodies.erase(bodies.begin() + b);

		addCube((body.pos+glm::vec3(-body.radius))*SCALE, (body.pos+glm::vec3(body.radius))*SCALE, body.color);

		if (config.show_trails && should_draw_trail) {
			should_draw_trail = false;
			lineVertices[b].push_back(body.pos * SCALE);
			lineColors[b].push_back(glm::vec3(0.7));
			if (totalTime >= config.trail_length) {
				lineVertices[b].erase(lineVertices[b].begin());
				lineVertices[b].erase(lineVertices[b].begin());
				lineColors[b].erase(lineColors[b].begin());
				lineColors[b].erase(lineColors[b].begin());
			}
		}
	}
}

int main() {
	GLFWwindow* window;
	GLFWmonitor* monitor;
	if(glSetup(WIDTH, HEIGHT, window, monitor) != 0) return -1;

	Shader basicShader("Basic.vs.glsl", "Basic.fs.glsl");

	initWorld();
	tickWorld(0, 0);

	unsigned int VBO, EBO, CBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/* ----------------------------------------------- */

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STREAM_DRAW);
	// position attribute
	glVertexAttribPointer(
		0,          // index
		3,          // size
		GL_FLOAT,   // type
		GL_FALSE,   // normalized
		0,          // stride
		(void*) 0   // offset
	);
	glEnableVertexAttribArray(0);

	/* ----------------------------------------------- */

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STREAM_DRAW);

	/* ----------------------------------------------- */

	glGenBuffers(1, &CBO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STREAM_DRAW);
	// color attribute
	glVertexAttribPointer(
		1, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		0, 
		(void*) 0
	);
	glEnableVertexAttribArray(1);

	/* ----------------------------------------------- */



	/* -------------         VAO 2        ------------ */
	unsigned int VBO2, CBO2, VAO2;
	if (config.show_trails) {
		std::vector<glm::vec3> sLineVertices;
		std::vector<glm::vec3> sLineColors;
		for (int i = 0; i < lineVertices.size(); i++) {
			for (int j = 0; j < lineVertices[i].size(); j++) {
				sLineVertices.push_back(lineVertices[i][j]);
				sLineColors.push_back(lineColors[i][j]);
				//push_vec3(sLineVertices, lineVertices[i][j]);
				//push_vec3(sLineColors, lineVertices[i][j]);
			}
		}


		glGenVertexArrays(1, &VAO2);
		glBindVertexArray(VAO2);

		glGenBuffers(1, &VBO2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sLineVertices.size() * sizeof(glm::vec3), &sLineVertices[0], GL_STREAM_DRAW); // update vertices
		// position attribute
		glVertexAttribPointer(
			0,          // index
			3,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // normalized
			0,          // stride
			(void*)0   // offset
		);
		glEnableVertexAttribArray(0);


		glGenBuffers(1, &CBO2);
		glBindBuffer(GL_ARRAY_BUFFER, CBO2);
		glBufferData(GL_ARRAY_BUFFER, sLineColors.size() * sizeof(glm::vec3), glm::value_ptr(sLineColors[0]), GL_STREAM_DRAW); // update vertices
		// position attribute
		glVertexAttribPointer(
			1,          // index
			3,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // normalized
			0,          // stride
			(void*)0   // offset
		);
		glEnableVertexAttribArray(1);
	}
	/* ----------------------------------------------- */


	glm::mat4 view(1.0f);
	glm::mat4 model(1.0f);
	glm::mat4 projection(1.0f);

	while (!glfwWindowShouldClose(window)) {

		camera.updateKeyboard(deltaTime);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		vertices.clear();
		indices.clear();
		colors.clear();
		tickWorld(currentFrame, deltaTime);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STREAM_DRAW); // update vertices
		glBindBuffer(GL_ARRAY_BUFFER, CBO);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STREAM_DRAW); // update colors
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STREAM_DRAW); // update indices

		/** Render **/
		//glClearColor(0.2039f, 0.2039f, 0.2078f, 1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basicShader.use();
		// camera/view transformation
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3());
		/** ------- **/


		/* Uniforms */
		basicShader.setMat4("view", view);
		basicShader.setMat4("model", model);
		basicShader.setMat4("projection", projection);
		/* -------- */


		/** Draw Triangles **/
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // indices
		glPointSize(5);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // with indices
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // vertices only
		/** ---- **/

		/** Draw Lines **/
		if (config.show_trails) {
			glLineWidth(3);

			std::vector<glm::vec3> sLineVertices;
			std::vector<glm::vec3> sLineColors;
			for (int i = 0; i < lineVertices.size(); i++) {
				for (int j = 0; j < lineVertices[i].size(); j++) {
					sLineVertices.push_back(lineVertices[i][j]);
					sLineColors.push_back(lineColors[i][j]);
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, VBO2);
			glBufferData(GL_ARRAY_BUFFER, sLineVertices.size() * sizeof(glm::vec3), glm::value_ptr(sLineVertices[0]), GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, CBO2);
			glBufferData(GL_ARRAY_BUFFER, sLineColors.size() * sizeof(glm::vec3), glm::value_ptr(sLineColors[0]), GL_STREAM_DRAW);

			glBindVertexArray(VAO2);
			glDrawArrays(GL_LINES, 0, sLineVertices.size()); // vertices only
		}
		/** ---- ----- **/



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &CBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	bool toggle = false;
	if (action == GLFW_PRESS) toggle = true;
	else if (action == GLFW_RELEASE) toggle = false;
	else return;

	switch (key) {
		case GLFW_KEY_W: camera.toggles.forward = toggle; break;
		case GLFW_KEY_S: camera.toggles.backward = toggle; break;
		case GLFW_KEY_A: camera.toggles.left = toggle; break;
		case GLFW_KEY_D: camera.toggles.right = toggle; break;
		case GLFW_KEY_SPACE: camera.toggles.up = toggle; break;
		case GLFW_KEY_LEFT_SHIFT: camera.toggles.down = toggle; break;
		case GLFW_KEY_LEFT_CONTROL: camera.toggles.sprint = toggle; break;
		case GLFW_KEY_R: bodies.clear(); initWorld(); break;
	};
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}