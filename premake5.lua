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
IncludeDir["Glad"] = "VoxGL/ThirdParty/Glad/include"
IncludeDir["ImGui"] = "VoxGL/ThirdParty/imgui"

include "VoxGL/ThirdParty/GLFW"
include "VoxGL/ThirdParty/Glad"
include "VoxGL/ThirdParty/imgui"

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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0"

		defines
		{
			"VOX_PLATFORM_WINDOWS",
			"VOX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "VOX_DEBUG"
		buildoptions "/MDd"
		symbols "On"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "VOX_DIST"
		buildoptions "/MD"
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
		buildoptions "/MDd"
		symbols "On"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "VOX_DIST"
		buildoptions "/MD"
		optimize "On"