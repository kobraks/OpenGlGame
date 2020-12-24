#include "pch.h"
#include "OpenGlFunctions.h"

namespace Game
{
	void OpenGlFunctions::Clear(const BufferBit buffer)
	{
		glClear(static_cast<GLbitfield>(buffer));
	}
	void OpenGlFunctions::Enable(const Capability capability)
	{
		m_Capabilities[capability] = true;
		glEnable(static_cast<GLenum>(capability));
	}
	void OpenGlFunctions::Disable(const Capability capability)
	{
		m_Capabilities[capability] = false;
		glDisable(static_cast<GLenum>(capability));
	}
	
	bool OpenGlFunctions::Enabled(const Capability capability)
	{
		const auto iter = m_Capabilities.find(capability);

		if (iter != m_Capabilities.end())
			return iter->second;

		return m_Capabilities[capability] = glIsEnabled(static_cast<GLenum>(capability));
	}
}
