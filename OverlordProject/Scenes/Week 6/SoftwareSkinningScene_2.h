#pragma once
class BoneObject;

class SoftwareSkinningScene_2 : public GameScene
{
public:
	SoftwareSkinningScene_2() : GameScene(L"SoftwareSkinningScene_2") {}
	~SoftwareSkinningScene_2() override = default;
	SoftwareSkinningScene_2(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2(SoftwareSkinningScene_2&& other) noexcept = delete;
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2& operator=(SoftwareSkinningScene_2&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	BoneObject* m_pBone0{}, * m_pBone1{};

	float m_BoneRotation{};
	int m_RotationSign{ 1 };

	bool m_AutoRotate{ true };
	float m_Bone0Rotation[3]{ 0.0f, 0.0f, 0.0f };
	float m_Bone1Rotation[3]{ 0.0f, 0.0f, 0.0f };

	//PART_2
	struct VertexSoftwareSkinned
	{
		VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color)
			:transformedVertex{position, normal, color}
		,originalVertex{position, normal, color}
		{}
		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};
	};

	void InitializeVertices(float length);

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};
};

