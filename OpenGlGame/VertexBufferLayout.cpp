#include "pch.h"
#include "VertexBufferLayout.h"

void Game::VertexBufferLayout::Clear()
{
	m_Stride = 0;
	m_Elements.clear();
}
