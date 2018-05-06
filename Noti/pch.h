//
// pch.h
// Precompiled header for commonly included header files
//

#pragma once

#include <vector>
#include <string>

#include <winrt/Windows.Foundation.h>
//#include <winrt/Windows.Networking.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Storage.Streams.h>

struct Dcontext{
	std::vector<std::wstring> urls;
	std::wstring out;
	int tries{ 5 };
	int location{ 3 };
	bool disposition{ true };
	bool verbose{ false };
};

using winrt::Windows::Foundation::IAsyncAction;

IAsyncAction Notidownload(const Dcontext &dctx);