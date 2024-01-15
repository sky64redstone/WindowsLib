#include "window.hpp"

namespace winLib {
    LRESULT __stdcall Window::window_callback(HWND window_handle, _In_ UINT message, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept {
        Window* this_pointer = nullptr;

        if (message == WM_NCCREATE) {
            CREATESTRUCT* create = (CREATESTRUCT*)lparam;

            this_pointer = static_cast<Window*>(create->lpCreateParams);

            if (this_pointer) {
                SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)this_pointer);

                this_pointer->window_handle = window_handle;
            }
        } else
            this_pointer = (Window*)GetWindowLongPtr(window_handle, GWLP_USERDATA);

        if (this_pointer)
            return this_pointer->window_event(message, wparam, lparam);

        return DefWindowProc(window_handle, message, wparam, lparam);
    }

    Window::Window() noexcept {
        state = {};
        window_handle = nullptr;
        device_context_handle = nullptr;
        gdi_plus_image_loader = {};
        minDelta = 0;
        onInitalise = nullptr;
        onUpdate = nullptr;
        onTerminate = nullptr;
        running = false;
        keyboard = {};
        mouse = {};
    }

    void Window::create(int width, int height, DWORD window_style, HINSTANCE instance, NATIV_CURSOR cursor) noexcept {
        const wchar_t* name = L"Window-Class Framework";

        WNDCLASS wc = { };

        wc.lpfnWndProc = window_callback;
        wc.hInstance = instance;
        wc.lpszClassName = name;
        wc.hCursor = load_nativ_cursor(cursor);

        RegisterClass(&wc);

        // Create the window.

        window_handle = CreateWindowEx(
            0,                              // Optional window styles.
            name,                           // Window class
            L"Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,

            nullptr,       // Parent window    
            nullptr,       // Menu
            instance,      // Instance handle
            this           // Additional application data
        );

        if (!window_handle) {
            DEBUGSTRING(Txt("Failed to create a Window! Last Error: %ul\n"), 100, GetLastError())
        }
    }

    void Window::start(int showCMD, unsigned int maxFrameRate) noexcept {
        if (!window_handle) {
            DEBUGSTRING(Txt("The Window does not exist! Last Error: %ul\n"), 100, GetLastError())
            return;
        }

        if (maxFrameRate < UNLIMITED_FRAME_RATE)
            minDelta = 1.0f / maxFrameRate;

        ShowWindow(window_handle, showCMD);

        running = true;

        device_context_handle = GetDC(window_handle);

        // I'm sorry
        gdi_plus_image_loader.gdi_plus_startup();

        LARGE_INTEGER cpu_frequency;
        QueryPerformanceFrequency(&cpu_frequency);

        LARGE_INTEGER last_counter;
        QueryPerformanceCounter(&last_counter);

        if (onInitalise)
            onInitalise(this);

        while (running) {
            float delta;

            LARGE_INTEGER current_counter;
            QueryPerformanceCounter(&current_counter);

            const long long counter_elapsed = current_counter.QuadPart - last_counter.QuadPart;

            delta = static_cast<float>(counter_elapsed) / static_cast<float>(cpu_frequency.QuadPart);	// in seconds

            last_counter = current_counter;

            DEBUGSTRING(Txt("delta: %f\n"), 100, delta)

            MSG msg = {};
            while (PeekMessage(&msg, window_handle, NULL, NULL, PM_REMOVE)) {
                if (!handle_input(&msg, &keyboard, &mouse)) {
                    // TODO error handeling
                }
            }

            if (onUpdate) {
                onUpdate(this, delta);
                mouse.scroll_delta = 0;
            }

            StretchDIBits(
                device_context_handle,
                0, state.height, state.width, -static_cast<long long>(state.height),
                0, 0, state.width, state.height,
                state.memory,
                &state.info,
                DIB_RGB_COLORS,
                SRCCOPY
            );
        }

        if (onTerminate)
            onTerminate(this);

        // I'm sorry
        gdi_plus_image_loader.gdi_plus_shutdown();
    }

    LRESULT Window::window_event(UINT message, WPARAM wparam, LPARAM lparam) noexcept {

        switch (message) {
        case WM_DESTROY: {
            running = false;
            return 0;
        }
        case WM_SIZE: {

            RECT client_rect = {};

            GetClientRect(window_handle, &client_rect);

            state.width = client_rect.right - client_rect.left;
            state.height = client_rect.bottom - client_rect.top;

            const int buffersize = int(static_cast<unsigned long long>(state.width) * state.height * sizeof(unsigned int));

            if (state.memory)
                VirtualFree(state.memory, 0, MEM_RELEASE);

            state.memory = static_cast<unsigned int*>(VirtualAlloc(nullptr, buffersize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

            state.info.bmiHeader.biSize = sizeof(state.info.bmiHeader);
            state.info.bmiHeader.biWidth = state.width;
            state.info.bmiHeader.biHeight = state.height;
            state.info.bmiHeader.biPlanes = 1;
            state.info.bmiHeader.biBitCount = 32;
            state.info.bmiHeader.biCompression = BI_RGB;

            return 0;
        }
        default:
            return DefWindowProc(window_handle, message, wparam, lparam);
        }
    }

    void Window::setTitle(const std::string title) {
        #if defined(UNICODE) || defined(_UNICODE)
            SetWindowText(window_handle, convertS2W(title).c_str());
        #else
            SetWindowText(window_handle, title.c_str());
        #endif
    }

    std::string Window::getTitle() {
        #if defined(UNICODE) || defined(_UNICODE)
            const int str_length = GetWindowTextLength(window_handle);
            LPWSTR wtitle = {};
            if (GetWindowText(window_handle, wtitle, str_length + 1) && str_length) {
                return convertW2S(wtitle);
            }
        #else
            const int str_length = GetWindowTextLength(window_handle);
            LPSTR title = {};
            if (GetWindowText(window_handle, title, str_length + 1) && str_length) {
                return std::string(title);
            }
        #endif
        return "";
    }

    void Window::clearScreen(unsigned int color_code) const noexcept {
        unsigned int* pixel = state.memory;

        if (pixel == nullptr)
            return;

        for (unsigned int y = 0; y < state.height; y++) {
            for (unsigned int x = 0; x < state.width; x++) {
                *pixel++ = color_code;
            }
        }
    }

    bool Window::draw(unsigned int x, unsigned int y, unsigned int color_code) const noexcept {
        if (x < 0 || x >= state.width || y < 0 || y >= state.height)
            return false;

        unsigned int* pixel = state.memory + x + y * state.width;

        *pixel = color_code;

        return true;
    }

    void Window::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color_code, unsigned int pattern) const {
        unsigned int x, y, dx1, dy1, px, py, xe, ye, i;
        const int dx = x2 - x1, dy = y2 - y1;

        auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

        if (dx == 0) {
            if (y2 < y1)
                std::swap(y1, y2);

            if (x1 >= state.width)
                return;

            #ifdef min
                y2 = min(y2, state.height - 1);
            #else
                y2 = std::min(y2, state.height - 1);
            #endif

            for (y = y1; y <= y2; y++)
                if (rol())
                    draw(x1, y, color_code);

            return;
        }

        if (dy == 0) {
            if (x2 < x1)
                std::swap(x1, x2);

            if (y1 >= state.height)
                return;

            #ifdef min
                x2 = min(x2, state.width - 1);
            #else
                x2 = std::min(x2, state.width - 1);
            #endif

            for (x = x1; x <= x2; x++)
                if (rol())
                    draw(x, y1, color_code);

            return;
        }

        const vector2i p1(x1, y1), p2(x2, y2);

        dx1 = abs(dx); dy1 = abs(dy);
        px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
        if (dy1 <= dx1) {
            if (dx >= 0) {
                x = x1;
                y = y1;
                xe = x2;
            }
            else {
                x = x2;
                y = y2;
                xe = x1;
            }

            if (rol())
                draw(x, y, color_code);

            for (i = 0; x < xe; i++) {
                x = x + 1;
                if (px < 0)
                    px = px + 2 * dy1;
                else {
                    if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                        y = y + 1;
                    else
                        y = y - 1;

                    px = px + 2 * (dy1 - dx1);
                }
                if (rol())
                    draw(x, y, color_code);
            }

        }
        else {
            if (dy >= 0) {
                x = x1;
                y = y1;
                ye = y2;
            }
            else {
                x = x2;
                y = y2;
                ye = y1;
            }

            if (rol())
                draw(x, y, color_code);

            for (i = 0; y < ye; i++) {
                y = y + 1;
                if (py <= 0)
                    py = py + 2 * dx1;
                else {
                    if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                        x = x + 1;
                    else
                        x = x - 1;

                    py = py + 2 * (dx1 - dy1);
                }
                if (rol())
                    draw(x, y, color_code);
            }
        }
    }

    void Window::drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color_code, unsigned int pattern) const {
        const int xw = x + w, yh = y + h;
        drawLine(x, y, xw, y, color_code, pattern);
        drawLine(xw, y, xw, yh, color_code, pattern);
        drawLine(xw, yh, x, yh, color_code, pattern);
        drawLine(x, yh, x, y, color_code, pattern);
    }

    void Window::fillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color_code) const {
        if (x < 0 || y < 0 || w < 0 || h < 0)
            return;

        if (x >= state.width)
            x = state.width - 1;
        if (y >= state.height)
            y = state.height - 1;

        unsigned int x2 = x + w;
        unsigned int y2 = y + h;

        if (w == 0) {
            if (h == 0) {
                draw(x, y, color_code);
                return;
            }

            drawLine(x, y, x, y2, color_code);
            return;
        }

        if (h == 0) {
            drawLine(x, y, x2, y, color_code);
            return;
        }

        if (x2 >= state.width)
            x2 = state.width - 1;
        if (y2 >= state.height)
            y2 = state.height - 1;

        for (unsigned int y_ = y; y_ < y2; y_++) {
            unsigned int* pixel = state.memory + x + y_ * state.width;
            for (unsigned int x_ = x; x_ < x2; x_++) {
                // more efficient in this case than the draw(...) method
                *pixel++ = color_code;
            }
        }
    }

    void Window::drawTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color_code, unsigned int pattern) const {
        drawLine(x1, y1, x2, y2, color_code, pattern);
        drawLine(x2, y2, x3, y3, color_code, pattern);
        drawLine(x3, y3, x1, y1, color_code, pattern);
    }

    void Window::fillTriangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned int color_code) {
        auto drawline = [&](unsigned int sx, unsigned int ex, unsigned int ny) {
            if (ny >= state.height)
                return;

            unsigned int* pixel = state.memory + sx + ny * state.width;

            #ifdef min
                ex = min(ex, state.width - 1);
            #else
                ex = std::min(ex, state.width - 1);
            #endif

            for (unsigned int x_ = sx; x_ <= ex; x_++) {
                // more efficient in this case than the draw(...) method
                *pixel++ = color_code;
            }
            };

        unsigned int t1x, t2x, y, minx, maxx, t1xp, t2xp;
        bool changed1 = false;
        bool changed2 = false;
        int signx1, signx2, dx1, dy1, dx2, dy2;
        int e1, e2;

        // Sort vertices
        if (y1 > y2) {
            std::swap(y1, y2);
            std::swap(x1, x2);
        }

        if (y1 > y3) {
            std::swap(y1, y3);
            std::swap(x1, x3);
        }
        if (y2 > y3) {
            std::swap(y2, y3);
            std::swap(x2, x3);
        }

        t1x = t2x = x1; y = y1;
        dx1 = static_cast<int>(x2 - x1);

        if (dx1 < 0) {
            dx1 = -dx1;
            signx1 = -1;
        } else
            signx1 = 1;

        dy1 = static_cast<int>(y2 - y1);

        dx2 = static_cast<int>(x3 - x1);

        if (dx2 < 0) {
            dx2 = -dx2;
            signx2 = -1;
        }
        else
            signx2 = 1;

        dy2 = static_cast<int>(y3 - y1);

        if (dy1 > dx1) {
            std::swap(dx1, dy1);
            changed1 = true;
        }

        if (dy2 > dx2) {
            std::swap(dy2, dx2);
            changed2 = true;
        }

        e2 = static_cast<int>(dx2 >> 1);

        if (y1 == y2)
            goto next;

        e1 = static_cast<int>(dx1 >> 1);

        for (int i = 0; i < dx1;) {
            t1xp = 0; t2xp = 0;
            if (t1x < t2x) {
                minx = t1x;
                maxx = t2x;
            }
            else {
                minx = t2x;
                maxx = t1x;
            }
            while (i < dx1) {
                i++;
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;

                    if (changed1)
                        t1xp = signx1;
                    else
                        goto next1;
                }
                if (changed1)
                    break;
                else
                    t1x += signx1;
            }

        next1:

            while (1) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;

                    if (changed2)
                        t2xp = signx2;
                    else
                        goto next2;
                }
                if (changed2)
                    break;
                else
                    t2x += signx2;
            }
        next2:
            if (minx > t1x)
                minx = t1x;

            if (minx > t2x)
                minx = t2x;

            if (maxx < t1x)
                maxx = t1x;

            if (maxx < t2x)
                maxx = t2x;

            drawline(minx, maxx, y);

            if (!changed1)
                t1x += signx1;

            t1x += t1xp;
            if (!changed2)
                t2x += signx2;

            t2x += t2xp;
            y += 1;
            if (y == y2)
                break;
        }
    next:

        dx1 = static_cast<int>(x3 - x2);

        if (dx1 < 0) {
            dx1 = -dx1;
            signx1 = -1;
        }
        else
            signx1 = 1;

        dy1 = static_cast<int>(y3 - y2);
        t1x = x2;
        if (dy1 > dx1) {
            std::swap(dy1, dx1);
            changed1 = true;
        }
        else
            changed1 = false;

        e1 = static_cast<int>(dx1 >> 1);

        for (int i = 0; i <= dx1; i++) {
            t1xp = 0; t2xp = 0;

            if (t1x < t2x) {
                minx = t1x;
                maxx = t2x;
            }
            else {
                minx = t2x;
                maxx = t1x;
            }

            while (i < dx1) {
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) {
                        t1xp = signx1;
                        break;
                    }
                    else
                        goto next3;
                }
                if (changed1)
                    break;
                else
                    t1x += signx1;

                if (i < dx1)
                    i++;
            }
        next3:

            while (t2x != x3) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2)
                        t2xp = signx2;
                    else
                        goto next4;
                }
                if (changed2)
                    break;
                else
                    t2x += signx2;
            }
        next4:

            if (minx > t1x)
                minx = t1x;
            if (minx > t2x)
                minx = t2x;
            if (maxx < t1x)
                maxx = t1x;
            if (maxx < t2x)
                maxx = t2x;
            drawline(minx, maxx, y);
            if (!changed1)
                t1x += signx1;

            t1x += t1xp;
            if (!changed2)
                t2x += signx2;

            t2x += t2xp;
            y += 1;
            if (y > y3)
                return;
        }
    }

    void Window::fillTexturedTriangle(const std::span<vector2u>& screen_pos, const std::span<vector2f>& texture_pos, const std::span<unsigned int>& colors, Image* texture) {
        vector2i point_1 = screen_pos[0];
        vector2i point_2 = screen_pos[1];
        vector2i point_3 = screen_pos[2];

        if (point_2.y < point_1.y) { std::swap(point_1.y, point_2.y); std::swap(point_1.x, point_2.x); std::swap(texture_pos[0].x, texture_pos[1].x); std::swap(texture_pos[0].y, texture_pos[1].y); std::swap(colors[0], colors[1]); }
        if (point_3.y < point_1.y) { std::swap(point_1.y, point_3.y); std::swap(point_1.x, point_3.x); std::swap(texture_pos[0].x, texture_pos[2].x); std::swap(texture_pos[0].y, texture_pos[2].y); std::swap(colors[0], colors[2]); }
        if (point_3.y < point_2.y) { std::swap(point_2.y, point_3.y); std::swap(point_2.x, point_3.x); std::swap(texture_pos[1].x, texture_pos[2].x); std::swap(texture_pos[1].y, texture_pos[2].y); std::swap(colors[1], colors[2]); }

        vector2i pos_1 = point_2 - point_1;
        vector2f tex_1 = texture_pos[1] - texture_pos[0];
        int color_1_red = (getRedColorValue(colors[1])) - (getRedColorValue(colors[0]));
        int color_1_green = (getGreenColorValue(colors[1])) - (getGreenColorValue(colors[0]));
        int color_1_blue = (getBlueColorValue(colors[1])) - (getBlueColorValue(colors[0]));
        int color_1_alpha = (getAlphaColorValue(colors[1])) - (getAlphaColorValue(colors[0]));

        const vector2i pos_2 = point_3 - point_1;
        const vector2f tex_2 = texture_pos[2] - texture_pos[0];
        const int color_2_red = (getRedColorValue(colors[2])) - (getRedColorValue(colors[0]));
        const int color_2_green = (getGreenColorValue(colors[2])) - (getGreenColorValue(colors[0]));
        const int color_2_blue = (getBlueColorValue(colors[2])) - (getBlueColorValue(colors[0]));
        const int color_2_alpha = (getAlphaColorValue(colors[2])) - (getAlphaColorValue(colors[0]));

        float ax_step = 0, bx_step = 0, color_1_red_step = 0, color_2_red_step = 0, color_1_green_step = 0, color_2_green_step = 0, color_1_blue_step = 0, color_2_blue_step = 0, color_1_alpha_step = 0, color_2_alpha_step = 0;
        vector2f tex_1_step, tex_2_step;

        if (pos_1.y) {
            ax_step = pos_1.x / static_cast<float>(abs(pos_1.y));
            tex_1_step = tex_1 / static_cast<float>(abs(pos_1.y));
            color_1_red_step = color_1_red / static_cast<float>(abs(pos_1.y));
            color_1_green_step = color_1_green / static_cast<float>(abs(pos_1.y));
            color_1_blue_step = color_1_blue / static_cast<float>(abs(pos_1.y));
            color_1_alpha_step = color_1_alpha / static_cast<float>(abs(pos_1.y));
        }

        if (pos_2.y) {
            bx_step = pos_2.x / static_cast<float>(abs(pos_2.y));
            tex_2_step = tex_2 / static_cast<float>(abs(pos_2.y));
            color_2_red_step = color_2_red / static_cast<float>(abs(pos_2.y));
            color_2_green_step = color_2_green / static_cast<float>(abs(pos_2.y));
            color_2_blue_step = color_2_blue / static_cast<float>(abs(pos_2.y));
            color_2_alpha_step = color_2_alpha / static_cast<float>(abs(pos_2.y));
        }

        vector2i start;
        vector2i end;
        int start_idx;

        for (int pass = 0; pass < 2; pass++) {
            if (pass == 0) {
                start = point_1; end = point_2;	start_idx = 0;
            }
            else {
                pos_1 = point_3 - point_2;
                tex_1 = texture_pos[2] - texture_pos[1];

                color_1_red = (getRedColorValue(colors[2])) - (getRedColorValue(colors[1]));
                color_1_green = (getGreenColorValue(colors[2])) - (getGreenColorValue(colors[1]));
                color_1_blue = (getBlueColorValue(colors[2])) - (getBlueColorValue(colors[1]));
                color_1_alpha = (getAlphaColorValue(colors[2])) - (getAlphaColorValue(colors[1]));

                color_1_red_step = 0; color_1_green_step = 0; color_1_blue_step = 0; color_1_alpha_step = 0;

                if (pos_2.y) bx_step = pos_2.x / static_cast<float>(abs(pos_2.y));
                if (pos_1.y) {
                    ax_step = pos_1.x / static_cast<float>(abs(pos_1.y));
                    tex_1_step = tex_1 / static_cast<float>(abs(pos_1.y));
                    color_1_red_step = color_1_red / static_cast<float>(abs(pos_1.y));
                    color_1_green_step = color_1_green / static_cast<float>(abs(pos_1.y));
                    color_1_blue_step = color_1_blue / static_cast<float>(abs(pos_1.y));
                    color_1_alpha_step = color_1_alpha / static_cast<float>(abs(pos_1.y));
                }

                start = point_2; end = point_3; start_idx = 1;
            }

            if (pos_1.y) {
                for (int i = start.y; i <= end.y; i++) {
                    int ax = static_cast<int>(start.x + static_cast<float>(i - start.y) * ax_step);
                    int bx = static_cast<int>(point_1.x + static_cast<float>(i - point_1.y) * bx_step);

                    vector2f tex_start(texture_pos[start_idx].x + static_cast<float>(i - start.y) * tex_1_step.x, texture_pos[start_idx].y + static_cast<float>(i - start.y) * tex_1_step.y);
                    vector2f tex_end(texture_pos[0].x + static_cast<float>(i - point_1.y) * tex_2_step.x, texture_pos[0].y + static_cast<float>(i - point_1.y) * tex_2_step.y);

                    unsigned int col_start = rgbaColorCode(
                        getRedColorValue(colors[start_idx]) + static_cast<unsigned char>(static_cast<float>(i - start.y) * color_1_red_step),
                        getGreenColorValue(colors[start_idx]) + static_cast<unsigned char>(static_cast<float>(i - start.y) * color_1_green_step),
                        getBlueColorValue(colors[start_idx]) + static_cast<unsigned char>(static_cast<float>(i - start.y) * color_1_blue_step),
                        getAlphaColorValue(colors[start_idx]) + static_cast<unsigned char>(static_cast<float>(i - start.y) * color_1_alpha_step)
                    );

                    unsigned int col_end = rgbaColorCode(
                        getRedColorValue(colors[0]) + static_cast<unsigned char>(static_cast<float>(i - point_1.y) * color_2_red_step),
                        getGreenColorValue(colors[0]) + static_cast<unsigned char>(static_cast<float>(i - point_1.y) * color_2_green_step),
                        getBlueColorValue(colors[0]) + static_cast<unsigned char>(static_cast<float>(i - point_1.y) * color_2_blue_step),
                        getAlphaColorValue(colors[0]) + static_cast<unsigned char>(static_cast<float>(i - point_1.y) * color_2_alpha_step)
                    );

                    if (ax > bx) { std::swap(ax, bx); std::swap(tex_start, tex_end); std::swap(col_start, col_end); }

                    const float step = 1.0f / (static_cast<float>(bx - ax));
                    float t = 0.0f;

                    for (int j = ax; j < bx; j++) {
                        unsigned int color = colorLerp(col_start, col_end, t);
                        if (texture) {
                            const vector2f tex_pos = tex_start.lerp(tex_end, t);
                            const unsigned int tex_color = texture->getColor(static_cast<int>(tex_pos.x), static_cast<int>(tex_pos.y));

                            const unsigned char r1 = static_cast<unsigned char>(getRedColorValue(color));
                            const unsigned char g1 = static_cast<unsigned char>(getGreenColorValue(color));
                            const unsigned char b1 = static_cast<unsigned char>(getBlueColorValue(color));
                            const unsigned char a1 = static_cast<unsigned char>(getAlphaColorValue(color));

                            const unsigned char r2 = static_cast<unsigned char>(getRedColorValue(tex_color));
                            const unsigned char g2 = static_cast<unsigned char>(getGreenColorValue(tex_color));
                            const unsigned char b2 = static_cast<unsigned char>(getBlueColorValue(tex_color));
                            const unsigned char a2 = static_cast<unsigned char>(getAlphaColorValue(tex_color));

                            color = rgbaColorCode(r1 * r2, g1 * g2, b1 * b2, a1 * a2);
                        }
                        draw(j, i, color);
                        t += step;
                    }
                }
            }
        }
    }

    void Window::fillPolygon(const std::span<vector2u>& points, unsigned int color_code, unsigned char structure) {
        if (structure == PolygonStructure::LINE)
            return;

        if (points.size() < 3)
            return;

        if (structure == PolygonStructure::LIST) {
            for (int triangle = 0; triangle < points.size() / 3; triangle++) {
                const vector2u pos[] = { points[triangle * 3 + 0], points[triangle * 3 + 1], points[triangle * 3 + 2] };

                fillTriangle(pos[0].x, pos[0].y, pos[1].x, pos[1].y, pos[2].x, pos[2].y, color_code);
            }
            return;
        }

        if (structure == PolygonStructure::STRIP) {
            for (int triangle = 2; triangle < points.size(); triangle++) {
                const vector2u pos[] = { points[triangle - 2], points[triangle - 1], points[triangle] };

                fillTriangle(pos[0].x, pos[0].y, pos[1].x, pos[1].y, pos[2].x, pos[2].y, color_code);
            }
            return;
        }

        if (structure == PolygonStructure::FAN) {
            for (int triangle = 2; triangle < points.size(); triangle++) {
                const vector2u pos[] = { points[0], points[triangle - 1], points[triangle] };

                fillTriangle(pos[0].x, pos[0].y, pos[1].x, pos[1].y, pos[2].x, pos[2].y, color_code);
            }
            return;
        }
    }

    void Window::fillTexturedPolygon(const std::span<vector2u>& screen_pos, const std::span<vector2f>& texture_pos, const std::span<unsigned int>& colors, Image* texture, unsigned char structure) {
        if (structure == PolygonStructure::LINE)
            return;

        if (screen_pos.size() < 3 || texture_pos.size() < 3 || colors.size() < 3)
            return;

        if (structure == PolygonStructure::LIST) {
            for (int triangle = 0; triangle < screen_pos.size() / 3; triangle++) {
                const int triangleIdx = triangle * 3;
                vector2u points[] = { screen_pos[triangleIdx + 0], screen_pos[triangleIdx + 1], screen_pos[triangleIdx + 2] };
                vector2f texture_points[] = { texture_pos[triangleIdx + 0], texture_pos[triangleIdx + 1], texture_pos[triangleIdx + 2] };
                unsigned int colors_[] = { colors[triangleIdx + 0], colors[triangleIdx + 1], colors[triangleIdx + 2] };

                fillTexturedTriangle(points, texture_points, colors_, texture);
            }
            return;
        }

        if (structure == PolygonStructure::STRIP) {
            for (int triangle = 2; triangle < screen_pos.size(); triangle++) {
                vector2u points[] = { screen_pos[triangle - 2], screen_pos[triangle - 1], screen_pos[triangle] };
                vector2f texture_points[] = { texture_pos[triangle - 2], texture_pos[triangle - 1], texture_pos[triangle] };
                unsigned int colors_[] = { colors[triangle - 2], colors[triangle - 1], colors[triangle] };

                fillTexturedTriangle(points, texture_points, colors_, texture);
            }
            return;
        }

        if (structure == PolygonStructure::FAN) {
            for (int triangle = 2; triangle < screen_pos.size(); triangle++) {
                vector2u points[] = { screen_pos[0], screen_pos[triangle - 1], screen_pos[triangle] };
                vector2f texture_points[] = { texture_pos[0], texture_pos[triangle - 1], texture_pos[triangle] };
                unsigned int colors_[] = { colors[0], colors[triangle - 1], colors[triangle] };

                fillTexturedTriangle(points, texture_points, colors_, texture);
            }
            return;
        }
    }

    void Window::drawCircle(int x, int y, int radius, unsigned int color_code, unsigned char mask) const noexcept {
        // TODO check if the circle would be visible

        if (radius > 0) {
            int x0 = 0;
            int y0 = radius;
            int d = 3 - 2 * radius;

            while (y0 >= x0) { // only formulate 1/8 of circle
                // Draw even octants
                if (mask & 0x01) draw(x + x0, y - y0, color_code);// Q6 - upper right right
                if (mask & 0x04) draw(x + y0, y + x0, color_code);// Q4 - lower lower right
                if (mask & 0x10) draw(x - x0, y + y0, color_code);// Q2 - lower left left
                if (mask & 0x40) draw(x - y0, y - x0, color_code);// Q0 - upper upper left
                if (x0 != 0 && x0 != y0) {
                    if (mask & 0x02) draw(x + y0, y - x0, color_code);// Q7 - upper upper right
                    if (mask & 0x08) draw(x + x0, y + y0, color_code);// Q5 - lower right right
                    if (mask & 0x20) draw(x - y0, y + x0, color_code);// Q3 - lower lower left
                    if (mask & 0x80) draw(x - x0, y - y0, color_code);// Q1 - upper left left
                }
                if (d < 0)
                    d += 4 * x0++ + 6;
                else
                    d += 4 * (x0++ - y0--) + 10;
            }
        }
        else
            draw(x, y, color_code);
    }

    void Window::fillCircle(int x, int y, int radius, unsigned int color_code) const {
        // TODO check if the circle would be visible

        auto drawline = [&](int sx, int ex, int y) {
            if (y >= state.height)
                return;

            if (sx < 0)
                sx = 0;

            #ifdef min
                ex = min(ex, static_cast<int>(state.width - 1));
            #else
                ex = std::min(ex, static_cast<int>(state.width - 1));
            #endif

            for (int x = sx; x <= ex; x++)
                draw(x, y, color_code);
        };

        if (radius > 0) {
            int x0 = 0;
            int y0 = radius;
            int d = 3 - 2 * radius;

            while (y0 >= x0) {
                drawline(x - y0, x + y0, y - x0);
                if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

                if (d < 0)
                    d += 4 * x0++ + 6;
                else {
                    if (x0 != y0) {
                        drawline(x - x0, x + x0, y - y0);
                        drawline(x - x0, x + x0, y + y0);
                    }
                    d += 4 * (x0++ - y0--) + 10;
                }
            }
        }
        else
            draw(x, y, color_code);
    }

    void Window::drawImage(unsigned int x, unsigned int y, Image* image, unsigned int scale, unsigned char flip) const noexcept {
        if (!image)
            return;

        int fxs = 0, fxm = 1, fx = 0;
        int fys = 0, fym = 1, fy = 0;

        if (flip & Image::Flip::HORIZ) { fxs = image->width - 1; fxm = -1; }
        if (flip & Image::Flip::VERT) { fys = image->height - 1; fym = -1; }

        const int width  = std::min(image->width, state.width);
        const int height = std::min(image->height, state.height);

        fx = fxs;
        if (scale > 1) {
            for (unsigned int i = 0; i < width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < height; j++, fy += fym)
                    for (unsigned int is = 0; is < scale; is++)
                        for (unsigned int js = 0; js < scale; js++)
                            draw(x + (i * scale) + is, y + (j * scale) + js, image->getColor(fx, fy));
            }
        } else {
            for (unsigned int i = 0; i < width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < height; j++, fy += fym)
                    draw(x + i, y + j, image->getColor(fx, fy));
            }
        }
    }

    void Window::drawImageF(unsigned int x, unsigned int y, Image* image, float scale, unsigned char flip) const noexcept {
        if (!image || !scale) // if the image is nullptr OR scale == 0
            return;

        int fxs = 0, fxm = 1, fx = 0;
        int fys = 0, fym = 1, fy = 0;

        if (flip & Image::Flip::HORIZ) { fxs = image->width - 1; fxm = -1; }
        if (flip & Image::Flip::VERT) { fys = image->height - 1; fym = -1; }

        const int width = std::min(image->width, state.width);
        const int height = std::min(image->height, state.height);

        if (scale > 1.f) {
            fx = fxs;
            for (unsigned int i = 0; i < width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < height; j++, fy += fym)
                    for (unsigned int is = 0; is < scale; is++)
                        for (unsigned int js = 0; js < scale; js++)
                            draw(static_cast<unsigned int>(x + (i * scale) + is), static_cast<unsigned int>(y + (j * scale) + js), image->getColor(fx, fy));
            }
        } else if (scale == 1.f) { // draw the image normal
            fx = fxs;
            for (unsigned int i = 0; i < width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < height; j++, fy += fym)
                    draw(x + i, y + j, image->getColor(fx, fy));
            }
        } else if (scale > 0.f) {
            // TODO
        } else { // scale < 0
            // TODO
        }
    }

    void Window::drawSubImage(unsigned int x, unsigned int y, unsigned int src_x, unsigned int src_y, unsigned int w, unsigned h, Image* image, unsigned int scale, unsigned char flip) const noexcept {
        if (!image)
            return;

        if (src_x + w >= image->width || src_y + h >= image->height)
            return;

        int fxs = 0, fxm = 1, fx = 0;
        int fys = 0, fym = 1, fy = 0;

        if (flip & Image::Flip::HORIZ) { fxs = image->width - 1; fxm = -1; }
        if (flip & Image::Flip::VERT) { fys = image->height - 1; fym = -1; }

        if (scale > 1) {
            fx = fxs;
            for (unsigned int i = 0; i < image->width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < image->height; j++, fy += fym)
                    for (unsigned int is = 0; is < scale; is++)
                        for (unsigned int js = 0; js < scale; js++)
                            draw(x + (i * scale) + is, y + (j * scale) + js, image->getColor(fx + src_x, fy + src_y));
            }
        }
        else {
            fx = fxs;
            for (unsigned int i = 0; i < image->width; i++, fx += fxm) {
                fy = fys;
                for (unsigned int j = 0; j < image->height; j++, fy += fym)
                    draw(x + i, y + j, image->getColor(fx + src_x, fy + src_y));
            }
        }
    }

    unsigned int Window::colorLerp(unsigned int color1, unsigned int color2, float t) noexcept {
        const float t1 = 1.0f - t;

        const unsigned char r1 = static_cast<unsigned char>(getRedColorValue(color1)) * t1;
        const unsigned char g1 = static_cast<unsigned char>(getGreenColorValue(color1)) * t1;
        const unsigned char b1 = static_cast<unsigned char>(getBlueColorValue(color1)) * t1;
        const unsigned char a1 = static_cast<unsigned char>(getAlphaColorValue(color1)) * t1;

        const unsigned char r2 = static_cast<unsigned char>(getRedColorValue(color2)) * t;
        const unsigned char g2 = static_cast<unsigned char>(getGreenColorValue(color2)) * t;
        const unsigned char b2 = static_cast<unsigned char>(getBlueColorValue(color2)) * t;
        const unsigned char a2 = static_cast<unsigned char>(getAlphaColorValue(color2)) * t;

        return rgbaColorCode(r1 + r2, g1 + g2, b1 + b2, a1 + a2);
    }

    unsigned long Window::getFrameRateOfMonitor() noexcept {
        DEVMODE mode{};
        mode.dmSize = sizeof(mode);
        mode.dmDriverExtra = 0;

        if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode))
            return mode.dmDisplayFrequency;

        // handle error
        return 60;
    }

    void Window::setCursor(const HCURSOR cursor) noexcept {
        SetCursor(cursor);
    }

    void Window::showCursor(bool visible) noexcept {
        ShowCursor(visible);
    }

    bool Window::isKeyDown(const unsigned short key_code) const noexcept {
        return keyboard.buttons[key_code];
    }

    bool Window::isKeyUp(const unsigned short key_code) const noexcept {
        return !keyboard.buttons[key_code];
    }

    bool Window::isMouseDown(const unsigned short mouse_code) const noexcept {
        return mouse.buttons[mouse_code];
    }

    bool Window::isMouseUp(const unsigned short mouse_code) const noexcept {
        return !mouse.buttons[mouse_code];
    }

    unsigned short Window::getMouseX() const noexcept {
        return mouse.x;
    }

    unsigned short Window::getMouseY() const noexcept {
        return mouse.y;
    }

    vector2u Window::getMousePos() const noexcept {
        return vector2u{ mouse.x, mouse.y };
    }

    short Window::getMouseWheelDelta() const noexcept {
        return mouse.scroll_delta;
    }

}