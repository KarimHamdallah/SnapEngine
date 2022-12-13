project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. project_output .. "/%{prj.name}")
	objdir ("bin-int/" .. project_output .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"include/**.h"
	}

	includedirs
	{
		"include"
	}
	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	filter "system:windows"
		systemversion "latest"


	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
        buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
        buildoptions "/MT"
		optimize "on"