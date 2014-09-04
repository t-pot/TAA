#include "DXUT.h"
#include "RenderTarget.h"

namespace tpot
{
	Texture::Texture(ID3D11Device *pd3dDevice, TEX::TYPE type, UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC Desc;
		ZeroMemory(&Desc, sizeof(D3D11_TEXTURE2D_DESC));
		Desc.MipLevels = 1;
		Desc.ArraySize = 1;
		Desc.SampleDesc.Count = 1;
		Desc.Height = height;
		Desc.Width = width;
		Desc.Usage = D3D11_USAGE_DEFAULT;

		char *name = nullptr;
		
		switch (type){
		case TEX::RGBA32:
			// Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			name = "TEX::RGBA32";
			break;

		case TEX::DEPTH:
			Desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			name = "TEX::DEPTH";
			break;
		}

		HRESULT hr;
		V(pd3dDevice->CreateTexture2D(&Desc, nullptr, &pTex_));

		DXUT_SetDebugName(pTex_, name);
	}
	Texture::~Texture()
	{
		SAFE_RELEASE(pTex_);
	}

	DepthStencilView::DepthStencilView(ID3D11DepthStencilView *pDS_View)
		: mustRelease(false)
	{
		pDSView_ = pDS_View;
	}

	DepthStencilView::DepthStencilView(ID3D11Device *pd3dDevice, DS_VIEW::TYPE type, ID3D11Resource *pResource)
		: mustRelease(true)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipSlice = 0;

		HRESULT hr;
		V(pd3dDevice->CreateDepthStencilView(
			pResource,
			&Desc,
			&pDSView_
			));
	}

	DepthStencilView::~DepthStencilView()
	{
		if (mustRelease){
			SAFE_RELEASE(pDSView_);
		}
	}

	ShaderResourceView::ShaderResourceView(ID3D11Device *pd3dDevice, SR_VIEW::TYPE type, ID3D11Resource *pResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipLevels = 1;

		switch (type){
		case SR_VIEW::RGBA32:
			// Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case SR_VIEW::DEPTH:
			Desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		}

		HRESULT hr;
		V(pd3dDevice->CreateShaderResourceView( pResource, &Desc, &pSRView_));
	}

	ShaderResourceView::~ShaderResourceView()
	{
		SAFE_RELEASE(pSRView_);
	}

	RenderTargetView::RenderTargetView(ID3D11Device *pd3dDevice, RT_VIEW::TYPE type, ID3D11Resource *pResource)
	{
		D3D11_RENDER_TARGET_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		switch (type){
		case RT_VIEW::RGBA32:
			// Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		}

		HRESULT hr;
		V(pd3dDevice->CreateRenderTargetView(
			pResource,
			&Desc,
			&pRTView_
			));
	}

	RenderTargetView::~RenderTargetView()
	{
		SAFE_RELEASE(pRTView_);
	}

	void RenderTarget::create(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height)
	{
		switch (type){
		case RENDER_TARGET::DEPTH:
			pTex_ = new Texture(pd3dDevice, TEX::DEPTH, width, height);
			pDS_View_ = new DepthStencilView(pd3dDevice, DS_VIEW::DEPTH, pTex_->get());
			pSR_View_ = new ShaderResourceView(pd3dDevice, SR_VIEW::DEPTH, pTex_->get());
			pRT_View_ = nullptr;
			break;

		case RENDER_TARGET::HDR_SCREEN:
			pTex_ = new Texture(pd3dDevice, TEX::RGBA32, width, height);
			pDS_View_ = nullptr;
			pSR_View_ = new ShaderResourceView(pd3dDevice, SR_VIEW::RGBA32, pTex_->get());
			pRT_View_ = new RenderTargetView(pd3dDevice, RT_VIEW::RGBA32, pTex_->get());
			break;
		}
	}
	void RenderTarget::release()
	{
		SAFE_DELETE(pRT_View_);
		SAFE_DELETE(pSR_View_);
		SAFE_DELETE(pDS_View_);
		SAFE_DELETE(pTex_);
	}

	RenderTarget::RenderTarget(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height)
		: type_(type)
	{
		create(pd3dDevice, type, width, height);
	}

	RenderTarget::~RenderTarget()
	{
		release();
	}

	void RenderTarget::ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
	{
		UINT width = pBackBufferSurfaceDesc->Width;
		UINT height = pBackBufferSurfaceDesc->Height;

		switch (type_){
		case RENDER_TARGET::DEPTH:
			break;

		case RENDER_TARGET::HDR_SCREEN:
			release();
			create(pd3dDevice, type_, width, height);
			break;
		}
	}

	void RenderTarget::ReleasingSwapChain(){
		switch (type_){
		case RENDER_TARGET::DEPTH:
			break;

		case RENDER_TARGET::HDR_SCREEN:
			release();
			break;
		}
	}

	RenderTargets::RenderTargets(ID3D11Device *pd3dDevice)
		: pOrigRTV_(nullptr), pOrigDSV_(nullptr)
	{
	}

	RenderTargets::~RenderTargets()
	{
		for (auto &x : aRT_){
			SAFE_DELETE(x);
		}
	}

	void RenderTargets::ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
	{
		for (auto &p : aRT_){
			p->ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc);
		}
	}

	void RenderTargets::ReleasingSwapChain()
	{
		for (auto &p : aRT_){
			p->ReleasingSwapChain();
		}
	}

	UINT RenderTargets::create(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height)
	{
		RenderTarget *p = new RenderTarget(pd3dDevice, type, width, height);
		aRT_.push_back(p);

		return aRT_.size() - 1;
	}

	void RenderTargets::Clear(UINT color, ID3D11DeviceContext *pd3dImmediateContext)
	{
		float ClearColor[4] = {
			(1.0f / 255.0f)*(float)((color >> 16) & 0xff),
			(1.0f / 255.0f)*(float)((color >> 8) & 0xff),
			(1.0f / 255.0f)*(float)(color & 0xff),
			(1.0f / 255.0f)*(float)(color >> 24)
		};
		ID3D11RenderTargetView* pRTV = (pCurrentRTV_ == nullptr) ? DXUTGetD3D11RenderTargetView() : pCurrentRTV_;
		pd3dImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
	}

	void RenderTargets::ClearDepth(float depth, ID3D11DeviceContext *pd3dImmediateContext)
	{
		ID3D11DepthStencilView* pDSV = (pCurrentDSV_ == nullptr) ? DXUTGetD3D11DepthStencilView() : pCurrentDSV_;
		pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, depth, 0);
	}

	void RenderTargets::set(UINT id, ID3D11DeviceContext *pd3dImmediateContext)
	{
		if (~0 == id){
			// ˆê’UA‚¨‚©‚µ‚È’l‚Ídepth ‚¾‚¯—ˆ‚é‚±‚Æ‚É‚µ‚Ä‘Î‰ž
			ID3D11RenderTargetView*      pRTV[1] = { pCurrentRTV_ };
			pd3dImmediateContext->OMSetRenderTargets(1, pRTV, nullptr);
			return;
		}

		RenderTarget* pRT = aRT_[id];

		if (pRT->type() == RENDER_TARGET::HDR_SCREEN){
			pCurrentRTV_ = pRT->getRenderTargetView();
		}
		else if (pRT->type() == RENDER_TARGET::DEPTH){
			pCurrentDSV_ = pRT->getDepthStencilView();
		}
		ID3D11RenderTargetView*      pRTV[1] = { pCurrentRTV_ };
		pd3dImmediateContext->OMSetRenderTargets(1, pRTV, pCurrentDSV_);
	}
	ID3D11ShaderResourceView *RenderTargets::get(UINT id)
	{
		return aRT_[id]->getShaderResourceView();
	}
	void RenderTargets::pushDefault(ID3D11DeviceContext *pd3dImmediateContext)
	{
		pd3dImmediateContext->OMGetRenderTargets(1, &pOrigRTV_, &pOrigDSV_);
		pCurrentRTV_ = pOrigRTV_;
		pCurrentDSV_ = pOrigDSV_;
	}
	void RenderTargets::popDefault(ID3D11DeviceContext *pd3dImmediateContext)
	{
		ID3D11RenderTargetView* aRTViews[1] = { pOrigRTV_ };
		pd3dImmediateContext->OMSetRenderTargets(1, aRTViews, pOrigDSV_);

		SAFE_RELEASE(pOrigRTV_);
		SAFE_RELEASE(pOrigDSV_);

		pCurrentRTV_ = nullptr;
		pCurrentDSV_ = nullptr;
	}

}// namespace tpot