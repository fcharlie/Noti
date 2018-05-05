// main.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "argv.hpp"
#include "noti.hpp"

using namespace winrt;
using namespace Windows::Foundation;


template<typename Integer>
ErrorResult Fromwchars(std::wstring_view wsv, Integer &iv)
{
	return Integer_from_wchars(wsv, iv, 10);
}

ErrorResult Fromwchars(std::wstring_view wsv, bool &bv) {
	if (_wcsicmp(wsv.data(), L"true") == 0 || _wcsicmp(wsv.data(), L"on") == 0
		|| _wcsicmp(wsv.data(), L"yes") == 0 || wsv.compare(L"1") == 0) {
		bv = true;
		return ErrorResult{};
	}
	if (_wcsicmp(wsv.data(), L"false") == 0 || _wcsicmp(wsv.data(), L"off") == 0
		|| _wcsicmp(wsv.data(), L"no") == 0 || wsv.compare(L"0") == 0) {
		bv = false;
		return ErrorResult{};
	}
	return ErrorResult{ std::wstring(L"Illegal boolean ").append(wsv),1 };
}

void PrintUsage()
{
	const wchar_t *ua = LR"(OVERVIEW: Noti windows download utils
Usage: noti [options] <input>
OPTIONS:
  -C [--content-disposition]       honor the Content-Disposition header when
                                     choosing local file names
  -h [--help]                      print noti usage information and exit
  -L [--location]                  location redirect level.
  -O [--output]                    save to path. single download task.
  -T [--tries]                     set number of retries to NUMBER (0 unlimits)
  -U [--uri]                       set download uri, default value.
  -v [--version]                   print noti version and exit
  -V [--verbose]                   print noti download verbose message
)";
	wprintf(L"%s", ua);
}


int ParseArgvImplement(int Argc, wchar_t **Argv, Dcontext &dctx) {
	ParseArgv pa(Argc, Argv);
	std::vector<ParseArgv::option> opts = {
		{L"content-disposition",ParseArgv::optional_argument,L'C'}, /// --content-disposition=false -dfalse -dtrue -d=true -d=false
		{L"help",ParseArgv::no_argument,L'h'},
		{L"location",ParseArgv::required_argument,L'L'},
		{L"uri",ParseArgv::required_argument,L'U'},
		{L"output",ParseArgv::required_argument,L'O'},
		{L"tries",ParseArgv::required_argument,L'T'},
		{L"version",ParseArgv::no_argument,L'v'},
		{L"verbose",ParseArgv::no_argument,L'V'}
	};
	auto err = pa.ParseArgument(opts, [&](int ch, const wchar_t *optarg, const wchar_t *raw) {
		switch (ch) {
		case 'h':
			PrintUsage();
			exit(0);
		case 'C':
			if (optarg != nullptr) {
				auto err = Fromwchars(optarg, dctx.disposition);
				if (err.errorcode != 0) {
					return false;
				}
			}
			break;
		case 'L':
			if (optarg != nullptr) {
				auto err = Fromwchars(optarg, dctx.location);
				if (err.errorcode != 0) {
					return false;
				}
			}
			break;
		case 'U':
			if (optarg != nullptr) {
				dctx.urls.push_back(optarg);
			}
			break;
		case 'T':
			if (optarg != nullptr) {
				auto err = Fromwchars(optarg, dctx.tries);
				if (err.errorcode != 0) {
					return false;
				}
			}
			break;
		case 'O':
			if (optarg != nullptr) {
				dctx.out = optarg;
			}
			break;
		case 'v':
			wprintf(L"1.0.0\n");
			exit(0);
		case 'V':
			dctx.verbose = true;
			break;
		default:
			wprintf(L"Error Argument: %s\n", raw != nullptr ? raw : L"unknown");
			return false;
		}
		return true;
	});
	if (err.errorcode != 0) {
		if (err.errorcode == 1) {
			wprintf(L"ParseArgv: %s\n", err.message.c_str());
		}
		return 1;
	}
	for (auto &v : pa.UnresolvedArgs()) {
		dctx.urls.push_back(v.data()); /// url append
	}
	return 0;
}
// --content-disposition
int wmain(int argc, wchar_t **argv)
{
	Dcontext dctx;
	if (ParseArgvImplement(argc, argv, dctx) != 0) {
		return 1;
	}
	init_apartment();
	Uri uri(L"http://aka.ms/cppwinrt");
	printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
	return 0;
}
