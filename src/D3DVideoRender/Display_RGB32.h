/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "display.h"

class CDisplay_RGB32 :
	public CDisplay
{
	struct RENDER2FRAME_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;

		D3DXVECTOR2 tex1 ;
	} ;

	enum
	{
		D3DFVF_RENDER2FRAME_VERTEX_macro = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
	} ;

public:

	CDisplay_RGB32(LPDIRECT3DDEVICE9 pDevice)
		: m_pVertices(NULL)
		, m_pTexture(NULL)
		, m_lWidth(0)
		, m_lHeight(0)
		, m_lSize(0)
	{
		m_pDevice = pDevice ;
	}
public:

	~CDisplay_RGB32(void)
	{
		InvalidateDeviceObjects () ;
	}

	static CDisplay *Create (LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
	{
		CDisplay_RGB32 *p = new CDisplay_RGB32 ( pDevice ) ;
		return p ;
	}

public:
	virtual bool SetImageBuffer ( long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect )
	{
		m_ColorSpace = cs ;
		return CreateVT ( lWidth, lHeight, rect ) ;
	}

	virtual bool UpdateImage ( BYTE *pData )
	{
		D3DLOCKED_RECT lrect ;
		HRESULT hr = m_pTexture->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
		if ( SUCCEEDED(hr) )
		{
			BYTE *pDest = (BYTE*)lrect.pBits ;
			if ( lrect.Pitch == m_lWidth )
			{
				memcpyMMX ( pDest, pData, m_lSize ) ;
			}
			else
			{
				for ( int j = 0 ; j < m_lHeight ; j++ )
				{
					memcpyMMX ( pDest, pData, m_lWidth ) ;
					pDest += lrect.Pitch ;
					pData += m_lWidth ;
				}
			}

			hr = m_pTexture->UnlockRect ( 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pTexture[i]->UnlockRect FAILED!\nhr=%x"), hr ) ;
			}
		}
		else
		{
			__asm emms ;
			g_pDebug->Log_Error ( _T("m_pTexture[i]->LockRect FAILED!\nhr=%x"), hr ) ;
			return false ;
		}

		__asm emms ;
		return true ;
	}

	virtual bool DrawImage ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *rect )
	{
		HRESULT hr = m_pDevice->SetFVF ( D3DFVF_RENDER2FRAME_VERTEX_macro ) ;
		hr = m_pDevice->SetPixelShader ( NULL ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader failed!\nhr=%x"), hr ) ;
		} 

		SetImageRect ( Transformation, rect ) ;

		hr = m_pDevice->SetTexture ( 0, m_pTexture ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices, 0, sizeof(RENDER2FRAME_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}

		return true ;
	}

	virtual bool InvalidateDeviceObjects ()
	{
		SAFE_RELEASE ( m_pVertices ) ;
		SAFE_RELEASE ( m_pTexture ) ;
		return true ;
	}

	virtual bool RestoreDeviceObjects ()
	{
		long lWidth ;
		if ( m_ColorSpace == ID3DVRInterface::CS_RGB32 )
		{
			lWidth = m_lWidth / 4 ;
		}
		else
		{
			lWidth = m_lWidth / 2 ;
		}

		return CreateVT ( lWidth , m_lHeight, &m_DisplayRect ) ;
	}

	virtual bool Fill ( BYTE r, BYTE g, BYTE b )
	{
		if ( m_pTexture == NULL )
		{
			return false ;
		}

		long lWidth, lHeight ;
		D3DFORMAT format ;
		if ( m_ColorSpace == ID3DVRInterface::CS_RGB32 )
		{
			format = D3DFMT_A8R8G8B8 ;
			lWidth = m_lWidth / 4 ;
		}
		else if ( m_ColorSpace == ID3DVRInterface::CS_RGB16 )
		{
			format = D3DFMT_R5G6B5 ;
			lWidth = m_lWidth / 2 ;
		}
		else if ( m_ColorSpace == ID3DVRInterface::CS_RGB15 )
		{
			format = D3DFMT_X1R5G5B5 ;
			lWidth = m_lWidth / 2 ;
		}
		else
		{
			return false ;
		}
		
		lHeight = m_lHeight ;

		LPDIRECT3DTEXTURE9 pTex_Target = NULL ;
		IDirect3DSurface9 *pSur_Target = NULL ;
		LPDIRECT3DTEXTURE9 pTex_Mem = NULL ;
		IDirect3DSurface9 *pSur_Mem = NULL ;
		HRESULT hr = S_OK ;
		do
		{
			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &pTex_Target, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = pTex_Target->GetSurfaceLevel ( 0, &pSur_Target ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->ColorFill ( pSur_Target, NULL, D3DCOLOR_ARGB(255, r, g, b) ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, 0, format, D3DPOOL_SYSTEMMEM, &pTex_Mem, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = pTex_Mem->GetSurfaceLevel ( 0, &pSur_Mem ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->GetRenderTargetData ( pSur_Target, pSur_Mem ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->UpdateTexture ( pTex_Mem, m_pTexture ) ;
		
		} while ( false ) ;

		SAFE_RELEASE ( pSur_Mem ) ;
		SAFE_RELEASE ( pSur_Target ) ;
		SAFE_RELEASE ( pTex_Target ) ;
		SAFE_RELEASE ( pTex_Mem ) ;
		
		if ( FAILED(hr) )
		{
			return false ;
		}
		return true ;
	}

private:
	bool CreateVT ( long lWidth, long lHeight, RECT *rect )
	{
		HRESULT hr ;
		D3DFORMAT format ;
		long factor ;
		if ( m_ColorSpace == ID3DVRInterface::CS_RGB32 )
		{
			format = D3DFMT_A8R8G8B8 ;
			factor = 4 ;
		}
		else if ( m_ColorSpace == ID3DVRInterface::CS_RGB16 )
		{
			format = D3DFMT_R5G6B5 ;
			factor = 2 ;
		}
		else if ( m_ColorSpace == ID3DVRInterface::CS_RGB15 )
		{
			format = D3DFMT_X1R5G5B5 ;
			factor = 2 ;
		}
		else
		{
			return false ;
		}

		do
		{
			long x = rect->left ;
			long y = rect->top ;
			long dx = rect->right ;
			long dy = rect->bottom ;
			m_DisplayRect.left = x ;
			m_DisplayRect.top = y ;
			m_DisplayRect.right = dx ;
			m_DisplayRect.bottom = dy ;

			hr = m_pDevice->CreateVertexBuffer ( 4 * sizeof(RENDER2FRAME_VERTEX_st), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_RENDER2FRAME_VERTEX_macro, D3DPOOL_DEFAULT, &m_pVertices, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateVertexBuffer failed!\nhr=%x"), hr ) ;
				break ;
			}

			RENDER2FRAME_VERTEX_st *pVB ;

			hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
				break ;
			}

			pVB[0].pos = D3DXVECTOR4 ( x-0.5f,         y-0.5f,			0.0f, 1.0f ) ;
			pVB[1].pos = D3DXVECTOR4 ( dx - 0.5f,     y-0.5f,			0.0f, 1.0f ) ;
			pVB[2].pos = D3DXVECTOR4 ( dx - 0.5f,     dy - 0.5f,   0.0f, 1.0f ) ;
			pVB[3].pos = D3DXVECTOR4 ( x-0.5f,         dy - 0.5f,   0.0f, 1.0f ) ;

			DWORD dw = D3DCOLOR_RGBA(255,255,255,255) ;
			pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = dw ;

			if ( m_Flip == ID3DVRInterface::Not_Upper_Down_Flip )
			{
				pVB[0].tex1 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
				pVB[1].tex1 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
				pVB[2].tex1 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
				pVB[3].tex1 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
			}
			else
			{
				pVB[0].tex1 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
				pVB[1].tex1 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
				pVB[2].tex1 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
				pVB[3].tex1 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
			}

			hr = m_pVertices->Unlock () ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, D3DUSAGE_DYNAMIC, format, D3DPOOL_DEFAULT, &m_pTexture, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_lWidth = lWidth * factor ;
			m_lHeight = lHeight ;
			m_lSize = m_lWidth * lHeight ;
			
			return true ;

		} while ( false ) ;

		InvalidateDeviceObjects () ;
		return false ;
	}

	bool SetImageRect ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Flip, RECT *rect )
	{
		if ( Flip == m_Flip && rect == NULL )
		{
			return true ;
		}

		RECT *pRt = &m_DisplayRect ;
		if ( rect != NULL )
		{
			if ( Flip == m_Flip && pRt->left == rect->left && pRt->right == rect->right
				&& pRt->top == rect->top && pRt->bottom == rect->bottom )
			{
				return true ;
			}
			else
			{
				pRt->left = rect->left ;
				pRt->top = rect->top ;
				pRt->right = rect->right ;
				pRt->bottom = rect->bottom ;
			}
		}
		
		long x = pRt->left ;
		long y = pRt->top ;
		long dx = pRt->right ;
		long dy = pRt->bottom ;
		

		RENDER2FRAME_VERTEX_st *pVB ;

		HRESULT hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, D3DLOCK_DISCARD ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
			return false ;
		}

		pVB[0].pos = D3DXVECTOR4 ( x-0.5f,         y-0.5f,			0.0f, 1.0f ) ;
		pVB[1].pos = D3DXVECTOR4 ( dx - 0.5f,     y-0.5f,			0.0f, 1.0f ) ;
		pVB[2].pos = D3DXVECTOR4 ( dx - 0.5f,     dy - 0.5f,   0.0f, 1.0f ) ;
		pVB[3].pos = D3DXVECTOR4 ( x-0.5f,         dy - 0.5f,   0.0f, 1.0f ) ;

		pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = 0xffffffff ;

		if ( Flip == ID3DVRInterface::Not_Upper_Down_Flip )
		{
			pVB[0].tex1 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
			pVB[1].tex1 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
			pVB[2].tex1 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
			pVB[3].tex1 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
		}
		else
		{
			pVB[0].tex1 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
			pVB[1].tex1 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
			pVB[2].tex1 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
			pVB[3].tex1 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
		}
		m_Flip = Flip ;

		hr = m_pVertices->Unlock () ;

		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
			return false ;
		}
		return true ;
	}

private:

	LPDIRECT3DVERTEXBUFFER9 m_pVertices ;
	LPDIRECT3DTEXTURE9 m_pTexture ;

	long m_lWidth, m_lHeight, m_lSize ;
};
