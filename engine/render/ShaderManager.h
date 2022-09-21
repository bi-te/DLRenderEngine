#pragma once
#include <unordered_map>

#include "wchar_algorithms.h"
#include "Direct11/Direct3D.h"

struct InputLayout
{
	comptr<ID3D11InputLayout> ptr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
};

struct Shader
{
	comptr<ID3D11VertexShader> vertexShader;
	comptr<ID3D11GeometryShader> geometryShader;
	comptr<ID3D11PixelShader> pixelShader;
	InputLayout inputLayout;

	void bind() const
	{
		Direct3D& direct = Direct3D::instance();

		direct.context4->VSSetShader(vertexShader.Get(), nullptr, NULL);
		direct.context4->GSSetShader(geometryShader.Get(), nullptr, NULL);
		direct.context4->PSSetShader(pixelShader.Get(), nullptr, NULL);
		direct.context4->IASetInputLayout(inputLayout.ptr.Get());
	}

	void reset()
	{
		vertexShader.Reset();
		geometryShader.Reset();
		pixelShader.Reset();
		inputLayout.ptr.Reset();
	}
};

const char PER_INSTANCE_PREFIX[] = "Inst_";


class ShaderManager
{
	static ShaderManager* s_manager;
	ShaderManager() = default;

	ShaderManager(const ShaderManager& other) = delete;
	ShaderManager(ShaderManager&& other) noexcept = delete;
	ShaderManager& operator=(const ShaderManager& other) = delete;
	ShaderManager& operator=(ShaderManager&& other) noexcept = delete;

	std::unordered_map<LPCWSTR, std::shared_ptr<Shader>, pwchar_hash, pwchar_comparator> shaders;

	void compile_vertex_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader);
	void compile_geometry_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader);
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
		for (auto shader : s_manager->shaders)
			shader.second->reset();
		delete s_manager;
	}

	Shader& get_shader(LPCWSTR shader);
	std::shared_ptr<Shader> get_ptr(LPCWSTR shader);

	void add_shader(LPCWSTR filename, LPCSTR vertex_shader_entry, LPCSTR pixel_shader_entry);
	void add_shader(LPCWSTR filename, LPCSTR vs_entry, LPCSTR gs_entry, LPCSTR ps_entry);
};

