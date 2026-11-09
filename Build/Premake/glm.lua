local GLMRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/glm")

project "GLM"
    kind "None"
    language "C++"
    
    location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/glm"))

    files
    {
        path.join(GLMRoot, "glm/**.h"),
        path.join(GLMRoot, "glm/**.hpp"),
        path.join(GLMRoot, "glm/**.inl")
    }