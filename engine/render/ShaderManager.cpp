#include "ShaderManager.h"
#include "Direct11/Direct3D.h"


void Shader::bind() const
{
	Direct3D& direct = Direct3D::instance();

	direct.context4->VSSetShader(vertexShader.Get(), nullptr, NULL);
	direct.context4->GSSetShader(geometryShader.Get(), nullptr, NULL);
	direct.context4->PSSetShader(pixelShader.Get(), nullptr, NULL);
	direct.context4->IASetInputLayout(inputLayout.ptr.Get());
}

ShaderManager* ShaderManager::s_manager;

void ShaderManager::compile_vertex_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader)
{
	comptr<ID3DBlob> errBlob, vsBlob;
	HRESULT res;
	res = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &vsBlob, &errBlob);
	if (FAILED(res))
	{
		if (errBlob.Get())
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			errBlob.Reset();
		}
		if (vsBlob)
			vsBlob.Reset();
		assert(false && "CompileFromFile Vertex ShaderClass");
	}

	res = Direct3D::instance().device5->CreateVertexShader(
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		nullptr, &shader.vertexShader);
	assert(SUCCEEDED(res) && "CreateVertexShader");

	generate_input_layout(vsBlob, shader);
}

void ShaderManager::compile_geometry_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader)
{
	comptr<ID3DBlob> errBlob, gsBlob;
	HRESULT res;
	res = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, "gs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &gsBlob, &errBlob);

	if(FAILED(res))
	{
		if(errBlob.Get())
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			errBlob.Reset();
		}
		if (gsBlob) gsBlob.Reset();
		assert(res && "CompileFromFile Geometry ShaderClass");
	}

	res = Direct3D::instance().device5->CreateGeometryShader(
		gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &shader.geometryShader
	);
	assert(SUCCEEDED(res) && "CreateGeometryShader");
}

void ShaderManager::compile_pixel_shader(LPCWSTR filename, LPCSTR entry_point, Shader& shader)
{
	comptr<ID3DBlob> errBlob, psBlob;
	HRESULT res;
	res = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &psBlob, &errBlob);
	if (FAILED(res))
	{
		if (errBlob.Get())
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			errBlob.Reset();
		}
		if (psBlob) psBlob.Reset();
		assert(false && "CompileFromFile Pixel ShaderClass");
	}

	res = Direct3D::instance().device5->CreatePixelShader(
		psBlob->GetBufferPointer(), psBlob->GetBufferSize(),nullptr, &shader.pixelShader
	);
	assert(SUCCEEDED(res) && "CreatePixelShader");
}

void ShaderManager::generate_input_layout(const comptr<ID3DBlob>& vs_blob, Shader& shader)
{
	comptr<ID3D11ShaderReflection> vertexShaderReflection;
	D3DReflect(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), IID_ID3D11ShaderReflection, &vertexShaderReflection);

	D3D11_SHADER_DESC shaderDesc;
	vertexShaderReflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (uint32_t i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc{};
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		if (strncmp(paramDesc.SemanticName, PER_INSTANCE_PREFIX, ARRAYSIZE(PER_INSTANCE_PREFIX)- 1) == 0)
		{
			elementDesc.InputSlot = 1;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}
		else
		{
			elementDesc.InputSlot = 0;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;
		}

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		inputLayoutDesc.push_back(elementDesc);
	}

	HRESULT result = Direct3D::instance().device5->CreateInputLayout(&inputLayoutDesc[0],
		inputLayoutDesc.size(), vs_blob->GetBufferPointer(),
		vs_blob->GetBufferSize(), &shader.inputLayout.ptr);
	assert(SUCCEEDED(result) && "CreateInputLayout");
}

Shader& ShaderManager::get_shader(LPCWSTR shader)
{
	assert(shaders.find(shader) != shaders.end() && "Shader is not loaded");
	return *shaders.at(shader);
}

std::shared_ptr<Shader> ShaderManager::get_ptr(LPCWSTR shader)
{
	assert(shaders.find(shader) != shaders.end() && "Shader is not loaded");
	return shaders.at(shader);
}

void ShaderManager::add_shader(LPCWSTR filename, LPCSTR vertex_shader_entry, LPCSTR pixel_shader_entry)
{
	if (shaders.find(filename) != shaders.end()) return;

	std::shared_ptr<Shader> shader{ new Shader };
	compile_vertex_shader(filename, vertex_shader_entry, *shader);
	compile_pixel_shader(filename, pixel_shader_entry, *shader);
	shaders.insert({filename, shader});
}

void ShaderManager::add_shader(LPCWSTR filename, LPCSTR vs_entry, LPCSTR gs_entry, LPCSTR ps_entry)
{
	if (shaders.find(filename) != shaders.end()) return;

	std::shared_ptr<Shader> shader{ new Shader };
	if(vs_entry)
		compile_vertex_shader(filename, vs_entry, *shader);
	if(gs_entry)
		compile_geometry_shader(filename, gs_entry, *shader);
	if(ps_entry)
		compile_pixel_shader(filename, ps_entry, *shader);
	shaders.insert({ filename, shader });
}
