#include "DXUT.h"
#include "SDKmisc.h"
#include <array>
#include "RenderTarget.h"
#include "renderer.h"
#include "mesh.h"

namespace tpot
{

class RasterStates
{
public:

private:
	ID3D11RasterizerState* pRasterizerStates_[RASTERIZER_STATE::MAX];

public:

	RasterStates( ID3D11Device *pd3dDevice );
	~RasterStates();

	void set(RASTERIZER_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext);
};

class SamplerStates
{
public:

private:
	ID3D11SamplerState* pSamplerStates_[SAMPLER_STATE::MAX];

public:

	SamplerStates(ID3D11Device *pd3dDevice);
	~SamplerStates();

	void set(UINT slot, SAMPLER_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext);
};

class DepthStencilStates
{
public:

private:
	ID3D11DepthStencilState* pDepthStencilStates_[DEPTH_STATE::MAX];

public:

	DepthStencilStates(ID3D11Device *pd3dDevice);
	~DepthStencilStates();

	void set(DEPTH_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext);
};


class ConstantBuffer
{
	ID3D11Buffer*                       pCB_;
public:
	ConstantBuffer( UINT size, ID3D11Device *pd3dDevice );
	~ConstantBuffer();

	void *Map(ID3D11DeviceContext *pd3dImmediateContext);
	void UmMap(ID3D11DeviceContext *pd3dImmediateContext);
	ID3D11Buffer* const *get();
};


ConstantBuffer::ConstantBuffer( UINT size, ID3D11Device *pd3dDevice )
{

	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = size;
	HRESULT hr = pd3dDevice->CreateBuffer( &Desc, NULL, &pCB_ );
	if ( hr != S_OK )
	{
		return;
	}

	DXUT_SetDebugName( pCB_, "CB_DEFAULT" );
}


ConstantBuffer::~ConstantBuffer()
{
	SAFE_RELEASE( pCB_ );
}


void *ConstantBuffer::Map(ID3D11DeviceContext *pd3dImmediateContext)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dImmediateContext->Map( pCB_, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );

	return MappedResource.pData;
}


void ConstantBuffer::UmMap(ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->Unmap( pCB_, 0 );
}


ID3D11Buffer* const *ConstantBuffer::get()
{
	return &pCB_;
}


RasterStates::RasterStates( ID3D11Device *pd3dDevice )
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory( &RasterDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.DepthClipEnable = TRUE;

	hr = pd3dDevice->CreateRasterizerState(&RasterDesc, &pRasterizerStates_[RASTERIZER_STATE::SOLID]);
	if( hr != S_OK )
	{
		return;
	}
	DXUT_SetDebugName(pRasterizerStates_[RASTERIZER_STATE::SOLID], "Solid");

	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = pd3dDevice->CreateRasterizerState(&RasterDesc, &pRasterizerStates_[RASTERIZER_STATE::WIREFRAME]);
	if( hr != S_OK )
	{
		return;
	}
	DXUT_SetDebugName(pRasterizerStates_[RASTERIZER_STATE::WIREFRAME], "Wireframe");
}


RasterStates::~RasterStates()
{
	for( auto it : pRasterizerStates_ ){
		SAFE_RELEASE( it );
	}
}

void RasterStates::set(RASTERIZER_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->RSSetState( pRasterizerStates_[id] );
}

SamplerStates::SamplerStates(ID3D11Device *pd3dDevice)
{
	HRESULT hr;

	// Point
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 1.0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V(pd3dDevice->CreateSamplerState(&SamDesc, &pSamplerStates_[SAMPLER_STATE::POINT]));
	DXUT_SetDebugName(pSamplerStates_[SAMPLER_STATE::POINT], "Point");

	// Linear
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	V(pd3dDevice->CreateSamplerState(&SamDesc, &pSamplerStates_[SAMPLER_STATE::LINEAR]));
	DXUT_SetDebugName(pSamplerStates_[SAMPLER_STATE::LINEAR], "Linear");

}

SamplerStates::~SamplerStates()
{
	for( auto it : pSamplerStates_ )
	{
	    SAFE_RELEASE( it );
	}
}

void SamplerStates::set(UINT slot, SAMPLER_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetSamplers(slot, 1, &pSamplerStates_[id]);
}

DepthStencilStates::DepthStencilStates(ID3D11Device *pd3dDevice)
{
	HRESULT hr;

	pDepthStencilStates_[DEPTH_STATE::UNUSED] = nullptr;

	D3D11_DEPTH_STENCIL_DESC Desc;

	ZeroMemory(&Desc, sizeof(Desc));
	V(pd3dDevice->CreateDepthStencilState(&Desc, &pDepthStencilStates_[DEPTH_STATE::DISABLE]));

}

DepthStencilStates::~DepthStencilStates()
{
	for (auto it : pDepthStencilStates_)
	{
		SAFE_RELEASE(it);
	}
}

void DepthStencilStates::set(DEPTH_STATE::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->OMSetDepthStencilState(pDepthStencilStates_[id], 0);
}

class Shader
{
	std::array<ID3D11VertexShader*,		VS::MAX>	pVertexShader_;
	std::array<ID3D11HullShader*,		HS::MAX>	pHullShader_;
	std::array<ID3D11DomainShader*,		DS::MAX>	pDomainShader_;
	std::array<ID3D11GeometryShader*,	GS::MAX>	pGeometryShader_;
	std::array<ID3D11PixelShader*,		PS::MAX>	pPixelShader_;

	std::array<ID3D11InputLayout*,		VS::MAX>	pLayout_;
	std::array<ConstantBuffer*,			VS::MAX>	pCB_;

	UINT								vs_current_;

public:
	Shader( ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext );
	~Shader();

	void setVS(VS::ID id, ID3D11DeviceContext *pd3dImmediateContext );
	void setHS(HS::ID id, ID3D11DeviceContext *pd3dImmediateContext );
	void setDS(DS::ID id, ID3D11DeviceContext *pd3dImmediateContext);
	void setGS(GS::ID id, ID3D11DeviceContext *pd3dImmediateContext);
	void setPS(PS::ID id, ID3D11DeviceContext *pd3dImmediateContext);
	void setCS(ID3D11DeviceContext *pd3dImmediateContext );

	void setCB_VS( ID3D11DeviceContext *pd3dImmediateContext );
	void setCB_HS( ID3D11DeviceContext *pd3dImmediateContext );
	void setCB_DS( ID3D11DeviceContext *pd3dImmediateContext );
	void setCB_GS( ID3D11DeviceContext *pd3dImmediateContext );
	void setCB_PS( ID3D11DeviceContext *pd3dImmediateContext );

	void *Map(ID3D11DeviceContext *pd3dImmediateContext);
	void UmMap(ID3D11DeviceContext *pd3dImmediateContext);

	ID3D11InputLayout *InputLayout();
};


//--------------------------------------------------------------------------------------
// Find and compile the specified shader
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint,
                               LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
	HRESULT hr = S_OK;

	// find the file
	WCHAR str[MAX_PATH];
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows
	// the shaders to be optimized and to run exactly the way they will run in
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile( str, pDefines, NULL, szEntryPoint, szShaderModel,
	                            dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if( FAILED( hr ) )
	{
		if( pErrorBlob != NULL )
		{
			OutputDebugStringA( ( char* )pErrorBlob->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBlob );
		return hr;
	}
	SAFE_RELEASE( pErrorBlob );

	return S_OK;
}

Shader::Shader( ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext )
{
	vs_current_ = VS::MAX;

	// Compile shaders
	ID3DBlob* pBlobVS = NULL;
	ID3DBlob* pBlobHSInt = NULL;
	ID3DBlob* pBlobHSFracEven = NULL;
	ID3DBlob* pBlobHSFracOdd = NULL;
	ID3DBlob* pBlobDS = NULL;
	ID3DBlob* pBlobPS = NULL;
	ID3DBlob* pBlobPSSolid = NULL;

	// This macro is used to compile the hull shader with different partition modes
	// Please see the partitioning mode attribute for the hull shader for more information
	D3D_SHADER_MACRO integerPartitioning[] = { { "BEZIER_HS_PARTITION", "\"integer\"" }, { 0 } };
	D3D_SHADER_MACRO fracEvenPartitioning[] = { { "BEZIER_HS_PARTITION", "\"fractional_even\"" }, { 0 } };
	D3D_SHADER_MACRO fracOddPartitioning[] = { { "BEZIER_HS_PARTITION", "\"fractional_odd\"" }, { 0 } };

	HRESULT hr;
	V( CompileShaderFromFile( L"sample.hlsl", NULL, "BezierVS", "vs_5_0",  &pBlobVS ) );
	V( CompileShaderFromFile( L"sample.hlsl", integerPartitioning, "BezierHS", "hs_5_0", &pBlobHSInt ) );
	V( CompileShaderFromFile( L"sample.hlsl", fracEvenPartitioning, "BezierHS", "hs_5_0", &pBlobHSFracEven ) );
	V( CompileShaderFromFile( L"sample.hlsl", fracOddPartitioning, "BezierHS", "hs_5_0", &pBlobHSFracOdd ) );
	V( CompileShaderFromFile( L"sample.hlsl", NULL, "BezierDS", "ds_5_0", &pBlobDS ) );
	V( CompileShaderFromFile( L"sample.hlsl", NULL, "BezierPS", "ps_5_0", &pBlobPS ) );
	V( CompileShaderFromFile( L"sample.hlsl", NULL, "SolidColorPS", "ps_5_0", &pBlobPSSolid ) );

	// Create shaders
	V(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &pVertexShader_[VS::BEZIER]));
	DXUT_SetDebugName(pVertexShader_[VS::BEZIER], "BezierVS");

	V(pd3dDevice->CreateHullShader(pBlobHSInt->GetBufferPointer(), pBlobHSInt->GetBufferSize(), NULL, &pHullShader_[HS::PARTITION_INTEGER]));
	DXUT_SetDebugName( pHullShader_[HS::PARTITION_INTEGER], "BezierHS int" );

	V(pd3dDevice->CreateHullShader(pBlobHSFracEven->GetBufferPointer(), pBlobHSFracEven->GetBufferSize(), NULL, &pHullShader_[HS::PARTITION_FRACTIONAL_EVEN]));
	DXUT_SetDebugName(pHullShader_[HS::PARTITION_FRACTIONAL_EVEN], "BezierHS frac even");

	V(pd3dDevice->CreateHullShader(pBlobHSFracOdd->GetBufferPointer(), pBlobHSFracOdd->GetBufferSize(), NULL, &pHullShader_[HS::PARTITION_FRACTIONAL_ODD]));
	DXUT_SetDebugName(pHullShader_[HS::PARTITION_FRACTIONAL_ODD], "BezierHS frac odd");

	V(pd3dDevice->CreateDomainShader(pBlobDS->GetBufferPointer(), pBlobDS->GetBufferSize(), NULL, &pDomainShader_[DS::BEZIER]));
	DXUT_SetDebugName(pDomainShader_[DS::BEZIER], "BezierDS");

	V(pd3dDevice->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), NULL, &pPixelShader_[PS::BEZIER]));
	DXUT_SetDebugName(pPixelShader_[PS::BEZIER], "BezierPS");

	V(pd3dDevice->CreatePixelShader(pBlobPSSolid->GetBufferPointer(), pBlobPSSolid->GetBufferSize(), NULL, &pPixelShader_[PS::SOLID_COLOR]));
	DXUT_SetDebugName(pPixelShader_[PS::SOLID_COLOR], "SolidColorPS");

	// Create our vertex input layout - this matches the BEZIER_CONTROL_POINT structure
	const D3D11_INPUT_ELEMENT_DESC layout_BEZIER[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	V(pd3dDevice->CreateInputLayout(layout_BEZIER, ARRAYSIZE(layout_BEZIER), pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), &pLayout_[VS::BEZIER]));
	DXUT_SetDebugName(pLayout_[VS::BEZIER], "Primary");

	SAFE_RELEASE( pBlobVS );
	SAFE_RELEASE( pBlobHSInt );
	SAFE_RELEASE( pBlobHSFracEven );
	SAFE_RELEASE( pBlobHSFracOdd );
	SAFE_RELEASE( pBlobDS );
	SAFE_RELEASE( pBlobPS );
	SAFE_RELEASE( pBlobPSSolid );

	pCB_[VS::BEZIER] = new ConstantBuffer(sizeof(CB_BEZIER), pd3dDevice);

	// Render Scene
	V(CompileShaderFromFile(L"scene.hlsl", NULL, "VS_RenderScene", "vs_5_0", &pBlobVS));
	V(CompileShaderFromFile(L"scene.hlsl", NULL, "PS_RenderScene", "ps_5_0", &pBlobPS));
	V(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &pVertexShader_[VS::SCENE]));
	DXUT_SetDebugName(pVertexShader_[VS::SCENE], "SceneVS");
	V(pd3dDevice->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), NULL, &pPixelShader_[PS::SCENE]));
	DXUT_SetDebugName(pPixelShader_[PS::SCENE], "ScenePS");

	const D3D11_INPUT_ELEMENT_DESC SceneLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pd3dDevice->CreateInputLayout(SceneLayout, ARRAYSIZE(SceneLayout), pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), &pLayout_[VS::SCENE]);
	DXUT_SetDebugName(pLayout_[VS::SCENE], "SceneLayout");

	SAFE_RELEASE(pBlobVS);
	SAFE_RELEASE(pBlobPS);

	pCB_[VS::SCENE] = new ConstantBuffer(sizeof(CB_SCENE), pd3dDevice);

	// Render Decal
	V(CompileShaderFromFile(L"decal.hlsl", NULL, "VS", "vs_5_0", &pBlobVS));
	V(CompileShaderFromFile(L"decal.hlsl", NULL, "PS", "ps_5_0", &pBlobPS));
	V(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &pVertexShader_[VS::DECAL]));
	DXUT_SetDebugName(pVertexShader_[VS::DECAL], "DecalVS");
	V(pd3dDevice->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), NULL, &pPixelShader_[PS::DECAL]));
	DXUT_SetDebugName(pPixelShader_[PS::DECAL], "DecalPS");

	const D3D11_INPUT_ELEMENT_DESC DecalLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pd3dDevice->CreateInputLayout(DecalLayout, ARRAYSIZE(DecalLayout), pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), &pLayout_[VS::DECAL]);
	DXUT_SetDebugName(pLayout_[VS::DECAL], "DecalLayout");

	SAFE_RELEASE(pBlobVS);
	SAFE_RELEASE(pBlobPS);

	pCB_[VS::DECAL] = new ConstantBuffer(sizeof(CB_DECAL), pd3dDevice);

	// Render Shadow maps
	V(CompileShaderFromFile(L"shadow.hlsl", NULL, "VSMain", "vs_5_0", &pBlobVS));
	V(CompileShaderFromFile(L"shadow.hlsl", NULL, "PSMain", "ps_5_0", &pBlobPS));
	V(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &pVertexShader_[VS::SHADOW]));
	DXUT_SetDebugName(pVertexShader_[VS::SHADOW], "ShadowVS");
	V(pd3dDevice->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), NULL, &pPixelShader_[PS::SHADOW]));
	DXUT_SetDebugName(pPixelShader_[PS::SHADOW], "ShadowPS");

	const D3D11_INPUT_ELEMENT_DESC LayoutShadow[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pd3dDevice->CreateInputLayout(LayoutShadow, ARRAYSIZE(LayoutShadow), pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), &pLayout_[VS::SHADOW]);
	DXUT_SetDebugName(pLayout_[VS::SHADOW], "ShadowLayout");

	SAFE_RELEASE(pBlobVS);
	SAFE_RELEASE(pBlobPS);

	pCB_[VS::SHADOW] = new ConstantBuffer(sizeof(CB_SHADOW), pd3dDevice);

	// TAA
	V(CompileShaderFromFile(L"taa.hlsl", NULL, "VS", "vs_5_0", &pBlobVS));
	V(CompileShaderFromFile(L"taa.hlsl", NULL, "PS", "ps_5_0", &pBlobPS));
	V(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &pVertexShader_[VS::TAA]));
	DXUT_SetDebugName(pVertexShader_[VS::TAA], "TAA VS");
	V(pd3dDevice->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), NULL, &pPixelShader_[PS::TAA]));
	DXUT_SetDebugName(pPixelShader_[PS::TAA], "TAA PS");

	const D3D11_INPUT_ELEMENT_DESC LayoutTaa[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pd3dDevice->CreateInputLayout(LayoutTaa, ARRAYSIZE(LayoutTaa), pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), &pLayout_[VS::TAA]);
	DXUT_SetDebugName(pLayout_[VS::TAA], "TAA Layout");

	SAFE_RELEASE(pBlobVS);
	SAFE_RELEASE(pBlobPS);

	pCB_[VS::TAA] = new ConstantBuffer(sizeof(CB_TAA), pd3dDevice);
}


Shader::~Shader()
{
	std::for_each(pCB_.begin(), pCB_.end(), [](ConstantBuffer* it) {
		SAFE_DELETE(it);
	});

	std::for_each(pLayout_.begin(), pLayout_.end(), [](ID3D11InputLayout* it) {
		SAFE_RELEASE(it);
	});

	std::for_each( pVertexShader_.begin(), pVertexShader_.end(), [](ID3D11VertexShader* it) {
		SAFE_RELEASE( it );
	});
	std::for_each( pHullShader_.begin(),pHullShader_.end(), [](ID3D11HullShader* it) {
		SAFE_RELEASE( it );
	});
	std::for_each( pDomainShader_.begin(),pDomainShader_.end(), [](ID3D11DomainShader* it) {
		SAFE_RELEASE( it );
	});
	std::for_each( pGeometryShader_.begin(),pGeometryShader_.end(), [](ID3D11GeometryShader* it) {
		SAFE_RELEASE( it );
	});
	std::for_each( pPixelShader_.begin(),pPixelShader_.end(), [](ID3D11PixelShader* it) {
		SAFE_RELEASE( it );
	});
}

void Shader::setVS( VS::ID id, ID3D11DeviceContext *pd3dImmediateContext )
{
	vs_current_ = id;
	pd3dImmediateContext->VSSetShader( pVertexShader_[id], NULL, 0 );
}
void Shader::setHS(HS::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->HSSetShader( pHullShader_[id], NULL, 0 );
}
void Shader::setDS(DS::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->DSSetShader( pDomainShader_[id], NULL, 0 );
}
void Shader::setGS(GS::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->GSSetShader( pGeometryShader_[id], NULL, 0 );
}
void Shader::setPS(PS::ID id, ID3D11DeviceContext *pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetShader( pPixelShader_[id], NULL, 0 );
}
void Shader::setCS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->CSSetShader(NULL, NULL, 0);
}

void Shader::setCB_VS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, pCB_[vs_current_]->get());
}
void Shader::setCB_HS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->HSSetConstantBuffers(0, 1, pCB_[vs_current_]->get());
}
void Shader::setCB_DS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->DSSetConstantBuffers(0, 1, pCB_[vs_current_]->get());
}
void Shader::setCB_GS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->GSSetConstantBuffers(0, 1, pCB_[vs_current_]->get());
}
void Shader::setCB_PS( ID3D11DeviceContext *pd3dImmediateContext )
{
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, pCB_[vs_current_]->get());
}

void *Shader::Map(ID3D11DeviceContext *pd3dImmediateContext)
{
	return pCB_[vs_current_]->Map(pd3dImmediateContext);
}
void Shader::UmMap(ID3D11DeviceContext *pd3dImmediateContext)
{
	pCB_[vs_current_]->UmMap(pd3dImmediateContext);
}

ID3D11InputLayout *Shader::InputLayout()
{
	return pLayout_[vs_current_];
}

Renderer::Renderer( ID3D11Device *pd3dDevice )
{
	pd3dDevice_ = pd3dDevice;
	pd3dImmediateContext_ = DXUTGetD3D11DeviceContext();

	TR_ = new RenderTargets(pd3dDevice);
	RS_ = new RasterStates(pd3dDevice);
	SAMP_ = new SamplerStates(pd3dDevice);
	DSS_ = new DepthStencilStates(pd3dDevice);
	Shader_ = new Shader(pd3dDevice, pd3dImmediateContext_);
}

Renderer::~Renderer()
{
	SAFE_DELETE(Shader_);
	SAFE_DELETE(DSS_);
	SAFE_DELETE(SAMP_);
	SAFE_DELETE(RS_);
	SAFE_DELETE(TR_);

	pd3dDevice_ = nullptr;
	pd3dImmediateContext_ = nullptr;
}

void Renderer::ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
	pBackBufferSurfaceDesc_ = *pBackBufferSurfaceDesc;

	TR_->ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc);
}
void Renderer::ReleasingSwapChain()
{
	TR_->ReleasingSwapChain();
}


void Renderer::Draw( Mesh *pMesh )
{
	// Set the input assembler
	// This sample uses patches with 16 control points each
	// Although the Mobius strip only needs to use a vertex buffer,
	// you can use an index buffer as well by calling IASetIndexBuffer().
	pd3dImmediateContext_->IASetInputLayout(Shader_->InputLayout());

	pMesh->Draw(pd3dImmediateContext_);
}

ID3D11DeviceContext *Renderer::pd3dImmediateContext()
{
	return pd3dImmediateContext_;
}

UINT Renderer::create(RENDER_TARGET::TYPE type, UINT width, UINT height)
{
	return TR_->create(pd3dDevice_, type, width, height);
}

void Renderer::setRenderTarget(UINT id)
{
	TR_->set(id, pd3dImmediateContext_);
}
void Renderer::setDepth(UINT id)
{
	TR_->set(id, pd3dImmediateContext_);
}
ID3D11ShaderResourceView *Renderer::getTexture(UINT id)
{
	return TR_->get(id);
}
void Renderer::pushRenderTarget()
{
	TR_->pushDefault(pd3dImmediateContext_);
}
void Renderer::popRenderTarget()
{
	TR_->popDefault(pd3dImmediateContext_);
}
D3DXMATRIX Renderer::screenProjMatrix()
{
	D3DXMATRIX mP, mT, mS;
	D3DXMatrixOrthoRH(&mP, (float)pBackBufferSurfaceDesc_.Width, (float)pBackBufferSurfaceDesc_.Height, 0, 1.0f);
	D3DXMatrixScaling(&mS, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&mT, -1.0f, +1.0f, 0.0f);

	return  mP * mS * mT;
}


void Renderer::Clear( UINT color )
{
	TR_->Clear(color, pd3dImmediateContext_);
}

void Renderer::ClearDepth( float depth )
{
	TR_->ClearDepth(depth, pd3dImmediateContext_);
}

void Renderer::set(RASTERIZER_STATE::ID id)
{
	RS_->set(id, pd3dImmediateContext_);
}
void Renderer::set(UINT slot, SAMPLER_STATE::ID id)
{
	SAMP_->set(slot, id, pd3dImmediateContext_);
}
void Renderer::set(VS::ID id)
{
	Shader_->setVS( id , pd3dImmediateContext_ );
}
void Renderer::set(HS::ID id)
{
	Shader_->setHS( id , pd3dImmediateContext_ );
}
void Renderer::set(DS::ID id)
{
	Shader_->setDS( id , pd3dImmediateContext_ );
}
void Renderer::set(GS::ID id)
{
	Shader_->setGS( id , pd3dImmediateContext_ );
}
void Renderer::set(PS::ID id)
{
	Shader_->setPS( id , pd3dImmediateContext_ );
}
void Renderer::set(DEPTH_STATE::ID id)
{
	DSS_->set(id, pd3dImmediateContext_);
}

void Renderer::disableVS()
{
	pd3dImmediateContext_->VSSetShader( NULL, NULL, 0 );
}
void Renderer::disableHS()
{
	pd3dImmediateContext_->HSSetShader( NULL, NULL, 0 );
}
void Renderer::disableDS()
{
	pd3dImmediateContext_->DSSetShader( NULL, NULL, 0 );
}
void Renderer::disableGS()
{
	pd3dImmediateContext_->GSSetShader( NULL, NULL, 0 );
}
void Renderer::disablePS()
{
	pd3dImmediateContext_->PSSetShader( NULL, NULL, 0 );
}

void Renderer::setCB_VS()
{
	Shader_->setCB_VS( pd3dImmediateContext_ );
}
void Renderer::setCB_HS()
{
	Shader_->setCB_HS( pd3dImmediateContext_ );
}
void Renderer::setCB_DS()
{
	Shader_->setCB_DS( pd3dImmediateContext_ );
}
void Renderer::setCB_GS()
{
	Shader_->setCB_GS( pd3dImmediateContext_ );
}
void Renderer::setCB_PS()
{
	Shader_->setCB_PS( pd3dImmediateContext_ );
}

void *Renderer::Map()
{
	return Shader_->Map(pd3dImmediateContext_);
}
void Renderer::UmMap()
{
	Shader_->UmMap(pd3dImmediateContext_);
}

}// namespace tpot