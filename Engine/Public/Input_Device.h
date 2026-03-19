#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class  CInput_Device final : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)		
	{ 
		return m_byKeyState[byKeyID]; 
	}

	_bool	Get_DIKeyPress(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID] & 0x80;
	}

	_bool	Get_DIKeyDown(_ubyte byKeyID)
	{
		return (m_byKeyState[byKeyID] != m_prevKeyState[byKeyID]) && (m_prevKeyState[byKeyID] == false);
	}

	_bool	Get_DIKeyUp(_ubyte byKeyID)
	{
		return (m_byKeyState[byKeyID] != m_prevKeyState[byKeyID]) && (m_byKeyState[byKeyID] == false);
	}

	_bool	Get_DIMousePress(DIMB eMouse)
	{
		return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)];
	}

	_bool	Get_DIMouseDown(DIMB eMouse)
	{
		return (m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] != m_tprevMouseState.rgbButtons[static_cast<_uint>(eMouse)]) && (m_tprevMouseState.rgbButtons[static_cast<_uint>(eMouse)] == false);
	}

	_bool	Get_DIMouseUp(DIMB eMouse)
	{
		return (m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] != m_tprevMouseState.rgbButtons[static_cast<_uint>(eMouse)]) && (m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] == false);
	}

	_byte	Get_DIMouseState(DIMB eMouse)
	{ 	
		return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)]; 	
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(DIMM eMouseState)
	{	
		return *((reinterpret_cast<_long*>(&m_tMouseState)) 
			+ static_cast<_uint>(eMouseState));
	}
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	_byte					m_prevKeyState[256];	// 이전 프레임에서 키 상태를 확인하는 변수
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState = {};
	DIMOUSESTATE			m_tprevMouseState;		// 이전 프레임에서 마우스 상태를 저장하는 변수

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free(void);

};
NS_END

#endif // InputDev_h__


