#ifndef RENDERER_H__
#define RENDERER_H__

#include "types.h"

namespace tpot
{

	class RenderTargets;
	class RasterStates;
	class DepthStencilStates;
	class SamplerStates;
	class ConstantBuffer;
	class Shader;
	class Mesh;

	class Renderer
	{
		ID3D11Device *pd3dDevice_;
		ID3D11DeviceContext *pd3dImmediateContext_;
		DXGI_SURFACE_DESC pBackBufferSurfaceDesc_;

		RenderTargets *TR_;
		RasterStates *RS_;
		DepthStencilStates *DSS_;
		SamplerStates *SAMP_;
		Shader       *Shader_;

	public:
		Renderer( ID3D11Device *pd3dDevice );
		~Renderer();

		void ResizedSwapChain(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
		void ReleasingSwapChain();

		ID3D11DeviceContext *pd3dImmediateContext();

		UINT create(RENDER_TARGET::TYPE type, UINT width, UINT height);
		void setRenderTarget(UINT id);
		void setDepth(UINT id);
		ID3D11ShaderResourceView *getTexture(UINT id);
		void pushRenderTarget();
		void popRenderTarget();
		D3DXMATRIX screenProjMatrix();

		void Clear( UINT color ); // AARRGGBB
		void ClearDepth( float depth );

		void set(RASTERIZER_STATE::ID id);
		void set(UINT slot, SAMPLER_STATE::ID id);
		void set(VS::ID id);
		void set(HS::ID id);
		void set(DS::ID id);
		void set(GS::ID id);
		void set(PS::ID id);
		void set(DEPTH_STATE::ID type);

		void disableVS();
		void disableHS();
		void disableDS();
		void disableGS();
		void disablePS();

		void setCB_VS();
		void setCB_HS();
		void setCB_DS();
		void setCB_GS();
		void setCB_PS();

		void *Map();
		void UmMap();

		void Draw( Mesh *pMesh );
	};

}// namespace tpot
#endif // RENDERER_H__