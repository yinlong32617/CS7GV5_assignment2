// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "../../stb_image.h"
#include "../../maths_funcs.h"
#include "../../camera.h"
#include "../../shader.h"


//#include "../../render_sphere.h"
//#include "../../render_african_head.h"

#include "../../renderCube.h"
#include "../../render_bone.h"

using namespace std;

// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
//#define MESH_NAME "D:/Programming Workplace/ComputerGraphics/assimp-demo/monkeyhead_smooth.dae"
//#define MESH_NAME "basketball/basketball.obj" //0.05
//#define SPHERE_MESH_NAME "sphere.obj" //0.05
//#define MESH_NAME "bunny.obj" //0.5
//#define SPHERE_MESH_NAME "teapot.obj" //0.005
//#define MESH_NAME "african_head/african_head.obj" //0.1

int scr_width = 800;
int scr_height = 620;

Camera camera(vec3(0, 0, 10.0));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

int deltaTime = 0;
int lastFrame = 0;

GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

GLfloat bone1_start_z_deg = 0.0f;
GLfloat bone1_rotate_z = 0.0f;

GLfloat bone2_start_z_deg = 0.0f;
GLfloat bone2_rotate_z = 0.0f;

GLfloat bone3_start_z_deg = 0.0f;
GLfloat bone3_rotate_z = 0.0f;

GLfloat bone4_start_z_deg = 0.0f;
GLfloat bone4_rotate_z = 0.0f;

GLfloat target_x = -2.2;
GLfloat target_y = -2.0;
GLfloat target_z = 0.0;

mat4 model1;
mat4 model2;
mat4 model3;
mat4 model4;
mat4 model5;
mat4 modelTarget;

bool CCDRunning = false;
bool solve = false;

Shader shader;

void coutPos(mat4 model) {
	cout << model.m[12] << "," << model.m[13] << "," << model.m[14] << endl;
}

void coutPos(vec3 vec) {
	cout << vec.v[0] << "," << vec.v[1] << "," << vec.v[2] << endl;
}

void init() {

	shader.CompileShader("../RealTimeAnimation/Assignment2/simpleVertexShader.txt",
				"../RealTimeAnimation/Assignment2/simpleFragmentShader.txt");
}

void display() {
	int currentFrame = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();
		
	mat4 view = identity_mat4();
	//mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 persp_proj = perspective(camera.Zoom, (float)scr_width / (float)scr_height, 0.1f, 100.0f);
	//view = look_at(vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0));
	view = camera.GetViewMatrix();
	shader.setMat4("proj", persp_proj);
	shader.setMat4("view", view);

	// box, target
	modelTarget = identity_mat4();
	modelTarget = scale(modelTarget, vec3(0.2,0.2,0.2));
	modelTarget = rotate_x_deg(modelTarget, rotate_x);
	modelTarget = rotate_y_deg(modelTarget, rotate_y);
	modelTarget = rotate_z_deg(modelTarget, rotate_z);
	//modelTarget = translate(modelTarget, vec3(0.0, -3.5, 0.0));
	modelTarget = translate(modelTarget, vec3(target_x, target_y, target_z));
	shader.setMat4("model", modelTarget);
	renderCube();

	// bone 1, root
	model1 = identity_mat4();
	model1 = rotate_z_deg(model1, bone1_start_z_deg + bone1_rotate_z);
	shader.setMat4("model", model1);
	renderBone();

	// bone 2
	model2 = identity_mat4();
	model2 = rotate_z_deg(model2, bone2_start_z_deg + bone2_rotate_z);
	model2 = translate(model2, vec3(-1.05, 0.0, 0.0));
	model2 = model1 * model2;
	shader.setMat4("model", model2);
	renderBone();

	// bone 3
	model3 = identity_mat4();
	model3 = rotate_z_deg(model3, bone3_start_z_deg + bone3_rotate_z);
	model3 = translate(model3, vec3(-1.05, 0.0, 0.0));
	model3 = model2 * model3;
	shader.setMat4("model", model3);
	renderBone();

	// end point
	model4 = identity_mat4();
	model4 = scale(model4, vec3(0.5, 0.5, 0.5));
	model4 = rotate_z_deg(model4, bone4_start_z_deg + bone4_rotate_z);
	model4 = translate(model4, vec3(-1.05, 0.0, 0.0));
	model4 = model3 * model4;
	shader.setMat4("model", model4);
	//renderBone();

	glutSwapBuffers();
}

vec3 getPos(mat4 model){
	return vec3(model.m[12], model.m[13], model.m[14]);
}


float clamp(float x) {
	return min(1.0, max(-1.0, x));
}

void CCD(int frame) {
	static int i = 0;
	if (++i > 100) {
		i = 0;
		CCDRunning = false;
		return;
	}
	
	vec3 endPos = getPos(model4);
	vec3 tarPos = getPos(modelTarget);
	vec3 curPos;
	vec3 e_c;
	vec3 t_c;
	vec3 r;
	float cos_theta;
	float angle;

	float distance = sqrt(pow((tarPos.v[0] - endPos.v[0]), 2) + pow((tarPos.v[1] - endPos.v[1]), 2));
	float tolerance = 0.1;

	if (distance < tolerance) {
		solve = true;
		CCDRunning = false;
	}
	else
	{
		if (frame == 1) {
			endPos = getPos(model4);
			tarPos = getPos(modelTarget);
			curPos = getPos(model3);
			e_c = normalise(endPos - curPos);
			t_c = normalise(tarPos - curPos);
			r = cross(e_c, t_c);
			cos_theta = clamp(dot(e_c, t_c));
			angle = acos(cos_theta) * ONE_RAD_IN_DEG;

			if (r.v[2] < 0) {
				angle *= -1;
			}
			bone3_rotate_z += angle;
		}
		else if (frame == 2) {
			endPos = getPos(model4);
			tarPos = getPos(modelTarget);
			curPos = getPos(model2);
			e_c = normalise(endPos - curPos);
			t_c = normalise(tarPos - curPos);
			r = cross(e_c, t_c);
			cos_theta = clamp(dot(e_c, t_c));
			angle = acos(cos_theta) * ONE_RAD_IN_DEG;

			if (r.v[2] < 0) {
				angle *= -1;
			}

			bone2_rotate_z += angle;
		}
		else if (frame == 3) {
			endPos = getPos(model4);
			tarPos = getPos(modelTarget);
			curPos = getPos(model1);
			e_c = normalise(endPos - curPos);
			t_c = normalise(tarPos - curPos);
			r = cross(e_c, t_c);
			cos_theta = clamp(dot(e_c, t_c));
			angle = acos(cos_theta) * ONE_RAD_IN_DEG;

			if (r.v[2] < 0) {
				angle *= -1;
			}
			bone1_rotate_z += angle;
		}
	}
}

void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	// Rotate the model slowly around the y axis at 20 degrees per second
	rotate_y += 20.0f * delta;
	//rotate_y = fmodf(rotate_y, 360.0f);

	rotate_x += 20.0f * delta;
	//rotate_x = fmodf(rotate_x, 360.0f);

	rotate_z += 20.0f * delta;
	//rotate_z = fmodf(rotate_z, 360.0f);

	bone4_rotate_z += 20.0f * delta;
	bone4_rotate_z = fmodf(bone4_rotate_z, 45.0f);


	if (CCDRunning) {
		static int frame = 1;
		CCD(frame);
		frame++;
		if (frame > 3) frame = 1;
	}

	// Draw the next frame
	glutPostRedisplay();
}

void keypress(unsigned char key, int x, int y) {
	if (deltaTime < 1) deltaTime = 1;
	float delta = deltaTime/1000.0;

	if (key == 27) {
		exit(0);
	}
	else if (key == 'q') {
		bone3_rotate_z += 900.0f * delta;
	}
	else if (key == 'w') {
		bone3_rotate_z -= 900.0f * delta;
	}
	else if (key == 'a') {
		bone2_rotate_z += 900.0f * delta;
	}
	else if (key == 's') {
		bone2_rotate_z -= 900.0f * delta;
	}
	else if (key == 'z') {
		bone1_rotate_z += 900.0f * delta;
	}
	else if (key == 'x') {
		bone1_rotate_z -= 900.0f * delta;
	}
	else if (key == 'c') {
		CCDRunning = true;

		//static int frame = 1;
		//CCD(frame);
		//frame++;
		//if (frame > 3) frame = 1;

	}
}

void PassiveMouse_Callback(int x, int y) {
	
}

void SpecialKeys(int key, int x, int y) {

	GLfloat stepSize = 0.025f;

	switch (key) {
		case GLUT_KEY_UP:
			target_y += stepSize;
			break;
		case GLUT_KEY_DOWN:
			target_y -= stepSize;
			break;
		case GLUT_KEY_LEFT:
			target_x -= stepSize;
			break;
		case GLUT_KEY_RIGHT:
			target_x += stepSize;
			break;
		default:
			break;
	}
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(scr_width, scr_height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(SpecialKeys);
	glutPassiveMotionFunc(PassiveMouse_Callback);
		
	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}


