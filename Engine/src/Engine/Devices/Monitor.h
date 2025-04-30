#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Rect.h"

namespace Engine {
	struct GammaRamp {
		uint32_t Size = 0;
		uint16_t *Red = nullptr;
		uint16_t *Green = nullptr;
		uint16_t *Blue = nullptr;

		~GammaRamp();
	};

	struct VideoMode {
		Vector2u Size;

		int RedBits = 0;
		int GreenBits = 0;
		int BlueBits = 0;
		int RefreshRate = 0;
	};

	inline bool operator==(const VideoMode &lth, const VideoMode &rth) {
		return lth.Size == rth.Size && lth.RedBits == rth.RedBits && lth.GreenBits == rth.GreenBits && lth.BlueBits == rth.BlueBits && lth.RefreshRate == rth.RefreshRate;
	}

	class Monitor {
	public:
		Monitor(const Monitor &) = delete;
		Monitor(Monitor &&) = delete;

		Monitor& operator=(const Monitor &) = delete;
		Monitor& operator=(Monitor &&) = delete;

		std::string_view GetName() const { return m_Name; }

		Vector2u GetPhysicalSize() const { return m_Size; }
		Vector2f GetScale() const { return m_Scale; }
		Vector2i GetPosition() const { return m_Pos; }
		IntRect GetWorkArea() const {return m_WorkArea; }

		Vector2f GetDPI() const;
		Vector2f GetContentScale() const;

		void *GetUserData() const { return m_UserData; }

		template<typename T>
		T* GetNativeHandle() const { return static_cast<T*>(m_NativePointer); }

		void *GetNativeHandle() const { return m_NativePointer; }

		const VideoMode* GetVideoMode() const { return m_VideoMode; }
		const std::vector<Scope<VideoMode>> &GetVideoModes() const;

		const VideoMode* FindClosestMode(const Vector2u& size, int refreshRate) const;

		int32_t GetCurrentRefreshRate() const { return m_VideoMode ? m_VideoMode->RefreshRate : 0; }

		std::vector<int32_t> GetAvailableRefreshRates(const Vector2u& resolution) const;

		void SetUserData(void *userData);
		void SetGamma(float gamma);
		void SetGammaRamp(const GammaRamp &ramp);

		GammaRamp GetGammaRamp() const;

		bool IsSameMonitor(const Monitor& other) const { return m_NativePointer == other.m_NativePointer; }
		bool IsPrimary() const { return this == GetPrimary(); }

		static Monitor* GetPrimary();
		static Monitor* Get(const size_t monitor);
		static const std::vector<Scope<Monitor>>& GetAll();
	private:
		Monitor() = default;

		static Scope<Monitor> Create(void *pointer);
		static void Populate();
		static void RegisterCallbacks();
		static void Refresh();
		static Monitor* AddNewMonitor(void* pointer);
		static void RemoveMonitor(void* pointer);

		std::string m_Name;

		Vector2u m_Size{};
		Vector2f m_Scale{};
		Vector2i m_Pos{};
		IntRect m_WorkArea{};

		void *m_NativePointer = nullptr;
		void *m_UserData = nullptr;

		VideoMode* m_VideoMode;
		std::vector<Scope<VideoMode>> m_VideoModes;

		static std::vector<Scope<Monitor>> s_Monitors;
		static Monitor* s_PrimaryMonitor;
		static bool s_Initialized;
		static bool s_RegisteredCallbacks;
	};
}
