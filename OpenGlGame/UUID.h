#pragma once
#include <xhash>

namespace Game
{
	class UUID
	{
		uint64_t m_UUID;

	public:
		UUID();
		UUID(uint64_t uuid);

		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	};
}

namespace std
{
	template<>
	struct hash<Game::UUID>
	{
		std::size_t operator()(const Game::UUID& uuid) const noexcept
		{
			return hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}

