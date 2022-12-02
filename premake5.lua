workspace "SnapEngine"
      startproject "SandBox"
	  architecture "x64"

	  configurations
	  {
	     "Debug",
		 "Release",
		 "Distribution"
	  }

project_output = "%{cfg.buildcgf}-%{cfg.system}-%{cfg.architecture}"

Dependencies = {}
Dependencies["GLFW"] = "SnapCore/vendor/GLFW/include"
Dependencies["GLAD"] = "SnapCore/vendor/GLAD/include"
Dependencies["ImGui"] = "SnapCore/vendor/imgui"
Dependencies["GLM"] = "SnapCore/vendor/GLM"
Dependencies["STB"] = "SnapCore/vendor/STB"

include "SnapCore/vendor/GLFW"
include "SnapCore/vendor/GLAD"
include "SnapCore/vendor/imgui"

project "SnapCore"
    location "SnapCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("Build/Bin".. project_output .. "{prj.name}")
	objdir ("Build/intermediate/".. project_output .. "{prj.name}")

	pchheader "SnapPCH.h"
	pchsource "SnapCore/src/SnapPCH.cpp"

	files
	{
	    "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/GLM/glm/**.hpp"
	}

	includedirs
	{
		"SnapCore/src",
        "%{prj.name}/vendor/spdlog/include",
		"%{Dependencies.GLFW}",
		"%{Dependencies.GLAD}",
		"%{Dependencies.ImGui}",
		"%{Dependencies.GLM}",
		"%{Dependencies.STB}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
	    cppdialect "c++17"
		staticruntime "On"
		systemversion "latest"

		defines
	{
		"NOMINMAX",
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"SNAP_PLATFORM_WINDOWS",
		"SNAP_ENABLE_ASSERTION"
	}
	
	 filter "configurations:Debug"
	 	defines "SNAP_ENGINE_DEBUG"
		buildoptions "/MTd"
	 	runtime "Debug"
	 	symbols "on"
	 
	 filter "configurations:Release"
	 	defines "SNAP_ENGINE_RELEASE"
		buildoptions "/MT"
	 	runtime "Release"
	 	optimize "on"
	 
	 filter "configurations:Dist"
	 	defines "SNAP_ENGINE_DIST"
	    buildoptions "/MT"
	 	runtime "Release"
	 	optimize "on"

project "SandBox"
    location "SandBox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("Build/Bin".. project_output .. "{prj.name}")
	objdir ("Build/intermediate/".. project_output .. "{prj.name}")

	files
	{
	    "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
        "SnapCore/vendor/spdlog/include",
		"SnapCore/src",
		"%{Dependencies.ImGui}",
		"%{Dependencies.GLM}"
	}

	links
	{
		"SnapCore"
	}

	filter "system:windows"
	    cppdialect "c++17" 
		staticruntime "On"
		systemversion "latest"

		defines
	{
		"SNAP_PLATFORM_WINDOWS"
	}
	
	filter "configurations:Debug"
		defines "SNAP_ENGINE_DEBUG"
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "SNAP_ENGINE_RELEASE"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SNAP_ENGINE_DIST"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

project "SnapEditor"
    location "SnapEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("Build/Bin".. project_output .. "{prj.name}")
	objdir ("Build/intermediate/".. project_output .. "{prj.name}")

	files
	{
	    "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
        "SnapCore/vendor/spdlog/include",
		"SnapCore/src",
		"%{Dependencies.ImGui}",
		"%{Dependencies.GLM}"
	}

	links
	{
		"SnapCore"
	}

	filter "system:windows"
	    cppdialect "c++17" 
		staticruntime "On"
		systemversion "latest"

		defines
	{
		"SNAP_PLATFORM_WINDOWS"
	}
	
	filter "configurations:Debug"
		defines "SNAP_ENGINE_DEBUG"
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "SNAP_ENGINE_RELEASE"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SNAP_ENGINE_DIST"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"