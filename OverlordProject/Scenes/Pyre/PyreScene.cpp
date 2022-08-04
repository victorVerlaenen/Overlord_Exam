#include "stdafx.h"
#include "PyreScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial/DiffuseMaterial.h"
#include "Materials/Shadow/ColorMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/UberMaterial/UberMaterial.h"
#include "Prefabs/CharacterHUD/CharacterHUD.h"
#include "Prefabs/Orb/Orb.h"
#include "Prefabs/Pedestal/Pedestal.h"
#include "Prefabs/Player/Player.h"

void PyreScene::GoalReset() const
{
	Player::m_pTeam01[0]->GetTransform()->Translate(m_StartingPositions[0]);
	Player::m_pTeam01[0]->SetActive(true);

	Player::m_pTeam01[1]->GetTransform()->Translate(m_StartingPositions[1]);
	Player::m_pTeam01[1]->SetActive(false);

	Player::m_pTeam01[2]->GetTransform()->Translate(m_StartingPositions[2]);
	Player::m_pTeam01[2]->SetActive(false);

	//Player2
	Player::m_pTeam02[0]->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[0].x, m_StartingPositions[0].y, -m_StartingPositions[0].z));
	Player::m_pTeam02[0]->SetActive(true);

	Player::m_pTeam02[1]->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[1].x, m_StartingPositions[1].y, -m_StartingPositions[1].z));
	Player::m_pTeam02[1]->SetActive(false);

	Player::m_pTeam02[2]->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[2].x, m_StartingPositions[2].y, -m_StartingPositions[2].z));
	Player::m_pTeam02[2]->SetActive(false);

	m_pOrb->GetTransform()->Translate(0, 4, 0);
	m_pOrb->SetIsPickedUp(false);
}

void PyreScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_SceneContext.pLights->SetDirectionalLight({ 13.f,36.4f,-45.5f }, { -0.248543f, -0.690067f, 0.67973f });

	//physics Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Character
	PlayerDesc playerDesc{ pDefaultMaterial };
	playerDesc.actionId_Jump = Player01Jump;
	playerDesc.actionId_PassNext = Player01PassNext;
	playerDesc.actionId_PassPrevious = Player01PassPrevious;

	//Player1
	auto pCharacter = AddChild(new Player(playerDesc));

	pCharacter->GetTransform()->Translate(m_StartingPositions[0]);
	pCharacter->SetActive(true);

	pCharacter = AddChild(new Player(playerDesc));

	pCharacter->GetTransform()->Translate(m_StartingPositions[1]);
	pCharacter->SetActive(false);

	pCharacter = AddChild(new Player(playerDesc));

	pCharacter->GetTransform()->Translate(m_StartingPositions[2]);
	pCharacter->SetActive(false);

	//Player2
	playerDesc.actionId_Jump = Player02Jump;
	playerDesc.actionId_PassNext = Player02PassNext;
	playerDesc.actionId_PassPrevious = Player02PassPrevious;

	pCharacter = AddChild(new Player(playerDesc, false));

	pCharacter->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[0].x, m_StartingPositions[0].y, -m_StartingPositions[0].z));
	pCharacter->SetActive(true);

	pCharacter = AddChild(new Player(playerDesc, false));

	pCharacter->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[1].x, m_StartingPositions[1].y, -m_StartingPositions[1].z));
	pCharacter->SetActive(false);

	pCharacter = AddChild(new Player(playerDesc, false));

	pCharacter->GetTransform()->Translate(XMFLOAT3(-m_StartingPositions[2].x, m_StartingPositions[2].y, -m_StartingPositions[2].z));
	pCharacter->SetActive(false);

	/*auto pAura = m_pCharacter->AddChild(new GameObject());
	pAura->GetTransform()->Translate(0, -1.f, 0);
	auto pModel = pAura->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));

	const auto pAuraMaterial_01 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pAuraMaterial_01->SetDiffuseTexture(L"Textures/Aura.png");

	pModel->SetMaterial(pAuraMaterial_01);*/

	//The map
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Map.ovm"));

	const auto pCliffMaterial_01 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCliffMaterial_01->SetDiffuseTexture(L"Textures/Map/Mat1_diffuse.png");

	const auto pCliffMaterial_02 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCliffMaterial_02->SetDiffuseTexture(L"Textures/Map/Mat2_diffuse.png");

	const auto pCliffMaterial_03 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCliffMaterial_03->SetDiffuseTexture(L"Textures/Map/Mat3_diffuse.png");

	const auto pCliffMaterial_04 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCliffMaterial_04->SetDiffuseTexture(L"Textures/Map/Mat4_diffuse.png");

	const auto pCliffMaterial_05 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCliffMaterial_05->SetDiffuseTexture(L"Textures/Map/Mat5_diffuse.png");

	pLevelMesh->SetMaterial(pCliffMaterial_04, 0);
	pLevelMesh->SetMaterial(pCliffMaterial_05, 1);
	pLevelMesh->SetMaterial(pCliffMaterial_02, 2);
	pLevelMesh->SetMaterial(pCliffMaterial_01, 3);
	pLevelMesh->SetMaterial(pCliffMaterial_03, 4);
	pLevelObject->GetTransform()->Scale(.01f, .01f, .01f);
	pLevelObject->GetTransform()->Rotate(-2.f, 20.f, 0.f);
	pLevelObject->GetTransform()->Translate(7.f, -.3f, 15.f);

	const auto pMapActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pMapTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Map.ovpt");
	pMapActor->AddCollider(PxTriangleMeshGeometry(pMapTriangleMesh, PxMeshScale({ .01f, .01f, .01f })), *pDefaultMaterial);

	//Ground 
	const auto pGroundPlaneObject = AddChild(new GameObject());
	pGroundPlaneObject->GetTransform()->Scale(10);
	pGroundPlaneObject->GetTransform()->Translate(0, 0.f, 0);
	const auto pGroundPlaneMesh = pGroundPlaneObject->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));

	const auto pGroundPlaneMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pGroundPlaneMaterial->SetColor(XMFLOAT4(0.62f, 0.38f, 0.23f, 1.0f));

	pGroundPlaneMesh->SetMaterial(pGroundPlaneMaterial);

	//Orb
	m_pOrb = AddChild(new Orb());
	m_pOrb->GetTransform()->Translate(0, 4, 0);


	//Rocks
	auto pSmallRockObject_01 = AddChild(new GameObject());
	pSmallRockObject_01->GetTransform()->Translate(-7, 1, -10);
	pSmallRockObject_01->GetTransform()->Rotate(95, 0, -5);
	pSmallRockObject_01->GetTransform()->Scale(.7f, .7f, .5f);
	const auto pSmallRockMesh = pSmallRockObject_01->AddComponent(new ModelComponent(L"Meshes/SmallRock.ovm"));

	const auto pSmallRockActor_01 = pSmallRockObject_01->AddComponent(new RigidBodyComponent(true));
	const auto pSmallRockConvexMesh_01 = ContentManager::Load<PxConvexMesh>(L"Meshes/SmallRock.ovpc");
	pSmallRockActor_01->AddCollider(PxConvexMeshGeometry(pSmallRockConvexMesh_01, PxMeshScale({ .7f, .7f, .5f })), *pDefaultMaterial);

	const auto pSmallRockMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pSmallRockMaterial->SetDiffuseTexture(L"Textures/Rocks/SmallRock_diffuse.png");

	pSmallRockMesh->SetMaterial(pSmallRockMaterial);

	auto pSmallRockObject_02 = AddChild(new GameObject());
	pSmallRockObject_02->GetTransform()->Translate(7, 1, -10);
	pSmallRockObject_02->GetTransform()->Rotate(95, 180, -5);
	pSmallRockObject_02->GetTransform()->Scale(.7f, .7f, .5f);
	const auto pSmallRockMesh_02 = pSmallRockObject_02->AddComponent(new ModelComponent(L"Meshes/SmallRock.ovm"));

	const auto pSmallRockActor_02 = pSmallRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pSmallRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/SmallRock.ovpc");
	pSmallRockActor_02->AddCollider(PxConvexMeshGeometry(pSmallRockConvexMesh_02, PxMeshScale({ .7f, .7f, .5f })), *pDefaultMaterial);

	pSmallRockMesh_02->SetMaterial(pSmallRockMaterial);

	auto pSmallRockObject_03 = AddChild(new GameObject());
	pSmallRockObject_03->GetTransform()->Translate(-5, 1, 0);
	pSmallRockObject_03->GetTransform()->Rotate(95, 100, -5);
	pSmallRockObject_03->GetTransform()->Scale(.7f, .7f, .5f);
	const auto pSmallRockMesh_03 = pSmallRockObject_03->AddComponent(new ModelComponent(L"Meshes/SmallRock.ovm"));

	const auto pSmallRockActor_03 = pSmallRockObject_03->AddComponent(new RigidBodyComponent(true));
	const auto pSmallRockConvexMesh_03 = ContentManager::Load<PxConvexMesh>(L"Meshes/SmallRock.ovpc");
	pSmallRockActor_03->AddCollider(PxConvexMeshGeometry(pSmallRockConvexMesh_03, PxMeshScale({ .7f, .7f, .5f })), *pDefaultMaterial);

	pSmallRockMesh_03->SetMaterial(pSmallRockMaterial);

	auto pMediumRockObject_01 = AddChild(new GameObject());
	pMediumRockObject_01->GetTransform()->Translate(4, 0, 0);
	pMediumRockObject_01->GetTransform()->Rotate(90, -30, 0);
	pMediumRockObject_01->GetTransform()->Scale(.5f, .5f, .2f);
	const auto pMediumRockMesh_01 = pMediumRockObject_01->AddComponent(new ModelComponent(L"Meshes/MediumRock.ovm"));

	const auto pMediumRockActor_01 = pMediumRockObject_01->AddComponent(new RigidBodyComponent(true));
	const auto pMediumRockConvexMesh_01 = ContentManager::Load<PxConvexMesh>(L"Meshes/MediumRock.ovpc");
	pMediumRockActor_01->AddCollider(PxConvexMeshGeometry(pMediumRockConvexMesh_01, PxMeshScale({ .5f, .5f, .2f })), *pDefaultMaterial);

	const auto pMediumRockMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMediumRockMaterial->SetDiffuseTexture(L"Textures/Rocks/MediumRock_diffuse.png");

	pMediumRockMesh_01->SetMaterial(pMediumRockMaterial);

	auto pMediumRockObject_02 = AddChild(new GameObject());
	pMediumRockObject_02->GetTransform()->Translate(1, 0, -12);
	pMediumRockObject_02->GetTransform()->Rotate(90, -30, -180);
	pMediumRockObject_02->GetTransform()->Scale(.5f, .5f, .2f);
	const auto pMediumRockMesh_02 = pMediumRockObject_02->AddComponent(new ModelComponent(L"Meshes/MediumRock.ovm"));

	const auto pMediumRockActor_02 = pMediumRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pMediumRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/MediumRock.ovpc");
	pMediumRockActor_02->AddCollider(PxConvexMeshGeometry(pMediumRockConvexMesh_02, PxMeshScale({ .5f, .5f, .2f })), *pDefaultMaterial);

	pMediumRockMesh_02->SetMaterial(pMediumRockMaterial);

	auto pBigRockObject_01 = AddChild(new GameObject());
	pBigRockObject_01->GetTransform()->Translate(-5, 1.4f, 6);
	pBigRockObject_01->GetTransform()->Rotate(90, 0, 220);
	pBigRockObject_01->GetTransform()->Scale(.3f, .6f, 1.3f);
	const auto pBigRockMesh_01 = pBigRockObject_01->AddComponent(new ModelComponent(L"Meshes/BigRock.ovm"));

	const auto pBigRockActor_01 = pBigRockObject_01->AddComponent(new RigidBodyComponent(true));
	const auto pBigRockConvexMesh_01 = ContentManager::Load<PxConvexMesh>(L"Meshes/BigRock.ovpc");
	pBigRockActor_01->AddCollider(PxConvexMeshGeometry(pBigRockConvexMesh_01, PxMeshScale({ .3f, .6f, 1.3f })), *pDefaultMaterial);

	const auto pBigRockMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pBigRockMaterial->SetDiffuseTexture(L"Textures/Rocks/BigRock_diffuse.png");

	pBigRockMesh_01->SetMaterial(pBigRockMaterial);

	auto pBigRockObject_02 = AddChild(new GameObject());
	pBigRockObject_02->GetTransform()->Translate(5, 1.4f, 6);
	pBigRockObject_02->GetTransform()->Rotate(90, 0, 20);
	pBigRockObject_02->GetTransform()->Scale(.3f, .6f, 1.3f);
	const auto pBigRockMesh_02 = pBigRockObject_02->AddComponent(new ModelComponent(L"Meshes/BigRock.ovm"));

	const auto pBigRockActor_02 = pBigRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pBigRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/BigRock.ovpc");
	pBigRockActor_02->AddCollider(PxConvexMeshGeometry(pBigRockConvexMesh_02, PxMeshScale({ .3f, .6f, 1.3f })), *pDefaultMaterial);

	pBigRockMesh_02->SetMaterial(pBigRockMaterial);

	//Pedestals
	const auto pPedestalObject_01 = AddChild(new Pedestal(XMFLOAT2{ 20, m_SceneContext.windowHeight - 84 }, false));
	pPedestalObject_01->GetTransform()->Scale(2);
	pPedestalObject_01->GetTransform()->Rotate(90, 0, 0);
	pPedestalObject_01->GetTransform()->Translate(-15, -.6f, 0);

	const auto pPedestalObject_02 = AddChild(new Pedestal(XMFLOAT2{ m_SceneContext.windowWidth - 100, m_SceneContext.windowHeight - 84 }, true));
	pPedestalObject_02->GetTransform()->Scale(2);
	pPedestalObject_02->GetTransform()->Rotate(90, 0, 0);
	pPedestalObject_02->GetTransform()->Translate(15, -.6f, 0);

	//Fire
	ParticleEmitterSettings settings{};

	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 2.f;
	settings.maxSize = 3.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = -3.f;
	settings.maxScale = -4.0f;
	settings.minEmitterRadius = .5f;
	settings.maxEmitterRadius = .7f;
	settings.color = { 0.435f,0.109f,0.721f, .6f };

	const auto pRightFire = AddChild(new GameObject);
	auto pRightFireEmitter = pRightFire->AddComponent(new ParticleEmitterComponent(L"Textures/Fire.png", settings, 200));
	pRightFireEmitter->GetTransform()->Translate(15, -.6f, 0);

	const auto pLeftFire = AddChild(new GameObject);
	auto pLeftFireEmitter = pLeftFire->AddComponent(new ParticleEmitterComponent(L"Textures/Fire.png", settings, 200));
	pLeftFireEmitter->GetTransform()->Translate(-15, -.6f, 0);

	//Camera
	const auto pFixedCamera = AddChild(new FixedCamera());

	pFixedCamera->GetTransform()->Translate(0.f, 21.f, -20.3f);

	constexpr auto cameraRotation = XMFLOAT4{ 0.413094f, 0.0101533f, -0.00460595f, 1.0f };
	const auto cameraRotationLoaded = XMLoadFloat4(&cameraRotation);
	pFixedCamera->GetTransform()->Rotate(cameraRotationLoaded);

	pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);

	//HUD
	auto pCharacterHUD = AddChild(new CharacterHUD());
	pCharacterHUD->GetTransform()->Translate(150, m_SceneContext.windowHeight - 100, .8f);

	pCharacterHUD = AddChild(new CharacterHUD());
	pCharacterHUD->GetTransform()->Translate(m_SceneContext.windowWidth- 150, m_SceneContext.windowHeight - 100, .8f);

	//Input
	//Player 1
	auto inputAction = InputAction(Player01Jump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player01PassNext, InputState::pressed, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player01PassPrevious, InputState::pressed, -1, -1, XINPUT_GAMEPAD_LEFT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Player2
	inputAction = InputAction(Player02Jump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player02PassNext, InputState::pressed, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player02PassPrevious, InputState::pressed, -1, -1, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);
}


void PyreScene::OnGUI()
{

}

void PyreScene::Update()
{

}

void PyreScene::PostDraw()
{

}
