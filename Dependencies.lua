BOOST = os.getenv("BOOST_DIR")

IncludeDir = {}
IncludeDir["Boost"] = "%{BOOST_DIR}"

IncludeDir["Glad"] = "%{wks.location}/vendor/glad/include"
IncludeDir["Spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["Lua"] = "%{wks.location}/vendor/lua/src"
IncludeDir["Sol2"] = "%{wks.location}/vendor/sol2/include"
IncludeDir["Fmt"] = "%{wks.location}/vendor/fmt/include"
IncludeDir["GLFW"] = "%{wks.location}/vendor/GLFW/include"
IncludeDir["FreeImage"] = "%{wks.location}/vendor/FreeImage/Source"
IncludeDir["ImGui"] = "%{wks.location}/vendor/ImGui"
IncludeDir["Glm"] = "%{wks.location}/vendor/glm"
IncludeDir["ImGuizmo"] = "%{wks.location}/vendor/ImGuizmo"
IncludeDir["Entt"] = "%{wks.location}/vendor/entt"


LibraryDir = {}
LibraryDir["Boost"] = "%{BOOST_DIR}/stage/lib"

Library = {}
