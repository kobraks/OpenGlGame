#include "pch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Log.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "Window.h"

using namespace Game;

const std::string vertexSourceCode =
	R"(#version 330

struct light
{
	vec3 position;
	vec3 location;
	vec3 normal;
};

uniform int mode;
uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;
uniform vec4 test[10];
uniform light lights;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexcoord;
layout(location = 3) in vec3 inNormal;


out vec2 texCoord; 
out vec4 outColor;
out vec3 normal;

void main()
{
	texCoord = inTexcoord;
	outColor = inColor;
	
	normal = inNormal;
	normal = normalize(mat3x3(Model) * inNormal);

	gl_Position = Projection * View * Model * vec4(inPosition, 1);

	for(int i = 0; i < 10; ++i)
	gl_Position += test[i];
	gl_Position.x += lights.position.x + lights.location.x + lights.normal.x;
})";
const std::string fragmentSourceCode =
	R"(#version 330

uniform sampler2D my_texture; 
uniform int mode;
uniform vec4 color = vec4(1.f, 1.f, 1.f, 1.f);

in vec2 texCoord; 
in vec4 outColor;
in vec3 normal;
out vec4 frag_color;

vec4 textured()
{
	vec4 result = vec4(texture(my_texture, texCoord.st).rgb, 1.f);

	return result * outColor;
}

vec4 no_textures()
{
	return outColor;
}

vec4 no_color()
{
	return color;
}

void main() 
{
	vec4 result;
	if (mode == 1)
		result = textured();
	else if (mode == 0)
		result = no_textures();
	else if (mode == -1)
		result = no_color();
   
	//gl_FragColor = result;

	frag_color = result;
	//frag_color = vec4(1.f, 1.f, 1.f, 1.f);
})";

int main(int argc, char **argv)
{
	Log::Init();

	if(!glfwInit())
	{
		LOG_CRITICAL("Failed to initialize window");
		return -1;
	}

	WindowProperties prop{"test", 800, 600};
	Window window(prop);
	window.SetEventCallback(
	                        [](Event &event)
	                        {
		                        LOG_TRACE(event.ToString());
	                        }
	                       );

	const ShaderSource vertexSource(vertexSourceCode);
	const ShaderSource fragmentSource(fragmentSourceCode);
	Shader vertex(Shader::Type::Vertex, vertexSource);
	Shader fragment(Shader::Type::Fragment, fragmentSource);

	ShaderProgram program("Test", vertex, fragment);

	for(auto i = 0; i < 10; ++i)
	{
		LOG_INFO("Location of Test[{}] = {}", i, program.GetUniformLocation( fmt::format("test[{}]", i)));
	}


	LOG_WARN(program.GetLog());


	while(true)
	{
		window.OnUpdate();
	}

	int32_t exitCode = EXIT_SUCCESS;

	try { }
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
		system("pause");
		exitCode = EXIT_FAILURE;
	}

	LOG_INFO("Exited with code: {}", exitCode);

	return 0;
}
