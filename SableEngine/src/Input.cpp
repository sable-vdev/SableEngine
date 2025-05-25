#include "Input.hpp"

Input::Input()
{
	Initialize();
}

Input::Input(const Input& other)
{
	Initialize();
}

Input::~Input()
{
}

void Input::Initialize()
{
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void Input::KeyDown(unsigned int key)
{
	//If key pressed down set state to pressed
	m_keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
	//If key released clear the state of it
	m_keys[key] = false;
}

bool Input::IsKeyDown(unsigned int key)
{
	//Return in what state the key is
	return m_keys[key];
}
