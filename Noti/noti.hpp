#ifndef NOTI_HPP
#define NOTI_HPP
#pragma once
#include <vector>
#include <string>

struct NotiContext{
	std::vector<std::wstring> urls;
	std::wstring out;
	int retries{ 5 };
	int location{ 3 };
	bool disposition{ true };
	bool verbose{ false };
};

#endif