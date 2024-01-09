#pragma once
#include <vector>

class ShaderManager
{
public:
	std::vector<unsigned int> m_Shaders;

	ShaderManager() {}
	~ShaderManager()
	{
		// delete all shaders
	}
};