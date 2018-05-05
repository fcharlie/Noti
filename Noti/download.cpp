#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

//https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Disposition
/*
HttpBaseProtocolFilter support HTTP2 by default.
*/
IAsyncAction Notidownload(const wchar_t *url,const wchar_t *savefile){
	try{
		if(savefile==nullptr){
			////
		}
		printf("download url: %ls %ls\n", url,savefile);
		HttpBaseProtocolFilter baseFilter;
		baseFilter.AllowAutoRedirect(true);
		//printf("HTTP version: %d\n",baseFilter.MaxVersion()); default enable HTTP2
		HttpClient client(baseFilter);
		client.DefaultRequestHeaders().Append(L"User-Agent", L"dolt/1.0");
		Uri uri(url);
		printf("request...  %ls\n",client.DefaultRequestHeaders().UserAgent().ToString().c_str());
		auto resp=co_await client.GetAsync(uri);
		resp.EnsureSuccessStatusCode();
		if(resp.Headers().HasKey(L"Content-Disposition")){
			auto disp=resp.Headers().Lookup(L"Content-Disposition");
			printf("Content-Disposition: %ls\n", disp.c_str());
		}
		auto body = co_await resp.Content().ReadAsStringAsync();
		printf("%ls\n", body.c_str());
	 }catch(std::exception &e){
		printf("download url: %s\n", e.what());
	 }
}