#pragma once
#include "Base.h"

namespace Engine {
	struct ApplicationCommandLineArgs {
		size_t Count = 0;
		char** Args = nullptr;

		std::string_view operator[](size_t index) const {
			ENGINE_ASSERT(index < Count);

			if (index >= Count) {
				throw std::out_of_range("Out of range");
			}

			return Args[index];
		}

		std::string_view At(size_t index) const {
			ENGINE_ASSERT(index < Count);

			if (index >= Count) {
				throw std::out_of_range("Out of range");
			}

			return Args[index];
		}

		char** begin() { return Args; }
		char** end() { return Args + Count; }

		char** begin() const { return Args; }
		char** end() const { return Args + Count; }
	};
}