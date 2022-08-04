#include "stdafx.h"
#include "Orb.h"

#include "Materials/UberMaterial/UberMaterial.h"
#include "Prefabs/Player/Player.h"

Orb::Orb()
{
}

void Orb::Initialize(const SceneContext& /*sceneContext*/)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	GetTransform()->Scale(.6f);
	const auto pOrbMesh = AddComponent(new ModelComponent(L"Meshes/Orb.ovm"));

	const auto pOrbActor = AddComponent(new RigidBodyComponent(false));
	pOrbActor->AddCollider(PxSphereGeometry(.1f), *pDefaultMaterial, false);
	pOrbActor->AddCollider(PxSphereGeometry(.8f), *pDefaultMaterial, true);
	SetOnTriggerCallBack(std::bind(&Orb::PickUpBall, this, std::placeholders::_2, std::placeholders::_3));
	
	const auto pOrbMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pOrbMaterial->SetDiffuseTexture(L"Textures/Orb/Orb_albedo.jpg");
	pOrbMaterial->SetSpecularTexture(L"Textures/Orb/Orb_roughness.jpg");
	pOrbMaterial->SetNormalTexture(L"Textures/Orb/Orb_normal.png");
	pOrbMaterial->SetEnvironmentTexture(L"Textures/Skybox.dds");

	pOrbMesh->SetMaterial(pOrbMaterial);
}

void Orb::Update(const SceneContext& /*sceneContext*/)
{
	if(m_pPlayerPickedUp != nullptr && m_IsPickedUp == true)
	{
		//this->GetScene()->RemoveChild(this);
		//m_pPlayerPickedUp->AddChild(this);
		//GetComponent<RigidBodyComponent>()->SetKinematic(true);
		GetTransform()->Translate(XMFLOAT3{m_pPlayerPickedUp->GetTransform()->GetPosition().x, m_pPlayerPickedUp->GetTransform()->GetPosition().y + 2.5f ,m_pPlayerPickedUp->GetTransform()->GetPosition().z});
		//m_pPlayerPickedUp = nullptr;
	}
}

void Orb::PickUpBall(GameObject* pPlayerObject, PxTriggerAction triggerAction)
{
	if (pPlayerObject->GetTag() == L"Player")
	{
		if (triggerAction == PxTriggerAction::ENTER)
		{
			m_pPlayerPickedUp = dynamic_cast<Player*>(pPlayerObject);
			m_pPlayerPickedUp->SetOrb(this);
			m_IsPickedUp = true;
		}
	}
}
