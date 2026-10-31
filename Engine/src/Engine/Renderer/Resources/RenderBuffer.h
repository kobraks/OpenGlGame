#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/Resources/TextureEnums.h"

namespace Engine {
	class RenderBuffer {
	public:
		friend class Framebuffer;

		using IDType = uint32_t;


		static Ref<RenderBuffer> Create(const Vector2u& size, uint32_t samples, TextureInternalFormat imageFormat, const std::string& label = {});


		IDType ID() const { return m_GLState->ID; }
		operator IDType() const { return m_GLState->ID; }

		void SetLabel(const std::string& label);

		void Bind() const;
		void Unbind() const;

		Vector2u Size() const { return m_GLState->Size; }
		uint32_t Width() const { return m_GLState->Size.Width; }
		uint32_t Height() const { return m_GLState->Size.Height; }

		uint32_t Samples() const { return m_GLState->Samples; }
		bool IsMultisampled() const { return m_GLState->Multisampled; }

		TextureInternalFormat ImageFormat() const { return m_GLState->ImageFormat; }

		std::string_view Label() const { return m_GLState->Label; }

		static uint32_t QueryMaxSamples();
		static Vector2u QueryMaxSize();

	protected:
		RenderBuffer(bool multisampled = false);

	private:
		struct GLState {
			IDType ID;
			Vector2u Size;

			uint32_t Samples = 1;
			bool Multisampled = false;

			enum TextureInternalFormat ImageFormat = TextureInternalFormat::RGBA8;

			std::string Label = {};

			GLState(bool multisampled = false);
			~GLState();
		};

		void CreateBuffer(uint32_t samples, const Vector2u& size, TextureInternalFormat internalFormat);
		void Allocate(uint32_t samples, const Vector2u& size, TextureInternalFormat imageFormat);
		void Allocate(const Vector2u& size, TextureInternalFormat imageFormat);

		Ref<GLState> m_GLState;
	};
}
