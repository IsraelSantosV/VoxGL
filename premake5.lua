include "./ThirdParty/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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

group "Dependencies"
	include "ThirdParty/premake"
	include "VoxGL/ThirdParty/Box2D"
	include "VoxGL/ThirdParty/GLFW"
	include "VoxGL/ThirdParty/Glad"
	include "VoxGL/ThirdParty/imgui"
	include "VoxGL/ThirdParty/yaml-cpp"

group ""

include "VoxGL"
include "Sandbox"
include "VoxGL-Editor"
