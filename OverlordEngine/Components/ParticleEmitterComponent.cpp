#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount) :
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	TODO_W9(L"Implement Destructor");
	//Delete particle pool
	delete[] m_ParticlesArray;
	//Release vertex buffer
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Initialize");
	//Create instance of particle material if isn't created yet
	if (m_pParticleMaterial == nullptr)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}
	//Call CreateVertexBuffer
	CreateVertexBuffer(sceneContext);
	//Load particle texture
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement CreateVertexBuffer");
	//if m_pVertexVuffer allready exists, release it
	SafeRelease(m_pVertexBuffer);
	//Create a dynamic vertexbuffer
	D3D11_BUFFER_DESC vertexBufferDesc{};
	//	a.Set the usage to Dynamic
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//	b.Use the ParticleCountand size of a VertexParticle to calculate the ByteWidth
	vertexBufferDesc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	//	c.This is a VertexBuffer, select the appropriate BindFlag
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	d.We want to edit the buffer at - runtime, so we need CPU write access.Select the appropriate CPUAccessFlag
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	e.There are no MiscFlags(0)
	vertexBufferDesc.MiscFlags = 0;
	//	f.Create the VertexBuffer(GraphicsDevice > …)
	const auto result = sceneContext.d3dContext.pDevice->CreateBuffer(&vertexBufferDesc, nullptr, &m_pVertexBuffer);
	//	g.Don’t forget to check for errors!(HANDLE_ERROR)
	HANDLE_ERROR(result);
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Update");
	const float elapsedTime = sceneContext.pGameTime->GetElapsed();

	//1. Create a local variable, called particleInterval of type float.
	float particleInterval = ((m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2) / m_MaxParticles;

	//2. Increase m_LastParticleInit by the elapsed GameTime.
	m_LastParticleSpawn += elapsedTime;

	//3. Time to validate the particles and add some of them to the VertexBuffer
	//	a.Set m_ActiveParticles to zero
	m_ActiveParticles = 0;
	//	b.Use DeviceContext::Map to map our vertexbuffer,
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	c.Create an empty pointer of type VertexParticle(ParticleVertex * pBuffer) and cast& assign pData to it.
	VertexParticle* pBuffer = static_cast<VertexParticle*>(mappedResource.pData);
	//	d.The VertexBuffer is mapped, and we have access to write new data to it.
	//	e.Iterate the Particle Array(You only want to iterate ‘m_ParticleCount’ number of times)
	for (UINT i{}; i < m_ParticleCount; ++i)
	{
		//	f.For every particle :
		//		1. If the particle is currently Active > UpdateParticle function.
		if (m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], elapsedTime);
		}
		//2. If the particle is NOT Active(do not use ‘else if’), and the value of m_LastParticleInit is bigger than or equal to particleInterval
		if (m_ParticlesArray[i].isActive == false && m_LastParticleSpawn >= particleInterval)
		{
			//> Call SpawnParticle
			SpawnParticle(m_ParticlesArray[i]);
		}
		//3. If(after potential update and /or spawn) the particle is Active,
		if (m_ParticlesArray[i].isActive == true)
		{
			//		add the particle to the VertexBuffer, use m_ActiveParticles as index.
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			//		After that we increase m_ActiveParticles by one.
			++m_ActiveParticles;
		}
	}
	//	g.Use DeviceContext::Unmap to unmap our vertexbuffer.
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	TODO_W9(L"Implement UpdateParticle");
	//check if active
	if (p.isActive == false)
	{
		return;
	}

	//Subtract the elapsedTime from the particle’s currentEnergy
	p.currentEnergy -= elapsedTime;
	//If currentEnergy is smaller than ZERO, deactivate the particle and return
	if (p.currentEnergy <= 0.f)
	{
		p.isActive = false;
		return;
	}

	//update all variables of VertexPArticle
	//Position
	p.vertexInfo.Position = { p.vertexInfo.Position.x + (m_EmitterSettings.velocity.x * elapsedTime),
					p.vertexInfo.Position.y + (m_EmitterSettings.velocity.y * elapsedTime),
					p.vertexInfo.Position.z + (m_EmitterSettings.velocity.z * elapsedTime) };
	//life percent variable
	const float lifePercent{ p.currentEnergy / p.totalEnergy };
	//Color
	p.vertexInfo.Color = { m_EmitterSettings.color.x, m_EmitterSettings.color.y,
					m_EmitterSettings.color.z, lifePercent };
	//Size
	p.vertexInfo.Size = (p.initialSize + p.sizeChange * (1.0f - lifePercent));
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	TODO_W9(L"Implement SpawnParticle");
	//Set particle’s isActive to true
	p.isActive = true;

	//Energy
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	//Position
	constexpr auto randomDirection = XMFLOAT3{ 1,0,0 };
	const auto rotationMatrix = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	const auto randomRotation{ XMVector3TransformNormal(XMLoadFloat3(&randomDirection), rotationMatrix) };

	const float randomDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	const auto position = XMLoadFloat3(&GetTransform()->GetPosition());
	XMStoreFloat3(&p.vertexInfo.Position, position + randomRotation * randomDistance);

	//Size
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//Rotation
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//Color
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement PostDraw");
	//Set varibles
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	//get technique- and device context
	auto techniqueContext{ m_pParticleMaterial->GetTechniqueContext() };
	auto pDeviceContext{ sceneContext.d3dContext.pDeviceContext };

	//set inputlayout
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);
	//set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	//set vertex buffer
	constexpr UINT offset = 0;
	constexpr UINT stride = sizeof(VertexParticle);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//For each pass of technique, apply pass and draw vertices
	D3DX11_TECHNIQUE_DESC techDesc{};
	techniqueContext.pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		techniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if (ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}