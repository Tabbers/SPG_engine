#pragma once
#include "gameobject.h"
#include "ParticelBillboardingShader.h"
#include "ParticleGenerationShader.h"
#include "globaldefinitions.h"

class Texture;

class ParticleSystem : public GameObject
{

public:
	ParticleSystem();
	~ParticleSystem();

	inline void SetMaximumNumberOfParticles(unsigned int i) { this->m_maximumNumberOfParticles = i; }
	void Init(ID3D11Device*, ID3D11DeviceContext*, XMVECTOR, XMVECTOR);
	void Render(ID3D11DeviceContext*, ID3D11Device*, float, Matrices, DirectX::XMVECTOR);
private:
	ParticleGenerationShader* m_pgs;
	ParticelBillboardingShader* m_pbs;
	Texture* m_diffuse;

	bool m_newEmitter = true;
	ID3D11ShaderResourceView* m_randomText;
	DirectX::XMFLOAT4 m_WorldUpVector = {0.0f,1.0f,0.0f,0.0f};
	float m_StartingVelocity = 10.0f;
	float m_Gravity = -10.0f;
	unsigned int m_maximumLifetime = 10;
	unsigned int m_maximumNumberOfParticles = 10000;
};

