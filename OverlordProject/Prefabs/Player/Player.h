#pragma once
class Orb;

struct PlayerDesc
{
	PlayerDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
		controller.stepOffset = 0.1f;
		controller.slopeLimit = std::cosf(0);
	}

	float maxMoveSpeed{ 5.f };
	float maxFallSpeed{ 10.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };
	
	int actionId_Jump{ -1 };
	int actionId_PassNext{ -1 };
	int actionId_PassPrevious{ -1 };
};

class Player : public GameObject
{
public:
	Player(const PlayerDesc& characterDesc, float radius);
	~Player() override = default;

	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) noexcept = delete;

	void DrawImGui();

	void SetActive(bool value);
	void SetHasOrb(bool value) { m_HasOrb = value; }
	bool HasOrb() const { return m_HasOrb; }
	
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};
	GameObject* m_pCharacterObject{};
	ModelAnimator* m_pAnimator{};

	PlayerDesc m_PlayerDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)
	const float m_RunningSpeed{ 2.5f },
		m_SprintingSpeed{6.f};

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	float m_PassingTimer{0};

	bool m_IsActive{ false };
	float m_CurrentAngle{0};
	bool m_IsSprinting{ false };

	bool m_HasOrb{ false };
	float m_AuraRadius{};
};