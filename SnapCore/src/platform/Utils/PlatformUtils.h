#pragma once

namespace SnapEngine
{
	class FileDialoge
	{
	public:
		// Returns Empty Strings When Dialoge Closed
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}