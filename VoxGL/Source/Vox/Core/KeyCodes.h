#pragma once

namespace Vox
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define IN_KEY_SPACE           ::Vox::Key::Space
#define IN_KEY_APOSTROPHE      ::Vox::Key::Apostrophe    /* ' */
#define IN_KEY_COMMA           ::Vox::Key::Comma         /* , */
#define IN_KEY_MINUS           ::Vox::Key::Minus         /* - */
#define IN_KEY_PERIOD          ::Vox::Key::Period        /* . */
#define IN_KEY_SLASH           ::Vox::Key::Slash         /* / */
#define IN_KEY_0               ::Vox::Key::D0
#define IN_KEY_1               ::Vox::Key::D1
#define IN_KEY_2               ::Vox::Key::D2
#define IN_KEY_3               ::Vox::Key::D3
#define IN_KEY_4               ::Vox::Key::D4
#define IN_KEY_5               ::Vox::Key::D5
#define IN_KEY_6               ::Vox::Key::D6
#define IN_KEY_7               ::Vox::Key::D7
#define IN_KEY_8               ::Vox::Key::D8
#define IN_KEY_9               ::Vox::Key::D9
#define IN_KEY_SEMICOLON       ::Vox::Key::Semicolon     /* ; */
#define IN_KEY_EQUAL           ::Vox::Key::Equal         /* = */
#define IN_KEY_A               ::Vox::Key::A
#define IN_KEY_B               ::Vox::Key::B
#define IN_KEY_C               ::Vox::Key::C
#define IN_KEY_D               ::Vox::Key::D
#define IN_KEY_E               ::Vox::Key::E
#define IN_KEY_F               ::Vox::Key::F
#define IN_KEY_G               ::Vox::Key::G
#define IN_KEY_H               ::Vox::Key::H
#define IN_KEY_I               ::Vox::Key::I
#define IN_KEY_J               ::Vox::Key::J
#define IN_KEY_K               ::Vox::Key::K
#define IN_KEY_L               ::Vox::Key::L
#define IN_KEY_M               ::Vox::Key::M
#define IN_KEY_N               ::Vox::Key::N
#define IN_KEY_O               ::Vox::Key::O
#define IN_KEY_P               ::Vox::Key::P
#define IN_KEY_Q               ::Vox::Key::Q
#define IN_KEY_R               ::Vox::Key::R
#define IN_KEY_S               ::Vox::Key::S
#define IN_KEY_T               ::Vox::Key::T
#define IN_KEY_U               ::Vox::Key::U
#define IN_KEY_V               ::Vox::Key::V
#define IN_KEY_W               ::Vox::Key::W
#define IN_KEY_X               ::Vox::Key::X
#define IN_KEY_Y               ::Vox::Key::Y
#define IN_KEY_Z               ::Vox::Key::Z
#define IN_KEY_LEFT_BRACKET    ::Vox::Key::LeftBracket   /* [ */
#define IN_KEY_BACKSLASH       ::Vox::Key::Backslash     /* \ */
#define IN_KEY_RIGHT_BRACKET   ::Vox::Key::RightBracket  /* ] */
#define IN_KEY_GRAVE_ACCENT    ::Vox::Key::GraveAccent   /* ` */
#define IN_KEY_WORLD_1         ::Vox::Key::World1        /* non-US #1 */
#define IN_KEY_WORLD_2         ::Vox::Key::World2        /* non-US #2 */

/* Function keys */
#define IN_KEY_ESCAPE          ::Vox::Key::Escape
#define IN_KEY_ENTER           ::Vox::Key::Enter
#define IN_KEY_TAB             ::Vox::Key::Tab
#define IN_KEY_BACKSPACE       ::Vox::Key::Backspace
#define IN_KEY_INSERT          ::Vox::Key::Insert
#define IN_KEY_DELETE          ::Vox::Key::Delete
#define IN_KEY_RIGHT           ::Vox::Key::Right
#define IN_KEY_LEFT            ::Vox::Key::Left
#define IN_KEY_DOWN            ::Vox::Key::Down
#define IN_KEY_UP              ::Vox::Key::Up
#define IN_KEY_PAGE_UP         ::Vox::Key::PageUp
#define IN_KEY_PAGE_DOWN       ::Vox::Key::PageDown
#define IN_KEY_HOME            ::Vox::Key::Home
#define IN_KEY_END             ::Vox::Key::End
#define IN_KEY_CAPS_LOCK       ::Vox::Key::CapsLock
#define IN_KEY_SCROLL_LOCK     ::Vox::Key::ScrollLock
#define IN_KEY_NUM_LOCK        ::Vox::Key::NumLock
#define IN_KEY_PRINT_SCREEN    ::Vox::Key::PrintScreen
#define IN_KEY_PAUSE           ::Vox::Key::Pause
#define IN_KEY_F1              ::Vox::Key::F1
#define IN_KEY_F2              ::Vox::Key::F2
#define IN_KEY_F3              ::Vox::Key::F3
#define IN_KEY_F4              ::Vox::Key::F4
#define IN_KEY_F5              ::Vox::Key::F5
#define IN_KEY_F6              ::Vox::Key::F6
#define IN_KEY_F7              ::Vox::Key::F7
#define IN_KEY_F8              ::Vox::Key::F8
#define IN_KEY_F9              ::Vox::Key::F9
#define IN_KEY_F10             ::Vox::Key::F10
#define IN_KEY_F11             ::Vox::Key::F11
#define IN_KEY_F12             ::Vox::Key::F12
#define IN_KEY_F13             ::Vox::Key::F13
#define IN_KEY_F14             ::Vox::Key::F14
#define IN_KEY_F15             ::Vox::Key::F15
#define IN_KEY_F16             ::Vox::Key::F16
#define IN_KEY_F17             ::Vox::Key::F17
#define IN_KEY_F18             ::Vox::Key::F18
#define IN_KEY_F19             ::Vox::Key::F19
#define IN_KEY_F20             ::Vox::Key::F20
#define IN_KEY_F21             ::Vox::Key::F21
#define IN_KEY_F22             ::Vox::Key::F22
#define IN_KEY_F23             ::Vox::Key::F23
#define IN_KEY_F24             ::Vox::Key::F24
#define IN_KEY_F25             ::Vox::Key::F25

/* Keypad */
#define IN_KEY_KP_0            ::Vox::Key::KP0
#define IN_KEY_KP_1            ::Vox::Key::KP1
#define IN_KEY_KP_2            ::Vox::Key::KP2
#define IN_KEY_KP_3            ::Vox::Key::KP3
#define IN_KEY_KP_4            ::Vox::Key::KP4
#define IN_KEY_KP_5            ::Vox::Key::KP5
#define IN_KEY_KP_6            ::Vox::Key::KP6
#define IN_KEY_KP_7            ::Vox::Key::KP7
#define IN_KEY_KP_8            ::Vox::Key::KP8
#define IN_KEY_KP_9            ::Vox::Key::KP9
#define IN_KEY_KP_DECIMAL      ::Vox::Key::KPDecimal
#define IN_KEY_KP_DIVIDE       ::Vox::Key::KPDivide
#define IN_KEY_KP_MULTIPLY     ::Vox::Key::KPMultiply
#define IN_KEY_KP_SUBTRACT     ::Vox::Key::KPSubtract
#define IN_KEY_KP_ADD          ::Vox::Key::KPAdd
#define IN_KEY_KP_ENTER        ::Vox::Key::KPEnter
#define IN_KEY_KP_EQUAL        ::Vox::Key::KPEqual

#define IN_KEY_LEFT_SHIFT      ::Vox::Key::LeftShift
#define IN_KEY_LEFT_CONTROL    ::Vox::Key::LeftControl
#define IN_KEY_LEFT_ALT        ::Vox::Key::LeftAlt
#define IN_KEY_LEFT_SUPER      ::Vox::Key::LeftSuper
#define IN_KEY_RIGHT_SHIFT     ::Vox::Key::RightShift
#define IN_KEY_RIGHT_CONTROL   ::Vox::Key::RightControl
#define IN_KEY_RIGHT_ALT       ::Vox::Key::RightAlt
#define IN_KEY_RIGHT_SUPER     ::Vox::Key::RightSuper
#define IN_KEY_MENU            ::Vox::Key::Menu