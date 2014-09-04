#include "DXUT.h"
#include "SDKmisc.h"
#include "mesh.h"

namespace tpot
{

	EmbeddedMesh::EmbeddedMesh()
{
}

	EmbeddedMesh::~EmbeddedMesh()
{
	destroy();
}

	void EmbeddedMesh::initialize(ID3D11Device *pd3dDevice, void *dummy)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.ByteWidth = sizeof(BEZIER_CONTROL_POINT) * ARRAYSIZE(g_MobiusStrip);
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vbInitData;
	ZeroMemory(&vbInitData, sizeof(vbInitData));
	vbInitData.pSysMem = g_MobiusStrip;
	V(pd3dDevice->CreateBuffer(&vbDesc, &vbInitData, &pVB_));
	DXUT_SetDebugName(pVB_, "Control Points");
}
	void EmbeddedMesh::destroy()
{
	SAFE_RELEASE(pVB_);
}

void EmbeddedMesh::Draw( ID3D11DeviceContext *pd3dImmediateContext )
{
	UINT Stride = sizeof(BEZIER_CONTROL_POINT);
	UINT Offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &pVB_, &Stride, &Offset);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	pd3dImmediateContext->Draw(ARRAYSIZE(g_MobiusStrip), 0);
}

SDKMesh::SDKMesh()
{
}

SDKMesh::~SDKMesh()
{
	destroy();
}


void SDKMesh::initialize(ID3D11Device *pd3dDevice, void *param)
{
	HRESULT hr;
	LPCWSTR path = (LPCWSTR)param;
	WCHAR str[256];

	V(DXUTFindDXSDKMediaFileCch(str, 256, path));
	Mesh_.Create(pd3dDevice, str, false);
}

void SDKMesh::destroy()
{
	Mesh_.Destroy();
}

void SDKMesh::Draw(ID3D11DeviceContext *pd3dImmediateContext)
{
	Mesh_.Render( pd3dImmediateContext, 0 );
}

TriangleListMesh::TriangleListMesh()
{
	p_SRV_[0] = nullptr;
	p_SRV_[1] = nullptr;
}

TriangleListMesh::~TriangleListMesh()
{
	destroy();
}


void TriangleListMesh::initialize(ID3D11Device *pd3dDevice, void *param)
{
	VERTEX_LIST_MESH_PARAM *p = (VERTEX_LIST_MESH_PARAM*)param;

	nIndicies_ = p->index_count;
	vs_id_ = p->vs;
	stride_ = VS::getStride(p->vs);

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = stride_ * p->vertex_count;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = p->verticies;
	V(pd3dDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer_));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * p->index_count;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = p->indicies;
	V(pd3dDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_));
}

void TriangleListMesh::destroy()
{
	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pIndexBuffer_);

	for (UINT i = 0; i < nSRV_; i++){
		p_SRV_[i] = nullptr;
	}
	nSRV_ = 0;
}

void TriangleListMesh::texture(ID3D11ShaderResourceView *pSRV, UINT slot)
{
	p_SRV_[slot] = pSRV;
	if (nSRV_ < slot + 1){
		nSRV_ = slot + 1;
	}
}

void TriangleListMesh::Draw(ID3D11DeviceContext *pd3dImmediateContext)
{
	// Set vertex buffer
	UINT offset = 0;
	pd3dImmediateContext->PSSetShaderResources(0, nSRV_, p_SRV_);
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride_, &offset);
	pd3dImmediateContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->DrawIndexed(nIndicies_, 0, 0);

	// 外れないままだと次にレンダーターゲットにした時に文句を言われるので、nullにセット(レンダラーの最後にすべきか？)
	static ID3D11ShaderResourceView *nulls[TEXTURE_MAX];
	for (UINT i = 0; i < nSRV_; i++){
		nulls[i] = nullptr;
	}
	pd3dImmediateContext->PSSetShaderResources(0, nSRV_, nulls);
	nSRV_ = 0;
}

Mesh *Mesh::create(MESH_TYPE type, ID3D11Device *pd3dDevice, void *param)
{
	Mesh *p = nullptr;

	switch (type){
	case MESH_TYPE_EMBEDDED:
		p = new EmbeddedMesh();
		break;

	case MESH_TYPE_SDKMESH:
		p = new SDKMesh();
		break;

	case MESH_TYPE_TRIANGLELIST:
		p = new TriangleListMesh();
		break;
	}
	p->initialize(pd3dDevice, param);

	return p;
}

}// namespace tpot