#pragma once
#include <cstdint>
#include <vector>
#include <windows.h>

struct rgb
{
	uint8_t b, g, r, a;
};

class Screen
{
	uint16_t width_, height_;
	std::vector<rgb> buffer_;
	bool wresize;
	BITMAPINFO bmi;

public:

	Screen(uint16_t width = 800, uint16_t height = 600) : width_(width), height_(height), wresize(false)
	{
		buffer_.resize(width * height);
		ZeroMemory(&bmi, sizeof(BITMAPINFO));

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
	}

	rgb& operator[](uint32_t i)
	{
		return buffer_[i];
	}

	uint16_t width() const
	{
		return width_;
	}

	uint16_t height() const
	{
		return height_;
	}

	void init_resize(int16_t w, int16_t h)
	{
		width_ = w ;
		height_ = h;
		wresize = true;
	}

	void resize()
	{
		if (wresize)
		{
			buffer_.resize(width_ * height_);
			wresize = false;
		}
	}

	void update(HWND hwnd)
	{
		HDC hdc = GetDC(hwnd);

		bmi.bmiHeader.biWidth = width_;
		bmi.bmiHeader.biHeight = -height_;

		SetDIBitsToDevice(hdc, 0, 0, width_, height_, 0, 0, 0, height_,
			buffer_.data(), &bmi, DIB_RGB_COLORS);
	}
};
