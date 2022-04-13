#include "Input.h"

Input::Input()
{
	// set default values
	mouse.left = MouseState::UP;
	mouse.right = MouseState::UP;
	mouse.x = 0;
	mouse.y = 0;
}

void Input::setKeyDown(int key)
{
	if (key >= 0)
	{
		keys[key] = true;
	}
}

void Input::setKeyUp(int key)
{
	if (key >= 0)
	{
		keys[key] = false;
	}
}

bool Input::isKeyDown(int key)
{
	if (key >= 0)
	{
		return keys[key];
	}
	return false;
}

bool Input::isPressed(int key)
{
	bool cond = isKeyDown(key);
	if (cond)
	{
		pressed.push_back(key);
		return cond;
	}
	return false;
}

void Input::update()
{
	for (int i = 0; i < pressed.size(); i++)
	{
		setKeyUp(pressed[i]);
	}
	pressed.clear();

	if (mouse.left == MouseState::PRESSED)
	{
		mouse.left = MouseState::UP;
	}
	if (mouse.right == MouseState::PRESSED)
	{
		mouse.right = MouseState::UP;
	}
}

void Input::setMouseX(int lx)
{
	mouse.x = lx;
}

void Input::setMouseY(int ly)
{
	mouse.y = ly;
}

void Input::setMousePosition(int lx, int ly)
{
	setMouseX(lx);
	setMouseY(ly);
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input::getMouseY()
{
	return mouse.y;
}

void Input::setLeftMouse(MouseState state)
{
	mouse.left = state;
}
bool Input::isLeftMouseDown()
{
	if (mouse.left == MouseState::DOWN || mouse.left == MouseState::PRESSED)
	{
		return true;
	}
	return false;
}

bool Input::isLeftMousePressed()
{
	if (mouse.left == MouseState::DOWN || mouse.left == MouseState::PRESSED)
	{
		mouse.left = MouseState::PRESSED;
		return true;
	}
	return false;
}

void Input::setRightMouse(MouseState state)
{
	mouse.right = state;
}
bool Input::isRightMouseDown()
{
	if (mouse.right == MouseState::DOWN || mouse.right == MouseState::PRESSED)
	{
		return true;
	}
	return false;
}

bool Input::isRightMousePressed()
{
	if (mouse.right == MouseState::DOWN || mouse.right == MouseState::PRESSED)
	{
		mouse.right = MouseState::PRESSED;
		return true;
	}
	return false;
}

