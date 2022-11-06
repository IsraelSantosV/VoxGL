#pragma once

#include "Vox/Core/Core.h"
#include <xhash>

namespace Vox 
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

	class UUID32
	{
	public:
		UUID32();
		UUID32(uint32_t uuid);
		UUID32(const UUID32& other);

		operator uint32_t () { return m_UUID; }
		operator const uint32_t() const { return m_UUID; }
	private:
		uint32_t m_UUID;
	};
}

namespace std 
{
	template <>
	struct hash<Vox::UUID>
	{
		std::size_t operator()(const Vox::UUID& uuid) const
		{
			return hash<uint64_t>{}(uuid);
		}
	};
}
