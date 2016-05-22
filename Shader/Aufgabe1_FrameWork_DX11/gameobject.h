#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
using namespace DirectX;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	//Getter
	XMVECTOR GetPosition();
	XMVECTOR GetRotation();
	inline bool	GetRenderOnShadowMap() { return m_renderonShadow; };
	//setter
	inline void SetTransform(XMVECTOR position, XMVECTOR rotation) {
		m_position = position;
		m_rotation = rotation;
	};
	inline void SetScale(float scalex, float scaley, float scalez) { m_scale = XMVectorSet(scalex, scaley, scalez, 0); };
	inline void SetPosition(XMVECTOR position) { m_position = position; };
	inline void SetRotation(XMVECTOR rotation) { m_rotation = rotation; };
	inline void	SetRenderOnShadowMap(bool renderonShadow) { m_renderonShadow = renderonShadow; };
	inline void	SetDrawNormalMap(bool drawNormalMap) { m_drawNormalMap = drawNormalMap; };
	inline bool	GetDrawNormalMap() { return m_drawNormalMap; };
	inline void	SetDrawSpec(bool drawSpecular) { m_drawSpecular = drawSpecular; };
	inline bool	GetDrawSpec() { return m_drawSpecular; };
	inline void	SetDrawDisp(bool drawDisplacement) { m_drawDisp = drawDisplacement; };
	inline bool	GetDrawDisp() { return m_drawDisp; };
	inline void	SetDrawHardShadows(bool HardShadows) { m_HardShadows = HardShadows; };
	inline bool	GetDrawhardShadows() { return m_HardShadows; };
protected:
	XMVECTOR m_position;
	XMVECTOR m_rotation;
	XMVECTOR m_scale;

	bool m_renderonShadow = true;
	bool m_drawNormalMap  = true;
	bool m_drawSpecular   = true;
	bool m_drawDisp = true;
	bool m_HardShadows = true;
};

