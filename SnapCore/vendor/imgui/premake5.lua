project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
   	staticruntime "off"

	targetdir ("bin/" .. project_output .. "/%{prj.name}")
	objdir ("bin-int/" .. project_output .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp"
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
