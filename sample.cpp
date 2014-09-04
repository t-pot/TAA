//--------------------------------------------------------------------------------------
// File: sample.cpp
//
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include <crtdbg.h>
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "resource.h"
#include "config.h"
#include "tpot/renderer.h"
#include "tpot/mesh.h"
#include "hud.h"

using namespace tpot;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
Renderer							*g_pRenderer;
MyHud								g_hud;
CModelViewerCamera                  g_Camera;                // A model viewing camera
Mesh								*g_pMesh;
Mesh								*g_pSceneMesh;
Mesh								*g_pPoleMesh;
Mesh								*g_pQuadMesh;
UINT                                 g_rt_color;
UINT                                 g_rt_taa[2];

CDXUTDialogResourceManager          g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                     g_D3DSettingsDlg;        // Device settings dialog



// Control variables
E_MODE                    g_iMode = TAA;
UINT g_iBlendWeight = 8;
UINT g_iBlurSize = 2;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                  float fElapsedTime, void* pUserContext );


void InitApp();
void RenderText();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D10 or D3D11) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    InitApp();
    DXUTInit( true, true ); // Parse the command line, show msgboxes on error, and an extra cmd line param to force REF for now
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"sample" );
    DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0,  true, 640, 480 );
    DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
	g_D3DSettingsDlg.Init(&g_DialogResourceManager);

	g_hud.init(&g_DialogResourceManager);

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye( 3.0f, 4.5f, -10.5f );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );
}




//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
	// Pass messages to settings dialog if its active
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	bool result = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (result) return 0;


	*pbNoFurtherProcessing = g_hud.MsgProc(hWnd, uMsg, wParam, lParam);
	if( *pbNoFurtherProcessing ) return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
		// Standard DXUT controls
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;

		case IDC_CHANGEDEVICE:
			g_D3DSettingsDlg.SetActive(!g_D3DSettingsDlg.IsActive());
			break;


		default:
			g_hud.OnEvent( nControlID );
            break;
    }
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
	g_pRenderer = new Renderer( pd3dDevice );
	HRESULT hr;
	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, g_pRenderer->pd3dImmediateContext()));
	V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));

	g_hud.create(pd3dDevice, g_pRenderer->pd3dImmediateContext(), &g_DialogResourceManager);

	g_pMesh = Mesh::create( MESH_TYPE_EMBEDDED, pd3dDevice, nullptr );
	g_pSceneMesh = Mesh::create( MESH_TYPE_SDKMESH, pd3dDevice, L"ColumnScene\\scene.sdkmesh" );
	g_pPoleMesh = Mesh::create( MESH_TYPE_SDKMESH, pd3dDevice, L"ColumnScene\\poles.sdkmesh" );

	VTX_DECAL quad_vertex[] = { 
			{ D3DXVECTOR3(0, 0, 0), D3DXVECTOR2(0, 0) },
			{ D3DXVECTOR3(1, 0, 0), D3DXVECTOR2(1, 0) },
			{ D3DXVECTOR3(0, 1, 0), D3DXVECTOR2(0, 1) },
			{ D3DXVECTOR3(1, 1, 0), D3DXVECTOR2(1, 1) },
	};
	WORD quad_index[] = { 0, 1, 2, 1, 3, 2 };
	VERTEX_LIST_MESH_PARAM quad_param = {
		VS::DECAL,
		quad_vertex, sizeof(quad_vertex) / sizeof(quad_vertex[0]),
		quad_index, sizeof(quad_index) / sizeof(quad_index[0]),
	};
	g_pQuadMesh = Mesh::create(MESH_TYPE_TRIANGLELIST, pd3dDevice, &quad_param);

	g_rt_color = g_pRenderer->create(RENDER_TARGET::HDR_SCREEN, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_rt_color = g_pRenderer->create(RENDER_TARGET::HDR_SCREEN, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_rt_taa[0] = g_pRenderer->create(RENDER_TARGET::HDR_SCREEN, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_rt_taa[1] = g_pRenderer->create(RENDER_TARGET::HDR_SCREEN, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
UINT scereen_width = 640;
UINT scereen_height = 480;
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	// Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 100.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON );

	g_pRenderer->ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc);

	HRESULT hr;
	V(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V(g_D3DSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	g_hud.Resize(pd3dDevice, pBackBufferSurfaceDesc);

	scereen_width = pBackBufferSurfaceDesc->Width;
	scereen_height = pBackBufferSurfaceDesc->Height;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                  float fElapsedTime, void* pUserContext )
{
	// If the settings dialog is being shown, then render it instead of rendering the app's scene
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.OnRender(fElapsedTime);
		return;
	}

    // WVP
    D3DXMATRIX mViewProjection;
    D3DXMATRIX mProj = *g_Camera.GetProjMatrix();
    D3DXMATRIX mView = *g_Camera.GetViewMatrix();

	D3DXMATRIX mOffset;
	static int frame_sub = 0;
#if 0
	const static float offset_tbl[16][2] = {
		{ 0.0f / 4.0f, 0.0f / 4.0f },
		{ 0.0f / 4.0f, 2.0f / 4.0f },
		{ 2.0f / 4.0f, 2.0f / 4.0f },
		{ 2.0f / 4.0f, 0.0f / 4.0f },

		{ 1.0f / 4.0f, 0.0f / 4.0f },
		{ 1.0f / 4.0f, 2.0f / 4.0f },
		{ 3.0f / 4.0f, 2.0f / 4.0f },
		{ 3.0f / 4.0f, 0.0f / 4.0f },

		{ 0.0f / 4.0f, 1.0f / 4.0f },
		{ 0.0f / 4.0f, 3.0f / 4.0f },
		{ 2.0f / 4.0f, 3.0f / 4.0f },
		{ 2.0f / 4.0f, 1.0f / 4.0f },

		{ 1.0f / 4.0f, 1.0f / 4.0f },
		{ 1.0f / 4.0f, 3.0f / 4.0f },
		{ 3.0f / 4.0f, 3.0f / 4.0f },
		{ 3.0f / 4.0f, 1.0f / 4.0f },
	};
	frame_sub = (frame_sub + 1) & 0xf;
#else
	const static float offset_tbl[8][2] = {
		// http://en.wikipedia.org/wiki/Halton_sequence
			{ 1.0f / 2.0f, 1.0f / 3.0f },
			{ 1.0f / 4.0f, 2.0f / 3.0f },
			{ 3.0f / 4.0f, 1.0f / 9.0f },
			{ 1.0f / 8.0f, 4.0f / 9.0f },
			{ 5.0f / 8.0f, 7.0f / 9.0f },
			{ 3.0f / 8.0f, 2.0f / 9.0f },
			{ 7.0f / 8.0f, 5.0f / 9.0f },
			{ 1.0f / 16.0f, 8.0f / 9.0f },

	};
	frame_sub = (frame_sub + 1) & 0x7;
#endif
	D3DXMatrixTranslation(&mOffset, offset_tbl[frame_sub][0] / (float)scereen_width, offset_tbl[frame_sub][1] / (float)scereen_height, 0.0f);

	if (g_iMode == TAA || g_iMode == CAMMOVE){
		mViewProjection = mView * mProj * mOffset;
	}
	else{
		mViewProjection = mView * mProj;
	}

	g_pRenderer->pushRenderTarget();

	static unsigned int bFrame = 0;
	bFrame = 1 - bFrame;

	g_pRenderer->setRenderTarget(g_rt_color);

    // Clear the render target and depth stencil
	g_pRenderer->Clear( 0xff080808 );
	g_pRenderer->ClearDepth( 1.0f );

	g_pRenderer->set(VS::SCENE);
	g_pRenderer->set(PS::SCENE);
	g_pRenderer->set(0, SAMPLER_STATE::LINEAR);

	CB_SCENE* pCBscene = (CB_SCENE*)g_pRenderer->Map();
	D3DXMatrixTranspose(&pCBscene->mViewProjection, &mViewProjection);
	g_pRenderer->UmMap();

	g_pRenderer->setCB_VS();

	g_pRenderer->Draw(g_pSceneMesh);
	g_pRenderer->Draw(g_pPoleMesh);

	if (g_iMode == TAA)
	{
		g_pRenderer->setRenderTarget(g_rt_taa[bFrame]);
		g_pRenderer->setDepth(~(UINT)0);
		g_pRenderer->set(DEPTH_STATE::DISABLE);

		D3DXMATRIX m;
		D3DXMatrixScaling(&m, (float)scereen_width, (float)scereen_height, 1.0f);
		mViewProjection = m * g_pRenderer->screenProjMatrix();

		g_pQuadMesh->texture(g_pRenderer->getTexture(g_rt_taa[1 - bFrame]), 0);
		g_pQuadMesh->texture(g_pRenderer->getTexture(g_rt_color), 1);
		g_pRenderer->set(VS::TAA);
		g_pRenderer->set(PS::TAA);
		g_pRenderer->set(0, SAMPLER_STATE::LINEAR);
		CB_TAA* pCBdecal = (CB_TAA*)g_pRenderer->Map();
		D3DXMatrixTranspose(&pCBdecal->mViewProjection, &mViewProjection);
		pCBdecal->fRate = 1.0f / (float)g_iBlendWeight;
		static bool init = false;
		if (!init){
			init = true;
			pCBdecal->fRate = 1.0f;
		}
		pCBdecal->inv_screen_size[0] = 1.0f / (float)scereen_width;
		pCBdecal->inv_screen_size[1] = 1.0f / (float)scereen_height;
		pCBdecal->fBlurSize = 0.1f * (float)g_iBlurSize;
		g_pRenderer->UmMap();
		g_pRenderer->setCB_VS();
		g_pRenderer->setCB_PS();
		g_pRenderer->Draw(g_pQuadMesh);
		g_pQuadMesh->texture(nullptr, 0);
		g_pQuadMesh->texture(nullptr, 1);

		g_pRenderer->popRenderTarget();
		g_pRenderer->Clear(0x00101010);
		g_pRenderer->ClearDepth(1.0f);

		{
			g_pQuadMesh->texture(g_pRenderer->getTexture(g_rt_taa[bFrame]), 0);
			g_pRenderer->set(VS::DECAL);
			g_pRenderer->set(PS::DECAL);
			CB_DECAL* pCBdecal = (CB_DECAL*)g_pRenderer->Map();
			D3DXMatrixTranspose(&pCBdecal->mViewProjection, &mViewProjection);
			g_pRenderer->setCB_VS();
			g_pRenderer->UmMap();
			g_pRenderer->Draw(g_pQuadMesh);
			g_pQuadMesh->texture(nullptr, 0);
		}
	}
	else{
		g_pRenderer->popRenderTarget();
		g_pRenderer->Clear(0x00101010);
		g_pRenderer->set(DEPTH_STATE::DISABLE);

		D3DXMATRIX m;
		D3DXMatrixScaling(&m, (float)scereen_width, (float)scereen_height, 1.0f);
		mViewProjection = m * g_pRenderer->screenProjMatrix();

		g_pQuadMesh->texture(g_pRenderer->getTexture(g_rt_color), 0);
		g_pRenderer->set(VS::DECAL);
		g_pRenderer->set(PS::DECAL);
		CB_DECAL* pCBdecal = (CB_DECAL*)g_pRenderer->Map();
		D3DXMatrixTranspose(&pCBdecal->mViewProjection, &mViewProjection);
		g_pRenderer->setCB_VS();
		g_pRenderer->UmMap();
		g_pRenderer->Draw(g_pQuadMesh);
		g_pQuadMesh->texture(nullptr, 0);
	}

#if 1
	{// レンダーターゲットの可視化
		D3DXMATRIX mS, m;
		D3DXMatrixScaling(&mS, 100.0f, 100.0f, 1.0f);
		D3DXMatrixTranslation(&m, 0.0f, (float)scereen_height - 100.0f, 0.0f);
		mViewProjection = mS * m * g_pRenderer->screenProjMatrix();

		g_pQuadMesh->texture(g_pRenderer->getTexture(g_rt_color));
		g_pRenderer->set(VS::DECAL);
		g_pRenderer->set(PS::DECAL);
		CB_DECAL* pCBdecal = (CB_DECAL*)g_pRenderer->Map();
		D3DXMatrixTranspose(&pCBdecal->mViewProjection, &mViewProjection);
		g_pRenderer->UmMap();
		g_pRenderer->setCB_VS();
		g_pRenderer->Draw(g_pQuadMesh);
	}
#endif


	g_pRenderer->set(DEPTH_STATE::UNUSED);

	g_hud.render(fElapsedTime);
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	g_pRenderer->ReleasingSwapChain();

	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
	g_hud.ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_D3DSettingsDlg.OnD3D11DestroyDevice();
	g_hud.destroy();

	SAFE_DELETE(g_pQuadMesh);
	SAFE_DELETE(g_pSceneMesh);
	SAFE_DELETE(g_pPoleMesh);
	SAFE_DELETE(g_pMesh);

	SAFE_DELETE( g_pRenderer );
}
