#ifndef _D3DCAMERA_H_
#define _D3DCAMERA_H_

#include "gameobject.h"
using namespace DirectX;

class D3DCamera: public GameObject
{

public:
	D3DCamera();
	~D3DCamera();

	void Init(int, int, ID3D11DeviceContext*, XMVECTOR, XMVECTOR);
	void Render(XMVECTOR, XMVECTOR,bool,bool);
	void ResetViewport(ID3D11DeviceContext * devCon);
	//getter
	inline void GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = this->m_viewMatrix; };
	inline void GetOrthomatrix(XMMATRIX& orthoMatrix) { orthoMatrix = this->m_orthoMatrix; };
	inline void GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = this->m_projectionMatrix; };
	inline XMFLOAT3 GetLookAt() { return m_lookAtVector; };
public:
	float speedMovement = 1;
	float speedRotation = 1;
private:	
	//Matrix housing View Postion
	XMMATRIX m_viewMatrix;
	//Matrix housing projection positions
	XMMATRIX m_projectionMatrix;
	//Matrix for orhtigraphic projection
	XMMATRIX m_orthoMatrix;

	D3D11_VIEWPORT m_viewport;
	float m_fieldOfView;
	float m_screenAspect;
	XMFLOAT3 m_lookAtVector;
};

#endif
