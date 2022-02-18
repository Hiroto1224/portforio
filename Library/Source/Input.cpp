#include "../Header/Input.h"
#include "../Header/Cursor.h"
#include "../Header/StartingOver.h"
#include <algorithm>

BYTE Input::keyState[256] = { 0x00 };
BYTE Input::keyOldState[256] = { 0x00 };
Vector2 Input::mousePos = { 0.0f,0.0f };
Vector2 Input::mouseOldPos = { 0.0f,0.0f };
float Input::mouseWheel = 0.0f;

void Input::Update()
{
	RECT rect;
	GetWindowRect(StartingOver::Window::hWnd, &rect);
	
	if (Cursor::lockState == CursorLockMode::Locked)
	{
		mouseOldPos = {
			Cursor::lockPos.x,
			Cursor::lockPos.y
		};
	}else
	{
		mouseOldPos = mousePos;
	}

	POINT mouse_Pos;
	GetCursorPos(&mouse_Pos);
	mousePos = {
		static_cast<float>(mouse_Pos.x - 0.5f),
		static_cast<float>(mouse_Pos.y - 20.0f)
	};

	memcpy(keyOldState, keyState, sizeof(keyState));
	if (!GetKeyboardState(keyState))
	{
		assert(false);
	}
}


bool Input::GetKey(KeyCode _key)
{
	return (keyState[static_cast<int>(_key)] & 0x80);
}

bool Input::GetKeyDown(KeyCode _key)
{
	return (keyState[static_cast<int>(_key)] & 0x80) && 
		!(keyOldState[static_cast<int>(_key)] & 0x80);
}


std::string Input::GetALLKeyDown()
{
	int down = 0;
	for (int i = 48; i < 89; i++)
	{
		if (GetKeyDown(static_cast<KeyCode>(i)))
		{
			down = i;
			break;
		}
	}

	std::string key;
	switch (down)
	{
	case 48:
		key = "0";
		break;
	case 49:
		key = "1";
		break;
	case 50:
		key = "2";
		break;
	case 51:
		key = "3";
		break;
	case 52:
		key = "4";
		break;
	case 53:
		key = "5";
		break;
	case 54:
		key = "6";
		break;
	case 55:
		key = "7";
		break;
	case 56:
		key = "8";
		break;
	case 57:
		key = "9";
		break;
	case 65:
		key = "a";
		break;
	case 66:
		key = "b";
		break;
	case 67:
		key = "c";
		break;
	case 68:
		key = "d";
		break;
	case 69:
		key = "e";
		break;
	case 70:
		key = "f";
		break;
	case 71:
		key = "g";
		break;
	case 72:
		key = "h";
		break;
	case 73:
		key = "i";
		break;
	case 74:
		key = "j";
		break;
	case 75:
		key = "k";
		break;
	case 76:
		key = "l";
		break;
	case 77:
		key = "m";
		break;
	case 78:
		key = "n";
		break;
	case 79:
		key = "o";
		break;
	case 80:
		key = "p";
		break;
	case 81:
		key = "q";
		break;
	case 82:
		key = "r";
		break;
	case 83:
		key = "s";
		break;
	case 84:
		key = "t";
		break;
	case 85:
		key = "u";
		break;
	case 86:
		key = "v";
		break;
	case 87:
		key = "w";
		break;
	case 88:
		key = "x";
		break;
	case 89:
		key = "y";
		break;
	case 90:
		key = "z";
		break;
	
	default:
		key = "";
		break;
	}

	if ((GetKey(KeyCode::RightShift) || GetKey(KeyCode::LeftShift)))
	{
		std::transform(key.cbegin(), key.cend(), key.begin(), toupper);
	}
	if (GetKeyDown(KeyCode::Backspace))
	{
		key = "BackSpace";
	}
	return key;

}



bool Input::GetKeyUp(KeyCode _key)
{
	return !(keyState[static_cast<int>(_key)] & 0x80) &&
		(keyOldState[static_cast<int>(_key)] & 0x80);
}

bool Input::GetMouseButton(int button)
{
	switch (button)
	{
	case 0:
		return (keyState[static_cast<int>(KeyCode::Mouse0)] & 0x80);
		break;
	case 1:
		return (keyState[static_cast<int>(KeyCode::Mouse1)] & 0x80);
		break;
	case 2:
		return (keyState[static_cast<int>(KeyCode::Mouse2)] & 0x80);
		break;
	default:
		break;
	}
	return false;
}

bool Input::GetMouseButtonDown(int button)
{
	switch (button)
	{
	case 0:
		return (keyState[static_cast<int>(KeyCode::Mouse0)] & 0x80) && 
			!(keyOldState[static_cast<int>(KeyCode::Mouse0)] & 0x80);
		break;
	case 1:
		return (keyState[static_cast<int>(KeyCode::Mouse1)] & 0x80) && 
			!(keyOldState[static_cast<int>(KeyCode::Mouse1)] & 0x80);
		break;
	case 2:
		return (keyState[static_cast<int>(KeyCode::Mouse2)] & 0x80) && 
			!(keyOldState[static_cast<int>(KeyCode::Mouse2)] & 0x80);
		break;
	default:
		break;
	}
	return false;
}

bool Input::GetMouseButtonUp(int button)
{
	switch (button)
	{
	case 0:
		return !(keyState[static_cast<int>(KeyCode::Mouse0)] & 0x80) && 
			(keyOldState[static_cast<int>(KeyCode::Mouse0)] & 0x80);
		break;
	case 1:
		return !(keyState[static_cast<int>(KeyCode::Mouse1)] & 0x80) &&
			(keyOldState[static_cast<int>(KeyCode::Mouse1)] & 0x80);
		break;
	case 2:
		return !(keyState[static_cast<int>(KeyCode::Mouse2)] & 0x80) && 
			(keyOldState[static_cast<int>(KeyCode::Mouse2)] & 0x80);
		break;
	default:
		break;
	}
	return false;
}

Vector2 Input::GetMousePos()
{
	return mousePos;
}

Vector2 Input::GetMouseRelativePos()
{
	return mousePos - mouseOldPos;
}

float Input::GetMouseWheel()
{
	return mouseWheel;
}

void Input::SetMouseWheel(const float wheel)
{
	mouseWheel = wheel;
}
