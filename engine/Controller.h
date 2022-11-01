#pragma once
#include <cstdint>
#include <iostream>

#include "EngineClock.h"
#include "Scene.h"
#include "PostProcess.h"
#include "math/math.h"
#include "win32/Window.h"
#include "win32/WinListener.h"

struct Mouse
{
	int32_t lmb_x, lmb_y;
	int32_t prev_x, prev_y;
	int32_t x, y;
	int32_t wheel;

	BUTTON rmb;
	BUTTON lmb;

};

struct Keyboard
{
	const uint8_t key_number = 254;
	std::vector<bool> keys;
	std::vector<bool> previus_key_state;

	Keyboard()
	{
		keys.resize(key_number + 1);
		previus_key_state.resize(key_number + 1);
	}

	bool once_pressed(Key key);
};

struct InputState
{
	Mouse mouse;
	Keyboard keyboard;
};

class  Controller : public IWinListener
{
	const float movement_speed = 20.f,
		shift = 5.f, dspeed = 1.1f,
		rotation_speed = 1.f,
		rotation_speed_mouse = 2.f * PI;

	IntersectionQuery record;
public:
	Scene scene;
	Camera camera;
	Window& window;
	PostProcess postProcess;

	explicit Controller(Window& window) : window(window)
	{
	}

	InputState is{};

	void render();
	void render_reset()
	{
		scene.render_reset();
		window.render_reset();
		postProcess.render_reset();
	}

	void move_camera(const vec3f& offset, const Angles& angles)
	{
		camera.add_relative_angles(angles);
		camera.add_relative_offset(offset);
		camera.update_matrices();
	}

	void OnSizeMoved() override { EngineClock::instance().skip(); }
	void OnResize(uint32_t width, uint32_t height) override;
	void MouseWheelEvent(uint32_t count) override { is.mouse.wheel += count; }
	void MouseEvent(Key button, BUTTON status, uint32_t x_pos, uint32_t y_pos) override;
	void KeyEvent(Key key, bool status) override { is.keyboard.keys[key] = status; }

	void process_input(float dt);
	void process_gui_input();

	void init_scene();
	void start_appearance();
	void update_appearance_instances(float dt);

	void spawn_decal();
};
