#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace ImGuiManager
{
	inline bool& active()
	{
		static bool act = false;
		return act;
	}

	inline void init_context()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}
	inline void init_render(HWND window, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(device, context);
	}

	inline void new_frame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	inline void flush()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	inline void reset()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
};