#pragma once

#include "Vox/Core/Core.h"
#include "Vox/Editor/FontAwesome6.h"

namespace Vox::UI
{
	const std::string GetText(const char* icon, const char* text)
	{
		return fmt::format("{}   {}", icon, text);
	}
}
