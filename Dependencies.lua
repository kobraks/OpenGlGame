BOOST = os.getenv("BOOST_DIR")

IncludeDir = {}
IncludeDir["Boost"] = "%{BOOST}"

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
IncludeDir["Yaml"] = "%{wks.location}/vendor/yaml-cpp/include"
IncludeDir["Assimp"] = "%{wks.location}/vendor/Assimp/include"
IncludeDir["AssimpGen"] = "%{wks.location}/vendor/Assimp/generated/include"


LibraryDir = {}
LibraryDir["Boost"] = "%{BOOST}/stage/lib"

Library = {}
