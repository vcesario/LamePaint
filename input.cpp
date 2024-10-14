#include "input.h"
#include <map>

namespace LameInput
{
	class KeyEventSystem
	{
	public:
		using Callback = std::function<void(int, int)>;

		int Subscribe(const Callback& callback)
		{
			int id = nextId++;

			callbacks[id] = callback;

			return id;
		}

		void Trigger(int key, int action)
		{
			for (const auto& pair : callbacks)
			{
				pair.second(key, action);
			}
		}

	private:
		int nextId = 0;
		std::map<int, Callback> callbacks;
	};

	KeyEventSystem KeyChanged;

	double CursorX;
	double CursorY;
	double CursorX_LastFrame;
	double CursorY_LastFrame;
	bool IsClicking;
	bool IsDragging;
	bool ClickDownConsumed;

	void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos);
	void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods);
	void OnKeyChanged_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void Init(GLFWwindow* window)
	{
		glfwSetCursorPosCallback(window, OnCursorMoved_Callback);
		glfwSetMouseButtonCallback(window, OnMouseClicked_Callback);
		glfwSetKeyCallback(window, OnKeyChanged_Callback);
	}

	int Subscribe_KeyChanged(std::function<void(int, int)> callback)
	{
		int id = KeyChanged.Subscribe(callback);
		return id;
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

		KeyChanged.Trigger(key, action);
	}


	bool IsCursorInsideWindow()
	{
		return true;
	}
}