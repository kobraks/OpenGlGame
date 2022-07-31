#include "pch.h"
#include "Engine/Scene/SceneSerializer.h"

#include "Engine/Scene/Scene.h"

#include "Engine/Utils/LuaUtils.h"


namespace Game
{
	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene)
	{}

}