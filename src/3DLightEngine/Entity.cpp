#include "3DLightEngine/Entity.h"
#include "3DLightEngine/GameManager.h"
#include "3DLightEngine/Utils.h"
#include "3DLightEngine/Debug.h"

Entity::~Entity()
{
}

void Entity::Initialize(float radius, gce::Vector3f32 color)
{
	mDirection = gce::Vector3f32(0.0f, 0.0f, 0.0f);

	m_Shape = new Sphere();
	m_Radius = radius;
	m_Shape->Scale({ radius, radius, radius });
	m_Shape->SetColor(color);
	
	mTarget.isSet = false;

	OnInitialize();
}

void Entity::Repulse(Entity* other) 
{
	gce::Vector3f32 distance = GetPosition() - other->GetPosition();
	
	float sqrLength = (distance.x * distance.x) + (distance.y * distance.y) + (distance.z * distance.z);
	float length = std::sqrt(sqrLength);

	float radius1 = m_Radius;
	float radius2 = other->m_Radius;

	float overlap = (length - (radius1 + radius2)) * 0.5f;

	gce::Vector3f32 normal = distance / length;

	gce::Vector3f32 translation = normal * overlap;

	gce::Vector3f32 position1 = GetPosition() - translation;
	gce::Vector3f32 position2 = other->GetPosition() + translation;

	SetPosition(position1.x, position1.y, position1.z);
	other->SetPosition(position2.x, position2.y, position2.z);
}

bool Entity::IsColliding(Entity* other) const
{
	gce::Vector3f32 distance = GetPosition() - other->GetPosition();

	float sqrLength = (distance.x * distance.x) + (distance.y * distance.y) + (distance.z * distance.z);

	float radius1 = m_Radius;
	float radius2 = other->m_Radius;

	float sqrRadius = (radius1 + radius2) * (radius1 + radius2);

	return sqrLength < sqrRadius;
}

bool Entity::IsInside(float x, float y, float z) const
{
	gce::Vector3f32 position = GetPosition();

	float dx = x - position.x;
	float dy = y - position.y;
	float dz = z - position.z;

	float radius = m_Radius;

	return (dx * dx + dy * dy + dz * dz) < (radius * radius);
}

void Entity::Destroy()
{
	mToDestroy = true;

	OnDestroy();
}

void Entity::SetPosition(float x, float y, float z)
{
	m_Shape->SetPosition({ x, y, z });

	//#TODO Optimise
	if (mTarget.isSet) 
	{
		gce::Vector3f32 position = GetPosition();
		mTarget.distance = Utils::GetDistance(position.x, position.y, position.z, mTarget.position.x, mTarget.position.y, mTarget.position.z);
		GoToDirection(mTarget.position.x, mTarget.position.y, mTarget.position.z);
		mTarget.isSet = true;
	}
}

gce::Vector3f32 Entity::GetPosition() const
{
	gce::Vector3f32 position = m_Shape->GetPosition();
	
	return position;
}

bool Entity::GoToDirection(float x, float y, float z, float speed)
{
	gce::Vector3f32 position = GetPosition();
	gce::Vector3f32 direction = gce::Vector3f32(x - position.x, y - position.y, z - position.z);
	
	bool success = Utils::Normalize(direction);
	if (success == false)
		return false;

	SetDirection(direction.x, direction.y, direction.z, speed);

	return true;
}

bool Entity::GoToPosition(float x, float y, float z, float speed)
{
	if (GoToDirection(x, y, z, speed) == false)
	{
		mTarget.isReached = true;
		return false;
	}

	gce::Vector3f32 position = GetPosition();

	mTarget.position = { x, y, z };
	mTarget.distance = Utils::GetDistance(position, mTarget.position);
	mTarget.isSet = true;
	mTarget.isReached = false;

	return true;
}

void Entity::SetDirection(float x, float y, float z, float speed)
{
	if (speed > 0)
		mSpeed = speed;

	mDirection = gce::Vector3f32({x, y, z});
	mTarget.isSet = false;
}

void Entity::Update()
{
	float dt = GetDeltaTime();
	float distance = dt * mSpeed * mSpeedFactor;
	gce::Vector3f32 translation = mDirection * distance;
	m_Shape->Translate(translation);
	
	if (mTarget.isSet) 
	{
		float x1 = GetPosition().x;
		float y1 = GetPosition().y;
		float z1 = GetPosition().z;

		float x2 = x1 + mDirection.x * mTarget.distance;
		float y2 = y1 + mDirection.y * mTarget.distance;
		float z2 = z1 + mDirection.z * mTarget.distance; 

		Debug::DrawLine(GetPosition(), mTarget.position, gce::Vector3f32({ 0.0f, 1.0f, 1.0f }));

		Debug::DrawSphere(mTarget.position, 0.5f, gce::Vector3f32({ 1.0f, 0.0f, 1.0f }));
		
		mTarget.distance = Utils::GetDistance(GetPosition(), mTarget.position);

		if (mTarget.distance <= 0.1f)
		{
			SetPosition(mTarget.position.x, mTarget.position.y, mTarget.position.z);
			mDirection = gce::Vector3f32({ 0.f, 0.f, 0.f });
			mTarget.isSet = false;
			mTarget.isReached = true;
		}
	}
	
	OnUpdate();
}

Scene* Entity::GetScene() const
{
	return GameManager::Get()->GetScene();
}

float Entity::GetDeltaTime() const
{
	return GameManager::Get()->GetDeltaTime();
}