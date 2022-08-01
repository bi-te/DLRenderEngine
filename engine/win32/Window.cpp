#include "Window.h"

#include <windowsx.h>


#include "imgui/imgui.h"
#include "Controller.h"
#include "imgui/ImGuiManager.h"
#include "imgui/imgui_impl_win32.h"


Window::Window(LPCWSTR class_name, HINSTANCE hInstance)
{
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = class_name;

    RegisterClassEx(&wc);
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
        return TRUE;
    }
    return ((Window*)(GetWindowLongPtr(hWnd, GWLP_USERDATA)))->classWindowProc(hWnd, message, wParam, lParam);
}

void Window::create_window(LPCWSTR name, LONG width, LONG height)
{
    width_ = width;
    height_ = height;
    RECT rect{ 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    window = CreateWindowEx(NULL, wc.lpszClassName, name, WS_OVERLAPPEDWINDOW,
        300, 30, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, this);

    init_swap_chain();
}

void Window::init_swap_chain()
{
    Direct3D& globals = Direct3D::instance();
    DXGI_SWAP_CHAIN_DESC1 desc;
    ZeroMemory(&desc, sizeof(desc));

    RECT winrect;
    GetClientRect(window, &winrect);

    width_ = winrect.right - winrect.left;
    height_ = winrect.bottom - winrect.top;

    desc.Width = width_;
    desc.Height = height_;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_NONE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = 0;
    desc.Stereo = false;

    HRESULT result = globals.factory5->CreateSwapChainForHwnd(globals.device5.Get(), window, &desc,
        NULL, NULL, &swap_chain);
    assert(result >= 0 && "CreateSwapChainForHwnd");

    init_render_target_view();
}

void Window::init_render_target_view()
{
    if (!swap_chain.Get()) return;

    comptr<ID3D11Texture2D> back_buffer;
    HRESULT result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer);
    assert(SUCCEEDED(result) && "GetBuffer");

    result = Direct3D::instance().device5->CreateRenderTargetView(back_buffer.Get(), NULL, &target_view);
    assert(SUCCEEDED(result) && "CreateRenderTargetView");
}


void Window::resize_buffer(uint32_t new_width, uint32_t new_height)
{
    if (!swap_chain.Get())
        return;

    target_view.Reset();
    HRESULT result = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    assert(SUCCEEDED(result) && "ResizeBuffers");

    width_ = new_width;
    height_ = new_height;

    init_render_target_view();
}

void Window::bind_target(const comptr<ID3D11DepthStencilView>& dsView) const 
{
    D3D11_VIEWPORT viewport = {0.f, 0.f,FLOAT(width_), FLOAT(height_),0.f, 1.f};

    Direct3D::instance().context4->OMSetRenderTargets(1, target_view.GetAddressOf(), dsView.Get());
    Direct3D::instance().context4->RSSetViewports(1, &viewport);
}

void Window::clear_buffer()
{
    Direct3D::instance().context4->ClearRenderTargetView(target_view.Get(), WINDOW_COLOR);
}

LRESULT Window::classWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGuiManager::active() && ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return 0;

    const ImGuiIO& io = ImGui::GetIO();

    int16_t x = GET_X_LPARAM(lParam);
    int16_t y = GET_Y_LPARAM(lParam);
    switch (message)
    {
    case WM_LBUTTONUP:
        if (io.WantCaptureMouse) break;
        for (auto listener : listeners) { listener->MouseEvent(LMOUSE, (BUTTON)((message + 1) % 3), x, y); }
        break;
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
        if (io.WantCaptureMouse) break;
        for (auto listener : listeners) { listener->MouseEvent((Key)wParam, (BUTTON)((message + 1) % 3), x, y); }
        break;
    case WM_RBUTTONUP:
        if (io.WantCaptureMouse) break;
        for (auto listener : listeners) { listener->MouseEvent(RMOUSE, (BUTTON)((message + 1) % 3), x, y); }
        break;
    case WM_MOUSEMOVE:
        if (io.WantCaptureMouse) break;
        for (auto listener : listeners) { listener->MouseEvent(NONE, (BUTTON)((message + 1) % 3), x, y); }
        break;

    case WM_MOUSEWHEEL:
        if (io.WantCaptureMouse) break;
        for (auto listener : listeners) { listener->MouseWheelEvent(GET_WHEEL_DELTA_WPARAM(wParam) / 120); }
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
        if (io.WantCaptureKeyboard)break;
        for (auto listener : listeners) { listener->KeyEvent((Key)wParam , !(bool)((1u<<31) & lParam)); }
        break;

    case WM_SIZE:
        if (LOWORD(lParam) && HIWORD(lParam))
        {
            resize_buffer(LOWORD(lParam), HIWORD(lParam));
            for (auto listener : listeners) { listener->OnResize(LOWORD(lParam), HIWORD(lParam)); }
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
