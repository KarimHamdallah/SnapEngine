project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
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
		"include",
		"src"
	}

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
