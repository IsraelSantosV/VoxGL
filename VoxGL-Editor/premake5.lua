project "VoxGL-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/VoxGL/ThirdParty/spdlog/include",
		"%{wks.location}/VoxGL/Source",
		"%{wks.location}/VoxGL/ThirdParty",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
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
