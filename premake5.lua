workspace "VoxGL"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "VoxGL/ThirdParty/GLFW/include"

include "VoxGL/ThirdParty/GLFW"

project "VoxGL"
	location "VoxGL"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "VoxPch.h"
	pchsource "VoxGL/Source/VoxPch.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/ThirdParty/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0"

		defines
		{
			"VOX_PLATFORM_WINDOWS",
			"VOX_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "VOX_DEBUG"
		symbols "On"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "VOX_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		"VoxGL/ThirdParty/spdlog/include",
		"VoxGL/Source"
	}

	links
	{
		"VoxGL"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0"

		defines
		{
			"VOX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VOX_DEBUG"
		symbols "On"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "VOX_DIST"
		optimize "On"