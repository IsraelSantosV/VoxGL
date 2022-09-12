include "./ThirdParty/premake/premake_customization/solution_items.lua"

workspace "VoxGL"
	architecture "x86_64"
	startproject "VoxGL-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/VoxGL/ThirdParty/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/VoxGL/ThirdParty/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/VoxGL/ThirdParty/imgui"
IncludeDir["glm"] = "%{wks.location}/VoxGL/ThirdParty/glm"
IncludeDir["stb_image"] = "%{wks.location}/VoxGL/ThirdParty/stb_image"
IncludeDir["entt"] = "%{wks.location}/VoxGL/ThirdParty/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/VoxGL/ThirdParty/yaml-cpp/include"

group "Dependencies"
	include "ThirdParty/premake"
	include "VoxGL/ThirdParty/GLFW"
	include "VoxGL/ThirdParty/Glad"
	include "VoxGL/ThirdParty/imgui"
	include "VoxGL/ThirdParty/yaml-cpp"

group ""

include "VoxGL"
include "Sandbox"
include "VoxGL-Editor"
