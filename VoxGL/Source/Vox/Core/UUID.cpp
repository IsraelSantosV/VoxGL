#include "VoxPch.h"
#include "Vox/Core/UUID.h"

#include <random>

#include <unordered_map>

namespace Vox 
{
	static std::random_device m_RandomDevice;
	static std::mt19937_64 m_Engine(m_RandomDevice());
	static std::uniform_int_distribution<uint64_t> m_UniformDistribution;

	UUID::UUID() : m_UUID(m_UniformDistribution(m_Engine))
	{
	}

	UUID::UUID(uint64_t uuid) : m_UUID(uuid)
	{
	}
}
