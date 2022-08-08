#include "stdafx.h"
#include "PyreScene.h"

#include "MenuScene.h"
#include "Materials/DiffuseMaterial_Skinned/DiffuseMaterial_Skinned.h"
#include "Materials/Post/Vignette.h"
#include "Materials/Shadow/ColorMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/CharacterHUD/CharacterHUD.h"
#include "Prefabs/Orb/Orb.h"
#include "Prefabs/Pedestal/Pedestal.h"
#include "Prefabs/Player/Player.h"
#include "Prefabs/Team/Team.h"

PyreScene::PyreScene()
	:GameScene(L"PyreScene")
	, m_pControlsFont(ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_32.fnt"))
{

}

void PyreScene::GoalReset(Player* pScoredPlayer) const
{
	m_pTeam_01->ResetTeam(pScoredPlayer);
	m_pTeam_02->ResetTeam(pScoredPlayer);

	m_pOrb->GetTransform()->Translate(0, 4, 0);
	m_pOrb->SetIsPickedUp(false);
}

void PyreScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_SceneContext.pLights->SetDirectionalLight({ 13.f,36.4f,-45.5f }, { -0.248543f, -0.690067f, 0.67973f });

	//Sound
	SoundManager::Get()->GetSystem()->createStream("Resources/Audio/GameTrack.mp3", FMOD_LOOP_NORMAL, nullptr, &m_pGameTrack);
	SoundManager::Get()->GetSystem()->playSound(m_pGameTrack, nullptr, true, &m_pChannel2D);
	m_pChannel2D->setVolume(.1f);
	
	//physics Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Orb
	m_pOrb = AddChild(new Orb());
	m_pOrb->GetTransform()->Translate(0, 4, 0);

	AddLevel();

	AddTeams();

	m_ControlsPosition = XMFLOAT3{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight - 120, 0.9f };
	AddControls();

	//Set Camera
	m_pCamera = AddChild(new FixedCamera());
	m_pCamera->GetTransform()->Translate(0.f, 25.f, -20.3f);
	constexpr auto cameraRotation = XMFLOAT4{ 0.413094f, 0.0101533f, -0.00460595f, 1.0f };
	const auto cameraRotationLoaded = XMLoadFloat4(&cameraRotation);
	m_CurrentCameraRotation = cameraRotationLoaded;
	m_pCamera->GetTransform()->Rotate(cameraRotationLoaded);
	m_pCamera->GetComponent<CameraComponent>()->SetActive(true);

	AddInputs();

	//Post Processing Stack
	//=====================
	m_pVignette = MaterialManager::Get()->CreateMaterial<Vignette>();
	AddPostProcessingEffect(m_pVignette);
}

void PyreScene::OnGUI()
{

}

void PyreScene::Update()
{
	UpdateCamera();
	if (m_SceneContext.pInput->IsActionTriggered(Pause))
	{
		const auto pSceneManager = SceneManager::Get();
		pSceneManager->AddGameScene(new MenuScene(true));
		pSceneManager->SetActiveGameScene(L"MenuScene");
	}
}

void PyreScene::Draw()
{
	TextRenderer::Get()->DrawText(m_pControlsFont, L"Move", XMFLOAT2{ m_ControlsPosition.x - 100, m_ControlsPosition.y-20}, XMFLOAT4{ Colors::White });
	TextRenderer::Get()->DrawText(m_pControlsFont, L"Sprint", XMFLOAT2{ m_ControlsPosition.x + 50, m_ControlsPosition.y - 20 }, XMFLOAT4{ Colors::White });
	TextRenderer::Get()->DrawText(m_pControlsFont, L"PassNext", XMFLOAT2{ m_ControlsPosition.x + 100, m_ControlsPosition.y + 10 }, XMFLOAT4{ Colors::White });
	TextRenderer::Get()->DrawText(m_pControlsFont, L"PassPrev", XMFLOAT2{ m_ControlsPosition.x - 185, m_ControlsPosition.y + 10 }, XMFLOAT4{ Colors::White });
}

void PyreScene::OnSceneActivated()
{
	m_pChannel2D->setPaused(false);
}

void PyreScene::OnSceneDeactivated()
{
	m_pChannel2D->setPaused(true);
}

void PyreScene::AddTeams()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// TEAM 1
	m_pTeam_01 = AddChild(new Team(1, m_pOrb, L"Textures/Mouse_albedo_blue.jpg", L"Textures/Aura.png"));
	//Characters
	PlayerDesc playerDesc{ pDefaultMaterial };
	playerDesc.actionId_Jump = Player01Jump;
	playerDesc.actionId_PassNext = Player01PassNext;
	playerDesc.actionId_PassPrevious = Player01PassPrevious;

	auto pCharacter = new Player(playerDesc,1);
	m_pTeam_01->AddPlayer(pCharacter, m_StartingPositions[0], true);

	pCharacter = new Player(playerDesc,1.2f);
	m_pTeam_01->AddPlayer(pCharacter, m_StartingPositions[1], false);

	pCharacter = new Player(playerDesc,.6f);
	m_pTeam_01->AddPlayer(pCharacter, m_StartingPositions[2], false);
	// pedestal
	m_pPedestal = new Pedestal(XMFLOAT2{ 100, m_SceneContext.windowHeight - 180 }, XMFLOAT4{ Colors::CadetBlue });
	m_pTeam_01->SetPedestal(m_pPedestal, { -15.f, -.6f, 0.f });
	// hud
	auto pCharacterHUD = new CharacterHUD();
	m_pTeam_01->SetHud(pCharacterHUD, { 200, m_SceneContext.windowHeight - 100, .8f });


	// TEAM 2
	m_pTeam_02 = AddChild(new Team(2, m_pOrb, L"Textures/Mouse_albedo_red.jpg", L"Textures/Aura_red.png"));
	//Characters
	playerDesc.actionId_Jump = Player02Jump;
	playerDesc.actionId_PassNext = Player02PassNext;
	playerDesc.actionId_PassPrevious = Player02PassPrevious;

	pCharacter = new Player(playerDesc,1);
	m_pTeam_02->AddPlayer(pCharacter, XMFLOAT3(-m_StartingPositions[0].x, m_StartingPositions[0].y, -m_StartingPositions[0].z), true);

	pCharacter = new Player(playerDesc,1.2f);
	m_pTeam_02->AddPlayer(pCharacter, XMFLOAT3(-m_StartingPositions[1].x, m_StartingPositions[1].y, -m_StartingPositions[1].z), false);

	pCharacter = new Player(playerDesc, .6f);
	m_pTeam_02->AddPlayer(pCharacter, XMFLOAT3(-m_StartingPositions[2].x, m_StartingPositions[2].y, -m_StartingPositions[2].z), false);
	// pedestal
	const auto pPedestalObject_02 = new Pedestal(XMFLOAT2{ m_SceneContext.windowWidth - 140, m_SceneContext.windowHeight - 180 }, XMFLOAT4{ Colors::IndianRed });
	m_pTeam_02->SetPedestal(pPedestalObject_02, { 15.f, -.6f, 0.f });
	// hud
	pCharacterHUD = new CharacterHUD();
	m_pTeam_02->SetHud(pCharacterHUD, { m_SceneContext.windowWidth - 200, m_SceneContext.windowHeight - 100, .8f });

}

void PyreScene::AddInputs() const
{
	//Pause
	auto inputAction = InputAction(Pause, InputState::pressed, -1, -1, XINPUT_GAMEPAD_START);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Player 1
	inputAction = InputAction(Player01Jump, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player01PassNext, InputState::pressed, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player01PassPrevious, InputState::pressed, -1, -1, XINPUT_GAMEPAD_LEFT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Player2
	inputAction = InputAction(Player02Jump, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player02PassNext, InputState::pressed, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Player02PassPrevious, InputState::pressed, -1, -1, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void PyreScene::AddLevel()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

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
	pGroundPlaneObject->GetTransform()->Scale(20);
	pGroundPlaneObject->GetTransform()->Translate(0, 0.f, 0);
	const auto pGroundPlaneMesh = pGroundPlaneObject->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));

	const auto pGroundPlaneMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	//const auto pGroundPlaneMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pGroundPlaneMaterial->SetColor(XMFLOAT4(0.39f, 0.23f, 0.12f, 1.0f));
	//pGroundPlaneMaterial->SetDiffuseTexture(L"Textures/Rocks/SmallRock_diffuse.png");

	pGroundPlaneMesh->SetMaterial(pGroundPlaneMaterial);


	//Rocks
	const auto pSmallRockObject_01 = AddChild(new GameObject());
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

	const auto pSmallRockObject_02 = AddChild(new GameObject());
	pSmallRockObject_02->GetTransform()->Translate(7, 1, -10);
	pSmallRockObject_02->GetTransform()->Rotate(95, 180, -5);
	pSmallRockObject_02->GetTransform()->Scale(.7f, .7f, .5f);
	const auto pSmallRockMesh_02 = pSmallRockObject_02->AddComponent(new ModelComponent(L"Meshes/SmallRock.ovm"));

	const auto pSmallRockActor_02 = pSmallRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pSmallRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/SmallRock.ovpc");
	pSmallRockActor_02->AddCollider(PxConvexMeshGeometry(pSmallRockConvexMesh_02, PxMeshScale({ .7f, .7f, .5f })), *pDefaultMaterial);

	pSmallRockMesh_02->SetMaterial(pSmallRockMaterial);

	const auto pSmallRockObject_03 = AddChild(new GameObject());
	pSmallRockObject_03->GetTransform()->Translate(-5, 1, 0);
	pSmallRockObject_03->GetTransform()->Rotate(95, 100, -5);
	pSmallRockObject_03->GetTransform()->Scale(.7f, .7f, .5f);
	const auto pSmallRockMesh_03 = pSmallRockObject_03->AddComponent(new ModelComponent(L"Meshes/SmallRock.ovm"));

	const auto pSmallRockActor_03 = pSmallRockObject_03->AddComponent(new RigidBodyComponent(true));
	const auto pSmallRockConvexMesh_03 = ContentManager::Load<PxConvexMesh>(L"Meshes/SmallRock.ovpc");
	pSmallRockActor_03->AddCollider(PxConvexMeshGeometry(pSmallRockConvexMesh_03, PxMeshScale({ .7f, .7f, .5f })), *pDefaultMaterial);

	pSmallRockMesh_03->SetMaterial(pSmallRockMaterial);

	const auto pMediumRockObject_01 = AddChild(new GameObject());
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

	const auto pMediumRockObject_02 = AddChild(new GameObject());
	pMediumRockObject_02->GetTransform()->Translate(1, 0, -12);
	pMediumRockObject_02->GetTransform()->Rotate(90, -30, -180);
	pMediumRockObject_02->GetTransform()->Scale(.5f, .5f, .2f);
	const auto pMediumRockMesh_02 = pMediumRockObject_02->AddComponent(new ModelComponent(L"Meshes/MediumRock.ovm"));

	const auto pMediumRockActor_02 = pMediumRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pMediumRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/MediumRock.ovpc");
	pMediumRockActor_02->AddCollider(PxConvexMeshGeometry(pMediumRockConvexMesh_02, PxMeshScale({ .5f, .5f, .2f })), *pDefaultMaterial);

	pMediumRockMesh_02->SetMaterial(pMediumRockMaterial);

	const auto pBigRockObject_01 = AddChild(new GameObject());
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

	const auto pBigRockObject_02 = AddChild(new GameObject());
	pBigRockObject_02->GetTransform()->Translate(5, 1.4f, 6);
	pBigRockObject_02->GetTransform()->Rotate(90, 0, 20);
	pBigRockObject_02->GetTransform()->Scale(.3f, .6f, 1.3f);
	const auto pBigRockMesh_02 = pBigRockObject_02->AddComponent(new ModelComponent(L"Meshes/BigRock.ovm"));

	const auto pBigRockActor_02 = pBigRockObject_02->AddComponent(new RigidBodyComponent(true));
	const auto pBigRockConvexMesh_02 = ContentManager::Load<PxConvexMesh>(L"Meshes/BigRock.ovpc");
	pBigRockActor_02->AddCollider(PxConvexMeshGeometry(pBigRockConvexMesh_02, PxMeshScale({ .3f, .6f, 1.3f })), *pDefaultMaterial);

	pBigRockMesh_02->SetMaterial(pBigRockMaterial);
}

void PyreScene::AddControls()
{
	const auto pControls = AddChild(new GameObject());
	pControls->GetTransform()->Scale(.7f);
	pControls->AddComponent(new SpriteComponent(L"Textures/Controls.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	pControls->GetTransform()->Translate(m_ControlsPosition);
}

void PyreScene::UpdateCamera()
{
	auto p1 = m_pTeam_01->GetActivePlayerPosition();
	auto p2 = m_pTeam_02->GetActivePlayerPosition();
	XMFLOAT3 midPoint = { (p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f, (p1.z + p2.z) / 2.f };
	XMFLOAT3 cameraPosition = m_pCamera->GetTransform()->GetPosition();

	//forward
	auto forwardVector = XMVector3Normalize(XMLoadFloat3(&midPoint) - XMLoadFloat3(&cameraPosition));
	XMFLOAT3 forward{};
	XMStoreFloat3(&forward, forwardVector);

	//right
	XMFLOAT3 worldUp{ 0,1,0 };
	auto rightVector = XMVector3Cross(XMLoadFloat3(&worldUp), forwardVector);
	XMFLOAT3 right{};
	XMStoreFloat3(&right, rightVector);

	//up
	auto upVector = XMVector3Cross(forwardVector, rightVector);
	XMFLOAT3 up{};
	XMStoreFloat3(&up, upVector);

	XMFLOAT4X4 rotationMatrix{
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		forward.x, forward.y, forward.z, 0,
		cameraPosition.x, cameraPosition.y, cameraPosition.z, 1
	};

	//const auto yAxis = (forward.x - right.z) / sqrt(powf((up.z - forward.y), 2.f) + powf((forward.x - right.z), 2.f) + powf((right.y - up.x), 2.f));
	auto rot = XMQuaternionRotationMatrix(XMLoadFloat4x4(&rotationMatrix));

	m_CurrentCameraRotation = XMVectorLerp(m_CurrentCameraRotation, rot, m_SceneContext.pGameTime->GetElapsed() * 2);

	const auto pCameraTransform = m_pCamera->GetTransform();
	pCameraTransform->Rotate(m_CurrentCameraRotation);
}
