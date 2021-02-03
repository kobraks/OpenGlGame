#pragma once
#include <vector>
#include <optional>
#include <utility>

#include "Types.h"

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
		class Internals
		{
			ValueList Values;
			std::optional<std::pair<size_t, size_t>> LastChange;
		};
		
		Pointer<Internals> m_Internals;
	public:
		explicit VertexBuffer(const BufferUsage &usage = BufferUsage::StaticDraw) : BufferObject(
			 BufferType::Vertex,
			 usage
			)
		{
			m_Internals = MakePointer<Internals>();
		}

		VertexBuffer(const size_t size, const BufferUsage &usage = BufferUsage::StaticDraw) : VertexBuffer(usage)
		{
			Reserve(size);
		}

		void Add(const ValueType &value)
		{
			MarkAsChanged();

			if(!m_Internals->LastChange)
				m_Internals->LastChange = std::make_pair(m_Internals->Values.size(), m_Internals->Values.size());
			else
				m_Internals->LastChange->second = m_Internals->Values.size();

			m_Internals->Values.emplace_back(value);
		}

		Iterator begin()
		{
			if(!m_Internals->LastChange)
				m_Internals->LastChange = std::make_pair<size_t, size_t>(0, m_Internals->Values.size() - 1);
			else
				m_Internals->LastChange->first = 0;

			MarkAsChanged();

			return m_Internals->Values.begin();
		}

		Iterator end() { return m_Internals->Values.end(); }

		ConstIterator begin() const { return m_Internals->Values.begin(); }
		ConstIterator end() const { return m_Internals->Values.end(); }

		uint32_t& Get(size_t index)
		{
			ASSERT(index < m_Internals->Values.size(), "Out of container range");

			if(index > m_Internals->Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			MarkAsChanged();
			if(!m_Internals->LastChange)
				m_Internals->LastChange = std::make_pair(index, index);
			else
			{
				if(index < m_Internals->LastChange->first)
					m_Internals->LastChange->first = index;
				if(index > m_Internals->LastChange->second)
					m_Internals->LastChange->second = index;
			}

			return m_Internals->Values[index];
		}

		uint32_t Get(size_t index) const
		{
			ASSERT(index > m_Internals->Values.size(), "Out of container range");

			if(index > m_Internals->Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			Remove(m_Internals->Values.begin() + index);
		}

		size_t Count() const { return m_Internals->Values.size(); }

		void Remove(const size_t &index)
		{
			ASSERT(index > m_Internals->Values.size(), "Out of container range");

			if(index > m_Internals->Values.size())
				std::out_of_range(fmt::format("Out of container range"));

			Remove(m_Internals->Values.begin() + index);
		}

		void Remove(ConstIterator iterator)
		{
			MarkAsChanged();

			m_Internals->LastChange = std::make_pair<size_t, size_t>(0, m_Internals->Values.size());

			m_Internals->Values.erase(iterator);
		}

		void Reserve(size_t size)
		{
			m_Internals->Values.reserve(size);
			Allocate(size * sizeof(ValueType));
		}

		void Resize(size_t size)
		{
			m_Internals->Values.resize(size);
			SendValues();
		}

		const ValueType* Data() const { return m_Internals->Values.data(); }

	protected:
		void SendValues() const override
		{
			constexpr auto calcSize = [](const std::pair<size_t, size_t> &pair)
			{
				return (pair.second - pair.first) * sizeof(ValueType);
			};

			if(m_Internals->LastChange)
			{
				const auto size = calcSize(*m_Internals->LastChange);

				if(size < Size())
					SendSubData(m_Internals->Values.data() + m_Internals->LastChange->first, size, m_Internals->LastChange->first);
				else
					SendData(m_Internals->Values.data(), m_Internals->Values.size() * sizeof(ValueType));
			}
			else
				SendData(m_Internals->Values.data(), m_Internals->Values.size() * sizeof(ValueType));

			m_Internals->LastChange = std::nullopt;
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
			Reserve(m_Internals->Values.size() + std::distance(begin, end));

			for(; begin != end; ++begin)
			{
				add(*begin);
			}
		}
	};
}
