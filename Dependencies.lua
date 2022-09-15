VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/VoxGL/ThirdParty/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/VoxGL/ThirdParty/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/VoxGL/ThirdParty/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/VoxGL/ThirdParty/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/VoxGL/ThirdParty/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/VoxGL/ThirdParty/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/VoxGL/ThirdParty/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/VoxGL/ThirdParty/glm"
IncludeDir["entt"] = "%{wks.location}/VoxGL/ThirdParty/entt/include"
IncludeDir["shaderc"] = "%{wks.location}/VoxGL/ThirdParty/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/VoxGL/ThirdParty/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
