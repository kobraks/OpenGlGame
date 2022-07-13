#pragma once
#include "Engine/Core/Rect.h"

#include <GLFW/glfw3.h>

namespace Game
{
	struct GammaRamp
	{
		size_t Size;
		uint16_t* Red = nullptr;
		uint16_t* Green = nullptr;
		uint16_t* Blue = nullptr;

		~GammaRamp();
	};

	struct VideoMode
	{
		Vector2u Size;

		int RedBits = 0;
		int GreenBits = 0;
		int BlueBits = 0;
		int RefreshRate = 0;
	};

	inline bool operator==(const VideoMode& left, const VideoMode& right)
	{
		return left.Size == right.Size && left.RedBits == right.RedBits && left.GreenBits == right.GreenBits && left.BlueBits == right.BlueBits && left.
			RefreshRate == right.RefreshRate;
	}

	class Monitor
	{
		std::string m_Name;

		Vector2u m_Size = {};
		Vector2f m_Scale = {};
		Vector2i m_Position = {};
		IntRect m_WorkArea = {};

		GLFWmonitor* m_NativePointer = nullptr;
		void* m_UserData = nullptr;

		VideoMode m_VideoMode;
		std::vector<VideoMode> m_VideoModes;

		static std::vector<Monitor> s_Monitors;
		static Monitor s_PrimaryMonitor;
		static bool s_Initialized;
		
	public:
		Monitor() = default;
		Monitor(const Monitor&) = default;
		Monitor(Monitor&&) = default;

		~Monitor() = default;

		Monitor& operator=(const Monitor&) = default;
		Monitor& operator=(Monitor&&) = default;
		
		std::string GetName() const { return m_Name; }
		Vector2u GetSize() const { return m_Size;  }
		Vector2f GetScale() const { return m_Scale;  }
		Vector2i GetPosition() const { return m_Position; }
		IntRect GetWorkArea() const { return m_WorkArea; }

		void* GetUserData() const { return m_UserData; }
		void* GetNativePointer() const { return m_NativePointer; }

		VideoMode GetVideoMode() const { return m_VideoMode; }
		std::vector<VideoMode> GetVideoModes() const { return m_VideoModes; }
		
		void SetUserData(void* userData);
		void SetGamma(float gamma);
		void SetGammaRamp(const GammaRamp& ramp);

		GammaRamp GetGammaRamp() const;

		static Monitor GetPrimary();
		static Monitor Get(size_t monitor);
		static std::vector<Monitor> GetAll();

	private:

		static Monitor Create(GLFWmonitor* pointer);
		static void Populate();
	};
}

