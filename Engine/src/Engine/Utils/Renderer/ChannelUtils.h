#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	static constexpr uint8_t ChannelsFor(DataFormat dataFormat) {
		switch (dataFormat) {
		case DataFormat::RGBA:
		case DataFormat::RGBAInteger:
		case DataFormat::BGRA:
		case DataFormat::BGRAInteger:
			return 4;

		case DataFormat::RGB:
		case DataFormat::RGBInteger:
		case DataFormat::BGR:
		case DataFormat::BGRInteger:
			return 3;

		case DataFormat::RG:
		case DataFormat::RGInteger:
			return 2;

		default:
			return 1;
		}
	}

	static constexpr uint8_t BytesPerChannel(DataType dataType) {
		switch (dataType) {
		case DataType::Byte:
		case DataType::UnsignedByte:
			return 1u;
		case DataType::Short:
		case DataType::UnsignedShort:
			return 2u;
		case DataType::Int:
		case DataType::UnsignedInt:
		case DataType::Float:
			return 4u;
		case DataType::Double:
			return 8u;
		default:
			return 1u;
		}
	}

	static constexpr int32_t IndexForChannel(DataFormat dataFormat, Channel channel) {
		switch (dataFormat) {
		case DataFormat::RGBA:
		case DataFormat::RGBAInteger:
			switch (channel) {
			case Channel::Red: return 0;
			case Channel::Green: return 1;
			case Channel::Blue: return 2;
			case Channel::Alpha: return 3;
			}

		case DataFormat::BGRA:
		case DataFormat::BGRAInteger:
			switch (channel) {
			case Channel::Red: return 2;
			case Channel::Green: return 1;
			case Channel::Blue: return 0;
			case Channel::Alpha: return 3;
			}

		case DataFormat::RGB:
		case DataFormat::RGBInteger:
			switch (channel) {
			case Channel::Red: return 0;
			case Channel::Green: return 1;
			case Channel::Blue: return 2;
			case Channel::Alpha: return -1;
			}

		case DataFormat::BGR:
		case DataFormat::BGRInteger:
			switch (channel) {
			case Channel::Red: return 2;
			case Channel::Green: return 1;
			case Channel::Blue: return 0;
			case Channel::Alpha: return -1;
			}

		case DataFormat::RG:
		case DataFormat::RGInteger:
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

	static constexpr uint32_t BytesPerPixel(DataFormat dataFormat, DataType dataType) {
		return ChannelsFor(dataFormat) * BytesPerChannel(dataType);
	}
}