#ifndef NOTI_HPP
#define NOTI_HPP
#pragma once
#include <vector>
#include <string>

struct Dcontext{
	std::vector<std::wstring> urls;
	std::wstring out;
	int tries{ 5 };
	int location{ 3 };
	bool disposition{ true };
	bool verbose{ false };
};

#endif