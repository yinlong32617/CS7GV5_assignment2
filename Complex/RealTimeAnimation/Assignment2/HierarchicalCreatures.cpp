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

Camera camera(vec3(0, 0, 12.0));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

int deltaTime = 0;
int lastFrame = 0;

GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

const int numsOfBones = 5;
GLfloat bone_rotate_z[numsOfBones] = { 0.0 };

GLfloat bone_left_rotate_z = 0.0f;
GLfloat bone_right_rotate_z = 0.0f;

GLfloat boneTop1_start_z_deg = 0.0f;
GLfloat boneTop1_rotate_z = 0.0f;

GLfloat boneTop2_start_z_deg = 0.0f;
GLfloat boneTop2_rotate_z = 0.0f;

GLfloat target_x = 0.0;
GLfloat target_y = -3.0;
GLfloat target_z = 0.0;

mat4 model[numsOfBones];
mat4 modelTop1;
mat4 modelTop2;
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
	model[0] = identity_mat4();
	model[0] = rotate_z_deg(model[0], 0 + bone_rotate_z[0]);
	//model1 = translate(model1, vec3(0.0, 0.0, 0.0));
	shader.setMat4("model", model[0]);
	renderBone();

	for (int i = 1; i < numsOfBones; i++)
	{
		model[i] = identity_mat4();
		model[i] = rotate_z_deg(model[i], 0 + bone_rotate_z[i]);
		model[i] = translate(model[i], vec3(-1.05, 0.0, 0.0));
		model[i] = model[i-1] * model[i];
		shader.setMat4("model", model[i]);
		renderBone();

		// bone left
		mat4 model_left = identity_mat4();
		model_left = scale(model_left, vec3(0.5, 0.5, 0.5));
		model_left = rotate_z_deg(model_left, 90 + bone_left_rotate_z);
		model_left = translate(model_left, vec3(-1.05, 0.0, 0.0));
		model_left = model[i-1] * model_left;
		shader.setMat4("model", model_left);
		renderBone();

		// bone right
		mat4 model_right = identity_mat4();
		model_right = scale(model_right, vec3(0.5, 0.5, 0.5));
		model_right = rotate_z_deg(model_right, -90 + bone_right_rotate_z);
		model_right = translate(model_right, vec3(-1.05, 0.0, 0.0));
		model_right = model[i-1] * model_right;
		shader.setMat4("model", model_right);
		renderBone();

	}
	
	// top1
	modelTop1 = identity_mat4();
	modelTop1 = scale(modelTop1, vec3(0.5, 0.5, 0.5));
	modelTop1 = rotate_z_deg(modelTop1, boneTop1_start_z_deg + boneTop1_rotate_z);
	modelTop1 = translate(modelTop1, vec3(-1.05, 0.0, 0.0));
	modelTop1 = model[numsOfBones-1] * modelTop1;
	shader.setMat4("model", modelTop1);
	renderBone();

	// top2
	modelTop2 = identity_mat4();
	modelTop2 = scale(modelTop2, vec3(0.5, 0.5, 0.5));
	modelTop2 = rotate_z_deg(modelTop2, boneTop2_start_z_deg + boneTop2_rotate_z);
	modelTop2 = translate(modelTop2, vec3(-1.05, 0.0, 0.0));
	modelTop2 = model[numsOfBones-1] * modelTop2;
	
	shader.setMat4("model", modelTop2);
	renderBone();

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
	if (++i > numsOfBones * 30) {
		i = 0;
		CCDRunning = false;
		return;
	}
	
	vec3 endPos = getPos(modelTop1);
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
		
		int frameIndex = numsOfBones - frame - 1;

		for (int i = numsOfBones - 1; i >= 0; i--)
		{
			if (frameIndex == i) {
				endPos = getPos(modelTop1);
				tarPos = getPos(modelTarget);
				curPos = getPos(model[i]);
				e_c = normalise(endPos - curPos);
				t_c = normalise(tarPos - curPos);
				r = cross(e_c, t_c);
				cos_theta = clamp(dot(e_c, t_c));
				angle = acos(cos_theta) * ONE_RAD_IN_DEG;

				if (r.v[2] < 0) {
					angle *= -1;
				}
				bone_rotate_z[i] += angle;
			}
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

	// animte the target
	rotate_y += 20.0f * delta;
	rotate_x += 20.0f * delta;
	rotate_z += 20.0f * delta;

	// animte the bone
	boneTop1_rotate_z += 20.0f * delta;
	boneTop1_rotate_z = fmodf(boneTop1_rotate_z, 45.0f);
	boneTop2_rotate_z -= 20.0f * delta;
	boneTop2_rotate_z = fmodf(boneTop2_rotate_z, -45.0f);

	// animte the bone
	bone_left_rotate_z += 70.0f * delta;
	bone_left_rotate_z = fmodf(bone_left_rotate_z, 90.0f);
	bone_right_rotate_z -= 70.0f * delta;
	bone_right_rotate_z = fmodf(bone_right_rotate_z, -90.0f);

	// CCD and animation
	if (CCDRunning) {
		static int frame = 0;
		CCD(frame);
		frame++;
		if (frame > numsOfBones) frame = 0;
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
	else if (key == 'e') {
		bone_rotate_z[4] += 900.0f * delta;
	}
	else if (key == 'r') {
		bone_rotate_z[4] -= 900.0f * delta;
	}
	else if (key == 'd') {
		bone_rotate_z[3] += 900.0f * delta;
	}
	else if (key == 'f') {
		bone_rotate_z[3] -= 900.0f * delta;
	}
	else if (key == 'q') {
		bone_rotate_z[2] += 900.0f * delta;
	}
	else if (key == 'w') {
		bone_rotate_z[2] -= 900.0f * delta;
	}
	else if (key == 'a') {
		bone_rotate_z[1] += 900.0f * delta;
	}
	else if (key == 's') {
		bone_rotate_z[1] -= 900.0f * delta;
	}
	else if (key == 'z') {
		bone_rotate_z[0] += 900.0f * delta;
	}
	else if (key == 'x') {
		bone_rotate_z[0] -= 900.0f * delta;
	}
	else if (key == 'c') {
		CCDRunning = true;
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


