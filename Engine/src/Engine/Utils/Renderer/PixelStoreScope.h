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

	class UnpackRowLengthScope {
	public:
		explicit UnpackRowLengthScope(int newRowLength) noexcept;
		~UnpackRowLengthScope() noexcept;

		UnpackRowLengthScope(const UnpackRowLengthScope&) = delete;
		UnpackRowLengthScope& operator=(const UnpackRowLengthScope&) = delete;
		UnpackRowLengthScope(UnpackRowLengthScope&&) = delete;
		UnpackRowLengthScope& operator=(UnpackRowLengthScope&&) = delete;

	private:
		int m_Prev = 0;
	};

	class PackRowLengthScope {
	public:
		explicit PackRowLengthScope(int newRowLength) noexcept;
		~PackRowLengthScope() noexcept;

		PackRowLengthScope(const PackRowLengthScope&) = delete;
		PackRowLengthScope& operator=(const PackRowLengthScope&) = delete;
		PackRowLengthScope(PackRowLengthScope&&) = delete;
		PackRowLengthScope& operator=(PackRowLengthScope&&) = delete;

	private:
		int m_Prev = 0;
	};
}