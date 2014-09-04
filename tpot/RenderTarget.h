#ifndef RENDER_TARGET_H__
#define RENDER_TARGET_H__

#include <vector>
#include "renderer.h"

namespace tpot
{
	struct TEX{
		enum TYPE
		{
			RGBA32,
			DEPTH,

			TYPE_MAX,
		};
	};

	class Texture
	{
		ID3D11Texture2D* pTex_;

	public:
		Texture(ID3D11Device *pd3dDevice, TEX::TYPE type, UINT width, UINT height);
		~Texture();

		ID3D11Resource *get(){ return pTex_; }
	};

	struct DS_VIEW{
		enum TYPE
		{
			DEPTH,

			TYPE_MAX,
		};
	};
	class DepthStencilView
	{
		ID3D11DepthStencilView *pDSView_;
		bool                   mustRelease;
	public:
		DepthStencilView(ID3D11Device *pd3dDevice, DS_VIEW::TYPE type, ID3D11Resource *pResource);
		DepthStencilView(ID3D11DepthStencilView *pDS_View);
		~DepthStencilView();

		ID3D11DepthStencilView *get(){ return pDSView_; }
	};

	struct SR_VIEW{
		enum TYPE
		{
			RGBA32,
			DEPTH,

			TYPE_MAX,
		};
	};
	class ShaderResourceView
	{
		ID3D11ShaderResourceView *pSRView_;
	public:
		ShaderResourceView(ID3D11Device *pd3dDevice, SR_VIEW::TYPE type, ID3D11Resource *pResource);
		~ShaderResourceView();

		ID3D11ShaderResourceView *get(){ return pSRView_; }
	};

	struct RT_VIEW{
		enum TYPE
		{
			RGBA32,

			TYPE_MAX,
		};
	};
	class RenderTargetView
	{
		ID3D11RenderTargetView *pRTView_;
	public:
		RenderTargetView(ID3D11Device *pd3dDevice, RT_VIEW::TYPE type, ID3D11Resource *pResource);
		~RenderTargetView();

		ID3D11RenderTargetView *get(){ return pRTView_; }
	};

	class RenderTarget
	{
		RENDER_TARGET::TYPE type_;
		RenderTargetView *pRT_View_;
		DepthStencilView *pDS_View_;
		Texture *pTex_;
		ShaderResourceView *pSR_View_;

		ID3D11RenderTargetView  *pRT_View11_;

		void create(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height);
		void release();
	public:
		RenderTarget(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height);
		~RenderTarget();

		void ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
		void ReleasingSwapChain();

		ID3D11RenderTargetView *getRenderTargetView(){ return pRT_View_->get(); }
		ID3D11DepthStencilView *getDepthStencilView(){ return pDS_View_->get(); }
		ID3D11ShaderResourceView *getShaderResourceView(){ return pSR_View_->get(); }
		ID3D11Resource *getTexture(){ return pTex_->get(); }

		RENDER_TARGET::TYPE type() const { return type_; }
	};

	class  RenderTargets
	{
		std::vector < RenderTarget* > aRT_;

		// テクスチャレンダリング用のカレントのバッファの確保
		ID3D11RenderTargetView* pOrigRTV_;
		ID3D11DepthStencilView* pOrigDSV_;

		ID3D11RenderTargetView* pCurrentRTV_ = nullptr;
		ID3D11DepthStencilView* pCurrentDSV_ = nullptr;
	public:
		RenderTargets(ID3D11Device *pd3dDevice);
		~RenderTargets();

		void ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
		void ReleasingSwapChain();

		UINT create(ID3D11Device *pd3dDevice, RENDER_TARGET::TYPE type, UINT width, UINT height);

		void Clear(UINT color, ID3D11DeviceContext *pd3dImmediateContext); // AARRGGBB
		void ClearDepth(float depth, ID3D11DeviceContext *pd3dImmediateContext);

		void set(UINT id, ID3D11DeviceContext *pd3dImmediateContext);
		ID3D11ShaderResourceView *get(UINT id);

		void pushDefault(ID3D11DeviceContext *pd3dImmediateContext);
		void popDefault(ID3D11DeviceContext *pd3dImmediateContext);
	};

}// namespace tpot
#endif // RENDER_TARGET_H__