#pragma once

#include <string>

namespace Vox
{
	class EditorTools
	{
	public:
		template<typename DrawFunction>
		static void DrawProperty(std::string& title, DrawFunction function);

		template<typename DrawFunction>
		static void DrawObjectProperty(std::string& title, DrawFunction function);
	};

}
