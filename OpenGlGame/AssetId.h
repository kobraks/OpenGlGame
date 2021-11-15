#pragma once

#include <functional>

namespace Game
{
	class AssetId
	{
		uint32_t m_Id;
		static uint32_t s_Id;

	public:
		AssetId() : m_Id(++s_Id) {}

		AssetId(uint32_t id) : m_Id(id) {}

		constexpr bool operator==(const AssetId& rhs) const
		{
			return rhs.m_Id == m_Id;
		}

		constexpr bool operator!=(const AssetId& rhs) const
		{
			return rhs.m_Id != m_Id;
		}

		constexpr bool operator>(const AssetId& rhs) const
		{
			return m_Id > rhs.m_Id;
		}

		constexpr bool operator>=(const AssetId& rhs) const
		{
			return m_Id >= rhs.m_Id;
		}

		constexpr bool operator<(const AssetId& rhs) const
		{
			return m_Id < rhs.m_Id;
		}

		constexpr bool operator<=(const AssetId& rhs) const
		{
			return m_Id <= rhs.m_Id;
		}

		uint32_t GetId() const
		{
			return m_Id;
		}

		bool IsValid() const { return m_Id != InvalidId; }
		static AssetId MakeInvalid() { return AssetId(InvalidId);  }
		static constexpr uint32_t InvalidId = -1;
	};
}

namespace std
{
	template<>
	struct hash<Game::AssetId>
	{
		std::size_t operator()(const Game::AssetId& key) const noexcept
		{
			return std::hash<uint32_t>{}(key.GetId());
		}
	};
}