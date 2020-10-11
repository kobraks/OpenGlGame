#include "pch.h"
#include "IndexBuffer.h"

#include "Assert.h"

namespace Game
{
	IndexBuffer::IndexBuffer(const BufferUsage &usage) : BufferObject(BufferType::Index, usage) { }

	IndexBuffer::IndexBuffer(const size_t size, const BufferUsage &usage) : IndexBuffer(usage)
	{
		Reserve(size);
	}

	void IndexBuffer::Add(uint32_t value)
	{
		MarkAsChanged();
		
		if (!m_LastChange)
			m_LastChange = std::make_pair(m_Values.size(), m_Values.size());
		else
			m_LastChange->second = m_Values.size();

		m_Values.emplace_back(value);
	}

	IndexBuffer::Iterator IndexBuffer::begin()
	{
		if(!m_LastChange)
			m_LastChange = std::make_pair<size_t, size_t>(0, m_Values.size() - 1);
		else
			m_LastChange->first = 0;

		MarkAsChanged();

		return m_Values.begin();
	}

	uint32_t IndexBuffer::Get(size_t index) const
	{
		ASSERT(index > m_Values.size(), "Out of container range");

		if(index > m_Values.size())
			std::out_of_range(fmt::format("Out of container range"));

		return m_Values[index];
	}

	uint32_t& IndexBuffer::Get(size_t index)
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

	void IndexBuffer::Remove(const size_t &index)
	{
		ASSERT(index > m_Values.size(), "Out of container range");

		if(index > m_Values.size())
			std::out_of_range(fmt::format("Out of container range"));

		Remove(m_Values.begin() + index);
	}

	void IndexBuffer::Remove(ConstIterator iterator)
	{
		MarkAsChanged();

		m_LastChange = std::make_pair<size_t, size_t>(0, m_Values.size());

		m_Values.erase(iterator);
	}

	void IndexBuffer::Reserve(const size_t size)
	{
		m_Values.reserve(size);
		Allocate(size);
	}

	void IndexBuffer::Resize(const size_t size)
	{
		m_Values.resize(size);
		SendValues();
	}

	void IndexBuffer::SendValues() const
	{
		constexpr auto calcSize = [](const std::pair<size_t, size_t> &pair)
		{
			return (pair.second - pair.first) * sizeof(uint32_t);
		};

		if(m_LastChange)
		{
			const auto size = calcSize(*m_LastChange);

			if(size < Size())
				SendSubData(m_Values.data() + m_LastChange->first, size, m_LastChange->first);
			else
				SendData(m_Values.data(), m_Values.size() * sizeof(uint32_t));
		}
		else
			SendData(m_Values.data(), m_Values.size() * sizeof(uint32_t));

		m_LastChange = std::nullopt;
	}
}
