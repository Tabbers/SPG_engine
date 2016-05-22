#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "gameobject.h"

class Light : public GameObject
{
public:
	Light();
	~Light();

	void Init(int, int, ID3D11DeviceContext*,XMVECTOR,XMVECTOR);
	void Render(XMVECTOR, XMVECTOR, bool);
	void ResetViewport(ID3D11DeviceContext * devCon);
	//Getter
	inline void GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix= this->m_viewMatrix; };
	inline void GetProjectionMatrix(XMMATRIX& projectMatrix) { projectMatrix = this->m_projectionMatrix; };
	inline XMMATRIX GetViewMatrix() { return this->m_viewMatrix; };
	inline XMMATRIX GetProjectionMatrix() { return this->m_projectionMatrix; };
	inline XMFLOAT4 GetDiffuseColor() { return this->m_diffuseColor; };
	inline XMFLOAT4 GetAmbientColor() { return this->m_ambientColor; };
	inline XMFLOAT4 GetSpecularColor() { return this->m_specularColor; };
	inline float GetSpecularIntensity() { return this->m_secularIntensity; };
	inline XMFLOAT3 GetXMFLOAT3Position() {
		XMFLOAT3 Position;
		Position.x = m_position.m128_f32[0];
		Position.z = m_position.m128_f32[2];
		return Position;
	}
	inline XMFLOAT3 GetXMFLOAT3Rotation()
	{
		XMFLOAT3 Rotation;
		Rotation.x = m_rotation.m128_f32[0];
		Rotation.y = m_rotation.m128_f32[1];
		Rotation.z = m_rotation.m128_f32[2];
		return Rotation;
	}
	inline XMFLOAT3 GetLookAt() { return m_lookAtVector; };
	//Setter
	inline void SetAmbientColor(float r, float g, float b, float a) 
	{
		m_ambientColor.x = r;
		m_ambientColor.y = g;
		m_ambientColor.z = b;
		m_ambientColor.w = a;
	};
	inline void SetDiffuseColor(float r, float g, float b, float a)
	{
		m_diffuseColor.x = r;
		m_diffuseColor.y = g;
		m_diffuseColor.z = b;
		m_diffuseColor.w = a;
	};
	inline void SetSpecularColor(float r, float g, float b, float a,float intensity)
	{
		m_specularColor.x = r;
		m_specularColor.y = g;
		m_specularColor.z = b;
		m_specularColor.w = a;
		m_secularIntensity = intensity;
	};
public:
	float speedMovement = 10;
	float speedRotation = 1;
private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_specularColor;
	float m_secularIntensity;
	XMFLOAT3 m_lookAtVector;
	XMMATRIX m_viewMatrix;

	//Matrix housing projection positions
	XMMATRIX m_projectionMatrix;
	//Matrix for orhtigraphic projection
	XMMATRIX m_orthoMatrix;

	D3D11_VIEWPORT m_viewport;
	float m_fieldOfView;
	float m_screenAspect;
};
#endif