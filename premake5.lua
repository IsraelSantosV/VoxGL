workspace "VoxGL"
	architecture "x86_64"
	startproject "VoxGL-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "VoxGL/ThirdParty/GLFW/include"
IncludeDir["Glad"] = "VoxGL/ThirdParty/Glad/include"
IncludeDir["ImGui"] = "VoxGL/ThirdParty/imgui"
IncludeDir["glm"] = "VoxGL/ThirdParty/glm"
IncludeDir["stb_image"] = "VoxGL/ThirdParty/stb_image"
IncludeDir["entt"] = "VoxGL/ThirdParty/entt/include"

group "Dependencies"
	include "VoxGL/ThirdParty/GLFW"
	include "VoxGL/ThirdParty/Glad"
	include "VoxGL/ThirdParty/imgui"

group ""

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
		"%{prj.name}/ThirdParty/stb_image/**.h",
		"%{prj.name}/ThirdParty/stb_image/**.cpp",
		"%{prj.name}/ThirdParty/glm/glm/**.hpp",
		"%{prj.name}/ThirdParty/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/ThirdParty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"VoxGL"
	}

	filter "system:windows"
		systemversion "latest"

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

project "VoxGL-Editor"
	location "VoxGL-Editor"
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"VoxGL"
	}

	filter "system:windows"
		systemversion "latest"

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