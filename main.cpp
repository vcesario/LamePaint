#include "ui.h"
#include "app.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include "imgui/imgui.h"

const int STARTING_WINDOW_W = 800;
const int STARTING_WINDOW_H = 600;

double CursorX;
double CursorY;
double CursorX_LastFrame;
double CursorY_LastFrame;
bool IsClicking;
bool IsDragging;
bool ClickDownConsumed;

unsigned int canvasTexId;

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos);
void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods);
void OnKeyChanged_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void HandleCursorVisibility(GLFWwindow* window);

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(STARTING_WINDOW_W, STARTING_WINDOW_H, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//glfwSwapInterval(0); // vsync off - useful to understand app performance even if above 60fps
	//glfwSwapInterval(1); // vsync on - needed for my personal notebook's screen, it seems

	glfwSetCursorPosCallback(window, OnCursorMoved_Callback);
	glfwSetMouseButtonCallback(window, OnMouseClicked_Callback);
	glfwSetKeyCallback(window, OnKeyChanged_Callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	InitUI(window);
	ImGuiIO& io = ImGui::GetIO(); // wanted to do this inside InitUI() but I don't know how to properly make it global

	// load icons texture
	GLuint texId = 0;
	int texWidth = 0;
	int texHeight = 0;
	bool ret = LoadTextureFromFile("tool-icons.png", &texId, &texWidth, &texHeight);
	IM_ASSERT(ret);
	TextureObject iconTex(texId, texWidth, texHeight);

	// *** setup canvas texture
	SetupCanvas(STARTING_WINDOW_W, STARTING_WINDOW_H);

	glGenTextures(1, &canvasTexId);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	// set wrapping/filtering options on currently bound texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//	glfwTerminate();
	//	return -1;
	//}
	//stbi_image_free(data);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CanvasWidth, CanvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D); // apparently this is mandatory or else the actual textures aren't generated? not sure
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, STARTING_W, STARTING_H, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned int fbo;
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//GLuint clearColor[4] = { 0, 0, 0, 0 };

	// ***

	// *** setup canvas quad
	Shader canvasShader("canvas.vert", "canvas.frag");
	canvasShader.use();

	float framePercent = (float)FrameHeight() / (float)STARTING_WINDOW_H;
	float canvasEdgeAnchor = (1 - framePercent) * 2 - 1;
	float vertices[] = {
		-1.0f,  canvasEdgeAnchor, 0.0f, 0.0f, 0.0f, // top left
		 1.0f,  canvasEdgeAnchor, 0.0f, 1.0f, 0.0f, // top right
		 1.0f, -canvasEdgeAnchor, 0.0f, 1.0f, 1.0f, // bottom right
		-1.0f, -canvasEdgeAnchor, 0.0f, 0.0f, 1.0f, // bottom left
	};
	int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// initialize VAO + VBO + EBO
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 1. bind VAO
	glBindVertexArray(VAO);
	// 2. copy vertices[] to VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy indices[] to EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. set VAO pointers
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	// tex coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// ***

	// *** setup cursor quad
	Shader cursorShader("cursor.vert", "cursor.frag");
	cursorShader.use();
	cursorShader.setVec2("windowSize", STARTING_WINDOW_W, STARTING_WINDOW_H);

	Shader bucketCursorShader("bucketCursor.vert", "bucketCursor.frag");
	bucketCursorShader.use();
	bucketCursorShader.setVec2("windowSize", STARTING_WINDOW_W, STARTING_WINDOW_H);

	// colocar vertices do tamanho exato do icone
	// calcular uv pra pegar o pedaço exato da textura
	// deslocar vertices pra casar exatamente a ponta do cursor com a ponta do balde

	float cursorVerts[] = {
		-1.0f,  1.0, 0.0f, 0.0f, 0.0f, // top left
		 1.0f,  1.0, 0.0f, 1.0f, 0.0f, // top right
		 1.0f, -1.0, 0.0f, 1.0f, 1.0f, // bottom right
		-1.0f, -1.0, 0.0f, 0.0f, 1.0f, // bottom left
	};
	int cursorIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	unsigned int VAO2, VBO2, EBO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cursorVerts), cursorVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cursorIndices), cursorIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// ***

	glViewport(0, 0, STARTING_WINDOW_W, STARTING_WINDOW_H);
	glClearColor(0.3f, 0.7f, 0.1f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glBindTexture(GL_TEXTURE_2D, canvasTexId); // canvas texture

		// app logic
		vec2int canvasCoord = CursorToCanvas(CursorX, CursorY);
		if (GetBrushMode() == BrushModes::BUCKET)
		{
			if (IsClicking)
			{
				if (!ClickDownConsumed)
				{
					PaintFill(CursorX, CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

					ClickDownConsumed = true;
				}
			}
		}
		else
		{
			if (IsClicking)
			{
				if (IsDragging)
				{
					PaintRectangle(CursorX_LastFrame, CursorY_LastFrame, CursorX, CursorY);
					PaintCircle(CursorX, CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
				}
				else if (!ClickDownConsumed)
				{
					PaintCircle(CursorX, CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

					ClickDownConsumed = true;
				}
			}
		}

		// app render
		glClear(GL_COLOR_BUFFER_BIT);
		canvasShader.use();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// draw cursor
		if (GetBrushMode() == BrushModes::BUCKET)
		{
			glBindTexture(GL_TEXTURE_2D, iconTex.id); // bucket texture
			bucketCursorShader.use();
			bucketCursorShader.setVec2("cursorPos", CursorX, CursorY);
		}
		else
		{
			cursorShader.use();
			cursorShader.setVec2("cursorPos", CursorX, CursorY);
			cursorShader.setFloat("brushSize", GetBrushSize());
			cursorShader.setVec3("brushColor", GetBrushColor().x, GetBrushColor().y, GetBrushColor().z);
		}

		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		DrawUI(window, canvasCoord.x, canvasCoord.y, io.Framerate, iconTex);

		HandleCursorVisibility(window);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// terminate imgui
	TerminateUI();

	glfwTerminate();
	return -1;
}

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos)
{
	CursorX_LastFrame = CursorX;
	CursorY_LastFrame = CursorY;

	CursorX = xpos;
	CursorY = ypos;

	IsDragging = IsClicking;
	if (IsDragging)
	{
		ClickDownConsumed = true;
	}
}

void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods)
{
	//std::cout << button << ", " << action << ", " << mods << std::endl;
	if (button == 0)
	{
		IsClicking = action;

		if (action == 0)
		{
			ClickDownConsumed = false;
		}
	}
}

void OnKeyChanged_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//std::cout << key << ", " << scancode << ", " << action << ", " << mods << std::endl;

	if (action == 1) // if button down
	{
		switch (key)
		{
		case 88: // X
			if (GetBrushMode() == BrushModes::BUCKET)
			{
				return;
			}

			SwapBrushMode();
			SetUIBrushSlider(GetBrushSize());
			break;
			//case 321: // numpad 1
			//	SetBrushColor(Colors::Black);
			//	break;
			//case 322:
			//	SetBrushColor(Colors::White);
			//	break;
			//case 323:
			//	SetBrushColor(Colors::Red);
			//	break;
			//case 324:
			//	SetBrushColor(Colors::Green);
			//	break;
			//case 325: // numpad 5
			//	SetBrushColor(Colors::Blue);
			//	break;
		}
	}
}

bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}
// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
{
	FILE* f;
	fopen_s(&f, file_name, "rb");

	if (f == NULL)
		return false;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return false;
	fseek(f, 0, SEEK_SET);
	void* file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);
	bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
	IM_FREE(file_data);
	return ret;
}

bool m_CursorOnUi;
void HandleCursorVisibility(GLFWwindow* window)
{
	if (m_CursorOnUi)
	{
		if (!IsCursorHoveringUI())
		{
			m_CursorOnUi = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // disable cursor (not working for some reason??)
			//std::cout << "cursor hidden." << std::endl;
		}
	}
	else
	{
		if (IsCursorHoveringUI())
		{
			m_CursorOnUi = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			//std::cout << "cursor visible!" << std::endl;
		}
	}
}

void BindCanvasTexture()
{
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
}