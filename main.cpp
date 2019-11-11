/*********************************************************
FILE : main.cpp (csci3260 2019-2020 Assignment 2)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155110447
Student Name: Yu Chi To
*********************************************************/

#include "Dependencies/glew/glew.h"
#include "Dependencies/freeglut/freeglut.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "Dependencies/glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Dependencies/stb_image/stb_image.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace std;

GLint programID;

//const init
constexpr float STEP = 0.1f;
constexpr float CAT_AUTO_STEP = 0.005f;
constexpr float CAT_AUTO_ROTA = 0.15f;

int current_x, current_y;
float cam_x, cam_y, cam_z;
float cam_yaw = -90.0f, cam_pitch = 13.0f;

float light_pos[4][2] = { {+5.0f, +5.0f}, {+5.0f, -5.0f}, {-5.0f, -5.0f}, {-5.0f, +5.0f} };
float current_light_orient = 0.0f;

//ofstream //log_str;

int rotaion_bool = 1;

int main_cat_tex_ind = 1;
float main_cat_orient = glm::radians(0.0f);
float main_cat_pos[3] = { +0.0f, -3.5f, +0.0f };

float lightcoefficient = 0.6f;

float moved_disp[2] = {0.0f};
int movement_type[2] = { 0 };
//other cat position init
float cat1pos[2] = { +5.0f, 0.0f };
float cat2pos[2] = { -5.0f, 0.0f };
float cat1orient = 0.0f;
float cat2orient = 0.0f;

//a series utilities for setting shader parameters
void setMat4(const std::string& name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}
void setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void mouse_callback(int button, int state, int x, int y)
{
	//TODO: Use mouse to do interactive events and animation
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		current_x = x;
		current_y = y;
		//log_str << "x: " << x << ", y:" << y << endl;
	}

}

void motion_callback(int x, int y)
{
	//TODO: Use mouse to do interactive events and animation
	cam_yaw += 0.007f * (current_x - x);
	cam_pitch += 0.007f * (y - current_y);
	cam_pitch = glm::clamp(cam_pitch, -89.0f, 89.0f);
	//log_str << "cam_yaw: " << cam_yaw << ", cam_pitch: " << cam_pitch << endl;
}

void keyboard_callback(unsigned char key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	if (key == '1')
	{
		main_cat_tex_ind = 1;
	}
	if (key == '2')
	{
		main_cat_tex_ind = 2;
	}
	if (key == 's')
	{
		lightcoefficient -=  (lightcoefficient > 0.0) ? 0.1f : 0.0f;
		//log_str << "light coefficient: " << lightcoefficient << endl;
	}
	if (key == 'w')
	{
		lightcoefficient += 0.1f;
		//log_str << "light coefficient: " << lightcoefficient << endl;
	}
	if (key == 'p')
	{
		rotaion_bool = (rotaion_bool == 0) ? 1 : 0;
	}

}

void special_callback(int key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	if (key == GLUT_KEY_UP)
	{
		main_cat_pos[0] += STEP * sin(main_cat_orient);
		main_cat_pos[2] += STEP * cos(main_cat_orient);
		//log_str << "cat pos: " << main_cat_pos[0] << ", " << main_cat_pos[1] << ", " << main_cat_pos[2] << endl;
	}
	if (key == GLUT_KEY_DOWN)
	{
		main_cat_pos[0] -= STEP * sin(main_cat_orient);
		main_cat_pos[2] -= STEP * cos(main_cat_orient);
		//log_str << "cat pos: " << main_cat_pos[0] << ", " << main_cat_pos[1] << ", " << main_cat_pos[2] << endl;
	}
	int rotation_axis = +1;
	if (key == GLUT_KEY_LEFT)
	{
		float rot = glm::radians(3.0f) * rotation_axis;
		main_cat_orient += rot;
		//log_str << "cat orient (deg): " << glm::degrees(main_cat_orient) << endl;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		float rot = glm::radians(3.0f) * rotation_axis * -1;
		main_cat_orient += rot;
		//log_str << "cat orient (deg): " << glm::degrees(main_cat_orient) << endl;
	}
}

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

GLuint loadTexture(const char* texturePath)
{
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	// load the texture data into "data"
	int Width, Height, BPP;
	unsigned char* data = stbi_load(texturePath, &Width, &Height, &BPP, 0);
	// Please pay attention to the format when sending the data to GPU
	GLenum format = 3;
	switch (BPP) {
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}
	if (!data) {
		std::cout << "Failed to load texture: " << texturePath << std::endl;
		exit(1);
	}

	//GLuint textureID = 0;
	//TODO: Create one OpenGL texture and set the texture parameter
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture :
	// to indicate all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	// OpenGL has now copied the data. Free our own version


	stbi_image_free(data);

	std::cout << "Load " << texturePath << " successfully!" << std::endl;


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


GLuint VAO, VBO, EBO;
GLuint catVAO[4], catVBO[4], catEBO[4];
Model floor1, cat[4];
GLuint Texture0, catTex[4], altcatTex[4];

void initFloor(void)
{
	//init floor
	floor1 = loadOBJ("resources\\floor\\floor.obj");
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//Create Vertex Buffer Objects
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, floor1.vertices.size() * sizeof(Vertex), &floor1.vertices[0], GL_STATIC_DRAW);
	//Create Element array Buffer Objects
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, floor1.indices.size() * sizeof(unsigned int), &floor1.indices[0], GL_STATIC_DRAW);
	// 1st attribute buffer : position
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		sizeof(Vertex), // stride
		(void*)offsetof(Vertex, position) // array buffer offset
	);
	//uv
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	//normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	Texture0 = loadTexture("resources\\floor\\floor_diff.jpg");
}

void initCat(void)
{
	//init file name
	const char* obj_file_path[4] = {"resources\\cat\\cat.obj", "resources\\cat1\\cat.obj", "resources\\cat2\\cat.obj", "resources\\pallas cat\\cat.obj"};
	const char* tex_file_path[4] = {"resources\\cat\\cat_01.jpg", "resources\\cat1\\Cat_diffuse.jpg", "resources\\cat2\\Cat_diffuse.jpg", "resources\\pallas cat\\Pallas_Cat_dif.jpg"};
	const char* alt_tex_file_path[4] = {"resources\\cat\\cat_02.jpg", "resources\\cat1\\Cat_bump.jpg", "resources\\cat2\\Cat_bump.jpg", "resources\\pallas cat\\Pallas_Cat_dif.jpg" };
	
	//repeat init cats
	for (int h = 0; h < 3; h++)//change if add cats
	{
		cat[h] = loadOBJ(obj_file_path[h]);

		glGenVertexArrays(1, &catVAO[h]);
		glBindVertexArray(catVAO[h]);
		//vertex
		glGenBuffers(1, &catVBO[h]);
		glBindBuffer(GL_ARRAY_BUFFER, catVBO[h]);
		glBufferData(GL_ARRAY_BUFFER, cat[h].vertices.size() * sizeof(Vertex), &cat[h].vertices[0], GL_STATIC_DRAW);
		//index
		glGenBuffers(1, &catEBO[h]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, catEBO[h]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cat[h].indices.size() * sizeof(unsigned int), &cat[h].indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, catVBO[h]);
		//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		//uv
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		//normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		//texture
		catTex[h] = loadTexture(tex_file_path[h]);
		altcatTex[h] = loadTexture(alt_tex_file_path[h]);
	}
}

void sendDataToOpenGL()
{
	//TODO:
	//Load objects and bind to VAO & VBO
	//Load texture
	initFloor();
	initCat();
}

void lightrotation(int direction)
{
	current_light_orient += 4.5f * direction;
	current_light_orient = (current_light_orient >= 360.0f) ? current_light_orient - 360.0f : current_light_orient;
	light_pos[0][0] = 5.0f * cos(current_light_orient);
	light_pos[0][1] = 5.0f * sin(current_light_orient);
	light_pos[1][0] = 5.0f * cos(current_light_orient);
	light_pos[1][1] = -5.0f * sin(current_light_orient);
	light_pos[2][0] = -5.0f * cos(current_light_orient);
	light_pos[2][1] = -5.0f * sin(current_light_orient);
	light_pos[3][0] = -5.0f * cos(current_light_orient);
	light_pos[3][1] = 5.0f * sin(current_light_orient);
}

//these function should be called using gluttimerfunc of random time and with random movement types as well
void moving_cat1(int mt)
{
	//implement 4 basic movements, forward, backward and 2 rotations
	switch (mt) {
	case 0:
		cat1pos[0] += (cat1pos[0] > -34.0f && cat1pos[0] < +34.0f) ? CAT_AUTO_STEP * sin(glm::radians(cat1orient)) : 0.0f;
		cat1pos[1] += (cat1pos[1] > -25.0f && cat1pos[1] < +25.0f) ? CAT_AUTO_STEP * cos(glm::radians(cat1orient)) : 0.0f;
		moved_disp[0] -= CAT_AUTO_STEP;
		break;
	case 1:
		cat1pos[0] -= (cat1pos[0] > -34.0f && cat1pos[0] < +34.0f) ? CAT_AUTO_STEP * sin(glm::radians(cat1orient)) : 0.0f;
		cat1pos[1] -= (cat1pos[1] > -25.0f && cat1pos[1] < +25.0f) ? CAT_AUTO_STEP * cos(glm::radians(cat1orient)) : 0.0f;
		moved_disp[0] -= CAT_AUTO_STEP;
		break;
	case 2:
		cat1orient += CAT_AUTO_ROTA;
		cat1orient = (cat1orient >= 360.0f) ? cat1orient - 360.0f : cat1orient;
		moved_disp[0] -= CAT_AUTO_ROTA;
		break;
	case 3:
		cat1orient -= CAT_AUTO_ROTA;
		cat1orient = (cat1orient < 0.0f) ? 360.0f - cat1orient : cat1orient;
		moved_disp[0] -= CAT_AUTO_ROTA;
		break;
	default:
		moved_disp[0] -= 300;
	}
	//additional implementation of jumping action
}

void moving_cat2(int mt)
{
	//implement 4 basic movements, forward, backward and 2 rotations
	switch (mt) {
	case 0:
		cat2pos[0] += (cat2pos[0] > -34.0f && cat2pos[0] < +34.0f) ? CAT_AUTO_STEP * sin(glm::radians(cat2orient)) : 0.0f;
		cat2pos[1] += (cat2pos[1] > -25.0f && cat2pos[1] < +25.0f) ? CAT_AUTO_STEP * cos(glm::radians(cat2orient)) : 0.0f;
		moved_disp[1] -= CAT_AUTO_STEP;
		break;
	case 1:
		cat2pos[0] -= (cat2pos[0] > -34.0f && cat2pos[0] < +34.0f) ? CAT_AUTO_STEP * sin(glm::radians(cat2orient)) : 0.0f;
		cat2pos[1] -= (cat2pos[1] > -25.0f && cat2pos[1] < +25.0f) ? CAT_AUTO_STEP * cos(glm::radians(cat2orient)) : 0.0f;
		moved_disp[1] -= CAT_AUTO_STEP;
		break;
	case 2:
		cat2orient += CAT_AUTO_ROTA;
		cat2orient = (cat2orient >= 360.0f) ? cat2orient - 360.0f : cat2orient;
		moved_disp[1] -= CAT_AUTO_ROTA;
		break;
	case 3:
		cat2orient -= CAT_AUTO_ROTA;
		cat2orient = (cat2orient < 0.0f) ? 360.0f - cat2orient : cat2orient;
		moved_disp[1] -= CAT_AUTO_ROTA;
		break;
	default:
		moved_disp[1] -= 300;
	}
	//additional implementation of jumping action
}

void moving_cats(void)
{
	srand(time(NULL));
	if (moved_disp[0] <= 0.0f)
	{
		movement_type[0] = rand() % 5;
		switch (movement_type[0]) {
		case 0:
			//forward
			moved_disp[0] = rand() % 10;
			break;
		case 1:
			//backward
			moved_disp[0] = rand() % 10;
			break;
		case 5:
			moved_disp[0] = rand() % 30000;
			break;
		default:
			moved_disp[0] = rand() % 180;
		}
		glutTimerFunc(300, moving_cat1, movement_type[0]);
	}
	else
	{
		glutTimerFunc(300, moving_cat1, movement_type[0]);
	}
	srand(time(NULL) + 1);
	if (moved_disp[1] <= 0.0f)
	{
		movement_type[1] = rand() % 5;
		switch (movement_type[1]) {
		case 0:
			//forward
			moved_disp[1] = rand() % 10;
			break;
		case 1:
			//backward
			moved_disp[1] = rand() % 10;
			break;
		case 5:
			moved_disp[1] = rand() % 30000;
			break;
		default:
			moved_disp[1] = rand() % 180;
		}
		glutTimerFunc(300, moving_cat2, movement_type[1]);
	}
	else
	{
		glutTimerFunc(300, moving_cat2, movement_type[1]);
	}
}

void paintGL(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	//Deph test
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	//model matrix init
	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	glm::mat4 modelRotationMatrix = glm::mat4(1.0f);
	glm::mat4 modelScalingMatrix = glm::mat4(1.0f);

	float cutoff = glm::cos(glm::radians(30.0f));

	//init cat movement
	moving_cats();

	//light data init
	glm::vec3 ambient_light_color = glm::vec3(+1.0f, +1.0f, +1.0f);
	glm::vec3 dirlgt = lightcoefficient * glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 dirlgtdir = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 pointlight_pos[6];
	pointlight_pos[0] = glm::vec3(+7.0f, +1.0f, +7.0f);
	pointlight_pos[1] = glm::vec3(-7.0f, +1.0f, -7.0f);

	if(rotaion_bool)
		glutTimerFunc(3000, lightrotation, 1);
	for (int k = 0; k < 4; k++)
	{
		pointlight_pos[k + 2] = glm::vec3(light_pos[k][0], +1.0f, light_pos[k][1]);
	}
	glm::vec3 pointlight_color = glm::vec3(1.0f);
	float pointlight_con[3] = { 1.0, 0.5f, 0.05f };

	glm::vec3 spotlight_pos[3];

	glm::vec3 ambient = glm::vec3(0.1f);
	float shininess;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 main_cat_spotlight_dir = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 main_cat_spotlight_pos = glm::vec3(main_cat_pos[0], +1.7f, main_cat_pos[2]);
	glm::vec3 cat_sl_pos[2];
	glm::vec3 spotlight_color = 5.0f * glm::vec3(1.0f);
	cat_sl_pos[0] = glm::vec3(cat1pos[0], +1.7f, cat1pos[1]);
	cat_sl_pos[1] = glm::vec3(cat2pos[0], +1.7f, cat2pos[1]);

	//floor
	glBindVertexArray(VAO);
	modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(+0.0f, -0.5f, +0.0f));
	modelRotationMatrix = glm::mat4(1.0f);
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(+4.0f, +4.0f, +4.0f));

	//find location
	GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	GLint modelRotateMatrixUniformLocation = glGetUniformLocation(programID, "modelRotationMatrix");
	GLint modelScalingMatrixUniformLocation = glGetUniformLocation(programID, "modelScalingMatrix");

	//send data to shader
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(modelRotateMatrixUniformLocation, 1, GL_FALSE, &modelRotationMatrix[0][0]);
	glUniformMatrix4fv(modelScalingMatrixUniformLocation, 1, GL_FALSE, &modelScalingMatrix[0][0]);

	//init projection and view matrix
	glm::mat4 Projection = glm::perspective(30.0f, 640.0f / 480.0f, 2.0f, 50.0f);

	cam_x = 20.0f * sin(cos(glm::radians(cam_yaw))) * cos(glm::radians(cam_pitch));
	cam_y = 20.0f * sin(glm::radians(cam_pitch));
	cam_z = -20.0f * cos(glm::radians(cam_pitch)) * sin(glm::radians(cam_yaw));
	glm::vec3 viewPos = glm::vec3(cam_x, cam_y, cam_z);

	glm::mat4 Lookat = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	glm::mat4 Tmp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -5.0f));
	glm::mat4 ProjectionMatrix = Projection * Lookat * Tmp;

	GLuint MatrixID = glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//fetch light shader variable location
	GLint ambient_light_loc = glGetUniformLocation(programID, "light_amb");
	GLint k_ambient_loc = glGetUniformLocation(programID, "material.ambient");
	GLint viewPos_loc = glGetUniformLocation(programID, "viewPos");
	GLint shininess_loc = glGetUniformLocation(programID, "material.shininess");
	GLint k_diffuse_loc = glGetUniformLocation(programID, "material.diffuse");
	GLint k_spec_loc = glGetUniformLocation(programID, "material.specular");
	GLint dlt_loc = glGetUniformLocation(programID, "directional_light.color");
	GLint dltdir_loc = glGetUniformLocation(programID, "directional_light.dir");
	GLint sptlt0light_color_loc = glGetUniformLocation(programID, "sptlt[0].light.color");
	GLint sptlt0light_pos_loc = glGetUniformLocation(programID, "sptlt[0].light.pos");
	GLint sptlt0light_con_loc = glGetUniformLocation(programID, "sptlt[0].light.con");
	GLint sptlt0light_lin_loc = glGetUniformLocation(programID, "sptlt[0].light.lin");
	GLint sptlt0light_quad_loc = glGetUniformLocation(programID, "sptlt[0].light.quad");
	GLint sptlt0dir_loc = glGetUniformLocation(programID, "sptlt[0].dir");
	GLint sptlt1light_color_loc = glGetUniformLocation(programID, "sptlt[1].light.color");
	GLint sptlt1light_pos_loc = glGetUniformLocation(programID, "sptlt[1].light.pos");
	GLint sptlt1light_con_loc = glGetUniformLocation(programID, "sptlt[1].light.con");
	GLint sptlt1light_lin_loc = glGetUniformLocation(programID, "sptlt[1].light.lin");
	GLint sptlt1light_quad_loc = glGetUniformLocation(programID, "sptlt[1].light.quad");
	GLint sptlt1dir_loc = glGetUniformLocation(programID, "sptlt[1].dir");
	GLint sptlt2light_color_loc = glGetUniformLocation(programID, "sptlt[2].light.color");
	GLint sptlt2light_pos_loc = glGetUniformLocation(programID, "sptlt[2].light.pos");
	GLint sptlt2light_con_loc = glGetUniformLocation(programID, "sptlt[2].light.con");
	GLint sptlt2light_lin_loc = glGetUniformLocation(programID, "sptlt[2].light.lin");
	GLint sptlt2light_quad_loc = glGetUniformLocation(programID, "sptlt[2].light.quad");
	GLint sptlt2dir_loc = glGetUniformLocation(programID, "sptlt[2].dir");
	GLint ptlt0color_loc = glGetUniformLocation(programID, "ptlt[0].color");
	GLint ptlt0pos_loc = glGetUniformLocation(programID, "ptlt[0].pos");
	GLint ptlt0con_loc = glGetUniformLocation(programID, "ptlt[0].con");
	GLint ptlt0lin_loc = glGetUniformLocation(programID, "ptlt[0].lin");
	GLint ptlt0quad_loc = glGetUniformLocation(programID, "ptlt[0].quad");
	GLint ptlt1color_loc = glGetUniformLocation(programID, "ptlt[1].color");
	GLint ptlt1pos_loc = glGetUniformLocation(programID, "ptlt[1].pos");
	GLint ptlt1con_loc = glGetUniformLocation(programID, "ptlt[1].con");
	GLint ptlt1lin_loc = glGetUniformLocation(programID, "ptlt[1].lin");
	GLint ptlt1quad_loc = glGetUniformLocation(programID, "ptlt[1].quad");
	GLint ptlt2color_loc = glGetUniformLocation(programID, "ptlt[2].color");
	GLint ptlt2pos_loc = glGetUniformLocation(programID, "ptlt[2].pos");
	GLint ptlt2con_loc = glGetUniformLocation(programID, "ptlt[2].con");
	GLint ptlt2lin_loc = glGetUniformLocation(programID, "ptlt[2].lin");
	GLint ptlt2quad_loc = glGetUniformLocation(programID, "ptlt[2].quad");
	GLint ptlt3color_loc = glGetUniformLocation(programID, "ptlt[3].color");
	GLint ptlt3pos_loc = glGetUniformLocation(programID, "ptlt[3].pos");
	GLint ptlt3con_loc = glGetUniformLocation(programID, "ptlt[3].con");
	GLint ptlt3lin_loc = glGetUniformLocation(programID, "ptlt[3].lin");
	GLint ptlt3quad_loc = glGetUniformLocation(programID, "ptlt[3].quad");
	GLint ptlt4color_loc = glGetUniformLocation(programID, "ptlt[4].color");
	GLint ptlt4pos_loc = glGetUniformLocation(programID, "ptlt[4].pos");
	GLint ptlt4con_loc = glGetUniformLocation(programID, "ptlt[4].con");
	GLint ptlt4lin_loc = glGetUniformLocation(programID, "ptlt[4].lin");
	GLint ptlt4quad_loc = glGetUniformLocation(programID, "ptlt[4].quad");
	GLint ptlt5color_loc = glGetUniformLocation(programID, "ptlt[5].color");
	GLint ptlt5pos_loc = glGetUniformLocation(programID, "ptlt[5].pos");
	GLint ptlt5con_loc = glGetUniformLocation(programID, "ptlt[5].con");
	GLint ptlt5lin_loc = glGetUniformLocation(programID, "ptlt[5].lin");
	GLint ptlt5quad_loc = glGetUniformLocation(programID, "ptlt[5].quad");
	GLint sptlt0cutoff_loc = glGetUniformLocation(programID, "sptlt[0].cutoff");
	GLint sptlt1cutoff_loc = glGetUniformLocation(programID, "sptlt[1].cutoff");
	GLint sptlt2cutoff_loc = glGetUniformLocation(programID, "sptlt[2].cutoff");

	//define material coefficient
	shininess = 32.0f;
	ambient = glm::vec3(0.3f);
	diffuse = glm::vec3(1.0f);
	specular = glm::vec3(1.0f);

	//fragment shader part
	//send texture
	unsigned int slot = 0;
	GLuint TexLoc = glGetUniformLocation(programID, "myTextureSampler0");
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, Texture0);
	glUniform1i(TexLoc, 0);
	//send light information
	glUniform3fv(ambient_light_loc, 1, &ambient_light_color[0]);
	glUniform3fv(k_ambient_loc, 1, &ambient[0]);
	glUniform3fv(viewPos_loc, 1, &viewPos[0]);
	glUniform1f(shininess_loc, shininess);
	glUniform3fv(k_diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(k_spec_loc, 1, &specular[0]);
	glUniform3fv(ptlt0pos_loc, 1, &pointlight_pos[0][0]);
	glUniform3fv(ptlt2pos_loc, 1, &pointlight_pos[2][0]);
	glUniform3fv(ptlt5pos_loc, 1, &pointlight_pos[5][0]);
	glUniform3fv(ptlt0color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt0con_loc, pointlight_con[0]);
	glUniform1f(ptlt0lin_loc, pointlight_con[1]);
	glUniform1f(ptlt0quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt2color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt2con_loc, pointlight_con[0]);
	glUniform1f(ptlt2lin_loc, pointlight_con[1]);
	glUniform1f(ptlt2quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt5color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt5con_loc, pointlight_con[0]);
	glUniform1f(ptlt5lin_loc, pointlight_con[1]);
	glUniform1f(ptlt5quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt3pos_loc, 1, &pointlight_pos[3][0]);
	glUniform3fv(ptlt1pos_loc, 1, &pointlight_pos[1][0]);
	glUniform3fv(ptlt3color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt3con_loc, pointlight_con[0]);
	glUniform1f(ptlt3lin_loc, pointlight_con[1]);
	glUniform1f(ptlt3quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt1color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt1con_loc, pointlight_con[0]);
	glUniform1f(ptlt1lin_loc, pointlight_con[1]);
	glUniform1f(ptlt1quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt4pos_loc, 1, &pointlight_pos[4][0]);
	glUniform3fv(ptlt4color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt4con_loc, pointlight_con[0]);
	glUniform1f(ptlt4lin_loc, pointlight_con[1]);
	glUniform1f(ptlt4quad_loc, pointlight_con[2]);
	glUniform3fv(sptlt0dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt0light_pos_loc, 1, &main_cat_spotlight_pos[0]);
	glUniform3fv(sptlt0light_color_loc, 1, &spotlight_color[0]);
	glUniform1f(sptlt0light_con_loc, 1.0f);
	glUniform1f(sptlt0light_lin_loc, 0.7f);
	glUniform1f(sptlt0light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_con_loc, 1.0f);
	glUniform1f(sptlt1light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_con_loc, 1.0f);
	glUniform1f(sptlt2light_quad_loc, 0.5f);
	glUniform3fv(sptlt1dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt2dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt1light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt2light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt1light_pos_loc, 1, &cat_sl_pos[0][0]);
	glUniform3fv(sptlt2light_pos_loc, 1, &cat_sl_pos[1][0]);
	glUniform1f(sptlt0cutoff_loc, cutoff);
	glUniform1f(sptlt1cutoff_loc, cutoff);
	glUniform1f(sptlt2cutoff_loc, cutoff);
	glUniform3fv(dlt_loc, 1, &dirlgt[0]);
	glUniform3fv(dltdir_loc, 1, &dirlgtdir[0]);

	//glBindTexture(GL_TEXTURE_2D, Texture0);
	glDrawElements(GL_TRIANGLES, floor1.indices.size(), GL_UNSIGNED_INT, 0);

	//cat0 main cat controllable
	glBindVertexArray(catVAO[0]);
	//init model matrix
	modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(main_cat_pos[0], main_cat_pos[1], main_cat_pos[2]));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(+0.15f, +0.15f, +0.15f));
	modelRotationMatrix = glm::rotate(glm::mat4(1.0f), main_cat_orient, glm::vec3(+0.0f, +1.0f, +0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, +0.0f, +0.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(modelRotateMatrixUniformLocation, 1, GL_FALSE, &modelRotationMatrix[0][0]);
	glUniformMatrix4fv(modelScalingMatrixUniformLocation, 1, GL_FALSE, &modelScalingMatrix[0][0]);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//init material coefficient
	shininess = 4.0f;
	ambient = glm::vec3(0.3f);
	diffuse = glm::vec3(1.0f);
	specular = glm::vec3(0.5f);

	//fragment shader part
	//send texture
	slot = 0;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, (main_cat_tex_ind == 1) ? catTex[0] : altcatTex[0]);
	glUniform1i(TexLoc, 0);
	//send light information
	glUniform3fv(ambient_light_loc, 1, &ambient_light_color[0]);
	glUniform3fv(k_ambient_loc, 1, &ambient[0]);
	glUniform3fv(viewPos_loc, 1, &viewPos[0]);
	glUniform1f(shininess_loc, shininess);
	glUniform3fv(k_diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(k_spec_loc, 1, &specular[0]);
	glUniform3fv(ptlt0pos_loc, 1, &pointlight_pos[0][0]);
	glUniform3fv(ptlt2pos_loc, 1, &pointlight_pos[2][0]);
	glUniform3fv(ptlt5pos_loc, 1, &pointlight_pos[5][0]);
	glUniform3fv(ptlt0color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt0con_loc, pointlight_con[0]);
	glUniform1f(ptlt0lin_loc, pointlight_con[1]);
	glUniform1f(ptlt0quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt2color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt2con_loc, pointlight_con[0]);
	glUniform1f(ptlt2lin_loc, pointlight_con[1]);
	glUniform1f(ptlt2quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt5color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt5con_loc, pointlight_con[0]);
	glUniform1f(ptlt5lin_loc, pointlight_con[1]);
	glUniform1f(ptlt5quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt3pos_loc, 1, &pointlight_pos[3][0]);
	glUniform3fv(ptlt1pos_loc, 1, &pointlight_pos[1][0]);
	glUniform3fv(ptlt3color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt3con_loc, pointlight_con[0]);
	glUniform1f(ptlt3lin_loc, pointlight_con[1]);
	glUniform1f(ptlt3quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt1color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt1con_loc, pointlight_con[0]);
	glUniform1f(ptlt1lin_loc, pointlight_con[1]);
	glUniform1f(ptlt1quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt4pos_loc, 1, &pointlight_pos[4][0]);
	glUniform3fv(ptlt4color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt4con_loc, pointlight_con[0]);
	glUniform1f(ptlt4lin_loc, pointlight_con[1]);
	glUniform1f(ptlt4quad_loc, pointlight_con[2]);
	glUniform3fv(sptlt0dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt0light_pos_loc, 1, &main_cat_spotlight_pos[0]);
	glUniform3fv(sptlt0light_color_loc, 1, &spotlight_color[0]);
	glUniform1f(sptlt0light_con_loc, 1.0f);
	glUniform1f(sptlt0light_lin_loc, 0.7f);
	glUniform1f(sptlt0light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_con_loc, 1.0f);
	glUniform1f(sptlt1light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_con_loc, 1.0f);
	glUniform1f(sptlt2light_quad_loc, 0.5f);
	glUniform3fv(sptlt1dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt2dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt1light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt2light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt1light_pos_loc, 1, &cat_sl_pos[0][0]);
	glUniform3fv(sptlt2light_pos_loc, 1, &cat_sl_pos[1][0]);
	glUniform1f(sptlt0cutoff_loc, cutoff);
	glUniform1f(sptlt1cutoff_loc, cutoff);
	glUniform1f(sptlt2cutoff_loc, cutoff);
	glUniform3fv(dlt_loc, 1, &dirlgt[0]);
	glUniform3fv(dltdir_loc, 1, &dirlgtdir[0]);


	glDrawElements(GL_TRIANGLES, cat[0].indices.size(), GL_UNSIGNED_INT, 0);

	//cat1
	//init model matrix
	glBindVertexArray(catVAO[1]);
	modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cat1pos[0], -3.5f, cat1pos[1]));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(+0.15f, +0.15f, +0.15f));
	modelRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(cat1orient), glm::vec3(+0.0f, +1.0f, +0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, +0.0f, +0.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(modelRotateMatrixUniformLocation, 1, GL_FALSE, &modelRotationMatrix[0][0]);
	glUniformMatrix4fv(modelScalingMatrixUniformLocation, 1, GL_FALSE, &modelScalingMatrix[0][0]);
	ProjectionMatrix = Projection * Lookat * Tmp;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//fragment shader
	slot = 0;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, catTex[1]);
	glUniform1i(TexLoc, 0);
	//light information
	glUniform3fv(ambient_light_loc, 1, &ambient_light_color[0]);
	glUniform3fv(k_ambient_loc, 1, &ambient[0]);
	glUniform3fv(viewPos_loc, 1, &viewPos[0]);
	glUniform1f(shininess_loc, shininess);
	glUniform3fv(k_diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(k_spec_loc, 1, &specular[0]);
	glUniform3fv(ptlt0pos_loc, 1, &pointlight_pos[0][0]);
	glUniform3fv(ptlt2pos_loc, 1, &pointlight_pos[2][0]);
	glUniform3fv(ptlt5pos_loc, 1, &pointlight_pos[5][0]);
	glUniform3fv(ptlt0color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt0con_loc, pointlight_con[0]);
	glUniform1f(ptlt0lin_loc, pointlight_con[1]);
	glUniform1f(ptlt0quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt2color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt2con_loc, pointlight_con[0]);
	glUniform1f(ptlt2lin_loc, pointlight_con[1]);
	glUniform1f(ptlt2quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt5color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt5con_loc, pointlight_con[0]);
	glUniform1f(ptlt5lin_loc, pointlight_con[1]);
	glUniform1f(ptlt5quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt3pos_loc, 1, &pointlight_pos[3][0]);
	glUniform3fv(ptlt1pos_loc, 1, &pointlight_pos[1][0]);
	glUniform3fv(ptlt3color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt3con_loc, pointlight_con[0]);
	glUniform1f(ptlt3lin_loc, pointlight_con[1]);
	glUniform1f(ptlt3quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt1color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt1con_loc, pointlight_con[0]);
	glUniform1f(ptlt1lin_loc, pointlight_con[1]);
	glUniform1f(ptlt1quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt4pos_loc, 1, &pointlight_pos[4][0]);
	glUniform3fv(ptlt4color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt4con_loc, pointlight_con[0]);
	glUniform1f(ptlt4lin_loc, pointlight_con[1]);
	glUniform1f(ptlt4quad_loc, pointlight_con[2]);
	glUniform3fv(sptlt0dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt0light_pos_loc, 1, &main_cat_spotlight_pos[0]);
	glUniform3fv(sptlt0light_color_loc, 1, &spotlight_color[0]);
	glUniform1f(sptlt0light_con_loc, 1.0f);
	glUniform1f(sptlt0light_lin_loc, 0.7f);
	glUniform1f(sptlt0light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_con_loc, 1.0f);
	glUniform1f(sptlt1light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_con_loc, 1.0f);
	glUniform1f(sptlt2light_quad_loc, 0.5f);
	glUniform3fv(sptlt1dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt2dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt1light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt2light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt1light_pos_loc, 1, &cat_sl_pos[0][0]);
	glUniform3fv(sptlt2light_pos_loc, 1, &cat_sl_pos[1][0]);
	glUniform1f(sptlt0cutoff_loc, cutoff);
	glUniform1f(sptlt1cutoff_loc, cutoff);
	glUniform1f(sptlt2cutoff_loc, cutoff);
	glUniform3fv(dlt_loc, 1, &dirlgt[0]);
	glUniform3fv(dltdir_loc, 1, &dirlgtdir[0]);

	glDrawElements(GL_TRIANGLES, cat[1].indices.size(), GL_UNSIGNED_INT, 0);

	//cat2
	//init matrix
	glBindVertexArray(catVAO[2]);
	modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cat2pos[0], -3.5f, cat2pos[1]));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(+0.15f, +0.15f, +0.15f));
	modelRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(cat2orient), glm::vec3(0.0f, +1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, +0.0f, +0.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(modelRotateMatrixUniformLocation, 1, GL_FALSE, &modelRotationMatrix[0][0]);
	glUniformMatrix4fv(modelScalingMatrixUniformLocation, 1, GL_FALSE, &modelScalingMatrix[0][0]);
	ProjectionMatrix = Projection * Lookat * Tmp;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//fragment shader part
	slot = 0;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, catTex[2]);
	glUniform1i(TexLoc, 0);
	//send light information
	glUniform3fv(ambient_light_loc, 1, &ambient_light_color[0]);
	glUniform3fv(k_ambient_loc, 1, &ambient[0]);
	glUniform3fv(viewPos_loc, 1, &viewPos[0]);
	glUniform1f(shininess_loc, shininess);
	glUniform3fv(k_diffuse_loc, 1, &diffuse[0]);
	glUniform3fv(k_spec_loc, 1, &specular[0]);
	glUniform3fv(ptlt0pos_loc, 1, &pointlight_pos[0][0]);
	glUniform3fv(ptlt2pos_loc, 1, &pointlight_pos[2][0]);
	glUniform3fv(ptlt5pos_loc, 1, &pointlight_pos[5][0]);
	glUniform3fv(ptlt0color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt0con_loc, pointlight_con[0]);
	glUniform1f(ptlt0lin_loc, pointlight_con[1]);
	glUniform1f(ptlt0quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt2color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt2con_loc, pointlight_con[0]);
	glUniform1f(ptlt2lin_loc, pointlight_con[1]);
	glUniform1f(ptlt2quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt5color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt5con_loc, pointlight_con[0]);
	glUniform1f(ptlt5lin_loc, pointlight_con[1]);
	glUniform1f(ptlt5quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt3pos_loc, 1, &pointlight_pos[3][0]);
	glUniform3fv(ptlt1pos_loc, 1, &pointlight_pos[1][0]);
	glUniform3fv(ptlt3color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt3con_loc, pointlight_con[0]);
	glUniform1f(ptlt3lin_loc, pointlight_con[1]);
	glUniform1f(ptlt3quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt1color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt1con_loc, pointlight_con[0]);
	glUniform1f(ptlt1lin_loc, pointlight_con[1]);
	glUniform1f(ptlt1quad_loc, pointlight_con[2]);
	glUniform3fv(ptlt4pos_loc, 1, &pointlight_pos[4][0]);
	glUniform3fv(ptlt4color_loc, 1, &pointlight_color[0]);
	glUniform1f(ptlt4con_loc, pointlight_con[0]);
	glUniform1f(ptlt4lin_loc, pointlight_con[1]);
	glUniform1f(ptlt4quad_loc, pointlight_con[2]);
	glUniform3fv(sptlt0dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt0light_pos_loc, 1, &main_cat_spotlight_pos[0]);
	glUniform3fv(sptlt0light_color_loc, 1, &spotlight_color[0]);
	glUniform1f(sptlt0light_con_loc, 1.0f);
	glUniform1f(sptlt0light_lin_loc, 0.7f);
	glUniform1f(sptlt0light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_con_loc, 1.0f);
	glUniform1f(sptlt1light_quad_loc, 0.5f);
	glUniform1f(sptlt1light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_lin_loc, 0.7f);
	glUniform1f(sptlt2light_con_loc, 1.0f);
	glUniform1f(sptlt2light_quad_loc, 0.5f);
	glUniform3fv(sptlt1dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt2dir_loc, 1, &main_cat_spotlight_dir[0]);
	glUniform3fv(sptlt1light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt2light_color_loc, 1, &spotlight_color[0]);
	glUniform3fv(sptlt1light_pos_loc, 1, &cat_sl_pos[0][0]);
	glUniform3fv(sptlt2light_pos_loc, 1, &cat_sl_pos[1][0]);
	glUniform1f(sptlt0cutoff_loc, cutoff);
	glUniform1f(sptlt1cutoff_loc, cutoff);
	glUniform1f(sptlt2cutoff_loc, cutoff);
	glUniform3fv(dlt_loc, 1, &dirlgt[0]);
	glUniform3fv(dltdir_loc, 1, &dirlgtdir[0]);

	glDrawElements(GL_TRIANGLES, cat[2].indices.size(), GL_UNSIGNED_INT, 0);

	/*//cat3
	glBindVertexArray(catVAO[3]);
	modelTransformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(+2.0f, +0.0f, +1.0f));
	modelScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(+0.05f, +0.05f, +0.05f));
	modelRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, +0.0f, +0.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(modelRotateMatrixUniformLocation, 1, GL_FALSE, &modelRotationMatrix[0][0]);
	glUniformMatrix4fv(modelScalingMatrixUniformLocation, 1, GL_FALSE, &modelScalingMatrix[0][0]);
	ProjectionMatrix = Projection * Lookat * Tmp;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	slot = 0;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, catTex[3]);
	glUniform1i(TexLoc, 0);
	glDrawElements(GL_TRIANGLES, cat[3].indices.size(), GL_UNSIGNED_INT, 0);*/

	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	sendDataToOpenGL();
}

int main(int argc, char* argv[])
{
	//log_str.open("log.txt", ofstream::out | ofstream::trunc);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Assignment 2");

	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();
	glutDisplayFunc(paintGL);

	glutMouseFunc(mouse_callback);
	glutMotionFunc(motion_callback);
	glutKeyboardFunc(keyboard_callback);
	glutSpecialFunc(special_callback);

	glutMainLoop();
	//system("PAUSE");
	return 0;
}

void mouseWheel_callback(int wheel, int direction, int x, int y)
{
	// Optional.
}
