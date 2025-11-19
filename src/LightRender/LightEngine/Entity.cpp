#include "Entity.h"

#include "GameManager.h"
#include "Utils.h"
#include "Debug.h"

void Entity::Initialize(float radius, gce::Vector3f32 color)
{
	mDirection = gce::Vector3f32(0.0f, 0.0f, 0.0f);

	m_Shape = new Sphere();
	m_Radius = radius;
	m_Shape->Scale({ radius * 2.f, radius * 2.f, radius * 2.f });
	m_Shape->SetColor(color);
	
	mTarget.isSet = false;

	OnInitialize();
}

void Entity::Repulse(Entity* other) 
{
	gce::Vector3f32 distance = GetPosition(0.5f, 0.5f) - other->GetPosition(0.5f, 0.5f);
	
	float sqrLength = (distance.x * distance.x) + (distance.y * distance.y);
	float length = std::sqrt(sqrLength);

	float radius1 = m_Radius;
	float radius2 = other->m_Radius;

	float overlap = (length - (radius1 + radius2)) * 0.5f;

	gce::Vector3f32 normal = distance / length;

	gce::Vector3f32 translation = normal * overlap;

	gce::Vector3f32 position1 = GetPosition(0.5f, 0.5f) - translation;
	gce::Vector3f32 position2 = other->GetPosition(0.5f, 0.5f) + translation;

	SetPosition(position1.x, position1.y, 0.5f, 0.5f);
	other->SetPosition(position2.x, position2.y, 0.5f, 0.5f);
}

bool Entity::IsColliding(Entity* other) const
{
	gce::Vector3f32 distance = GetPosition(0.5f, 0.5f) - other->GetPosition(0.5f, 0.5f);

	float sqrLength = (distance.x * distance.x) + (distance.y * distance.y);

	float radius1 = m_Radius;
	float radius2 = other->m_Radius;

	float sqrRadius = (radius1 + radius2) * (radius1 + radius2);

	return sqrLength < sqrRadius;
}

bool Entity::IsInside(float x, float y) const
{
	gce::Vector3f32 position = GetPosition(0.5f, 0.5f);

	float dx = x - position.x;
	float dy = y - position.y;

	float radius = m_Radius;

	return (dx * dx + dy * dy) < (radius * radius);
}

void Entity::Destroy()
{
	mToDestroy = true;

	OnDestroy();
}

void Entity::SetPosition(float x, float y, float z, float ratioX, float ratioY, float ratioZ)
{
	float size = m_Radius * 2;

	x -= size * ratioX;
	y -= size * ratioY;

	m_Shape->SetPosition({ x, y, z });

	//#TODO Optimise
	if (mTarget.isSet) 
	{
		gce::Vector3f32 position = GetPosition(0.5f, 0.5f, 0.5f);
		mTarget.distance = Utils::GetDistance(position.x, position.y, position.z, mTarget.position.x, mTarget.position.y, mTarget.position.z);
		GoToDirection(mTarget.position.x, mTarget.position.y, mTarget.position.z);
		mTarget.isSet = true;
	}
}

gce::Vector3f32 Entity::GetPosition(float ratioX, float ratioY, float ratioZ) const
{
	float size = m_Radius * 2;
	gce::Vector3f32 position = m_Shape->GetPosition();

	position.x += size * ratioX;
	position.y += size * ratioY;
	position.z += size * ratioZ;

	return position;
}

bool Entity::GoToDirection(float x, float y, float z, float speed)
{
	gce::Vector3f32 position = GetPosition(0.5f, 0.5f, 0.5f);
	gce::Vector3f32 direction = gce::Vector3f32(x - position.x, y - position.y, z - position.z);
	
	bool success = Utils::Normalize(direction);
	if (success == false)
		return false;

	SetDirection(direction.x, direction.y, direction.z, speed);

	return true;
}

bool Entity::GoToPosition(float x, float y, float z, float speed)
{
	if (GoToDirection(x, y, speed) == false)
	{
		mTarget.isReached = true;
		return false;
	}

	gce::Vector3f32 position = GetPosition(0.5f, 0.5f);

	mTarget.position = { x, y, z };
	mTarget.distance = Utils::GetDistance(position.x, position.y, position.z, x, y, z);
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
	m_Shape->SetPosition(translation);
	
	if (mTarget.isSet) 
	{
		float x1 = GetPosition(0.5f, 0.5f).x;
		float y1 = GetPosition(0.5f, 0.5f).y;

		float x2 = x1 + mDirection.x * mTarget.distance;
		float y2 = y1 + mDirection.y * mTarget.distance;

		//Debug::DrawLine(x1, y1, x2, y2, sf::Color::Cyan);

		//Debug::DrawCircle(mTarget.position.x, mTarget.position.y, 5.f, sf::Color::Magenta);

		mTarget.distance -= distance;

		if (mTarget.distance <= 0.f)
		{
			SetPosition(mTarget.position.x, mTarget.position.y, mTarget.position.z, 0.5f, 0.5f, 0.5f);
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