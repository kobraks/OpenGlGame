BOOST = os.getenv("BOOST_DIR")

IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/vendor/glad"
IncludeDir["Spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["Lua"] = "%{wks.location}/vendor/lua/src"
IncludeDir["Glm"] = "%{wks.location}/vendor/glm"
IncludeDir["Sol2"] = "%{wks.location}/vendor/sol2/include"
IncludeDir["Fmt"] = "%{wks.location}/vendor/fmt/include"

LibraryDir = {}

Library = {}
