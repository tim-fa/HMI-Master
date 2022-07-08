#pragma once
#include <string>
#include <Windows.h>
#include "StdWrapper/formatString.h"

namespace Log {
	enum LColor {
		DBLUE = 1,
		DGREEN,
		DGREENBLUE,
		DRED,
		DPINK,
		DYELLOW,
		DWHITE,
		DGREY,
		LBLUE,
		LGREEN,
		LGREENBLUE,
		LRED,
		LPINK,
		LYELLOW,
		LWHITE
	};

	static void setColor(LColor color) {
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

	template<class ... Args>
	static void info(const std::string& msg, Args ... args)
	{
		add(LWHITE, msg, args...);
	}
	template<class ... Args>
	static void error(const std::string& msg, Args ... args)
	{
		add(LRED, msg, args...);
	}
	template<class ... Args>
	static void warning(const std::string& msg, Args ... args)
	{
		add(LYELLOW, msg, args...);
	}

	template<class ... Args>
	static void add(LColor color, const std::string& msg, Args ... args)
	{
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		printf("> %s\n", fmt::format(msg, args...).c_str());
		SetConsoleTextAttribute(hConsole, LWHITE);
	}
};

