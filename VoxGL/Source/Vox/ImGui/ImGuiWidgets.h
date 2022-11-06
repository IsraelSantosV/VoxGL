#pragma once

#include "Vox/Tools/StringUtils.h"
#include "choc/text/choc_StringUtilities.h"

namespace Vox::UI
{

	static bool IsMatchingSearch(const std::string& item, std::string_view searchQuery, bool caseSensitive = false, bool stripWhiteSpaces = false, bool stripUnderscores = false)
	{
		if (searchQuery.empty())
			return true;

		if (item.empty())
			return false;

		std::string itemSanitized = stripUnderscores ? choc::text::replace(item, "_", " ") : item;

		if (stripWhiteSpaces)
			itemSanitized = choc::text::replace(itemSanitized, " ", "");

		std::string searchString = stripWhiteSpaces ? choc::text::replace(searchQuery, " ", "") : std::string(searchQuery);

		if (!caseSensitive)
		{
			itemSanitized = Utils::String::ToLower(itemSanitized);
			searchString = Utils::String::ToLower(searchString);
		}

		bool result = false;
		if (choc::text::contains(searchString, " "))
		{
			std::vector<std::string> searchTerms = choc::text::splitAtWhitespace(searchString);
			for (const auto& searchTerm : searchTerms)
			{
				if (!searchTerm.empty() && choc::text::contains(itemSanitized, searchTerm))
					result = true;
				else
				{
					result = false;
					break;
				}
			}
		}
		else
		{
			result = choc::text::contains(itemSanitized, searchString);
		}

		return result;
	}
}
