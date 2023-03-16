#pragma once

#include <GL/glew.h>
#include "mesh_loading.h"
								//Ҫ�޸�4��
unsigned int bone_VAO = 0;	//1.Ҫ�ŵ����溯��������������
ModelData sphere_mesh_data;		//2.Ҫ�ŵ����溯��������������

void renderBone() {           //3.Ҫ�����
	unsigned int VBO_VP, VBO_VN, VBO_VT;
	if (bone_VAO == 0) {
		//���ű���0.05����ǰĿ¼�� Debug                              //4.Ҫ�����
		//sphere_mesh_data = load_mesh("../resources/objects/basketball/basketball.obj");
		sphere_mesh_data = load_mesh("../resources/objects/bone.dae");

		glGenVertexArrays(1, &bone_VAO);
		glGenBuffers(1, &VBO_VP);
		glGenBuffers(1, &VBO_VN);
		glGenBuffers(1, &VBO_VT);

		glBindVertexArray(bone_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_VP);
		glBufferData(GL_ARRAY_BUFFER, sphere_mesh_data.mPointCount * sizeof(vec3), &sphere_mesh_data.mVertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VN);
		glBufferData(GL_ARRAY_BUFFER, sphere_mesh_data.mPointCount * sizeof(vec3), &sphere_mesh_data.mNormals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VT);
		glBufferData(GL_ARRAY_BUFFER, sphere_mesh_data.mPointCount * sizeof(vec2), &sphere_mesh_data.mTextureCoords[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VP);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VN);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_VT);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	glBindVertexArray(bone_VAO);
	glDrawArrays(GL_TRIANGLES, 0, sphere_mesh_data.mPointCount);
	glBindVertexArray(0);
}
