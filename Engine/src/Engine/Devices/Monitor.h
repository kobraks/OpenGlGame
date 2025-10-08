#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Rect.h"

#include <vector>
#include <string>
#include <string_view>
#include <atomic>
#include <mutex>

namespace Engine {
	class MonitorRegistry;

	struct GammaRamp {
		uint32_t Size = 0;
		Ref<uint16_t[]> Red = nullptr;
		Ref<uint16_t[]> Green = nullptr;
		Ref<uint16_t[]> Blue = nullptr;


		GammaRamp() = default;
		explicit GammaRamp(uint32_t size);
	};

	struct VideoMode {
		Vector2u Size;

		int RedBits = 0;
		int GreenBits = 0;
		int BlueBits = 0;
		int RefreshRate = 0;
	};

	constexpr bool operator==(const VideoMode &lth, const VideoMode &rth) noexcept {
		return lth.Size == rth.Size && lth.RedBits == rth.RedBits && lth.GreenBits == rth.GreenBits && lth.BlueBits == rth.BlueBits && lth.RefreshRate == rth.RefreshRate;
	}

	class Monitor {
		friend class MonitorRegistry;

	public:
		Monitor(const Monitor &) = delete;
		Monitor(Monitor &&) = delete;

		Monitor& operator=(const Monitor &) = delete;
		Monitor& operator=(Monitor &&) = delete;

		[[nodiscard]] std::string_view GetName() const { return m_Name; }

		[[nodiscard]] Vector2u GetPhysicalSize() const { return m_Size; }
		[[nodiscard]] Vector2f GetScale() const { return m_Scale; }
		[[nodiscard]] Vector2i GetPosition() const { return m_Pos; }
		[[nodiscard]] IntRect GetWorkArea() const {return m_WorkArea; }

		[[nodiscard]] Vector2f GetDPI() const;
		[[nodiscard]] Vector2f GetContentScale() const;

		[[nodiscard]] void *GetUserData() const { return m_UserData; }

		template<typename T>
		[[nodiscard]] T* GetNativeHandle() const { return static_cast<T*>(m_NativePointer); }

		[[nodiscard]] void *GetNativeHandle() const { return m_NativePointer; }

		[[nodiscard]] Ref<VideoMode> GetVideoMode() const { return m_VideoMode; }
		[[nodiscard]] const std::vector<Ref<VideoMode>> &GetVideoModes() const;

		[[nodiscard]] Ref<VideoMode> FindClosestMode(const Vector2u& size, int refreshRate) const;

		[[nodiscard]] int32_t GetCurrentRefreshRate() const { return m_VideoMode ? m_VideoMode->RefreshRate : 0; }

		[[nodiscard]] std::vector<int32_t> GetAvailableRefreshRates(const Vector2u& resolution) const;

		void SetUserData(void *userData);
		void SetGamma(float gamma);
		void SetGammaRamp(const GammaRamp &ramp);

		[[nodiscard]] GammaRamp GetGammaRamp() const;

		[[nodiscard]] bool IsSameMonitor(const Monitor& other) const { return m_NativePointer == other.m_NativePointer; }
		[[nodiscard]] bool IsConnected() const { return m_NativePointer != nullptr; }
		[[nodiscard]] bool IsPrimary() const;
	protected:
		void Invalidate();
		static Ref<Monitor> Create(void* pointer);

	private:
		Monitor() = default;

		std::string m_Name;

		Vector2u m_Size{};
		Vector2f m_Scale{};
		Vector2i m_Pos{};
		IntRect m_WorkArea{};

		void *m_NativePointer = nullptr;
		void *m_UserData = nullptr;

		Ref<VideoMode> m_VideoMode;
		std::vector<Ref<VideoMode>> m_VideoModes;
	};
}

template<>
struct fmt::formatter<Engine::VideoMode> {
	char Presentation = 'c';

	constexpr auto parse(format_parse_context& ctx) {
		auto it = ctx.begin(), end = ctx.end();

		if (it != end && (*it == 'c' || *it == 'l')) {
			Presentation = *it++;
		}
		if (it != end && *it != '}')
			throw format_error("invalid format spec for Engine::VideoMode");

		return it;
	}

	template <typename FormatContext>
	auto format(const Engine::VideoMode& vm, FormatContext& ctx) const {
		if (Presentation == 'l') {
			return format_to(ctx.out(), "{:x}@{}Hz ({}R, {}G, {}B)", vm.Size, vm.RefreshRate, vm.RedBits, vm.GreenBits, vm.BlueBits);
		}

		return format_to(ctx.out(), "{:x}@{}Hz", vm.Size, vm.RefreshRate);
	}
};
