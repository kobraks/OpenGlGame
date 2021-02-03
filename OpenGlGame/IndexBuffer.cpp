#include "pch.h"
#include "IndexBuffer.h"

#include "Assert.h"

namespace Game
{
	IndexBuffer::IndexBuffer(const BufferUsage &usage) : BufferObject(BufferType::Index, usage) { }

	IndexBuffer::IndexBuffer(const size_t size, const BufferUsage &usage) : IndexBuffer(usage)
	{
		Reserve(size * sizeof(uint32_t));
	}

	void IndexBuffer::Add(uint32_t value)
	{
		MarkAsChanged();

		if(!m_Internals->LastChange)
			m_Internals->LastChange = std::make_pair(m_Internals->Values.size(), m_Internals->Values.size());
		else
			m_Internals->LastChange->second = m_Internals->Values.size();

		m_Internals->Values.emplace_back(value);
	}

	IndexBuffer::Iterator IndexBuffer::begin()
	{
		if(!m_Internals->LastChange)
			m_Internals->LastChange = std::make_pair<size_t, size_t>(0, m_Internals->Values.size() - 1);
		else
			m_Internals->LastChange->first = 0;

		MarkAsChanged();

		return m_Internals->Values.begin();
	}

	IndexBuffer::Iterator IndexBuffer::end()
	{
		return m_Internals->Values.end();
	}

	IndexBuffer::ConstIterator IndexBuffer::begin() const
	{
		return m_Internals->Values.begin();
	}

	IndexBuffer::ConstIterator IndexBuffer::end() const
	{
		return m_Internals->Values.end();
	}

	size_t IndexBuffer::Count() const
	{
		return m_Internals->Values.size();
	}

	uint32_t IndexBuffer::operator[](const size_t index) const
	{
		return Get(index);
	}

	uint32_t& IndexBuffer::operator[](const size_t index)
	{
		return Get(index);
	}

	const uint32_t* IndexBuffer::Data() const
	{
		return m_Internals->Values.data();
	}

	uint32_t IndexBuffer::Get(size_t index) const
	{
		auto& values = m_Internals->Values;
		
		ASSERT(index < values.size(), "Out of container range");

		if(index > values.size())
			throw std::out_of_range("Out of container range");

		return values[index];
	}

	uint32_t& IndexBuffer::Get(size_t index)
	{
		auto& values = m_Internals->Values;
		ASSERT(index < values.size(), "Out of container range");

		if(index > values.size())
			throw std::out_of_range("Out of container range");

		auto& lastChange = m_Internals->LastChange;
		
		MarkAsChanged();
		if(!lastChange)
			lastChange = std::make_pair(index, index);
		else
		{
			if(index < lastChange->first)
				lastChange->first = index;
			if(index > lastChange->second)
				lastChange->second = index;
		}

		return values[index];
	}

	void IndexBuffer::Remove(const size_t &index)
	{
		auto& values = m_Internals->Values;
		ASSERT(index < values.size(), "Out of container range");

		if(index > values.size())
			throw std::out_of_range("Out of container range");

		Remove(values.begin() + index);
	}

	void IndexBuffer::Remove(ConstIterator iterator)
	{
		MarkAsChanged();

		m_Internals->LastChange = std::make_pair<size_t, size_t>(0, m_Internals->Values.size());

		m_Internals->Values.erase(iterator);
	}

	void IndexBuffer::Reserve(const size_t size)
	{
		m_Internals->Values.reserve(size);
		Allocate(size * sizeof(uint32_t));
	}

	void IndexBuffer::Resize(const size_t size)
	{
		m_Internals->Values.resize(size);
		SendValues();
	}

	void IndexBuffer::SendValues() const
	{
		auto& lastChange = m_Internals->LastChange;
		auto& values = m_Internals->Values;
		
		constexpr auto calcSize = [](const std::pair<size_t, size_t> &pair)
		{
			return (pair.second - pair.first) * sizeof(uint32_t);
		};

		if(lastChange)
		{
			const auto size = calcSize(*lastChange);

			if(size < Size())
				SendSubData(values.data() + lastChange->first, size, lastChange->first);
			else
				SendData(values.data(), values.size() * sizeof(uint32_t));
		}
		else
			SendData(values.data(), values.size() * sizeof(uint32_t));

		lastChange = std::nullopt;
	}
}
