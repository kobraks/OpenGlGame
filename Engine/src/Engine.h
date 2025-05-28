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

#include "Engine/ImGui/ImGuiUtils.h"

#include "Engine/Layers/Layer.h"

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/GraphicContext.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/RenderBuffer.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/OrthographicCamera.h"
#include "Engine/Renderer/PerspectiveCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RendererCommand.h"
#include "Engine/Renderer/ShaderSource.h"
#include "Engine/Renderer/ShaderStage.h"
#include "Engine/Renderer/ShaderProgram.h"

#include "Engine/Renderer/BufferObject.h"

#include "Engine/Renderer/Builders/FramebufferBuilder.h"
#include "Engine/Renderer/Builders/RenderBufferAttachmentBuilder.h"
#include "Engine/Renderer/Builders/ShaderProgramBuilder.h"
#include "Engine/Renderer/Builders/TextureAttachmentBuilder.h"
#include "Engine/Renderer/Builders/TextureBuilder.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Utils/Utils.h"