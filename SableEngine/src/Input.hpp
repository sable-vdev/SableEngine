#pragma once

class Input
{
private:
	bool m_keys[256];
public:
	Input();
	Input(const Input& other);
	~Input();
	void Initialize();
	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	bool IsKeyDown(unsigned int key);
};