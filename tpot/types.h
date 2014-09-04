#ifndef TPOT_TYPES_H__
#define TPOT_TYPES_H__

namespace tpot
{
	struct RASTERIZER_STATE{
		enum ID
		{
			SOLID,
			WIREFRAME,

			MAX,
		};
	};

	struct SAMPLER_STATE{
		enum ID
		{
			POINT,
			LINEAR,

			MAX,
		};
	};

	struct DEPTH_STATE{
		enum ID
		{
			UNUSED,
			DISABLE,

			MAX,
		};
	};

	struct VTX_SCENE
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 uv;
	};

	struct VTX_DECAL
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
	};

	struct VTX_SHADOW
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 uv;
	};

	struct VTX_TAA
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
	};

	struct VS{// VERTEX_SHADER
		enum ID
		{
			SCENE,
			DECAL,
			SHADOW,
			BEZIER,
			TAA,

			MAX,
		};
		static UINT getStride(VS::ID id){
			static const UINT stride[VS::MAX] = {
				sizeof(VTX_SCENE),
				sizeof(VTX_DECAL),
				sizeof(VTX_SHADOW),
				sizeof(VTX_SCENE),
			};
			return stride[id];
		};
	};

	struct HS{// HULL_SHADER
		enum ID
		{
			PARTITION_INTEGER,
			PARTITION_FRACTIONAL_EVEN,
			PARTITION_FRACTIONAL_ODD,

			MAX,
		};
	};

	struct DS{// DOMAIN_SHADER
		enum ID
		{
			BEZIER,

			MAX,
		};
	};

	struct GS{// GEOMETRY_SHADER
		enum ID
		{
			MAX,
		};
	};

	struct PS{
		enum ID
		{
			DECAL,
			SOLID_COLOR,
			SCENE,
			SHADOW,
			BEZIER,
			TAA,

			MAX,
		};
	};

	struct RENDER_TARGET{
		enum TYPE
		{
			DEPTH,
			HDR_SCREEN,

			TYPE_MAX,
		};
	};
	
	struct CB_BEZIER
	{
		D3DXMATRIX mViewProjection;
		D3DXVECTOR3 vCameraPosWorld;
		float fTessellationFactor;
	};

	struct CB_SCENE
	{
		D3DXMATRIX mViewProjection;
	};

	struct CB_SHADOW
	{
		D3DXMATRIX mViewProjection;
	};

	struct CB_DECAL
	{
		D3DXMATRIX mViewProjection;
	};

	struct CB_TAA
	{
		D3DXMATRIX mViewProjection;
		float      inv_screen_size[2];
		float      fRate;
		float      fBlurSize;
	};

}// namespace tpot
#endif // TPOT_TYPES_H__