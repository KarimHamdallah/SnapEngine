#pragma once
#include <Snap/Core/Core.h>
#include "Project.h"

namespace SnapEngine
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(const SnapPtr<Project>& Project);

		bool Serialize(const std::filesystem::path& ProjectPath);
		bool DeSerialize(const std::filesystem::path& ProjectPath);

	private:
		SnapPtr<Project> m_Project;
	};
}