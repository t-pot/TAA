#ifndef MESH_H__
#define MESH_H__

#include "MobiusStrip.h"
#include "SDKMesh.h"
#include "types.h"

namespace tpot
{

	enum MESH_TYPE{
		MESH_TYPE_EMBEDDED,
		MESH_TYPE_SDKMESH,
		MESH_TYPE_TRIANGLELIST,
	};

	class Mesh
	{
	public:
		Mesh(){};
		virtual ~Mesh(){};

		virtual void initialize(ID3D11Device *pd3dDevice, void *param) = 0;
		virtual void destroy() = 0;

		static Mesh *create(MESH_TYPE type, ID3D11Device *pd3dDevice, void *param);

		virtual void Draw(ID3D11DeviceContext *pd3dImmediateContext) = 0;

		virtual UINT stride() const { return 0; }
		virtual ID3D11Buffer *VB()  { return nullptr; }
		virtual UINT verticies_count() const { return 0; }

		virtual void texture(ID3D11ShaderResourceView *pSRV, UINT slot = 0){}
	};

	class EmbeddedMesh : public Mesh
	{
	public:

	private:
		ID3D11Buffer*   pVB_ = nullptr;

		UINT stride() const
		{
			return sizeof(BEZIER_CONTROL_POINT);
		};
		ID3D11Buffer *VB()
		{
			return pVB_;
		}
		UINT verticies_count() const
		{
			return ARRAYSIZE(g_MobiusStrip);
		}

	public:
		EmbeddedMesh();
		~EmbeddedMesh();

		void initialize(ID3D11Device *pd3dDevice, void *param);
		void destroy();

		void Draw(ID3D11DeviceContext *pd3dImmediateContext);
	};

	class SDKMesh : public Mesh {
	private:
		CDXUTSDKMesh                Mesh_;

	public:
		SDKMesh();
		~SDKMesh();
	
		void initialize(ID3D11Device *pd3dDevice, void *param);
		void destroy();

		void Draw(ID3D11DeviceContext *pd3dImmediateContext);
	};

	struct VERTEX_LIST_MESH_PARAM{
		VS::ID vs;
		void   *verticies;
		UINT   vertex_count;
		WORD   *indicies;
		UINT   index_count;
	};

	class TriangleListMesh : public Mesh {
	private:
		enum{
			TEXTURE_MAX = 8,
		};

		ID3D11Buffer*           pVertexBuffer_ = nullptr;
		ID3D11Buffer*           pIndexBuffer_ = nullptr;
		ID3D11ShaderResourceView *p_SRV_[TEXTURE_MAX];
		UINT                      nSRV_ = 0;
		UINT                    nIndicies_ = 0;
		UINT					stride_ = 0;
		VS::ID                  vs_id_;
	public:
		TriangleListMesh();
		~TriangleListMesh();

		void initialize(ID3D11Device *pd3dDevice, void *param);
		void destroy();

		void texture(ID3D11ShaderResourceView *pSRV, UINT slot = 0);

		void Draw(ID3D11DeviceContext *pd3dImmediateContext);
	};

}// namespace tpot
#endif // MESH_H__