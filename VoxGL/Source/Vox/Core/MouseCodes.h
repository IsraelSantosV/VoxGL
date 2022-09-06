#pragma once

namespace Vox
{
	typedef enum class MouseCode : uint16_t
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define IN_MOUSE_BUTTON_0      ::Vox::Mouse::Button0
#define IN_MOUSE_BUTTON_1      ::Vox::Mouse::Button1
#define IN_MOUSE_BUTTON_2      ::Vox::Mouse::Button2
#define IN_MOUSE_BUTTON_3      ::Vox::Mouse::Button3
#define IN_MOUSE_BUTTON_4      ::Vox::Mouse::Button4
#define IN_MOUSE_BUTTON_5      ::Vox::Mouse::Button5
#define IN_MOUSE_BUTTON_6      ::Vox::Mouse::Button6
#define IN_MOUSE_BUTTON_7      ::Vox::Mouse::Button7
#define IN_MOUSE_BUTTON_LAST   ::Vox::Mouse::ButtonLast
#define IN_MOUSE_BUTTON_LEFT   ::Vox::Mouse::ButtonLeft
#define IN_MOUSE_BUTTON_RIGHT  ::Vox::Mouse::ButtonRight
#define IN_MOUSE_BUTTON_MIDDLE ::Vox::Mouse::ButtonMiddle