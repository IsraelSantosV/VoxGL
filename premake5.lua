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
IncludeDir["glm"] = "VoxGL/ThirdParty/glm"

include "VoxGL/ThirdParty/GLFW"
include "VoxGL/ThirdParty/Glad"
include "VoxGL/ThirdParty/imgui"

project "VoxGL"
	location "VoxGL"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "VoxPch.h"
	pchsource "VoxGL/Source/VoxPch.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/ThirdParty/glm/glm/**.hpp",
		"%{prj.name}/ThirdParty/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/ThirdParty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VOX_PLATFORM_WINDOWS",
			"VOX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "VOX_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VOX_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"VoxGL/Source",
		"VoxGL/ThirdParty",
		"%{IncludeDir.glm}"
	}

	links
	{
		"VoxGL"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VOX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VOX_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "VOX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VOX_DIST"
		runtime "Release"
		optimize "on"