#include "Input.hpp"
#include "Utils.hpp"

#include <SDL.h>

namespace jm
{
	InputState* detail::currentIS;
	InputState* detail::previousIS;
	
	std::string detail::inputtedText;
	
	std::string_view ButtonDisplayName(Button button)
	{
		switch (button)
		{
		case Button::MouseLeft:
			return "Mouse Left";
		case Button::MouseRight:
			return "Mouse Right";
		case Button::MouseMiddle:
			return "Mouse Middle";
		case Button::MouseSide1:
			return "Mouse Side 1";
		case Button::MouseSide2:
			return "Mouse Side 2";
		case Button::CtrlrA:
			return "A";
		case Button::CtrlrB:
			return "B";
		case Button::CtrlrX:
			return "X";
		case Button::CtrlrY:
			return "Y";
		case Button::CtrlrBack:
			return "Back";
		case Button::CtrlrGuide:
			return "Guide";
		case Button::CtrlrStart:
			return "Start";
		case Button::CtrlrLeftStick:
			return "Left Stick";
		case Button::CtrlrRightStick:
			return "Right Stick";
		case Button::CtrlrLeftShoulder:
			return "Left Shoulder";
		case Button::CtrlrRightShoulder:
			return "Right Shoulder";
		case Button::CtrlrDPadUp:
			return "DPad Up";
		case Button::CtrlrDPadDown:
			return "DPad Down";
		case Button::CtrlrDPadLeft:
			return "DPad Left";
		case Button::CtrlrDPadRight:
			return "DPad Right";
		case Button::LeftShift:
			return "Left Shift";
		case Button::RightShift:
			return "Right Shift";
		case Button::LeftControl:
			return "Left Control";
		case Button::RightControl:
			return "Right Control";
		case Button::LeftAlt:
			return "Left Alt";
		case Button::RightAlt:
			return "Right Alt";
		case Button::Escape:
			return "Escape";
		case Button::Enter:
			return "Enter";
		case Button::Space:
			return "Space";
		case Button::Tab:
			return "Tab";
		case Button::Backspace:
			return "Backspace";
		case Button::LeftArrow:
			return "Left Arrow Key";
		case Button::UpArrow:
			return "Up Arrow Key";
		case Button::RightArrow:
			return "Right Arrow Key";
		case Button::DownArrow:
			return "Down Arrow Key";
		case Button::Grave:
			return "Grave";
		case Button::PageUp:
			return "Page Up";
		case Button::PageDown:
			return "Page Down";
		case Button::Home:
			return "Home";
		case Button::End:
			return "End";
		case Button::Delete:
			return "Delete";
		case Button::D0:
			return "0";
		case Button::D1:
			return "1";
		case Button::D2:
			return "2";
		case Button::D3:
			return "3";
		case Button::D4:
			return "4";
		case Button::D5:
			return "5";
		case Button::D6:
			return "6";
		case Button::D7:
			return "7";
		case Button::D8:
			return "8";
		case Button::D9:
			return "9";
		case Button::A:
			return "A";
		case Button::B:
			return "B";
		case Button::C:
			return "C";
		case Button::D:
			return "D";
		case Button::E:
			return "E";
		case Button::F:
			return "F";
		case Button::G:
			return "G";
		case Button::H:
			return "H";
		case Button::I:
			return "I";
		case Button::J:
			return "J";
		case Button::K:
			return "K";
		case Button::L:
			return "L";
		case Button::M:
			return "M";
		case Button::N:
			return "N";
		case Button::O:
			return "O";
		case Button::P:
			return "P";
		case Button::Q:
			return "Q";
		case Button::R:
			return "R";
		case Button::S:
			return "S";
		case Button::T:
			return "T";
		case Button::U:
			return "U";
		case Button::V:
			return "V";
		case Button::W:
			return "W";
		case Button::X:
			return "X";
		case Button::Y:
			return "Y";
		case Button::Z:
			return "Z";
		case Button::F1:
			return "F1";
		case Button::F2:
			return "F2";
		case Button::F3:
			return "F3";
		case Button::F4:
			return "F4";
		case Button::F5:
			return "F5";
		case Button::F6:
			return "F6";
		case Button::F7:
			return "F7";
		case Button::F8:
			return "F8";
		case Button::F9:
			return "F9";
		case Button::F10:
			return "F10";
		case Button::F11:
			return "F11";
		case Button::F12:
			return "F12";
		case Button::F13:
			return "F13";
		case Button::F14:
			return "F14";
		case Button::F15:
			return "F15";
		case Button::F16:
			return "F16";
		case Button::F17:
			return "F17";
		case Button::F18:
			return "F18";
		case Button::F19:
			return "F19";
		case Button::F20:
			return "F20";
		case Button::F21:
			return "F21";
		case Button::F22:
			return "F22";
		case Button::F23:
			return "F23";
		case Button::F24:
			return "F24";
		default:
			return "Unknown";
		}
	}
	
	static std::string_view buttonNames[] =
	{
		"Unknown",
		"MouseLeft",
		"MouseRight",
		"MouseMiddle",
		"MouseSide1",
		"MouseSide2",
		"ControllerA",
		"ControllerB",
		"ControllerX",
		"ControllerY",
		"ControllerBack",
		"ControllerGuide",
		"ControllerStart",
		"ControllerLeftStick",
		"ControllerRightStick",
		"ControllerLeftShoulder",
		"ControllerRightShoulder",
		"ControllerDPadUp",
		"ControllerDPadDown",
		"ControllerDPadLeft",
		"ControllerDPadRight",
		"LeftShift",
		"RightShift",
		"LeftControl",
		"RightControl",
		"LeftAlt",
		"RightAlt",
		"Escape",
		"Enter",
		"Space",
		"Tab",
		"Backspace",
		"LeftArrow",
		"UpArrow",
		"RightArrow",
		"DownArrow",
		"Grave",
		"PageUp",
		"PageDown",
		"Home",
		"End",
		"Delete",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24"
	};
	
	Button TranslateSDLKey(SDL_Scancode scancode)
	{
		switch (scancode)
		{
		case SDL_SCANCODE_LSHIFT: return Button::LeftShift;
		case SDL_SCANCODE_RSHIFT: return Button::RightShift;
		case SDL_SCANCODE_LCTRL: return Button::LeftControl;
		case SDL_SCANCODE_RCTRL: return Button::RightControl;
		case SDL_SCANCODE_LALT: return Button::LeftAlt;
		case SDL_SCANCODE_RALT: return Button::RightAlt;
		case SDL_SCANCODE_ESCAPE: return Button::Escape;
		case SDL_SCANCODE_BACKSPACE: return Button::Backspace;
		case SDL_SCANCODE_RETURN: return Button::Enter;
		case SDL_SCANCODE_TAB: return Button::Tab;
		case SDL_SCANCODE_SPACE: return Button::Space;
		case SDL_SCANCODE_LEFT: return Button::LeftArrow;
		case SDL_SCANCODE_UP: return Button::UpArrow;
		case SDL_SCANCODE_RIGHT: return Button::RightArrow;
		case SDL_SCANCODE_DOWN: return Button::DownArrow;
		case SDL_SCANCODE_GRAVE: return Button::Grave;
		case SDL_SCANCODE_PAGEUP: return Button::PageUp;
		case SDL_SCANCODE_PAGEDOWN: return Button::PageDown;
		case SDL_SCANCODE_HOME: return Button::Home;
		case SDL_SCANCODE_END: return Button::End;
		case SDL_SCANCODE_DELETE: return Button::Delete;
		case SDL_SCANCODE_0: return Button::D0;
		case SDL_SCANCODE_1: return Button::D1;
		case SDL_SCANCODE_2: return Button::D2;
		case SDL_SCANCODE_3: return Button::D3;
		case SDL_SCANCODE_4: return Button::D4;
		case SDL_SCANCODE_5: return Button::D5;
		case SDL_SCANCODE_6: return Button::D6;
		case SDL_SCANCODE_7: return Button::D7;
		case SDL_SCANCODE_8: return Button::D8;
		case SDL_SCANCODE_9: return Button::D9;
		case SDL_SCANCODE_A: return Button::A;
		case SDL_SCANCODE_B: return Button::B;
		case SDL_SCANCODE_C: return Button::C;
		case SDL_SCANCODE_D: return Button::D;
		case SDL_SCANCODE_E: return Button::E;
		case SDL_SCANCODE_F: return Button::F;
		case SDL_SCANCODE_G: return Button::G;
		case SDL_SCANCODE_H: return Button::H;
		case SDL_SCANCODE_I: return Button::I;
		case SDL_SCANCODE_J: return Button::J;
		case SDL_SCANCODE_K: return Button::K;
		case SDL_SCANCODE_L: return Button::L;
		case SDL_SCANCODE_M: return Button::M;
		case SDL_SCANCODE_N: return Button::N;
		case SDL_SCANCODE_O: return Button::O;
		case SDL_SCANCODE_P: return Button::P;
		case SDL_SCANCODE_Q: return Button::Q;
		case SDL_SCANCODE_R: return Button::R;
		case SDL_SCANCODE_S: return Button::S;
		case SDL_SCANCODE_T: return Button::T;
		case SDL_SCANCODE_U: return Button::U;
		case SDL_SCANCODE_V: return Button::V;
		case SDL_SCANCODE_W: return Button::W;
		case SDL_SCANCODE_X: return Button::X;
		case SDL_SCANCODE_Y: return Button::Y;
		case SDL_SCANCODE_Z: return Button::Z;
		case SDL_SCANCODE_F1: return Button::F1;
		case SDL_SCANCODE_F2: return Button::F2;
		case SDL_SCANCODE_F3: return Button::F3;
		case SDL_SCANCODE_F4: return Button::F4;
		case SDL_SCANCODE_F5: return Button::F5;
		case SDL_SCANCODE_F6: return Button::F6;
		case SDL_SCANCODE_F7: return Button::F7;
		case SDL_SCANCODE_F8: return Button::F8;
		case SDL_SCANCODE_F9: return Button::F9;
		case SDL_SCANCODE_F10: return Button::F10;
		case SDL_SCANCODE_F11: return Button::F11;
		case SDL_SCANCODE_F12: return Button::F12;
		case SDL_SCANCODE_F13: return Button::F13;
		case SDL_SCANCODE_F14: return Button::F14;
		case SDL_SCANCODE_F15: return Button::F15;
		case SDL_SCANCODE_F16: return Button::F16;
		case SDL_SCANCODE_F17: return Button::F17;
		case SDL_SCANCODE_F18: return Button::F18;
		case SDL_SCANCODE_F19: return Button::F19;
		case SDL_SCANCODE_F20: return Button::F20;
		case SDL_SCANCODE_F21: return Button::F21;
		case SDL_SCANCODE_F22: return Button::F22;
		case SDL_SCANCODE_F23: return Button::F23;
		case SDL_SCANCODE_F24: return Button::F24;
		default: return Button::Unknown;
		}
	}
	
	Button TranslateSDLControllerButton(int button)
	{
		switch (button)
		{
		case SDL_CONTROLLER_BUTTON_A: return Button::CtrlrA;
		case SDL_CONTROLLER_BUTTON_B: return Button::CtrlrB;
		case SDL_CONTROLLER_BUTTON_X: return Button::CtrlrX;
		case SDL_CONTROLLER_BUTTON_Y: return Button::CtrlrY;
		case SDL_CONTROLLER_BUTTON_BACK: return Button::CtrlrBack;
		case SDL_CONTROLLER_BUTTON_GUIDE: return Button::CtrlrGuide;
		case SDL_CONTROLLER_BUTTON_START: return Button::CtrlrStart;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Button::CtrlrLeftStick;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Button::CtrlrRightStick;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return Button::CtrlrLeftShoulder;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Button::CtrlrRightShoulder;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return Button::CtrlrDPadUp;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return Button::CtrlrDPadDown;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return Button::CtrlrDPadLeft;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return Button::CtrlrDPadRight;
		default: return Button::Unknown;
		}
	}
	
	Button TranslateSDLMouseButton(int button)
	{
		switch (button)
		{
		case SDL_BUTTON_LEFT: return Button::MouseLeft;
		case SDL_BUTTON_RIGHT: return Button::MouseRight;
		case SDL_BUTTON_MIDDLE: return Button::MouseMiddle;
		case SDL_BUTTON_X1: return Button::MouseSide1;
		case SDL_BUTTON_X2: return Button::MouseSide2;
		default: return Button::Unknown;
		}
	}
	
	std::vector<GameController> controllers;
	SDL_GameController* activeController = nullptr;
	
	void AddGameController(SDL_GameController* controller)
	{
		controllers.push_back({ SDL_GameControllerName(controller), controller });
		if (activeController == nullptr)
		{
			std::cout << "Using game controller: " << controllers.back().name << std::endl;
			activeController = controller;
		}
	}
	
	void LoadGameControllers()
	{
		SDL_GameControllerEventState(SDL_ENABLE);
		SDL_GameControllerUpdate();
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_JoystickUpdate();
		
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			if (!SDL_IsGameController(i))
			{
				std::cerr << "Joystick '" << SDL_JoystickNameForIndex(i) << "' is not a game controller" << std::endl;
				continue;
			}
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller == nullptr)
			{
				std::cerr << "Could not open game controller " << i << ": " << SDL_GetError() << std::endl;
				continue;
			}
			AddGameController(controller);
		}
	}
	
	Button ButtonFromString(std::string_view str)
	{
		for (size_t i = 0; i < std::size(buttonNames); i++)
		{
			if (StringEqualCaseInsensitive(buttonNames[i], str))
				return (Button)i;
		}
		return Button::Unknown;
	}
	
	std::string_view ButtonToString(Button button)
	{
		if ((size_t)button >= std::size(buttonNames))
			return buttonNames[0];
		return buttonNames[(int)button];
	}
	
	static bool g_relMouseMode = false;
	
	void SetRelativeMouseMode(bool relMouseMode)
	{
		g_relMouseMode = relMouseMode;
		SDL_SetRelativeMouseMode((SDL_bool)relMouseMode);
	}
	
	bool RelativeMouseModeActive()
	{
		return g_relMouseMode;
	}
	
	float AxisButtonValue(std::initializer_list<Button> btnNeg, std::initializer_list<Button> btnPos,
		ControllerAxis axis, const InputState& inputState)
	{
		if (std::any_of(btnNeg.begin(), btnNeg.end(), [&] (Button btn) { return inputState.IsButtonDown(btn); }))
			return -1;
		if (std::any_of(btnPos.begin(), btnPos.end(), [&] (Button btn) { return inputState.IsButtonDown(btn); }))
			return 1;
		return inputState.AxisValue(axis);
	}
	
	struct InputBinding
	{
		std::string name;
		std::vector<Button> buttons;
		
		bool operator<(std::string_view o) const
		{
			return name < o;
		}
	};
	static std::vector<InputBinding> inputBindings;
	
	void SetInputBinding(std::string_view name, std::vector<Button> buttons)
	{
		auto it = std::lower_bound(inputBindings.begin(), inputBindings.end(), name);
		if (it != inputBindings.end() && it->name == name)
		{
			it->buttons = std::move(buttons);
		}
		else
		{
			inputBindings.insert(it, InputBinding { std::string(name), std::move(buttons) });
		}
	}
	
	static std::vector<std::string> warnedMissingBindings;
	
	bool IsBindingDown(std::string_view bindingName, const InputState& inputState)
	{
		auto it = std::lower_bound(inputBindings.begin(), inputBindings.end(), bindingName);
		if (it == inputBindings.end() || it->name != bindingName)
		{
			if (!Contains(warnedMissingBindings, bindingName))
			{
				std::cerr << "Input binding not initialized: '" << bindingName << "'.\n";
				warnedMissingBindings.emplace_back(bindingName);
			}
			return false;
		}
		
		return std::any_of(it->buttons.begin(), it->buttons.end(), [&] (Button btn)
		{
			return inputState.IsButtonDown(btn);
		});
	}
}
