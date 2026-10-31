#pragma once
#include "Engine/Renderer/Resources/TextureEnums.h"
#include "Engine/Renderer/Resources/FrameBufferEnums.h"

namespace Engine::Utils {
	static constexpr uint8_t ChannelsFor(PixelFormat dataFormat) {
		switch (dataFormat) {
		case PixelFormat::RGBA:
		case PixelFormat::RGBAInteger:
		case PixelFormat::BGRA:
		case PixelFormat::BGRAInteger:
			return 4;

		case PixelFormat::RGB:
		case PixelFormat::RGBInteger:
		case PixelFormat::BGR:
		case PixelFormat::BGRInteger:
			return 3;

		case PixelFormat::RG:
		case PixelFormat::RGInteger:
			return 2;

		default:
			return 1;
		}
	}

	static constexpr uint8_t BytesPerChannel(PixelDataType dataType) {
		switch (dataType) {
		case PixelDataType::Byte:
		case PixelDataType::UnsignedByte:
			return 1u;
		case PixelDataType::Short:
		case PixelDataType::UnsignedShort:
			return 2u;
		case PixelDataType::Int:
		case PixelDataType::UnsignedInt:
		case PixelDataType::Float:
			return 4u;
		case PixelDataType::Double:
			return 8u;
		default:
			return 1u;
		}
	}

	static constexpr int32_t IndexForChannel(PixelFormat dataFormat, Channel channel) {
		switch (dataFormat) {
		case PixelFormat::RGBA:
		case PixelFormat::RGBAInteger:
			switch (channel) {
			case Channel::Red: return 0;
			case Channel::Green: return 1;
			case Channel::Blue: return 2;
			case Channel::Alpha: return 3;
			}

		case PixelFormat::BGRA:
		case PixelFormat::BGRAInteger:
			switch (channel) {
			case Channel::Red: return 2;
			case Channel::Green: return 1;
			case Channel::Blue: return 0;
			case Channel::Alpha: return 3;
			}

		case PixelFormat::RGB:
		case PixelFormat::RGBInteger:
			switch (channel) {
			case Channel::Red: return 0;
			case Channel::Green: return 1;
			case Channel::Blue: return 2;
			case Channel::Alpha: return -1;
			}

		case PixelFormat::BGR:
		case PixelFormat::BGRInteger:
			switch (channel) {
			case Channel::Red: return 2;
			case Channel::Green: return 1;
			case Channel::Blue: return 0;
			case Channel::Alpha: return -1;
			}

		case PixelFormat::RG:
		case PixelFormat::RGInteger:
			switch (channel) {
			case Channel::Red: return 0;
			case Channel::Green: return 1;
			default:
				return -1;
			}

		default:
			return (channel == Channel::Red) ? 0 : -1;
		}
	}

	static constexpr uint32_t BytesPerPixel(PixelFormat dataFormat, PixelDataType dataType) {
		return ChannelsFor(dataFormat) * BytesPerChannel(dataType);
	}
}