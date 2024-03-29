// Header files
#include <windows.h>
#include "..\lib\OGL.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// OpenGL Header Files
#include <GL/glew.h>	// This must be above GL.h
#include <GL/gl.h>
#include "..\lib\vmath.h"
#include "..\lib\Sphere.h"
using namespace vmath; // only works in cpp


#define WINWIDTH 800
#define WINHEIGHT 600
#define MAX_STACK_SIZE 32

#define TEXTURE_PRESENT 1
#define TEXTURE_NOT_PRESENT 0

// OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "openGL32.lib")
#pragma comment(lib, "Sphere.lib")

// Global Function Declarations / Signatures
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL; // HGLRC -> Handle to opengl rendering context

BOOL gbFullscreen = FALSE;
FILE* gpFile = NULL;
FILE *vshaderFile = NULL;
FILE *fshaderFile = NULL;
BOOL gbActiveWindow = FALSE;
BOOL bDone = FALSE;
int iRetVal = 0;

// Programmamble pipeline related global variables
GLuint shaderProgramObject_sphere;
GLuint shaderProgramObject_ground;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
	// VDG_ATTRIBUTE_NORMAL,
	// VDG_ATTRIBUTE_VERTEX
};

GLuint gVao_cube;
GLuint gVbo_cube_position;
GLuint vbo_cube_texcoord;

GLuint gVao_sphere;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_texture;
GLuint modelMatrixUniformSphere;
GLuint viewMatrixUniformSphere;
GLuint projectionMatrixUniformSphere;

GLuint modelMatrixUniformGround;
GLuint viewMatrixUniformGround;
GLuint projectionMatrixUniformGround;
GLuint textureSamplerUniformGround;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];


GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};
GLfloat MaterialDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};


int gNumVertices;
int gNumElements;

GLuint texture_spheres;
GLuint texture_ground;

mat4 perspectiveProjectionMatrix;

// sphere properties
float sphereRoll = 0.0f;
float sphereMoveX = 0.0f;
float sphereMoveY = 0.0f;
int sphereMoveDirection = 0;
int sphereJump = 0;
int sphereAtTop = 0;

vec3 eyeVector = vec3(0.0f, 0.0f, 4.0f);
vec3 centerVector = vec3(0.0f, 0.0f, -1.0f);
vec3 upVector = vec3(0.0f, 1.0f, 0.0);

const float cameraSpeed = 1.03f;
const float sphereMoveSpeed = 0.5f;

struct stack
{
	int top;
	mat4 items[MAX_STACK_SIZE];
};

struct stack s;

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	
	int initialise(void);
	void display(void);
	void update(void);
	void uninitialise(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	int xscreen = GetSystemMetrics(SM_CXSCREEN);
	int yscreen = GetSystemMetrics(SM_CYSCREEN);

	// code
	
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log File Failed. Exiting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File is successfully created\n");
	}
	 
	// INitialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	//wndclass.

	// Registering above wndclassex
	RegisterClassEx(&wndclass);

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						szAppName,
						TEXT("OpenGL Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, // My window will clip others, others will not clip me! (clipchildren and siblings), WS_VISIBLE given because there is no WM_PAINT. initially visible irrespective of WM_PAINT
						(xscreen - WINWIDTH) / 2,
						(yscreen - WINHEIGHT) / 2,
						WINWIDTH,
						WINHEIGHT,
						NULL,
						NULL,
						hInstance,
						NULL);
	ghwnd = hwnd;

	// Initialise
	iRetVal = initialise();

	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel Format Failed");
		uninitialise();
	}

	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format Failed");
		uninitialise();
	}

	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL Context Failed");
		uninitialise();
	}

	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making openGL context as current context failed");
		uninitialise();
	}

	else if (iRetVal == -5)
	{
		fprintf(gpFile, "");
		uninitialise();
	}

	else if (iRetVal == -6)
	{
		fprintf(gpFile, "");
		uninitialise();
	}

	else
	{
		fprintf(gpFile, "Initialisation Succesful\n");
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	// Update the Window
	// UpdateWindow(hwnd);

	// Foregrounding and Focusing the Window
	SetForegroundWindow(hwnd); // ghwnd and hwnd both will work - but hwnd is inside winmain while ghwnd was created for functions other than winmain
	SetFocus(hwnd);

	// Message Loop
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				// Render the Scene
				display();

				// Update the Scene
				update();

			}
		}
	}

	uninitialise();

	return (int)msg.wParam;
}

// CALLBACK FUNCTION
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullScreen(void);
	void resize(int, int);
	// void uninitialise(void);

	// code
	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			DestroyWindow(hwnd);
			break;
		case VK_RIGHT:
			sphereMoveDirection = -1;
			eyeVector += vmath::normalize(vmath::cross(centerVector, upVector)) * cameraSpeed;
			sphereRoll -= 1.0f;
			sphereMoveX += 1.03f;
			if (sphereRoll <= -360.0f)
			{
				sphereRoll = 0.0f;
			}
			break;
		case VK_LEFT:
			sphereMoveDirection = 1;
			eyeVector -= vmath::normalize(vmath::cross(centerVector, upVector)) * cameraSpeed;
			sphereRoll += 1.0f;
			sphereMoveX -= 1.03f;
			if (sphereRoll >= 360.0f)
			{
				sphereRoll = 0.0f;
			}
			break;
		case VK_UP:
			sphereMoveDirection = 2;
			sphereRoll = sphereRoll;
			sphereJump = sphereJump ? 0 : 1;
			break;
		default:
			break;
		}
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		// break;
		return 0;
	case WM_CHAR:
		switch (wParam)
		{
		case 'a':
			sphereRoll -= 1.0f;
			if (sphereRoll <= -360.0f)
			{
				sphereRoll = 0.0f;
			}
			break;
		case 'd':
			sphereRoll += 1.0f;
			if (sphereRoll >= 360.0f)
			{
				sphereRoll = 0.0f;
			}
			break;
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		// uninitialise();
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullScreen(void)
{
	// Variable Declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;
	
	// code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
}

int initialise(void)
{
	// Function Declarations
	void resize(int, int);
	void printGLinfo(void);
	void uninitialise(void);
	void SphereVaoVbo(void);
	void CubeVaoVbo(void);
	GLuint compileFragmentShader(GLchar*);
	GLuint compileVertexShader(GLchar*);
	GLuint createShaderProgramObject(int, GLuint, GLuint);
	void getAllUniformLocations(GLuint, GLuint*, GLchar*, GLuint*, GLchar* ,GLuint*, GLchar*);
	GLchar *LoadShader(FILE *, char *);
	BOOL loadGLtexture(GLuint *, TCHAR[]);

	// Variable Declarations

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // Initialisation of pixelformatdescriptor structure :: memset((void *) &pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR))
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // n -> short int
	pfd.nVersion = 1; // conventionally it should be 1.0
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //gimme a pixel that draws on a window | pixel format should support OpenGL | 
	pfd.iPixelType = PFD_TYPE_RGBA; // gimme a pixel that uses RGBA color scheme
	pfd.cColorBits = 32; // 8 bits - green, 8 bits - red, 8 bits - blue, 8 bits - alpha -> 32 bit color bits
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	
	// GetDC

	ghdc = GetDC(ghwnd);

	// Choose Pixel Format

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd); // pfd is an in-out parameter
	if (iPixelFormatIndex == 0)
	{
		return -1;
	}

	// Set the chosen pixel format

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) // 
		return -2;

	// Create OpenGL rendering context

	ghrc = wglCreateContext(ghdc); // wgl -> bridging API for windows
	if (ghrc == NULL)
	{
		return -3;
	}

	// Make the rendering context as current context

	if (wglMakeCurrent(ghdc, ghrc) == FALSE) // make ghrc the current context, remove ghdc from current context
	{
		return -4;
	}

	// GLEW initialisation

	if (glewInit() != GLEW_OK)
	{
		return -5;
	}

	// print OpenGL info

	// printGLinfo();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// vertex shader for SPHERE
	GLuint vertexShaderObject_sphere = compileVertexShader("src\\main\\vertexshader_sphere.vert");

	// fragment shader
	GLuint fragmentShaderObject_sphere = compileFragmentShader("src\\main\\fragmentshader_sphere.frag");

	// shader program object
	shaderProgramObject_sphere = createShaderProgramObject(TEXTURE_NOT_PRESENT, vertexShaderObject_sphere, fragmentShaderObject_sphere);
	
	// Uniforms
	// getAllUniformLocations(shaderProgramObject_sphere, &modelMatrixUniformSphere, "u_modelMatrix_sphere", &viewMatrixUniformSphere, "u_viewMatrix_sphere", &projectionMatrixUniformSphere, "u_projectionMatrix_sphere");
	// fprintf(gpFile, "%d\t%d\t%d\n", modelMatrixUniformSphere, viewMatrixUniformSphere, projectionMatrixUniformSphere);
	modelMatrixUniformSphere = glGetUniformLocation(shaderProgramObject_sphere, "u_modelMatrix_sphere");
	viewMatrixUniformSphere = glGetUniformLocation(shaderProgramObject_sphere, "u_viewMatrix_sphere");
	projectionMatrixUniformSphere = glGetUniformLocation(shaderProgramObject_sphere, "u_projectionMatrix_sphere");
	fprintf(gpFile, "%d\t%d\t%d\n", modelMatrixUniformSphere, viewMatrixUniformSphere, projectionMatrixUniformSphere);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// vertex shader for GROUND
	GLuint vertexShaderObject_ground = compileVertexShader("src\\main\\vertexshader_ground.vert");

	// fragment shader
	GLuint fragmentShaderObject_ground = compileFragmentShader("src\\main\\fragmentshader_ground.frag");

	// shader program object
	shaderProgramObject_ground = createShaderProgramObject(TEXTURE_PRESENT, vertexShaderObject_ground, fragmentShaderObject_ground);
	
	// Uniforms
	// getAllUniformLocations(shaderProgramObject_ground, &modelMatrixUniformGround, "u_modelMatrix_ground", &viewMatrixUniformGround, "u_viewMatrix_ground", &projectionMatrixUniformGround, "u_projectionMatrix_ground");
	// fprintf(gpFile, "%d\t%d\t%d\n", modelMatrixUniformGround, viewMatrixUniformGround, projectionMatrixUniformGround);
	modelMatrixUniformGround = glGetUniformLocation(shaderProgramObject_ground, "u_modelMatrix_ground");
	viewMatrixUniformGround = glGetUniformLocation(shaderProgramObject_ground, "u_viewMatrix_ground");
	projectionMatrixUniformGround = glGetUniformLocation(shaderProgramObject_ground, "u_projectionMatrix_ground");
	textureSamplerUniformGround = glGetUniformLocation(shaderProgramObject_ground, "u_textureSampler");
	fprintf(gpFile, "%d\t%d\t%d\n", textureSamplerUniformGround, viewMatrixUniformGround, projectionMatrixUniformGround);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// declaration of vertex data arrays
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

	SphereVaoVbo();

	CubeVaoVbo();

	if (loadGLtexture(&texture_ground, MAKEINTRESOURCE(IDBITMAP_GROUND))==FALSE)
	{
		fprintf(gpFile, "error loading ground texture\n");

		return -6;
	}
	else
	{
		fprintf(gpFile, "Ground texture loaded\n");
	}

	glEnable(GL_TEXTURE_2D);


	// Here starts openGL code

	// depth and clear color related code

	glClearDepth(1.0f); // ?
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// glShadeModel(GL_SMOOTH);
	// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// loadGLtexture();
	// Clear the screen using Blue Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // this function only tells the program what color to use to clear the screen. The actual clearing DOES NOT happen here

	perspectiveProjectionMatrix = mat4::identity();

	resize(WINWIDTH, WINHEIGHT);

	return 0;
}

void printGLinfo(void)
{
	// local variable declarations
	GLint numExtensions = 0;


	// code
	fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number of supported extensions: %d\n", numExtensions);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            

	for(int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	
}

void resize(int width, int height)
{
	// Code
	if (height == 0) // To avoid possible divide by zero in future code
	{
		height = 1;
	}

	glViewport(0, 0, width, height); // screen cha konta bhaag dakhvu?

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);

}

void display(void)
{
	void push(mat4);
	mat4 pop();
	void drawSphere();
	void karan_code(mat4, mat4, mat4, mat4);
	// Code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Actual coloring and clearing happens here

	// // Use the shader program object
	// glUseProgram(shaderProgramObject);

	// transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	// mat4 modelViewProjectionMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	// vec3 eyeVector = vec3(0.0f, 0.0f, 4.0f);
	// vec3 centerVector = vec3(0.0f, 0.0f, -1.0f);
	// vec3 upVector = vec3(0.0f, 1.0f, 0.0);
	
	viewMatrix = vmath::lookat(eyeVector, eyeVector + centerVector, upVector);

	// drawSphere();

	push(modelMatrix);

		karan_code(translationMatrix, modelMatrix, viewMatrix, rotationMatrix);

	pop();

	// push(modelMatrix);
    // Do Usual Stuff Here Onwards

	// unuse the shader program object
	// glUseProgram(0);
	SwapBuffers(ghdc);

}

void update(void)
{
	// Code
}

void uninitialise(void)
{
	// Function Declarations
	void ToggleFullScreen(void);
	// Code
	if (gbFullscreen)
	{
		ToggleFullScreen();
	}

	// deletion and uninitialisation of vbo
	if (gVbo_sphere_texture)
	{
		glDeleteTextures(1, &gVbo_sphere_texture);
		gVbo_sphere_texture = 0;
	}


	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	// deletion and uninitiaisaion of vao

	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	// shader uninitialisation
	if (shaderProgramObject_sphere)
	{
		glUseProgram(shaderProgramObject_sphere);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;

		// allocate enough memory to this buffer according to the number of attached shaders and fill it with the attached shader objects
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(shaderProgramObject_sphere, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject_sphere, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_sphere);
		shaderProgramObject_sphere = 0;
	}

	if (shaderProgramObject_ground)
	{
		glUseProgram(shaderProgramObject_ground);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject_ground, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;

		// allocate enough memory to this buffer according to the number of attached shaders and fill it with the attached shader objects
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(shaderProgramObject_ground, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject_ground, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_ground);
		shaderProgramObject_ground = 0;
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
		if (ghrc)
		{
			wglDeleteContext(ghrc);
			ghrc = NULL;
		}
	}

	if(ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
	}

	if (gpFile)
	{
		fprintf(gpFile, "Logfile Successfully Closed\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

////////////////////////////////////////////////// Drawing Functions ///////////////////////////////////////////////////////////

void drawSphere(void)
{
	    // *** bind vao ***
    glBindVertexArray(gVao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);
}

void drawCube(void)
{
	glBindVertexArray(gVao_cube);

	// here there be dragons (drawing code)

	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
}

void karan_code(mat4 translationMatrix, mat4 modelMatrix, mat4 viewMatrix, mat4 rotationMatrix)
{
	void push(mat4);
	mat4 pop();
	void drawSphere(void);
	void drawCube(void);

	// Use the shader program object
	glUseProgram(shaderProgramObject_sphere);

	if (sphereJump == 0)
	{
		if (sphereMoveY >= 0.0f)
		{
			sphereMoveY -= 0.01f;
		}
		translationMatrix = vmath::translate(sphereMoveX, sphereMoveY, -6.0f);
		modelMatrix = translationMatrix;

		push(modelMatrix);

		// Provided You Already Had Done Matrices Related Task Up Till Here

		rotationMatrix = vmath::rotate((GLfloat)sphereRoll, 0.0f, 0.0f, 1.0f);
		modelMatrix = pop() * rotationMatrix;

		glUniformMatrix4fv(modelMatrixUniformSphere, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformSphere, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformSphere, 1, GL_FALSE, perspectiveProjectionMatrix);

		drawSphere();

	}
	else
	{
		if (!sphereAtTop)
		{
			sphereMoveY += 0.005f;
			if (sphereMoveY >= 2.0f)
			{
				sphereAtTop = 1;
			}
			translationMatrix = vmath::translate(sphereMoveX, sphereMoveY, -6.0f);
			modelMatrix = translationMatrix;

			push(modelMatrix);

			// Provided You Already Had Done Matrices Related Task Up Till Here

			rotationMatrix = vmath::rotate((GLfloat)sphereRoll, 0.0f, 0.0f, 1.0f);
			modelMatrix = pop() * rotationMatrix;

			glUniformMatrix4fv(modelMatrixUniformSphere, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniformSphere, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniformSphere, 1, GL_FALSE, perspectiveProjectionMatrix);

			drawSphere();
		}
		else
		{
			sphereMoveY -= 0.001f;
			if (sphereMoveY <= 0.0f)
			{
				sphereMoveY = 0;
				sphereAtTop = 0;
				sphereJump = 0;
			}
			translationMatrix = vmath::translate(sphereMoveX, sphereMoveY, -6.0f);
			modelMatrix = translationMatrix;

			push(modelMatrix);

			// Provided You Already Had Done Matrices Related Task Up Till Here

			rotationMatrix = vmath::rotate((GLfloat)sphereRoll, 0.0f, 0.0f, 1.0f);
			modelMatrix = pop() * rotationMatrix;

			glUniformMatrix4fv(modelMatrixUniformSphere, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniformSphere, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniformSphere, 1, GL_FALSE, perspectiveProjectionMatrix);

			drawSphere();
		}
		

	}
	// translationMatrix = vmath::translate(sphereMoveX, sphereMoveY, -6.0f);
	// modelMatrix = translationMatrix;

	// push(modelMatrix);

	// // Provided You Already Had Done Matrices Related Task Up Till Here

	// rotationMatrix = vmath::rotate((GLfloat)sphereRoll, 0.0f, 0.0f, 1.0f);
	// modelMatrix = pop() * rotationMatrix;

	// glUniformMatrix4fv(modelMatrixUniformSphere, 1, GL_FALSE, modelMatrix);
	// glUniformMatrix4fv(viewMatrixUniformSphere, 1, GL_FALSE, viewMatrix);
	// glUniformMatrix4fv(projectionMatrixUniformSphere, 1, GL_FALSE, perspectiveProjectionMatrix);

	// drawSphere();

	glUseProgram(0);

	// push(modelMatrix);

	glUseProgram(shaderProgramObject_ground);

	translationMatrix = vmath::translate(0.0f, -1.0f, -10.0f);
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniformGround, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformGround, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformGround, 1, GL_FALSE, perspectiveProjectionMatrix);

	push(modelMatrix);
	// mat4 scaleMatrix = mat4::identity();
	mat4 scaleMatrix = vmath::scale(40.0f, 0.3f, 5.0f);

	modelMatrix = pop() * scaleMatrix;

	glUniformMatrix4fv(modelMatrixUniformGround, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformGround, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformGround, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ground);
	glUniform1i(textureSamplerUniformGround, 0);

	drawCube();

	glUseProgram(0);

	glUseProgram(shaderProgramObject_ground);

	translationMatrix = vmath::translate(90.0f, -1.0f, -10.0f);
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniformGround, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformGround, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformGround, 1, GL_FALSE, perspectiveProjectionMatrix);

	push(modelMatrix);

	scaleMatrix = vmath::scale(40.0f, 0.3f, 5.0f);

	modelMatrix = pop() * scaleMatrix;

	glUniformMatrix4fv(modelMatrixUniformGround, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformGround, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformGround, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ground);
	glUniform1i(textureSamplerUniformGround, 0);

	drawCube();

	glUseProgram(0);
}

////////////////////////////////////////////////// Object VAO/VBO Functions ////////////////////////////////////////////////////

void SphereVaoVbo(void)
{
	glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);

    // position vbo
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal vbo
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // element vbo
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void CubeVaoVbo(void)
{
	const GLfloat cubePosition[] =
	{
		// top
    	1.0f, 1.0f, -1.0f,
    	-1.0f, 1.0f, -1.0f, 
    	-1.0f, 1.0f, 1.0f,
    	1.0f, 1.0f, 1.0f,

	  	// bottom
        1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,

		 // front
        1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,

		// back
        -1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

		 // right
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,

		 // left
    	-1.0f, 1.0f, 1.0f,
    	-1.0f, 1.0f, -1.0f,
    	-1.0f, -1.0f, -1.0f,
    	-1.0f, -1.0f, 1.0f,
	};

	const GLfloat cubeTexcoords[] =
	{
		0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
	};


	glGenVertexArrays(1, &gVao_cube);
	glBindVertexArray(gVao_cube);
	// vao_pyramid done here

	// vbo for position
	glGenBuffers(1, &gVbo_cube_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_cube_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

////////////////////////////////////////////////// Other "utility" functions ///////////////////////////////////////////////////

BOOL loadGLtexture(GLuint *texture, TCHAR imageResourceID[])
{
	// Variable declarations
	HBITMAP hbitmap=NULL;

	BITMAP bmp;
	BOOL bResult=FALSE;

	// code
	hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP,0, 0, LR_CREATEDIBSECTION);

	if (hbitmap)
	{
		bResult = TRUE;
		GetObject(hbitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); /// for better performance

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//mipmapping

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// create the texture

		// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits); // TARGET, MIPMAPLEVEL, opengl image format, image width, image height, border width, our image format, bytes
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hbitmap);
	}

	return bResult;
}

GLchar *LoadShader(FILE *shaderFile, char *shaderFileName)
{
    long fileLength;

    if (fopen_s(&shaderFile, shaderFileName, "r") != 0)
    {
        fprintf(gpFile, "%s could not be read\n", shaderFileName);
    }
    else
    {
        fprintf(gpFile, "%s read successfully\n", shaderFileName);
    }

    // Get the file length
    fseek(shaderFile, 0, SEEK_END);
    fileLength = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);

    // allocate memory for buffer
    GLchar *shaderSourceCode = (char *)malloc(fileLength + 1);

    if (shaderSourceCode == NULL)
    {
        fprintf(gpFile, "Error allocating memory for vertex shader source code\n");
        fclose(shaderFile);
        return "Error";
    }

    // read the file into the buffer
    fread(shaderSourceCode, fileLength, 1, shaderFile);

    shaderSourceCode[fileLength] = '\0';

    // close the file
    fclose(shaderFile);

    return shaderSourceCode;
    
}

GLuint compileVertexShader(GLchar* path)
{
	// code
	const GLchar *vertexShaderSourceCode = LoadShader(vshaderFile, path);
	
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialise();
			}
		}
	}

	return vertexShaderObject;

}

GLuint compileFragmentShader(GLchar* path)
{
	// code

	const GLchar* fragmentShaderSourceCode = LoadShader(fshaderFile, path);

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	// pre linked binding
	
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
				free(log);
				uninitialise();
			}
		}
	}

	return fragmentShaderObject;
}

GLuint createShaderProgramObject(int textureUsed, GLuint vertexShaderObject, GLuint fragmentShaderObject)
{
	// code

	GLuint shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");

	if (textureUsed == 1)
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");

	glLinkProgram(shaderProgramObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Linking Log: %s\n", log);
				free(log);
				uninitialise();
			}
		}
	}

	return shaderProgramObject;

}

void getAllUniformLocations(GLuint shaderProgramObject, GLuint* modelMatrixUniform, GLchar* modelMatrixUniformName, GLuint* viewMatrixUniform, GLchar* viewMatrixUniformName, GLuint* projectionMatrixUniform, GLchar* projectionMatrixUniformName)
{
	// code
}



////////////////////////////////////////////////// stack related functions /////////////////////////////////////////////////////////

void push(mat4 mostRecentMatrix)
{   
    s.items[++(s.top)] = mostRecentMatrix;
}

mat4 pop()
{
	mat4 retval = s.items[s.top];
    s.items[s.top] = mat4::identity();
    s.top--;

	return retval;
}



