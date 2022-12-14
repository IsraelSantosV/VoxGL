#pragma once

namespace Vox::Theme
{
	static ImVec4 RgbColor(ImVec4 color)
	{
		return ImVec4{ color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, color.w };
	}

	inline static ImVec4& LightColor = RgbColor(ImVec4{ 183, 183, 183, 1 });
	inline static ImVec4& SemiLightColor = RgbColor(ImVec4{ 122, 122, 122, 1 });
	inline static ImVec4& MediumColor = RgbColor(ImVec4{ 92, 92, 92, 1 });
	inline static ImVec4& ContrastColor = RgbColor(ImVec4{ 50, 13, 109, 1 });
	inline static ImVec4& SemiBackgroundColor = RgbColor(ImVec4{ 40, 40, 40, 1 });
	inline static ImVec4& DarkBackgroundColor = RgbColor(ImVec4{ 25, 25, 25, 1 });
	inline static ImVec4& BackgroundColor = RgbColor(ImVec4{ 31, 31, 31, 1 });

	inline static ImVec4& WindowColor = BackgroundColor;

	inline static ImVec4& HeaderColor = ContrastColor;
	inline static ImVec4& HeaderHoveredColor = SemiBackgroundColor;
	inline static ImVec4& HeaderActiveColor = SemiLightColor;

	inline static ImVec4& ButtonColor = ContrastColor;
	inline static ImVec4& ButtonHoveredColor = SemiBackgroundColor;
	inline static ImVec4& ButtonActiveColor = SemiLightColor;

	inline static ImVec4& SecondaryButtonColor = DarkBackgroundColor;
	inline static ImVec4& SecondaryButtonHoveredColor = SemiBackgroundColor;
	inline static ImVec4& SecondaryButtonActiveColor = SemiLightColor;

	inline static ImVec4& FrameColor = SemiBackgroundColor;
	inline static ImVec4& FrameHoveredColor = SemiBackgroundColor;
	inline static ImVec4& FrameActiveColor = LightColor;

	inline static ImVec4& TabColor = SemiBackgroundColor;
	inline static ImVec4& TabHoveredColor = MediumColor;
	inline static ImVec4& TabActiveColor = SemiLightColor;
	inline static ImVec4& TabUnfocusedColor = SemiBackgroundColor;
	inline static ImVec4& TabUnfocusedActiveColor = MediumColor;

	inline static ImVec4& TitleColor = SemiBackgroundColor;
	inline static ImVec4& TitleActiveColor = ContrastColor;
	inline static ImVec4& TitleCollapseColor = MediumColor;
}
