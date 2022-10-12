#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

GLuint theProgram;
GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

glm::mat4 cameraToClipMatrix(0.0f);

float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

const float fFrustumScale = CalcFrustumScale(45.0f);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

	theProgram = Framework::CreateProgram(shaderList);

	positionAttrib = glGetAttribLocation(theProgram, "position");
	colorAttrib = glGetAttribLocation(theProgram, "color");

	modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");

	float fzNear = 1.0f; float fzFar = 100.0f;

	cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);
}

const int numberOfVertices = 24;

#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f

#define YELLOW_COLOR 1.0f, 1.0f, 0.0f, 1.0f
#define CYAN_COLOR 0.0f, 1.0f, 1.0f, 1.0f
#define MAGENTA_COLOR 	1.0f, 0.0f, 1.0f, 1.0f

const float vertexData[] =
{
	//Front
	+1.0f, +1.0f, +1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	//Top
	+1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,

	//Left
	+1.0f, +1.0f, +1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,

	//Back
	+1.0f, +1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	//Bottom
	+1.0f, -1.0f, +1.0f,
	+1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, +1.0f,

	//Right
	-1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,


	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,

	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,

	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	10, 11, 8,

	12, 13, 14,
	14, 15, 12,

	16, 17, 18,
	18, 19, 16,

	20, 21, 22,
	22, 23, 20,
};

GLuint vertexBufferObject;
GLuint indexBufferObject;
GLuint vao;

void InitializeVAO()
{
	glGenBuffers(1, &vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBufferObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(positionAttrib);
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

	glBindVertexArray(0);
}

inline float DegToRad(float fAngDeg)
{
	const float fDegToRad = 3.14159f * 2.0f / 360.0f;
	return fAngDeg * fDegToRad;
}

inline float Clamp(float fValue, float fMinValue, float fMaxValue)
{
	if(fValue < fMinValue)
		return fMinValue;

	if(fValue > fMaxValue)
		return fMaxValue;

	return fValue;
}

glm::mat3 RotateX(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[1].y = fCos; theMat[2].y = -fSin;
	theMat[1].z = fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateY(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[2].x = fSin;
	theMat[0].z = -fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateZ(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[1].x = -fSin;
	theMat[0].y = fSin; theMat[1].y = fCos;
	return theMat;
}

class TransformMatrix
{
public:
	TransformMatrix()
		: m_currMat(1.0f)
	{
	}

	const glm::mat4 &Current()
	{
		return m_currMat;
	}

	void RotateX(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateX(fAngDeg));
	}

	void RotateY(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateY(fAngDeg));
	}

	void RotateZ(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateZ(fAngDeg));
	}

	void Scale(const glm::vec3 &scaleVec)
	{
		glm::mat4 scaleMat(1.0f);
		scaleMat[0].x = scaleVec.x;
		scaleMat[1].y = scaleVec.y;
		scaleMat[2].z = scaleVec.z;

		m_currMat = m_currMat * scaleMat;
	}

	void Translate(const glm::vec3 &offsetVec)
	{
		glm::mat4 translateMat(1.0f);
		translateMat[3] = glm::vec4(offsetVec, 1.0f);

		m_currMat = m_currMat * translateMat;
	}

private:
	glm::mat4 m_currMat;
};

struct Node
{
	Node *parent = nullptr;
	std::vector<Node *> children;

	Node(const glm::vec3& pose_base,
	float yaw_,
	float roll_,
	float pitch_,
	const glm::vec3& scale,
	const glm::vec3& pose_model_,
	bool render_this = true)
	: pos_base(pose_base)
	, pos_model(pose_model_)
	, size(scale)
	, yaw(yaw_)
	, pitch(pitch_)
	, roll(roll_)
	, render_this_node(render_this)
	{

	}
	Node(){}

	void render(const glm::mat4& T_camera_model)
	{
		TransformMatrix stack;
		stack.Translate(pos_base);
		stack.RotateY(yaw);
		stack.RotateX(pitch);
		stack.RotateZ(roll);
		
		glm::mat4 T_camera_world = T_camera_model * stack.Current();

		if (render_this_node)
		{
			TransformMatrix stack2;
			stack2.Translate(pos_model);
			stack2.Scale(size);
			glm::mat4 T_word_model = stack2.Current();
			glm::mat4 T_camera_curr_model = T_camera_world * T_word_model;
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(T_camera_curr_model));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
		}
		for (auto& c : children)
		{
			c->render(T_camera_world);
		}
	}

	glm::vec3 pos_base = glm::vec3(0.0f);
	glm::vec3 pos_model = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(0.0f);
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
	bool render_this_node = true;
};

class Hierarchy
{
public:
	Hierarchy()
		: sizeUpperArm(9.0f)
		, widthLowerArm(1.5f)
		, lenLowerArm(5.0f)
		, lenWrist(2.0f)
		, widthWrist(2.0f)
		, lenFinger(2.0f)
		, widthFinger(0.5f)
		, scene(glm::vec3(3.0f, -5.0f, -40.0f), -45.0f, 0.0f, 0.0f, glm::vec3(1.0f), glm::vec3(0.0f), false)
		, base_left(glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(2.0f, 0.0f, 0.0f))
		, base_right(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f,glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(-2.0f, 0.0f, 0.0f))
		, upper_arm(glm::vec3(0.0), 0.0f, 0.0f, -33.75f, glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f), glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f))
		, lower_arm(glm::vec3(0.0f, 0.0f, 8.0f), 0.0f, 0.0f, 146.25f, glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f), glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f))
		, wrist(glm::vec3(0.0f, 0.0f, 5.0f), 0.0f, 0.0f, 67.5f, glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f), glm::vec3(0.0f))
		, left_upper_finger(glm::vec3(1.0f, 0.0f, 1.0f),  180.0f, 0.0f, 0.0f, glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f), glm::vec3(0.0f, 0.0f, lenFinger / 2.0f))
		, left_lower_finger(glm::vec3(0.0f, 0.0f, lenFinger),  -45.0f, 0.0f, 0.0f, glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f), glm::vec3(0.0f, 0.0f, lenFinger / 2.0f))
		, right_upper_finger(glm::vec3(-1.0f, 0.0f, 1.0f),  -180.0f, 0.0f, 0.0f, glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f), glm::vec3(0.0f, 0.0f, lenFinger / 2.0f))
		, right_lower_finger(glm::vec3(0.0f, 0.0f, lenFinger),  45.0f, 0.0f, 0.0f, glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f), glm::vec3(0.0f, 0.0f, lenFinger / 2.0f))
	{
		scene.children.push_back(&base_left);
		scene.children.push_back(&base_right);
		scene.children.push_back(&upper_arm);
		upper_arm.children.push_back(&lower_arm);
		lower_arm.children.push_back(&wrist);
		wrist.children.push_back(&left_upper_finger);
		left_upper_finger.children.push_back(&left_lower_finger);
		wrist.children.push_back(&right_upper_finger);
		right_upper_finger.children.push_back(&right_lower_finger);
	}

	void Draw()
	{
		TransformMatrix modelToCameraStack;

		glUseProgram(theProgram);
		glBindVertexArray(vao);
		scene.render(glm::mat4(1.0f));
		glBindVertexArray(0);
		glUseProgram(0);
	}

#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f

	void AdjBase(bool bIncrement)
	{
		scene.yaw += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		scene.yaw = fmodf(scene.yaw, 360.0f);
	}

	void AdjUpperArm(bool bIncrement)
	{
		
		upper_arm.pitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		upper_arm.pitch = Clamp(upper_arm.pitch, -90.0f, 0.0f);
	}

	void AdjLowerArm(bool bIncrement)
	{
		
		lower_arm.pitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		lower_arm.pitch = Clamp(lower_arm.pitch, 0.0f, 146.25f);
	}

	void AdjWristPitch(bool bIncrement)
	{
		wrist.pitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		wrist.pitch = Clamp(wrist.pitch, 0.0f, 90.0f);
	}

	void AdjWristRoll(bool bIncrement)
	{
		wrist.roll += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		wrist.roll = fmodf(wrist.roll, 360.0f);
	}

	void AdjFingerOpen(bool bIncrement)
	{
		left_upper_finger.yaw += bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
		left_upper_finger.yaw = Clamp(left_upper_finger.yaw, 9.0f, 180.0f);
		right_upper_finger.yaw -= bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
		right_upper_finger.yaw = Clamp(right_upper_finger.yaw,-180.0f, -9.0f);
		//right_upper_finger.yaw =-left_upper_finger.yaw;
	}

	void WritePose()
	{
		printf("angBase:\t%f\n", scene.yaw);
		printf("angUpperArm:\t%f\n", upper_arm.pitch);
		printf("angLowerArm:\t%f\n", lower_arm.pitch);
		printf("angWristPitch:\t%f\n", wrist.pitch);
		printf("angWristRoll:\t%f\n", wrist.roll);
		printf("angFingerOpen:\t%f\n", left_upper_finger.yaw);
		printf("\n");
	}

private:
	float			sizeUpperArm;

	float			lenLowerArm;
	float			widthLowerArm;

	float			lenWrist;
	float			widthWrist;

	float			lenFinger;
	float			widthFinger;



	Node scene;
	Node base_left;
	Node base_right;
	Node upper_arm;
	Node lower_arm;
	Node wrist;
	Node left_upper_finger;
	Node left_lower_finger;
	Node right_upper_finger;
	Node right_lower_finger;
};


Hierarchy g_armature;

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVAO();


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_armature.Draw();

	glutSwapBuffers();
	glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (int w, int h)
{
	cameraToClipMatrix[0].x = fFrustumScale * (h / (float)w);
	cameraToClipMatrix[1].y = fFrustumScale;

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a': g_armature.AdjBase(true); break;
	case 'd': g_armature.AdjBase(false); break;
	case 'w': g_armature.AdjUpperArm(false); break;
	case 's': g_armature.AdjUpperArm(true); break;
	case 'r': g_armature.AdjLowerArm(false); break;
	case 'f': g_armature.AdjLowerArm(true); break;
	case 't': g_armature.AdjWristPitch(false); break;
	case 'g': g_armature.AdjWristPitch(true); break;
	case 'z': g_armature.AdjWristRoll(true); break;
	case 'c': g_armature.AdjWristRoll(false); break;
	case 'q': g_armature.AdjFingerOpen(true); break;
	case 'e': g_armature.AdjFingerOpen(false); break;
	case 32: g_armature.WritePose(); break;
	}
}


unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}
