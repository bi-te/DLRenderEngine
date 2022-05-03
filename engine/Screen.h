#pragma once
#include <cstdint>
#include <vector>

struct rgb
{
	uint8_t b, g, r, a;
};

struct Screen
{
	uint16_t width, height;
	std::vector<rgb> buffer;
	bool wresize;
	BITMAPINFO bmi;

	Screen(uint16_t width = 800, uint16_t height = 600) : width(width), height(height), wresize(false)
	{
		buffer.resize(width * height);
		ZeroMemory(&bmi, sizeof(BITMAPINFO));

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
	}

	void init_resize(int16_t w, int16_t h)
	{
		width = w ;
		height = h;
		wresize = true;
	}

	void resize()
	{
		if (wresize)
		{
			buffer.resize(width * height);
			wresize = false;
		}
	}

	void update(HWND hwnd)
	{
		HDC hdc = GetDC(hwnd);

		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;

		SetDIBitsToDevice(hdc, 0, 0, width, height, 0, 0, 0, height,
			buffer.data(), &bmi, DIB_RGB_COLORS);
	}
};
