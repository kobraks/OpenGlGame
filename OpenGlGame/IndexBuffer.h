#pragma once
#include <vector>
#include <optional>
#include "BufferObject.h"

namespace Game
{
	class IndexBuffer: public BufferObject
	{
	public:
		class Element;

		using ValueList = std::vector<Element>;
		using Iterator = ValueList::iterator;
		using ConstIterator = ValueList::const_iterator;

		class Element
		{
			uint32_t m_Value;

			std::optional<std::pair<ConstIterator, ConstIterator>> &m_LastChange;
			ConstIterator m_Current;
		public:
			Element(
				uint32_t value,
				std::optional<std::pair<ConstIterator, ConstIterator>> &lastChange,
				const ConstIterator current
				) : m_Value(value),
				    m_LastChange(lastChange),
				    m_Current(current) {}

			void MarkChanged() const;

			Element& operator=(uint32_t value);
			Element& operator=(const Element &value);

			operator uint32_t() const { return m_Value; };
		};


	private:
		ValueList m_Values;

		std::optional<std::pair<ConstIterator, ConstIterator>> m_LastChange = std::nullopt;
	public:
		explicit IndexBuffer(const BufferUsage &usage = BufferUsage::StaticDraw);
		explicit IndexBuffer(size_t size, const BufferUsage &usage = BufferUsage::StaticDraw);

		template <class Iterator>
		explicit IndexBuffer(
			Iterator begin,
			const Iterator end,
			const BufferUsage &usage = BufferUsage::StaticDraw
			) : IndexBuffer(usage)
		{
			reserve(std::distance(begin, end));

			for(; begin != end; ++begin)
				Add(*begin);
		}

		void Add(uint32_t value);

		template <class Iterator>
		void Add(Iterator begin, const Iterator end)
		{
			reserve(m_Values.size() + std::distance(begin, end));

			for(; begin != end; ++begin)
			{
				add(*begin);
			}
		}

		Iterator begin() { return m_Values.begin(); }
		Iterator end() { return m_Values.end(); }

		ConstIterator begin() const { return m_Values.begin(); }
		ConstIterator end() const { return m_Values.end(); }

		uint32_t Get(size_t index) const;
		Element& Get(size_t index);

		size_t Count() const { return m_Values.size(); }
		void Remove(const size_t &index);
		void Remove(std::vector<uint32_t>::const_iterator iterator);
		void Reserve(const size_t &size);

		uint32_t operator[](const size_t index) const { return Get(index); }
		Element& operator[](const size_t index) { return Get(index); }

	protected:
		virtual void SendValues() const override;
	};
}
