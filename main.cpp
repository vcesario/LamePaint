#include "ui.h"
#include "app.h"
#include "input.h"
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

unsigned int canvasTexId;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(STARTING_WINDOW_W, STARTING_WINDOW_H, "LAMEPAINT", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//glfwSwapInterval(0); // vsync off - useful to understand app performance even if above 60fps
	//glfwSwapInterval(1); // vsync on - needed for my personal notebook's screen, it seems

	LameInput::Init(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	App::Init(STARTING_WINDOW_W, STARTING_WINDOW_H);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LameUI::Init(window);
	ImGuiIO& io = ImGui::GetIO(); // wanted to do this inside InitUI() but I don't know how to properly make it global

	// load icons texture
	GLuint texId = 0;
	int texWidth = 0;
	int texHeight = 0;
	bool ret = LoadTextureFromFile("tool-icons.png", &texId, &texWidth, &texHeight);
	IM_ASSERT(ret);
	TextureObject iconTex(texId, texWidth, texHeight);

	glGenTextures(1, &canvasTexId);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	// set wrapping/filtering options on currently bound texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App::CanvasWidth, App::CanvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &App::data[0]);
	glGenerateMipmap(GL_TEXTURE_2D); // apparently this is mandatory or else the actual textures aren't generated? not sure
	// ***

	// *** setup canvas quad
	Shader canvasShader("canvas.vert", "canvas.frag");
	canvasShader.use();

	float framePercent = (float)App::FrameHeight() / (float)STARTING_WINDOW_H;
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
		App::ProcessInput();

		App::Update();

		App::Render();

		glBindTexture(GL_TEXTURE_2D, canvasTexId); // canvas texture

		// app logic
		vec2int canvasCoord = App::CursorToCanvas(LameInput::CursorX, LameInput::CursorY);
		if (App::GetBrushMode() == BrushModes::BUCKET)
		{
			if (LameInput::IsClicking)
			{
				if (!LameInput::ClickDownConsumed)
				{
					App::PaintFill(LameInput::CursorX, LameInput::CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, App::CanvasWidth, App::CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &App::data[0]);

					LameInput::ClickDownConsumed = true;
				}
			}
		}
		else
		{
			if (LameInput::IsClicking)
			{
				if (LameInput::IsDragging)
				{
					App::PaintRectangle(LameInput::CursorX_LastFrame, LameInput::CursorY_LastFrame, LameInput::CursorX, LameInput::CursorY);
					App::PaintCircle(LameInput::CursorX, LameInput::CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, App::CanvasWidth, App::CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &App::data[0]);
				}
				else if (!LameInput::ClickDownConsumed)
				{
					App::PaintCircle(LameInput::CursorX, LameInput::CursorY);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, App::CanvasWidth, App::CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &App::data[0]);

					LameInput::ClickDownConsumed = true;
				}
			}
		}

		// app render
		glClear(GL_COLOR_BUFFER_BIT);
		canvasShader.use();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// draw cursor
		if (App::GetBrushMode() == BrushModes::BUCKET)
		{
			glBindTexture(GL_TEXTURE_2D, iconTex.id); // bucket texture
			bucketCursorShader.use();
			bucketCursorShader.setVec2("cursorPos", LameInput::CursorX, LameInput::CursorY);
		}
		else
		{
			cursorShader.use();
			cursorShader.setVec2("cursorPos", LameInput::CursorX, LameInput::CursorY);
			cursorShader.setFloat("brushSize", App::GetBrushSize());
			cursorShader.setVec3("brushColor", App::GetBrushColor().x, App::GetBrushColor().y, App::GetBrushColor().z);
		}

		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		LameUI::Render(window, canvasCoord.x, canvasCoord.y, io.Framerate, iconTex);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	App::Terminate();

	// terminate imgui
	LameUI::Terminate();

	glfwTerminate();
	return -1;
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

void BindCanvasTexture()
{
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
}