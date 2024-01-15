#include "input.hpp"

namespace winLib {
	bool handle_input(const MSG* msg, KeyBoard* keyboard, Mouse* mouse) noexcept {
		if (!msg || !keyboard || !mouse)
			return false;

		switch (msg->message) {
		case WM_KEYUP:
		case WM_KEYDOWN: {
			const unsigned int key_code = static_cast<unsigned int>(msg->wParam);
			const bool is_down = (msg->lParam & 0x80000000 /* 0x80000000 == (1U << 31)*/) == 0;

			#define key_case(first, winFirst, winLast)							\
				if (key_code >= winFirst && key_code <= winLast) {				\
					keyboard->buttons[key_code - winFirst + first] = is_down;	\
					break;														\
				}																\

			key_case(KEY_SPACE, VK_SPACE, '9')
			key_case(KEY_A, 'A', 'Z');
			key_case(KEY_NUM_0, VK_NUMPAD0, VK_F24)
			key_case(KEY_SHIFT_L, VK_LSHIFT, VK_RMENU)
			key_case(KEY_SHIFT, VK_SHIFT, VK_CAPITAL)

			break;
		}
		case WM_MOUSEWHEEL: {
			const short delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);

			mouse->scroll_delta += delta;

			break;
		}
		case WM_MOUSEMOVE: {
			const unsigned short x = msg->lParam & 0xFFFF, y = (msg->lParam >> 16) & 0xFFFF;

			mouse->x = x;
			mouse->y = y;

			break;
		}
		case WM_LBUTTONDOWN: {
			mouse->buttons[MOUSE_LEFT] = true;
			break;
		}
		case WM_LBUTTONUP: {
			mouse->buttons[MOUSE_LEFT] = false;
			break;
		}
		case WM_RBUTTONDOWN: {
			mouse->buttons[MOUSE_RIGHT] = true;
			break;
		}
		case WM_RBUTTONUP: {
			mouse->buttons[MOUSE_RIGHT] = false;
			break;
		}
		case WM_MBUTTONDOWN: {
			mouse->buttons[MOUSE_MIDDLE] = true;
			break;
		}
		case WM_MBUTTONUP: {
			mouse->buttons[MOUSE_MIDDLE] = false;
			break;
		}
		case WM_XBUTTONDOWN: {
			const unsigned short x_button = GET_XBUTTON_WPARAM(msg->wParam);

			if (x_button == XBUTTON1) {
				mouse->buttons[MOUSE_X1] = true;
				break;
			}

			if (x_button == XBUTTON2) {
				mouse->buttons[MOUSE_X2] = true;
			}

			break;
		}
		case WM_XBUTTONUP: {
			const unsigned short x_button = GET_XBUTTON_WPARAM(msg->wParam);

			if (x_button == XBUTTON1) {
				mouse->buttons[MOUSE_X1] = false;
				break;
			}

			if (x_button == XBUTTON2) {
				mouse->buttons[MOUSE_X2] = false;
			}

			break;
		}
		case WM_MOUSELEAVE: {
			mouse->focus = false;
			break;
		}
		case WM_MOUSEHOVER: {
			mouse->focus = true;
			break;
		}
		default: {
			TranslateMessage(msg);
			DispatchMessage(msg);
			break;
		}
		}

		return true;
	}

	LPCWSTR nativ_cursor_idc(NATIV_CURSOR cursor) noexcept {
		switch (cursor) {
		case NATIV_CURSOR::ARROW: {
			return IDC_ARROW;
		}
		case NATIV_CURSOR::TEXT: {
			return IDC_IBEAM;
		}
		case NATIV_CURSOR::WAIT: {
			return IDC_WAIT;
		}
		case NATIV_CURSOR::CROSS: {
			return IDC_CROSS;
		}
		case NATIV_CURSOR::ARROW_UP: {
			return IDC_UPARROW;
		}
		case NATIV_CURSOR::PEN: {
			// there is no macro from win32 api for this...
			return MAKEINTRESOURCE(32631);
		}
		case NATIV_CURSOR::DIAGONAL_1: {
			return IDC_SIZENWSE;
		}
		case NATIV_CURSOR::DIAGONAL_2: {
			return IDC_SIZENESW;
		}
		case NATIV_CURSOR::HORIZONTAL: {
			return IDC_SIZEWE;
		}
		case NATIV_CURSOR::VERTICAL: {
			return IDC_SIZENS;
		}
		case NATIV_CURSOR::MOVE: {
			return IDC_SIZEALL;
		}
		case NATIV_CURSOR::NO: {
			return IDC_NO;
		}
		case NATIV_CURSOR::LINK: {
			return IDC_HAND;
		}
		case NATIV_CURSOR::WAIT_ARROW: {
			return IDC_APPSTARTING;
		}
		case NATIV_CURSOR::HELP: {
			return IDC_HELP;
		}
		case NATIV_CURSOR::LOCATION: {
			return IDC_PIN;
		}
		case NATIV_CURSOR::PERSON: {
			return IDC_PERSON;
		}
		default: {
			return IDC_ARROW;
		}
		}
	}

	HCURSOR load_nativ_cursor(NATIV_CURSOR cursor) noexcept {
		return LoadCursor(nullptr, nativ_cursor_idc(cursor));
	}
}
