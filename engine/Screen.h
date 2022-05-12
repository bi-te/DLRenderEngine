#pragma once
#include <cstdint>
#include <vector>
#include <windows.h>

struct rgb
{
	uint8_t b, g, r, a;

	rgb() = default;

	rgb(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b)
	{
	}
};

class Screen
{
	uint16_t bwidth_, bheight_;
	uint16_t width_, height_;
	std::vector<rgb> buffer_;
	bool wresize;
	BITMAPINFO bmi;

public:

	Screen(uint16_t width = 800, uint16_t height = 600) : width_(width), height_(height),
	bwidth_(width / 2), bheight_(height / 2), wresize(false)
	{
		buffer_.resize(bwidth_ * bheight_);
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

	uint16_t buffer_width() const
	{
		return bwidth_;
	}

	uint16_t buffer_height() const
	{
		return bheight_;
	}

	void init_resize(int16_t w, int16_t h)
	{
		width_ = w ;
		height_ = h;
		bwidth_ = w / 2;
		bheight_ = h / 2;
		wresize = true;
	}

	void resize()
	{
		if (wresize)
		{
			buffer_.resize(bwidth_ * bheight_);
			wresize = false;
		}
	}

	void update(HWND hwnd)
	{
		HDC hdc = GetDC(hwnd);

		bmi.bmiHeader.biWidth = bwidth_;
		bmi.bmiHeader.biHeight = bheight_;

		//SetDIBitsToDevice(hdc, 0, 0, width_, height_, 0, 0, 0, height_,
		//	buffer_.data(), &bmi, DIB_RGB_COLORS);

		StretchDIBits(hdc, 0, 0, width_, height_, 0, 0,
			bwidth_, bheight_, buffer_.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
	}


};
