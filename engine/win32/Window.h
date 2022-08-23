#pragma once


#include <cstdint>
#include <vector>

#include "WinListener.h"
#include "win32/win.h"
#include "render/Direct11/d3d.h"
#include "render/Direct11/RenderBuffer.h"

const FLOAT WINDOW_COLOR[4] { 0.2f, 0.2f, 0.2f, 1.f };

class Window
{
    WNDCLASSEX wc;
    HWND window;

    uint32_t width_, height_;
public:
    comptr<IDXGISwapChain1> swap_chain;
    RenderBuffer buffer;

    std::vector<IWinListener*> listeners;

    Window(LPCWSTR class_name, HINSTANCE hInstance);

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK classWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND handle() const { return window; }

    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }

    void create_window(LPCWSTR name, LONG width, LONG height);
    void init_swap_chain();
    void init_render_target_view();
    void resize_buffer(uint32_t new_width, uint32_t new_height);

    void show_window(int nCmdShow) { ShowWindow(window, nCmdShow); }
    void swap_buffer() { swap_chain->Present(0, 0); }
    
    void clear_buffer();

    void render_reset()
    {
        swap_chain.Reset();
        buffer.reset();
    }
};
