#pragma once
#include "Engine/Core/Base.h"

#include <functional>

namespace Engine {
	//Starts a 2-column property grid table
	void BeginPropertyGrid(float labelColumnWidth = 120.0f);
	void BeginPropertyGrid(std::string_view label, float labelColumnWidth = 120.0f);

	//Ends the property grid table
	void EndPropertyGrid();

	//Starts a row with the label in column 0
	void PropertyRow(std::string_view label);
	void PropertyRow(std::string_view label, std::string_view tooltip);
	void PropertyRow(std::string_view label, const std::function<void()>& drawField);
	void PropertyRow(std::string_view label, std::string_view tooltip, const std::function<void()>& drawField);

	void PropertyRowWithReset(std::string_view label, const std::function<void()>& drawField, std::string_view tooltip = "", const std::function<void()> onReset = {}, float buttonSize = 20.0f);

	void PropertyRowEnd();
}
