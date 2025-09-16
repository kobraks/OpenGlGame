#pragma once
#include <cstdint>

namespace Engine::Utils {
	class PackAlignmentScope {
	public:
		explicit PackAlignmentScope(int newAlignment = 1) noexcept;
		~PackAlignmentScope() noexcept;

		PackAlignmentScope(const PackAlignmentScope&) = delete;
		PackAlignmentScope& operator=(const PackAlignmentScope&) = delete;
		PackAlignmentScope(PackAlignmentScope&&) = delete;
		PackAlignmentScope& operator=(PackAlignmentScope&&) = delete;

	private:
		int m_Prev = 0;
	};

	class UnpackAlignmentScope {
	public:
		explicit UnpackAlignmentScope(int newAlignment = 1) noexcept;
		~UnpackAlignmentScope() noexcept;

		UnpackAlignmentScope(const UnpackAlignmentScope&) = delete;
		UnpackAlignmentScope& operator=(const UnpackAlignmentScope&) = delete;
		UnpackAlignmentScope(UnpackAlignmentScope&&) = delete;
		UnpackAlignmentScope& operator=(UnpackAlignmentScope&&) = delete;

	private:
		int m_Prev = 0;
	};
}