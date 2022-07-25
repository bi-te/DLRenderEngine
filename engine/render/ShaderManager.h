#pragma once
#include <vector>

#include "Direct11/Direct3D.h"

struct Shader
{
	comptr<ID3D11VertexShader> vertexShader;
	comptr<ID3D11PixelShader> pixelShader;
	comptr<ID3D11InputLayout> inputLayout;
};

class ShaderManager
{
	std::vector<Shader> shaders;

	static ShaderManager* s_manager;
	ShaderManager()
	{
	}

	void compile_vertex_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader);
	void compile_pixel_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader);
	void generate_input_layout(const comptr<ID3DBlob>& vs_blob, Shader& shader);
public:

	static void init()
	{
		if (s_manager) reset();

		s_manager = new ShaderManager;
	}

	static ShaderManager& instance()
	{
		assert(s_manager && "ShaderManager not initialized");
		return *s_manager;
	}

	static void reset()
	{
		delete s_manager;
	}

	const Shader& operator [](uint32_t ind) { return shaders[ind]; }

	uint32_t add_shader(LPCWSTR filename, LPCSTR vertex_shader_entry, LPCSTR pixel_shader_entry);
	uint32_t add_shader(LPCWSTR vertex_shader_file, LPCSTR vertex_shader_entry,
		LPCWSTR pixel_shader_file, LPCSTR pixel_shader_entry);
};

