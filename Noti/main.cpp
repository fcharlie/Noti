// main.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "argv.hpp"
#include "noti.hpp"

using namespace winrt;
using namespace Windows::Foundation;


int ParseArgv(int Argc, wchar_t **Argv) {
	ParseArgs pa(Argc, Argv);
	std::vector<ParseArgs::option> opts = {
		{L"content-disposition",ParseArgs::optional_argument,L'd'}, /// --content-disposition=false -dfalse -dtrue -d=true -d=false
		{L"help",ParseArgs::no_argument,L'h'},
		{L"location",ParseArgs::optional_argument,L'L'},
		{L"uri",ParseArgs::required_argument,L'u'},
		{L"output",ParseArgs::required_argument,L'O'},
		{L"version",ParseArgs::no_argument,L'v'},
		{L"verbose",ParseArgs::optional_argument,L'V'},
		{L"retries",ParseArgs::required_argument,L'r'}
	};
	auto err = pa.ParseArgument(opts, [&](int ch, const wchar_t *optarg, const wchar_t *raw) {
		switch (ch) {
		case 'h':
			wprintf(L"--help\n");
			break;
		case 'u':
			if (optarg != nullptr) {
				wprintf(L"uri %s\n", optarg);
			}
			break;
		default:
			wprintf(L"unsupport %s\n", raw != nullptr ? raw : L"unknown");
			break;
		}
		return true;
	});
	if (err.errorcode != 0) {
		wprintf(L"error %s\n", err.message.c_str());
	}
	for (auto &v : pa.UnresolvedArgs()) {
		wprintf(L"Unresolved: %s\n", v.data());
	}
	return 0;
}
// --content-disposition
int wmain(int argc, wchar_t **argv)
{
	ParseArgv(argc, argv);
	init_apartment();
	Uri uri(L"http://aka.ms/cppwinrt");
	printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
	return 0;
}
