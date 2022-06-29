#pragma once
#include <cstdint>
#include <vector>
#include <windows.h>

struct rgb
{
	uint8_t b, g, r, a;

	rgb() = default;

	rgb(uint8_t r, uint8_t g, uint8_t b): b(b), g(g), r(r)
	{
	}

};

class Screen
{
	uint8_t shrink_;
	uint32_t width_, height_;
	uint32_t bwidth_, bheight_;

	bool wresize;
	BITMAPINFO bmi;
public:

	std::vector<rgb> buffer_;
	std::vector<vec3> hdr_buffer;
	std::vector<vec3> gi_hdr_buffer;

	Screen(uint32_t width = 800, uint32_t height = 600, uint8_t shrink = 4) :
	shrink_(shrink),
	width_(width), height_(height),
	bwidth_(width / shrink), bheight_(height / shrink), wresize(false)
	{
		buffer_.resize(width_ * height_);
		hdr_buffer.resize(width_ * height_);
		gi_hdr_buffer.resize(width_ * height_);

		ZeroMemory(&bmi, sizeof(BITMAPINFO));

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
	}

	rgb& operator[](uint32_t i) { return buffer_[i]; }

	void set(uint32_t row, uint32_t column, const vec3& color)
	{
		uint8_t r = color.x() > 255.f ? 255 : color.x();
		uint8_t g = color.y() > 255.f ? 255 : color.y();
		uint8_t b = color.z() > 255.f ? 255 : color.z();

		buffer_[row * bwidth_ + column] = { r, g, b };
	}

	uint32_t width() const { return width_;}
	uint32_t height() const { return height_; }
	uint32_t buffer_width() const { return bwidth_; }
	uint32_t buffer_height() const { return bheight_; }

	void clear()
	{
		for (int i = 0; i < buffer_.size(); ++i)
		{
			buffer_[i] = {0, 0 , 0};
		}
	}

	void set_shrink(uint8_t shrink)
	{
		shrink_ = shrink;
		bwidth_ = width_ / shrink_;
		bheight_ = height_ / shrink_;
	}

	void init_resize(uint32_t w, uint32_t h)
	{
		width_ = w ;
		height_ = h;
		bwidth_ = w / shrink_;
		bheight_ = h / shrink_;
		wresize = true;
	}

	void resize()
	{
		if (wresize)
		{
			buffer_.resize(width_ * height_);
			hdr_buffer.resize(width_ * height_);
			gi_hdr_buffer.resize(width_ * height_);
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
