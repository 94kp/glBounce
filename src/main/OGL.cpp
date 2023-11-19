// Header files
#include <windows.h>
#include "..\lib\OGL.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// OpenGL Header Files
#include <GL/glew.h>	// This must be above GL.h
#include <GL/gl.h>

#include "..\lib\karan.hpp"
// using namespace vmath; // only works in cpp


#define WINWIDTH 800
#define WINHEIGHT 600
#define MAX_STACK_SIZE 32

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
GLuint shaderProgramObject;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
	// VDG_ATTRIBUTE_NORMAL,
	// VDG_ATTRIBUTE_VERTEX
};

GLuint gVao_sphere;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_texture;

GLuint gVao_cube;
GLuint gVbo_cube_position;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;
GLuint texture2DSampler;

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

mat4 perspectiveProjectionMatrix;

// sphere properties
float sphereRoll = 0.0f;

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
		fprintf(gpFile, "Success");
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
			sphereRoll -= 1.0f;
			if (sphereRoll <= -360.0f)
			{
				sphereRoll = 0.0f;
			}
			break;
		case VK_LEFT:
			sphereRoll += 1.0f;
			if (sphereRoll >= 360.0f)
			{
				sphereRoll = 0.0f;
			}
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
	GLchar *LoadShader(FILE *, char *);

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

	// vertex shader - check where to put this
	const GLchar *vertexShaderSourceCode = LoadShader(vshaderFile, "src\\main\\vertexshader.vert");
	
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

	// fragment shader

	const GLchar* fragmentShaderSourceCode = LoadShader(fshaderFile, "src\\main\\fragmentshader.frag");

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	// pre linked binding
	
	status = 0;
	infoLogLength = 0;
	log = NULL;

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

	// shader program object
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

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

	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");

	// declaration of vertex data arrays
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

	SphereVaoVbo();
	CubeVaoVbo();

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

	// Use the shader program object
	glUseProgram(shaderProgramObject);

	// transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	// mat4 modelViewProjectionMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	// drawSphere();

	push(modelMatrix);

		karan_code(translationMatrix, modelMatrix, viewMatrix, rotationMatrix);

	pop();

	// push(modelMatrix);
    // Do Usual Stuff Here Onwards

	// unuse the shader program object
	glUseProgram(0);
	SwapBuffers(ghdc);

}

void update(void)
{
	// Code
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
	glBindVertexArray(0);
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
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;

		// allocate enough memory to this buffer according to the number of attached shaders and fill it with the attached shader objects
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
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

// stack related functions

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



