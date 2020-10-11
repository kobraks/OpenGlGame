#pragma once
#include <vector>
#include <optional>
#include "BufferObject.h"

namespace Game
{
	class IndexBuffer: public BufferObject
	{
	public:
		using ValueList = std::vector<uint32_t>;
		using Iterator = ValueList::iterator;
		using ConstIterator = ValueList::const_iterator;

	private:
		ValueList m_Values;

		mutable std::optional<std::pair<size_t, size_t>> m_LastChange = std::nullopt;
	public:
		explicit IndexBuffer(const BufferUsage &usage = BufferUsage::StaticDraw);
		explicit IndexBuffer(size_t size, const BufferUsage &usage = BufferUsage::StaticDraw);

		void Add(uint32_t value);

		Iterator begin();
		Iterator end() { return m_Values.end(); };

		ConstIterator begin() const { return m_Values.begin(); }
		ConstIterator end() const { return m_Values.end(); }

		uint32_t Get(size_t index) const;
		uint32_t& Get(size_t index);

		size_t Count() const { return m_Values.size(); }
		void Remove(const size_t &index);
		void Remove(ConstIterator iterator);
		void Reserve(size_t size);
		void Resize(size_t size);

		uint32_t operator[](const size_t index) const { return Get(index); }
		uint32_t& operator[](const size_t index) { return Get(index); }

		const uint32_t* Data() const { return m_Values.data(); }

	protected:
		void SendValues() const override;

			public:
		template <class Iterator>
		explicit IndexBuffer(
			Iterator begin,
			const Iterator end,
			const BufferUsage &usage = BufferUsage::StaticDraw
			) : IndexBuffer(usage)
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
