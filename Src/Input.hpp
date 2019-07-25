#pragma once

#include <atomic>
#include <bitset>
#include <gsl/gsl>
#include <glm/glm.hpp>

#include "Utils.hpp"
#include "API.hpp"

namespace jm
{
	enum class Button
	{
		Unknown,
		MouseLeft,
		MouseRight,
		MouseMiddle,
		MouseSide1,
		MouseSide2,
		CtrlrA,
		CtrlrB,
		CtrlrX,
		CtrlrY,
		CtrlrBack,
		CtrlrGuide,
		CtrlrStart,
		CtrlrLeftStick,
		CtrlrRightStick,
		CtrlrLeftShoulder,
		CtrlrRightShoulder,
		CtrlrDPadUp,
		CtrlrDPadDown,
		CtrlrDPadLeft,
		CtrlrDPadRight,
		LeftShift,
		RightShift,
		LeftControl,
		RightControl,
		LeftAlt,
		RightAlt,
		Escape,
		Enter,
		Space,
		Tab,
		Backspace,
		LeftArrow,
		UpArrow,
		RightArrow,
		DownArrow,
		Grave,
		PageUp,
		PageDown,
		Home,
		End,
		Delete,
		D0,
		D1,
		D2,
		D3,
		D4,
		D5,
		D6,
		D7,
		D8,
		D9,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		NUM_BUTTONS
	};
	
	enum class ControllerAxis
	{
		LeftX,
		LeftY,
		RightX,
		RightY,
		LeftTrigger,
		RightTrigger,
	};
	
	struct GameController
	{
		const char* name;
		void* _data;
	};
	
	JAPI gsl::span<const GameController> GameControllers();
	
	JAPI std::string_view ButtonToString(Button button);
	JAPI Button ButtonFromString(std::string_view str);
	JAPI std::string_view ButtonDisplayName(Button button);
	
	constexpr size_t NUM_BUTTONS = (size_t)Button::NUM_BUTTONS;
	
	class InputState;
	
	namespace detail
	{
		JAPI extern InputState* currentIS;
		JAPI extern InputState* previousIS;
		JAPI extern std::string inputtedText;
	}
	
	class JAPI InputState
	{
	public:
		InputState() = default;
		
		Button PressedButton() const
		{
			return m_pressed;
		}
		
		bool IsButtonDown(Button button) const
		{
			return (m_isButtonDown[(int)button / 8] & (1 << ((int)button % 8))) != 0;
		}
		
		bool IsCtrlDown() const
		{
			return IsButtonDown(Button::LeftControl) || IsButtonDown(Button::RightControl);
		}
		
		bool IsShiftDown() const
		{
			return IsButtonDown(Button::LeftShift) || IsButtonDown(Button::RightShift);
		}
		
		bool IsAltDown() const
		{
			return IsButtonDown(Button::LeftAlt) || IsButtonDown(Button::RightAlt);
		}
		
		void OnButtonDown(Button button)
		{
			m_isButtonDown[(int)button / 8] |= 1U << ((int)button % 8);
			m_pressed = button;
		}
		
		void OnButtonUp(Button button)
		{
			if (button == m_pressed)
				m_pressed = Button::Unknown;
			m_isButtonDown[(int)button / 8] &= ~(1U << ((uint32_t)button % 8));
		}
		
		void OnAxisMoved(ControllerAxis axis, float newValue)
		{
			m_axisValues[(int)axis] = newValue;
		}
		
		glm::ivec2 CursorPos() const
		{
			return { cursorX, cursorY };
		}
		
		glm::ivec2 CursorPosDelta() const
		{
			return { cursorDeltaX, cursorDeltaY };
		}
		
		glm::ivec2 ScrollPos() const
		{
			return { scrollX, scrollY };
		}
		
		float AxisValue(ControllerAxis axis)
		{
			return m_axisValues[(int)axis];
		}
		
		glm::vec2 LeftAnalogValue() const
		{
			return { m_axisValues[0], m_axisValues[1] };
		}
		
		glm::vec2 RightAnalogValue() const
		{
			return { m_axisValues[2], m_axisValues[3] };
		}
		
		static const InputState& Current()
		{
			return *detail::currentIS;
		}
		
		static const InputState& Previous()
		{
			return *detail::previousIS;
		}
		
		int32_t scrollX = 0;
		int32_t scrollY = 0;
		int32_t cursorX = 0;
		int32_t cursorY = 0;
		int32_t cursorDeltaX = 0;
		int32_t cursorDeltaY = 0;
		
	private:
		Button m_pressed = Button::Unknown;
		uint8_t m_isButtonDown[13] = { };
		float m_axisValues[6] = { };
		
		static_assert(sizeof(m_isButtonDown) * 8 >= NUM_BUTTONS);
	};
	
	JAPI void SetRelativeMouseMode(bool relMouseMode);
	JAPI bool RelativeMouseModeActive();
	
	inline const std::string& InputtedText()
	{
		return detail::inputtedText;
	}
	
	inline Button PressedButton()
	{
		return detail::currentIS->PressedButton();
	}
	
	inline bool IsButtonDown(Button button)
	{
		return detail::currentIS->IsButtonDown(button);
	}
	
	inline bool WasButtonDown(Button button)
	{
		return detail::previousIS->IsButtonDown(button);
	}
	
	inline glm::ivec2 CursorPos()
	{
		return detail::currentIS->CursorPos();
	}
	
	inline int32_t CursorX()
	{
		return detail::currentIS->cursorX;
	}
	
	inline int32_t CursorY()
	{
		return detail::currentIS->cursorY;
	}
	
	inline int32_t CursorDeltaX()
	{
		return detail::currentIS->cursorDeltaX;
	}
	
	inline int32_t CursorDeltaY()
	{
		return detail::currentIS->cursorDeltaY;
	}
	
	inline glm::ivec2 CursorPosDelta()
	{
		return detail::currentIS->CursorPosDelta();
	}
	
	inline glm::ivec2 PrevCursorPos()
	{
		return detail::previousIS->CursorPos();
	}
	
	inline int32_t PrevCursorX()
	{
		return detail::previousIS->cursorX;
	}
	
	inline int32_t PrevCursorY()
	{
		return detail::previousIS->cursorY;
	}
	
	inline float AxisValue(ControllerAxis axis)
	{
		return detail::currentIS->AxisValue(axis);
	}
	
	inline glm::vec2 LeftAnalogValue()
	{
		return detail::currentIS->LeftAnalogValue();
	}
	
	inline glm::vec2 RightAnalogValue()
	{
		return detail::currentIS->RightAnalogValue();
	}
	
	inline float PrevAxisValue(ControllerAxis axis)
	{
		return detail::previousIS->AxisValue(axis);
	}
	
	inline glm::vec2 PrevLeftAnalogValue()
	{
		return detail::previousIS->LeftAnalogValue();
	}
	
	inline glm::vec2 PrevRightAnalogValue()
	{
		return detail::previousIS->RightAnalogValue();
	}
}
