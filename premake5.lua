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
Dependencies["Entt"] = "SnapCore/vendor/Entt/include"
Dependencies["YAML"] = "SnapCore/vendor/YAML/include"
Dependencies["IMGUIZMO"] = "SnapCore/vendor/imguizmo"
Dependencies["Box2D"] = "SnapCore/vendor/Box2D/include"
Dependencies["mono"] = "SnapCore/vendor/mono/include"
Dependencies["FileWatch"] = "SnapCore/vendor/FileWatch"

LibraryDirectories = {}
LibraryDirectories["mono"] = "%{wks.location}/SnapCore/vendor/mono/lib/Debug"

Library = {}
Library["mono"] = "%{LibraryDirectories.mono}/libmono-static-sgen.lib"

include "SnapCore/vendor/GLFW"
include "SnapCore/vendor/GLAD"
include "SnapCore/vendor/imgui"
include "SnapCore/vendor/YAML"
include "SnapCore/vendor/Box2D"

project "SnapCore"                                                       -- SnapCore
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
		"%{prj.name}/vendor/GLM/glm/**.hpp",
		"%{prj.name}/vendor/imguizmo/ImGuizmo.h",
		"%{prj.name}/vendor/imguizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"SnapCore/src",
        "%{prj.name}/vendor/spdlog/include",
		"%{Dependencies.GLFW}",
		"%{Dependencies.GLAD}",
		"%{Dependencies.ImGui}",
		"%{Dependencies.GLM}",
		"%{Dependencies.STB}",
		"%{Dependencies.Entt}",
		"%{Dependencies.YAML}",
		"%{Dependencies.IMGUIZMO}",
		"%{Dependencies.Box2D}",
		"%{Dependencies.mono}",
		"%{Dependencies.FileWatch}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"yaml-cpp",
		"Box2D",
		"opengl32.lib",
		--"%{Library.mono}"
		"%{LibraryDirectories.mono}/%{cfg.buildcgf}/mono-2.0-sgen.lib"
	}

	filter "files:vendor/imguizmo/**.cpp"
	flags {"NoPCH"}

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
		"SNAP_ENABLE_ASSERTION",
		"YAML_CPP_STATIC_DEFINE"
	}

	links
	{
		"ws2_32.lib",
		"winmm.lib",
		"version.lib",
		"Bcrypt.lib"
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

project "SandBox"                                                                -- SandBox
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
		"%{Dependencies.GLM}",
		"%{Dependencies.Entt}",
		"%{Dependencies.IMGUIZMO}"
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

project "SnapEditor"                                                             -- SnapEditor
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
		"%{Dependencies.GLM}",
		"%{Dependencies.Entt}",
		"%{Dependencies.yaml_cpp}",
		"%{Dependencies.IMGUIZMO}"
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


project "Snap-ScriptCore"                                                               -- Snap-ScriptCore
    location "Snap-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/SnapEditor/Resources/Scripts")
	objdir ("Build/intermediate/SnapEditor/Resources/Scripts/intermediate")

	files
	{
	    "%{prj.name}/Source/**.cs",
        "%{prj.name}/Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "off"
		symbols "Default"

	filter "configurations:Release"
		optimize "on"
		symbols "Default"

	filter "configurations:Distribution"
		optimize "full"
		symbols "off"




		------------------- GameProjects --------------------

workspace "SandBoxGame"
      startproject "SandBoxGame"
	  architecture "x64"

	  configurations
	  {
	     "Debug",
		 "Release",
		 "Distribution"
	  }

	  flags
	  {
		  "MultiProcessorCompile"
	  }

project_output = "%{cfg.buildcgf}-%{cfg.system}-%{cfg.architecture}"

project "SandBoxGame"                                                               -- SandBoxGame
    location "SnapEditor/SandBoxGame"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/SnapEditor/Resources/Scripts")
	objdir ("Build/intermediate/SnapEditor/Resources/Scripts/intermediate")

	files
	{
	    "%{prj.name}/Assets/Scripts/**.cs"
	}

	links
	{
		"Snap-ScriptCore"
	}

	filter "configurations:Debug"
		optimize "off"
		symbols "Default"

	filter "configurations:Release"
		optimize "on"
		symbols "Default"

	filter "configurations:Distribution"
		optimize "full"
		symbols "off"