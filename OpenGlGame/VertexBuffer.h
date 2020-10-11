#pragma once
#include <vector>
#include <optional>
#include <utility>

#include "Assert.h"
#include "BufferObject.h"

namespace Game
{
	template <typename T>
	class VertexBuffer: public BufferObject
	{
	public:
		using ValueType = T;
		using ValueList = std::vector<ValueType>;
		using Iterator = typename ValueList::iterator;
		using ConstIterator = typename ValueList::const_iterator;

	private:
		ValueList m_Values;
		mutable std::optional<std::pair<size_t, size_t>> m_LastChange = std::nullopt;

	public:
		explicit VertexBuffer(const BufferUsage &usage = BufferUsage::StaticDraw) : BufferObject(
			 BufferType::Vertex,
			 usage
			) {}

		VertexBuffer(const size_t size, const BufferUsage &usage = BufferUsage::StaticDraw) : VertexBuffer(usage)
		{
			Reserve(size);
		}

		void Add(const ValueType &value)
		{
			MarkAsChanged();

			if(!m_LastChange)
				m_LastChange = std::make_pair(m_Values.size(), m_Values.size());
			else
				m_LastChange->second = m_Values.size();

			m_Values.emplace_back(value);
		}

		Iterator begin()
		{
			if(!m_LastChange)
				m_LastChange = std::make_pair<size_t, size_t>(0, m_Values.size() - 1);
			else
				m_LastChange->first = 0;

			MarkAsChanged();

			return m_Values.begin();
		}

		Iterator end() { return m_Values.end(); }

		ConstIterator begin() const { return m_Values.begin(); }
		ConstIterator end() const { return m_Values.end(); }

		uint32_t& Get(size_t index)
		{
			ASSERT(index > m_Values.size(), "Out of container range");

			if(index > m_Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			MarkAsChanged();
			if(!m_LastChange)
				m_LastChange = std::make_pair(index, index);
			else
			{
				if(index < m_LastChange->first)
					m_LastChange->first = index;
				if(index > m_LastChange->second)
					m_LastChange->second = index;
			}

			return m_Values[index];
		}

		uint32_t Get(size_t index) const
		{
			ASSERT(index > m_Values.size(), "Out of container range");

			if(index > m_Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			Remove(m_Values.begin() + index);
		}

		size_t Count() const { return m_Values.size(); }

		void Remove(const size_t &index)
		{
			ASSERT(index > m_Values.size(), "Out of container range");

			if(index > m_Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			Remove(m_Values.begin() + index);
		}

		void Remove(ConstIterator iterator)
		{
			MarkAsChanged();

			m_LastChange = std::make_pair<size_t, size_t>(0, m_Values.size());

			m_Values.erase(iterator);
		}

		void Reserve(size_t size)
		{
			m_Values.reserve(size);
			Allocate(size);
		}

		void Resize(size_t size)
		{
			m_Values.resize(size);
			SendValues();
		}

		const ValueType* Data() const { return m_Values.data(); }

	protected:
		void SendValues() const override
		{
			constexpr auto calcSize = [](const std::pair<size_t, size_t> &pair)
			{
				return (pair.second - pair.first) * sizeof(ValueType);
			};

			if(m_LastChange)
			{
				const auto size = calcSize(*m_LastChange);

				if(size < Size())
					SendSubData(m_Values.data() + m_LastChange->first, size, m_LastChange->first);
				else
					SendData(m_Values.data(), m_Values.size() * sizeof(ValueType));
			}
			else
				SendData(m_Values.data(), m_Values.size() * sizeof(ValueType));

			m_LastChange = std::nullopt;
		}

	public:
		template <class Iterator>
		VertexBuffer(
			Iterator begin,
			const Iterator end,
			const BufferUsage &usage = BufferUsage::StaticDraw
			) : VertexBuffer(usage)
		{
			Add(begin, end);
		}

		template <class Iterator>
		void Add(Iterator begin, const Iterator end)
		{
			Reserve(m_Values.size() + std::distance(begin, end));

			for(; begin != end; ++begin)
			{
				add(*begin);
			}
		}
	};
}
