#ifndef WINDOWS_WINDOW_INPUT_HPP
	#define WINDOWS_WINDOW_INPUT_HPP

	#include <windows.h>

	namespace winLib {
		enum {
			KEY_SPACE,
			KEY_PAGE_UP,
			KEY_PAGE_DOWN,
			KEY_END,
			KEY_HOME,
			KEY_LEFT,
			KEY_UP,
			KEY_RIGHT,
			KEY_DOWN,
			KEY_SELECT,
			KEY_PRINT,
			KEY_EXE,
			KEY_SNAPSHOT,
			KEY_INSERT,
			KEY_DELETE,
			KEY_HELP,
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,

			KEY_A,
			KEY_B,
			KEY_C,
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,

			KEY_NUM_0,
			KEY_NUM_1,
			KEY_NUM_2,
			KEY_NUM_3,
			KEY_NUM_4,
			KEY_NUM_5,
			KEY_NUM_6,
			KEY_NUM_7,
			KEY_NUM_8,
			KEY_NUM_9,
			KEY_MULTIPLY,
			KEY_ADD,
			KEY_SEPARATOR,
			KEY_SUBTRACT,
			KEY_DECIMAL,
			KEY_DIVIDE,
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,
			KEY_F13,
			KEY_F14,
			KEY_F15,
			KEY_F16,
			KEY_F17,
			KEY_F18,
			KEY_F19,
			KEY_F20,
			KEY_F21,
			KEY_F22,
			KEY_F23,
			KEY_F24,

			KEY_SHIFT_L,
			KEY_SHIFT_R,
			KEY_CTRL_L,
			KEY_CTRL_R,
			KEY_ALT_L,
			KEY_ALT_R,

			KEY_SHIFT,
			KEY_CTRL,
			KEY_ALT,
			KEY_PAUSE,
			KEY_CAPS_LOCK,

			KEY_ESC,


			KEYS_COUNT
		};

		struct KeyBoard {
			bool buttons[KEYS_COUNT];
		};

		enum {
			MOUSE_LEFT,
			MOUSE_RIGHT,
			MOUSE_MIDDLE,
			MOUSE_X1,
			MOUSE_X2,

			MOUSE_COUNT
		};

		struct Mouse {
			unsigned short x, y;		// the x and y position of the mouse
			short scroll_delta;			// the last scroll_delta before the next call of onUpdate of the mouse
			bool buttons[MOUSE_COUNT];	// All the button states of the mouse
			bool focus;					// When the mouse is inside of the window-client-area -> true
		};

		bool handle_input(const MSG* msg, KeyBoard* keyboard, Mouse* mouse) noexcept;

		enum NATIV_CURSOR {
			ARROW,			// Default cursor
			TEXT,			// Text input / select cursor
			WAIT,			// Busy cursor
			CROSS,			// Precision select cursor
			ARROW_UP,		// Alternate select cursor
			PEN,			// Handwriting cursor
			DIAGONAL_1,		// Resize cursor from top left to bottom right
			DIAGONAL_2,		// Resize cursor from top right to bottom left
			HORIZONTAL,		// Resize cursor
			VERTICAL,		// Resize cursor
			MOVE,			// Resize all cursor
			NO,				// Unavailable cursor
			LINK,			// Hand cursor
			WAIT_ARROW,		// Working in background cursor
			HELP,			// Help select cursor
			LOCATION,		// Location select cursor
			PERSON			// Person select cursor
		};

		LPCWSTR nativ_cursor_idc(NATIV_CURSOR cursor) noexcept;
		HCURSOR load_nativ_cursor(NATIV_CURSOR cursor) noexcept;

		// TODO
		//void create_key_shortcut() noexcept {}
	}

#endif