#include "pch.h"
#include "IndexBuffer.h"

namespace Game
{
	void IndexBuffer::Element::MarkChanged() const
	{
		if (m_LastChange)
		{
			if (m_LastChange->first < m_Current)
				m_LastChange->first = m_Current;
			if (m_LastChange->second > m_Current)
				m_LastChange->second = m_Current;
		}
		else
			m_LastChange->first = m_LastChange->second = m_Current;
	}
	
	IndexBuffer::Element & IndexBuffer::Element::operator=(const uint32_t value)
	{
		MarkChanged();
		m_Value = value;
	}
	
	IndexBuffer::Element & IndexBuffer::Element::operator=(const Element &value)
	{
		MarkChanged();
		m_Value = value;
	}
	IndexBuffer::IndexBuffer(const BufferUsage &usage) : BufferObject(BufferType::Index, usage) { }

	IndexBuffer::IndexBuffer(const size_t size, const BufferUsage &usage) : IndexBuffer(usage)
	{
		Reserve(size);
	}

	void IndexBuffer::Add(uint32_t value)
	{
		m_Values.emplace_back(value, m_LastChange, )
	}

	void IndexBuffer::SendValues() const
	{
		SendData(m_Values.data(), m_Values.size() * sizeof(uint32_t));
	}
}
