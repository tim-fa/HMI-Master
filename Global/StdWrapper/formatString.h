#pragma once
#include <string>

namespace fmt {
	static std::string format(const std::string& format)
	{
		return format;
	}

	template<typename ... Args>
	std::string format(const std::string& format, Args ... args);
}

#include "FormatString.impl.h"
