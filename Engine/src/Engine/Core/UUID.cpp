#include "pch.h"
#include "Engine/Core/UUID.h"

#include <boost/uuid/name_generator_sha1.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <stdexcept>

namespace Engine {
	UUID UUID::Generate() {
		thread_local boost::uuids::random_generator generator;
		return UUID(generator());
	}

	UUID UUID::FromName(std::string_view name) {
		static const UUID s_EngineNamespace = UUID::Parse("2bcfdccd-cb54-4ba1-9970-04b1a6de886d");

		return FromName(name, s_EngineNamespace);
	}

	UUID UUID::FromName(std::string_view name, const UUID& namespaceID) {
		const boost::uuids::name_generator_sha1 generator(namespaceID.Value());
		return UUID(generator(name.data(), name.size()));
	}

	UUID UUID::Parse(std::string_view text) {
		const auto result = TryParse(text);

		if (!result)
			throw std::invalid_argument("Invalid UUID string");

		return *result;
	}

	std::optional<UUID> UUID::TryParse(std::string_view text) noexcept {
		if (text.empty())
			return std::nullopt;

		try {
			return UUID(boost::uuids::string_generator{}(text.begin(), text.end()));
		} catch (const std::exception&) {
			return std::nullopt;
		}
	}

	std::string UUID::ToString() const {
		return boost::uuids::to_string(m_ID);
	}
}
