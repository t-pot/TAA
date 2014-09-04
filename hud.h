#ifndef HUD_H__
#define HUD_H__

#include "config.h"

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

enum E_MODE
{
	OFF,
	TAA,
	CAMMOVE,
};

extern E_MODE                    g_iMode;
extern UINT g_iBlendWeight;
extern UINT g_iBlurSize;
class MyHud
{
	CDXUTDialog                         g_HUD;                   // manages the 3D   
	CDXUTDialog                         g_SampleUI;              // dialog for sample specific controls
	CDXUTTextHelper*                    g_pTxtHelper;

public:
	MyHud():g_pTxtHelper(nullptr){}
	~MyHud(){}

	void create(ID3D11Device* pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext, CDXUTDialogResourceManager *pDialogResourceManager)
	{
		g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, pDialogResourceManager, 15);
	};
	void init(CDXUTDialogResourceManager *pDialogResourceManager){

		g_HUD.Init(pDialogResourceManager);
		g_SampleUI.Init(pDialogResourceManager);

		g_HUD.SetCallback( OnGUIEvent ); int iY = 20;
		g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22 );
		g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 26, 170, 22, VK_F3 );
		g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 26, 170, 22, VK_F2 );

		g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;

		WCHAR sz[100];
		iY += 24;
		swprintf_s(sz, L"Blend Weight: 1/%2d", g_iBlendWeight);
		g_SampleUI.AddStatic(IDC_BLEND_WEIGHT_STATIC, sz, 10, iY += 26, 150, 22);
		g_SampleUI.AddSlider(IDC_BLEND_WEIGHT, 10, iY += 24, 150, 22, 1, 32, (int)(g_iBlendWeight));

		iY += 24;
		swprintf_s(sz, L"Blur Size: %2.1f", 0.1f * (float)g_iBlurSize);
		g_SampleUI.AddStatic(IDC_BLUR_SIZE_STATIC, sz, 10, iY += 26, 150, 22);
		g_SampleUI.AddSlider(IDC_BLUR_SIZE, 10, iY += 24, 150, 22, 0, 10, (int)(g_iBlurSize));

		iY += 24;
		g_SampleUI.AddRadioButton(IDC_MODE_OFF, IDC_MODE, L"OFF", 20, iY += 26, 170, 22);
		g_SampleUI.AddRadioButton(IDC_MODE_TAA, IDC_MODE, L"TAA", 20, iY += 26, 170, 22);
		g_SampleUI.AddRadioButton(IDC_MODE_CAMMOVE, IDC_MODE, L"Move Camera", 20, iY += 26, 170, 22);
		g_SampleUI.GetRadioButton(IDC_MODE_TAA)->SetChecked(true);
	}

	void ReleasingSwapChain()
	{
	}

	void destroy()
	{
	    SAFE_DELETE( g_pTxtHelper );
	}

	void Resize( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc )
	{
		g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
		g_HUD.SetSize( 170, 170 );
		g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
		g_SampleUI.SetSize( 170, 300 );
	}

	void OnEvent( int nControlID )
	{
		switch(nControlID){
		case IDC_BLEND_WEIGHT:
			{
			g_iBlendWeight = g_SampleUI.GetSlider(IDC_BLEND_WEIGHT)->GetValue();

				WCHAR sz[100];
				swprintf_s(sz, L"Blend Weight: 1/%2d", g_iBlendWeight);
				g_SampleUI.GetStatic(IDC_BLEND_WEIGHT_STATIC)->SetText(sz);
			}
				break;
		case IDC_BLUR_SIZE:
		{
			g_iBlurSize = g_SampleUI.GetSlider(IDC_BLUR_SIZE)->GetValue();

			WCHAR sz[100];
			swprintf_s(sz, L"Blur Size: %2.1f", 0.1f*(float)g_iBlurSize);
			g_SampleUI.GetStatic(IDC_BLUR_SIZE_STATIC)->SetText(sz);
		}
			break;
		case IDC_TOGGLE_LINES:
//				g_bDrawWires = g_SampleUI.GetCheckBox( IDC_TOGGLE_LINES )->GetChecked();
				break;
			case IDC_MODE_OFF:
				g_iMode = OFF;
				break;
			case IDC_MODE_TAA:
				g_iMode = TAA;
				break;
			case IDC_MODE_CAMMOVE:
				g_iMode = CAMMOVE;
				break;
		}
	}
			

	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		bool result;

		// Give the dialogs a chance to handle the message first
		result = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
		if( result ) return true;

		result = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
		if( result ) return true;

		return false;
	}

	void render( float fElapsedTime )
	{
		// Render the HUD
		DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
		g_HUD.OnRender( fElapsedTime );
		g_SampleUI.OnRender( fElapsedTime );
		RenderText();
		DXUT_EndPerfEvent();
	}
private:
	void RenderText()
	{
		g_pTxtHelper->Begin();
		g_pTxtHelper->SetInsertionPos( 2, 0 );
		g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
		g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
		g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

		g_pTxtHelper->End();
	}
};


#endif // HUD_H__