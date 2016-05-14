#include "ParticleSystem.h"
#include "texture.h"
#include "Randomizer.h"
#include <time.h>
namespace ParticleHelper
{
	//Creates random Vectors inside a unitsphere
	//Used to Initialize the Particle System
	static DirectX::XMVECTOR RandUnitVect3()
	{
		XMVECTOR v = XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f);;
		XMVECTOR one = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		while (DirectX::XMVector3Greater(DirectX::XMVector3LengthSq(v),one))
		{
			v = XMVectorSet(Randomizer::GetRandom(-1,1), Randomizer::GetRandom(-1, 1), Randomizer::GetRandom(-1, 1),0.0f);
		}
		return v;
	}
	//create a random texture used to be sampled in the Shader 
	static ID3D11ShaderResourceView* CreateRandomeTexture(ID3D11Device* device )
	{
		DirectX::XMFLOAT4 randomValues[1024];

		//Fill the Data for the Random texture
		for (unsigned int i = 0; i < 1024; ++i)
		{
			randomValues[i].x = Randomizer::GetRandom(-1, 1);
			randomValues[i].y = Randomizer::GetRandom(-1, 1);
			randomValues[i].z = Randomizer::GetRandom(-1, 1);
			randomValues[i].w = Randomizer::GetRandom(-1, 1);
		}
		D3D11_SUBRESOURCE_DATA initData;
		initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		initData.pSysMem = randomValues;
		initData.SysMemSlicePitch = 0;

		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;
		
		ID3D11Texture1D* randomTex = 0;
		device->CreateTexture1D(&texDesc, &initData, &randomTex);
		//
		// Create the resource view.
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;
		ID3D11ShaderResourceView* randomTexSRV = 0;
		device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV);
		
		randomTex->Release();
		return randomTexSRV;
	}
}

ParticleSystem::ParticleSystem() : m_diffuse(nullptr),
m_pbs(nullptr),
m_pgs(nullptr),
m_randomText(nullptr)
{
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Init(ID3D11Device* device, ID3D11DeviceContext *devcon, XMVECTOR pos, XMVECTOR rot)
{
	this->m_position = pos;
	this->m_rotation = rot;

	m_randomText = ParticleHelper::CreateRandomeTexture(device);

	m_pgs = new ParticleGenerationShader();
	m_pbs = new ParticelBillboardingShader();

	Particle src;
	src.Age = 0.0f;
	src.InitialPos.x = m_position.m128_f32[0];
	src.InitialPos.y = m_position.m128_f32[1];
	src.InitialPos.z = m_position.m128_f32[2];
	src.InitialPos.w = 1.0f;
	src.InitialVel = { 0,0,0,0};
	src.Size = { 1,1 };
	src.Type = 0;
	src.Interval = 1.0f;

	m_pgs->Init(4096, device, &src, devcon, "Shaders/ParticleUpdateVS.hlsl", "Shaders/ParticleUpdateGS.hlsl");
	m_pbs->Init(devcon, device, "Shaders/ParticleQuadifyVs.hlsl", "Shaders/ParticleQuadifyGs.hlsl", "Shaders/ParticleQuadifyPs.hlsl");

	m_diffuse = new Texture();
	m_diffuse->Init(device, devcon, L"Texture/particle/smoke.dds");
}

void ParticleSystem::Render(ID3D11DeviceContext *devcon , ID3D11Device* device, float deltatime, Matrices sceneInfo, DirectX::XMVECTOR cameraPos)
{
	DataBufferType data;
	data.Data.x = deltatime;
	data.Data.y = static_cast<float>(rand()/RAND_MAX);
	data.Data.z = m_StartingVelocity;
	data.Data.w = static_cast<float>(m_maximumLifetime);

	data.Random.x = Randomizer::GetRandom(0, 10);
	data.Random.y = Randomizer::GetRandom(0, 10);
	data.Random.z = Randomizer::GetRandom(0, 10);
	data.Random.w = Randomizer::GetRandom(0, 10);

	if (m_newEmitter)
	{
		m_pgs->RenderSingle(devcon, device, sceneInfo, m_randomText, data);
		m_newEmitter = false;
	}
	else
	{
		m_pgs->Render(devcon, device, sceneInfo, m_randomText, data);
	}
	m_pbs->Render(devcon, device, sceneInfo,m_pgs->GetVertexBuffer(),m_diffuse->GetResourceView(), cameraPos);
	m_pgs->SwapBuffer();
}
