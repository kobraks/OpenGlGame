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
		class Internals
		{
		public:
			ValueList Values;
			std::optional<std::pair<size_t, size_t>> LastChange = std::nullopt;
		};

		Pointer<Internals> m_Internals;

	public:
		explicit IndexBuffer(const BufferUsage &usage = BufferUsage::StaticDraw);
		explicit IndexBuffer(size_t size, const BufferUsage &usage = BufferUsage::StaticDraw);

		void Add(uint32_t value);

		Iterator begin();
		Iterator end();;

		ConstIterator begin() const;
		ConstIterator end() const;

		uint32_t Get(size_t index) const;
		uint32_t& Get(size_t index);

		size_t Count() const;
		void Remove(const size_t &index);
		void Remove(ConstIterator iterator);
		void Reserve(size_t size);
		void Resize(size_t size);

		uint32_t operator[](const size_t index) const;
		uint32_t& operator[](const size_t index);

		const uint32_t* Data() const;

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
			Reserve(m_Internals->Values.size() + std::distance(begin, end) * sizeof(uint32_t));

			for(; begin != end; ++begin)
			{
				add(*begin);
			}
		}
	};
}
