/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/


#pragma once

// NOTE
//
// 1. ���к�������ֻ֧�ֵ��̣߳���֧�ֶ��̡߳�
// 2. ����D3D9.0c SDK�ĵ�˵������ģ�����к����������Ƶ��ʾ���ڵ���Ϣ�����߳��е��á�
// 3. ͬһ�����̿��Դ��������Ⱦģ��ʵ����һ����Ⱦģ��ʵ������Ⱦ�����Ƶͼ��
// 4. ʹ��Ĭ���Կ�����֧�ֶ��Կ���
// 5. ����SHADER�汾���ܵ���2.0
// 6. ��֧��ȫ��ģʽ��
// 7. VC++ 2005 �� D3D9.0c SDK ���롣NVIDIA GeForce 9800 GT�Կ��ϲ���ͨ����


class ID3DVRInterface
{
public:
	enum COLOR_SPACE
	{
		CS_YV12		= 0, // YVU420
		CS_I420		= 1, // YUV420
		CS_YUY2		= 2, // YUYV
		CS_UYVY		= 3, // UYVY 
		CS_NV12		= 4, // NV12
		CS_RGB24	= 5, // rgb rgb ...
		CS_BGR24	= 6, // bgr bgr ...
		CS_RGB16	= 7, // R5G6B5
		CS_RGB15	= 8, // X1R5G5B5
		CS_RGB32	= 9, // bgra bgra ...
		CS_UNKNOWN  = 0xffffffff
	} ;

	enum GEOMETRIC_TRANSFORMATION
	{
		Upper_Down_Flip = 1,    // ͼ�����µߵ�
		Not_Upper_Down_Flip = 2
	} ;

public:

	// CreateBackBuffer������֡���塣
	// ���������
	// hWnd:			��Ƶ��Ⱦ���ھ����
	// lWidth:			�󱸻�������ȣ���λ�����ء�
	// lHeight:			�󱸻������߶ȣ���λ�����ء�
	// dwBufferCount:   �󱸻�����������
	
	// ����������ޡ�
	virtual bool CreateBackBuffer ( HWND hWnd, long lWidth, long lHeight, DWORD dwBufferCount = 2 ) = 0 ;

	// CreateImageBuffer������Ƶͼ�񻺳�����
	// ���������
	// lWidth:  ��Ƶͼ�񻺳�����ȣ���λ�����ء�
	// lHeight: ��Ƶͼ�񻺳����߶ȣ���λ�����ء�
	// rect:	��Ƶͼ����ʾ����
	// cs:      ��Ƶͼ����ɫ�ռ䡣

	// ��������� 
	// dwIndex: ��Ƶͼ�񻺳���������
	virtual bool CreateImageBuffer ( DWORD *dwIndex, long lWidth, long lHeight, COLOR_SPACE cs, RECT *rect ) = 0 ;

	// DestroyImageBuffer������Ƶͼ�񻺳�����
	// ���������
	// dwIndex: ��Ƶͼ�񻺳���������
	virtual bool DestroyImageBuffer ( DWORD dwIndex ) = 0 ;

	// CreateOverlayText���������ı�, ֧�����ġ�
	// ���������
	// strText: �����ı����ݡ�
	// logfont: �����ı����塣

	// ���������
	// dwIndex: �����ı�������
	virtual bool CreateOverlayText ( DWORD *dwIndex, const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText���ٵ����ı���
	// ���������
	// dwIndex: �����ı�������
	virtual bool DestroyOverlayText ( DWORD dwIndex ) = 0 ;

	// ���������
	// dwIndex: �����ı�������

	// ���������
	// lWidth: �����ı���ʵ�ʿ�ȣ���λ�����ء�
	// lHeight: �����ı���ʵ�ʸ߶ȣ���λ�����ء�
	virtual bool GetOverlayTextWH ( DWORD dwIndex, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont�������ݶ�̬�仯�����֡�Ӣ���ַ��ı�����Ҫ������ʾ�仯�����֣���֧�����ġ�
	// ���������
	// strFongName: �������ơ�
	// dwHeight: ����߶ȡ�
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyOverlayText���ٶ�̬�ı���
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill�����Ƶ��������
	// ���������
	// dwIndex: ��Ƶ������������
	// r g b  : RGB COLOR��
	virtual bool ColorFill ( DWORD dwIndex, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost����D3D�豸��ʧ
	virtual bool HandleDeviceLost( void ) = 0 ;

	// UpdateImage������Ƶ���������ݡ�
	// ���������
	// lIndex: ��Ƶ������������
	// pData: �������ݻ�����ָ�롣
	virtual bool UpdateImage ( DWORD dwIndex, BYTE *pData ) = 0 ;

	// BeginRender��ʼD3D��Ⱦ��
	// ���������
	// bClearBackBuffer: �Ƿ���պ󱸻�����(����պ󱸻����������������)��
	// dwBKColor: ��պ󱸻�����Ϊָ����ɫ��[0Xargb]
	virtual bool BeginRender ( bool bClearBackBuffer, DWORD dwBKColor ) = 0 ;

	// DrawImage��Ⱦ��Ƶͼ��
	// ���������
	// dwIndex: ��Ƶ������������
	// bFlip:  �Ƿ����·�ת��Ƶͼ��
	// rect:   ��Ƶ��ʾ����
	virtual bool DrawImage ( DWORD dwIndex, GEOMETRIC_TRANSFORMATION Transformation = Not_Upper_Down_Flip, RECT *rect = NULL ) = 0 ;

	// DrawText��Ⱦ�����ı���
	// ���������
	// dwIndex: �ı�������
	// dwColor: �ı���ɫ��[0Xargb]
	// rect: �ı���ʾ����
	virtual bool DrawOverlayText ( DWORD dwIndext, DWORD dwColor, RECT *rect ) = 0 ;

	// DrawDynamicText��Ⱦ��̬�ı���
	// ���������
	// x,y: ��̬�ı���Ⱦ����(���Ͻ�)��
	// dwColor: ��̬�ı���ɫ��[0Xargb]
	// strText: ��̬�ı���
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender����D3D��Ⱦ��
	virtual bool EndRender ( void ) = 0 ;
} ;


#if defined(__cplusplus)
extern "C" 
{
#endif

namespace D3D_VIDEO_RENDER
{
	/*****************************************************************************/
	//	���ܣ�	������Ƶ��Ⱦģ�顣
	//
	//	����ԭ�ͣ�
	//
	/**/	ID3DVRInterface* D3D_Video_Render_Create () ;
	//
	//	������
	//
	//		���룺void��
	//
	//		������ޡ�
	//              
	//	����ֵ��ָ����Ƶ��Ⱦģ��ӿڵ�ָ��,��ΪNULL��ʾʧ�ܡ�
	//
	//	��ע��һ�����̿��Դ��������Ƶ��Ⱦģ��ʵ����
	/*****************************************************************************/

	/*****************************************************************************/
	//	���ܣ�	����������Ƶ��Ⱦģ�顣
	//
	//	����ԭ�ͣ�
	//
	/**/	void D3D_Video_Render_Destroy () ;
	//
	//	������
	//
	//		���룺�ޡ�
	//
	//		������ޡ�
	//              
	//	����ֵ���ޡ�
	//
	//	��ע��
	/*****************************************************************************/
}

#if defined(__cplusplus)
}
#endif