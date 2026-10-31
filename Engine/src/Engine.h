#pragma once

#include "Engine/Core/Config.h"
#include "Engine/Core/Base.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

#include "Engine/Core/Vector2.h"
#include "Engine/Core/Rect.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Clock.h"
#include "Engine/Core/Color.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Window.h"

#include "Engine/Core/Buffer.h"

#include "Engine/Core/Image.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Core/MouseCodes.h"

#include "Engine/Devices/Cursor.h"
#include "Engine/Devices/Monitor.h"
#include "Engine/Devices/Mouse.h"
#include "Engine/Devices/Keyboard.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include "Engine/ImGui/ImGuiScoped.h"
#include "Engine/ImGui/ImGuiDrawUtils.h"
#include "Engine/ImGui/ImGuiInput.h"
#include "Engine/ImGui/ImGuiWidgets.h"

#include "Engine/Layers/Layer.h"

#include "Engine/Renderer/Cameras/Camera.h"
#include "Engine/Renderer/OpenGL/GraphicContext.h"
#include "Engine/Renderer/Cameras/EditorCamera.h"
#include "Engine/Renderer/Resources/Texture.h"
#include "Engine/Renderer/Resources/RenderBuffer.h"
#include "Engine/Renderer/Resources/Framebuffer.h"
#include "Engine/Renderer/Cameras/OrthographicCamera.h"
#include "Engine/Renderer/Cameras/PerspectiveCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RendererCommand.h"
#include "Engine/Renderer/Resources/ShaderSource.h"
#include "Engine/Renderer/Resources/ShaderStage.h"
#include "Engine/Renderer/Resources/ShaderProgram.h"

#include "Engine/Renderer/Resources/BufferObject.h"

#include "Engine/Renderer/Resources/Builders/FramebufferBuilder.h"
#include "Engine/Renderer/Resources/Builders/RenderBufferAttachmentBuilder.h"
#include "Engine/Renderer/Resources/Builders/ShaderProgramBuilder.h"
#include "Engine/Renderer/Resources/Builders/TextureAttachmentBuilder.h"
#include "Engine/Renderer/Resources/Builders/TextureBuilder.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Utils/Utils.h"