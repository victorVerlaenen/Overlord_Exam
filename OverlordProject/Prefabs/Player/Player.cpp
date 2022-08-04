#include "stdafx.h"
#include "Player.h"

#include "Materials/DiffuseMaterial_Skinned/DiffuseMaterial_Skinned.h"
#include "Prefabs/CharacterHUD/CharacterHUD.h"
#include "Prefabs/Orb/Orb.h"

std::vector<Player*> Player::m_pTeam01 = std::vector<Player*>{};
std::vector<Player*> Player::m_pTeam02 = std::vector<Player*>{};

Player::Player(const PlayerDesc& characterDesc, bool isTeamOne) :
	m_PlayerDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_IsTeamOne(isTeamOne)
{

}

bool Player::HasOrb() const
{
	if (m_pOrb != nullptr)
	{
		return true;
	}
	return false;
}

bool Player::IsTeamOne() const
{
	if(m_IsTeamOne)
	{
		return true;
	}
	return false;
}

void Player::Initialize(const SceneContext& /*sceneContext*/)
{
	SetTag(L"Player");
	//const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_PlayerDesc.controller));
	m_pControllerComponent->SetCollisionGroup(CollisionGroup::Group0);

	//Player
	m_pCharacterObject = AddChild(new GameObject());
	m_pCharacterObject->GetTransform()->Scale(0.02f);
	const auto pModelComponent = m_pCharacterObject->AddComponent(new ModelComponent(L"Meshes/Mouse.ovm"));


	const auto pMouseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();

	if (m_IsTeamOne)
	{
		pMouseMaterial->SetDiffuseTexture(L"Textures/Mouse_albedo_red.jpg");
		m_pTeam01.emplace_back(this);
		m_pTeam = &m_pTeam01;
	}
	else
	{
		pMouseMaterial->SetDiffuseTexture(L"Textures/Mouse_albedo_blue.jpg");
		m_pTeam02.emplace_back(this);
		m_pTeam = &m_pTeam02;
	}
	pModelComponent->SetMaterial(pMouseMaterial);

	m_pCharacterObject->GetTransform()->Translate(m_pControllerComponent->GetFootPosition());

	m_pAnimator = pModelComponent->GetAnimator();
	m_pAnimator->SetAnimation(L"Idle");
	m_pAnimator->Play();
	std::wcout << m_pAnimator->GetClipName() << std::endl;

	//Camera
	//const auto pCamera = AddChild(new FixedCamera());
	//m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	//m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	//pCamera->GetTransform()->Translate(0.f, m_PlayerDesc.controller.height * .5f, -10.f);
}

void Player::Update(const SceneContext& sceneContext)
{
	if (m_IsActive)
	{
		//Passing behaviour
		///////////////////////
		//Check if this player has the orb
		if (HasOrb())
		{
			//If orb just got passed => do nothing
			if (m_pOrb->GetPassed() == true)
			{
				m_pOrb->SetPassed(false);
				return;
			}

			int currentIndex{ 0 };
			//Check for pass input
			if (sceneContext.pInput->IsActionTriggered(m_PlayerDesc.actionId_PassNext))
			{
				//get current active player
				for (int i{}; i < m_pTeam->size(); ++i)
				{
					if (m_pTeam->at(i) == this)
					{
						currentIndex = i;
						break;
					}
				}

				//Set next player in list to active (and also set the orb and other player variables)
				if (currentIndex == m_pTeam->size() - 1)
				{
					//Pass to 0
					m_pOrb->SetPlayer(m_pTeam->at(0));
					m_pTeam->at(0)->SetActive(true);
					m_pTeam->at(0)->SetOrb(m_pOrb);
				}
				else
				{
					//Pass next
					m_pOrb->SetPlayer(m_pTeam->at(currentIndex + 1));

					m_pTeam->at(currentIndex + 1)->SetActive(true);
					m_pTeam->at(currentIndex + 1)->SetOrb(m_pOrb);
				}

				//deactivate current player and set to idle
				SetActive(false);
				if (m_pAnimator->GetClipName() != L"Idle")
				{
					m_pAnimator->SetAnimation(L"Idle");
				}

				//set orb to just passed and make sure this player doesnt hold the orb anymore
				m_pOrb->SetPassed(true);
				m_pOrb = nullptr;

				//Set Character HUD
				

				return;
			}
			//Idem above
			if (sceneContext.pInput->IsActionTriggered(m_PlayerDesc.actionId_PassPrevious))
			{
				for (int i{}; i < m_pTeam->size(); ++i)
				{
					if (m_pTeam->at(i) == this)
					{
						currentIndex = i;
						break;
					}
				}
				if (currentIndex == 0)
				{
					//Pass to Last
					m_pOrb->SetPlayer(m_pTeam->at(m_pTeam->size() - 1));
					m_pTeam->at(m_pTeam->size() - 1)->SetActive(true);
					m_pTeam->at(m_pTeam->size() - 1)->SetOrb(m_pOrb);
				}
				else
				{
					//Pass previous
					m_pOrb->SetPlayer(m_pTeam->at(currentIndex - 1));
					m_pTeam->at(currentIndex - 1)->SetActive(true);
					m_pTeam->at(currentIndex - 1)->SetOrb(m_pOrb);
				}

				SetActive(false);
				if (m_pAnimator->GetClipName() != L"Idle")
				{
					m_pAnimator->SetAnimation(L"Idle");
				}
				m_pOrb->SetPassed(true);
				m_pOrb = nullptr;
				return;
			}
		}

		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
		bool isMoving{ false };

		//***************
		//HANDLE INPUT

		//if right trigger is pressed
		float triggerPressure{};
		triggerPressure = sceneContext.pInput->GetTriggerPressure(false, m_IsTeamOne ? GamepadIndex::playerOne : GamepadIndex::playerTwo);

		if (triggerPressure > epsilon)
		{
			m_PlayerDesc.maxMoveSpeed = m_SprintingSpeed;
			m_IsSprinting = true;
		}
		else
		{
			m_PlayerDesc.maxMoveSpeed = m_RunningSpeed;
			m_IsSprinting = false;
		}

		//## Input Gathering (move)
		XMFLOAT2 move{ 0,0 }; //Uncomment
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (std::abs(move.y) < epsilon)
		{
			if (m_IsTeamOne == true)
			{
				move.y = sceneContext.pInput->GetThumbstickPosition(true, GamepadIndex::playerOne).y;
			}
			else
			{
				move.y = sceneContext.pInput->GetThumbstickPosition(true, GamepadIndex::playerTwo).y;
			}
		}
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (std::abs(move.x) < epsilon)
		{
			if (m_IsTeamOne == true)
			{
				move.x = sceneContext.pInput->GetThumbstickPosition(true, GamepadIndex::playerOne).x;
			}
			else
			{
				move.x = sceneContext.pInput->GetThumbstickPosition(true, GamepadIndex::playerTwo).x;
			}
		}


		if (move.x != 0 || move.y != 0)
		{
			isMoving = true;
		}
		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		const auto transformComponent = GetTransform();
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		const XMVECTOR forward{ transformComponent->GetForward().x, transformComponent->GetForward().y, transformComponent->GetForward().z };
		const XMVECTOR right{ transformComponent->GetRight().x, transformComponent->GetRight().y, transformComponent->GetRight().z };

		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		const float currentAcceleration = m_MoveAcceleration * sceneContext.pGameTime->GetElapsed();
		//If the character is moving (= input is pressed)
		if (isMoving)
		{
			if (m_pAnimator->GetClipName() != L"Running" && m_IsSprinting == false)
			{
				m_pAnimator->SetAnimation(L"Running");
			}
			else if (m_pAnimator->GetClipName() != L"Sprinting" && m_IsSprinting == true)
			{
				m_pAnimator->SetAnimation(L"Sprinting");
			}
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			const XMVECTOR newDirection = forward * move.y + right * move.x;
			XMStoreFloat3(&m_CurrentDirection, newDirection);
			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += currentAcceleration;
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			m_MoveSpeed = std::clamp(m_MoveSpeed, 0.f, m_PlayerDesc.maxMoveSpeed);

			auto angle = XMVector3AngleBetweenVectors(XMLoadFloat3(&GetTransform()->GetForward()), XMVector3Normalize(newDirection));

			//Small rotation fix
			if (move.x < 0)
				angle *= -1;

			XMStoreFloat(&m_CurrentAngle, angle);
			//Rotate player in the move direction
			m_pCharacterObject->GetTransform()->Rotate(0, XMConvertToDegrees(m_CurrentAngle) + 180, 0);
		}
		//Else (character is not moving, or stopped moving)
		else
		{
			if (m_pAnimator->GetClipName() != L"Idle")
			{
				m_pAnimator->SetAnimation(L"Idle");
			}
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= currentAcceleration;
			//Make sure the current MoveSpeed doesn't get smaller than zero
			m_MoveSpeed = std::clamp(m_MoveSpeed, 0.f, m_PlayerDesc.maxMoveSpeed);
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		const XMFLOAT2 horizontalVelocity{ m_CurrentDirection.x * m_MoveSpeed, m_CurrentDirection.z * m_MoveSpeed };
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)
		m_TotalVelocity.x = horizontalVelocity.x;
		m_TotalVelocity.z = horizontalVelocity.y;

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
		if (m_pControllerComponent->GetCollisionFlags() != PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * sceneContext.pGameTime->GetElapsed();
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_PlayerDesc.maxFallSpeed);
		}
		//Else If the jump action is triggered
		else if (sceneContext.pInput->IsActionTriggered(m_PlayerDesc.actionId_Jump))
		{
			//Disable collision

		}
		//Else (=Character is grounded, no input pressed)
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = 0;
		}

		//************
		//DISPLACEMENT
		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		const XMFLOAT3 displacement{ m_TotalVelocity.x * sceneContext.pGameTime->GetElapsed(),
		m_TotalVelocity.y * sceneContext.pGameTime->GetElapsed(),
		m_TotalVelocity.z * sceneContext.pGameTime->GetElapsed() };

		m_pControllerComponent->Move(displacement);

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
}

void Player::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_PlayerDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_PlayerDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_PlayerDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_PlayerDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_PlayerDesc.maxMoveSpeed / m_PlayerDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_PlayerDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_PlayerDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_PlayerDesc.maxFallSpeed / m_PlayerDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_PlayerDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_PlayerDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if (ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}

void Player::SetActive(bool value)
{
	m_IsActive = value;
	if (m_pAnimator->GetClipName() != L"Idle")
	{
		m_pAnimator->SetAnimation(L"Idle");
	}
}
