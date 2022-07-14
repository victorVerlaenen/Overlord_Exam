#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	//TODO_W5(L"Implement Picking Logic")
	auto sceneContext = m_pScene->GetSceneContext();
	auto mousePosition = sceneContext.pInput->GetMousePosition();
	float halfWidth = sceneContext.windowWidth / 2;
	float halfHeight = sceneContext.windowHeight / 2;

	//Convert mouse coordinates to NDC coordinates
	float xNdc = (mousePosition.x - halfWidth) / halfWidth;
	float yNdc = (halfHeight - mousePosition.y) / halfHeight;

	//Calculate near point and far point
	XMMATRIX viewProjectionInverse = XMLoadFloat4x4(&m_ViewProjectionInverse);

	XMFLOAT3 vectorNear{ xNdc, yNdc, 0 };
	XMFLOAT3 vectorFar{ xNdc, yNdc, 1 };
	XMVECTOR nearPoint{ XMVector3TransformCoord(XMLoadFloat3(&vectorNear),viewProjectionInverse) };
	XMVECTOR farPoint{ XMVector3TransformCoord(XMLoadFloat3(&vectorFar),viewProjectionInverse) };

	XMFLOAT3 startFloat{};
	XMFLOAT3 DirectionFloat{};
	XMStoreFloat3(&startFloat, nearPoint);
	XMStoreFloat3(&DirectionFloat, farPoint - nearPoint);
	PxVec3 rayStart{ startFloat.x, startFloat.y, startFloat.z };
	PxVec3 rayDirection{ DirectionFloat.x, DirectionFloat.y, DirectionFloat.z };
	rayDirection.normalize();

	//Raycast
	PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	PxRaycastBuffer hit{};
	if (m_pScene->GetPhysxProxy()->Raycast(rayStart, rayDirection, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		return reinterpret_cast<BaseComponent*>(hit.block.actor->userData)->GetGameObject();
	}

	return nullptr;
}