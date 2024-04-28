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

		Vector2u GetSize() const { return m_Size; }
		Vector2f GetScale() const { return m_Scale; }
		Vector2i GetPosition() const { return m_Pos; }
		IntRect GetWorkArea() const {return m_WorkArea; }

		void *GetUserData() const { return m_UserData; }
		void *GetNativePointer() const { return m_NativePointer; }

		VideoMode* GetVideoMode() const { return m_VideoMode; }
		const std::vector<Scope<VideoMode>> &GetVideoModes() const;

		void SetUserData(void *userData);
		void SetGamma(float gamma);
		void SetGammaRamp(const GammaRamp &ramp);

		GammaRamp GetGammaRamp() const;

		static Monitor* GetPrimary();
		static Monitor* Get(const size_t monitor);
		static const std::vector<Scope<Monitor>> &GetAll();

	private:
		Monitor() = default;

		static Scope<Monitor> Create(void *pointer);
		static void Populate();

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
	};
}