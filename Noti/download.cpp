#include "pch.h"
#include <filesystem>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

using IAsyncHttpResultHandler =
    IAsyncOperationWithProgress<Streams::IBuffer *, HttpProgress>;

std::wstring PathFileName(const wchar_t *begin, const wchar_t *end) {
  std::wstring filename(begin, end - begin);
  while (filename.back() == '/') {
    filename.pop_back();
  }
  auto pos = filename.rfind('/');
  if (pos != std::wstring::npos) {
    return filename.substr(pos + 1);
  }
  return filename;
}

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Disposition
/*
HttpBaseProtocolFilter support HTTP2 by default.
*/
IAsyncAction NotiSignledownload(const wchar_t *url, const wchar_t *savefile) {
  try {
    std::wstring fulldir, filename;
    if (savefile != nullptr) {
      auto p = std::filesystem::path(savefile);
      auto pdir = std::filesystem::path(savefile).parent_path();
      if (!std::filesystem::exists(pdir)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(pdir, ec)) {
          throw std::filesystem::filesystem_error("create dir failed", ec);
        }
      }
      fulldir = pdir.wstring();
      if (p.has_filename()) {
        filename = p.filename().wstring();
      }
    } else {
      fulldir = std::filesystem::current_path().wstring();
    }
    printf("download url: %ls %ls\n", url, fulldir.c_str());
    HttpBaseProtocolFilter baseFilter;
    baseFilter.AllowAutoRedirect(true);
    // printf("HTTP version: %d\n",baseFilter.MaxVersion()); default enable
    // HTTP2
    HttpClient client(baseFilter);
    client.DefaultRequestHeaders().Append(L"User-Agent", L"noti/1.0");
    Uri uri(url);

    auto resp = co_await client.GetAsync(
        uri, HttpCompletionOption::ResponseHeadersRead); /// Only read header.
    resp.EnsureSuccessStatusCode();
    if (resp.Version() == HttpVersion::Http20) {
      printf("HTTP 2.0\n");
    }

    if (resp.Headers().HasKey(L"Content-Disposition")) {
      auto disp = resp.Headers().Lookup(L"Content-Disposition");
      printf("Content-Disposition: %ls\n", disp.c_str());
    }

    if (filename.empty()) {
      filename = PathFileName(uri.Path().c_str(),
                              uri.Path().c_str() + uri.Path().size());
      printf("filename: %ls\n", filename.c_str());
    }

    uint64_t blen = 0;
    if (resp.Content().TryComputeLength(blen)) {
    }
    /// If cannot download some dir,
    auto folder = co_await StorageFolder::GetFolderFromPathAsync(
        fulldir); /// Must fullpath
    auto file = co_await folder.CreateFileAsync(
        filename,
        CreationCollisionOption::ReplaceExisting); /// replace existing
    auto stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
    // HttpProgress
    auto pbh = AsyncOperationProgressHandler<uint64_t, uint64_t>(
        [&](IAsyncOperationWithProgress<uint64_t, uint64_t>, uint64_t pb) {
          wprintf(L"download %llu\n", pb);
        });
    auto aspb = resp.Content().WriteToStreamAsync(stream);
    aspb.Progress(pbh);
    auto result = co_await aspb;
    wprintf(L"total %llu\n", result);
  } catch (const hresult_error &e) {
    printf("download error: %ls\n", e.message().c_str());
  } catch (const std::exception &e) {
    printf("download url: %s\n", e.what());
  }
}

IAsyncAction Notidownload(const Dcontext &dctx) {
  try {
    co_await NotiSignledownload(dctx.urls[0].c_str(), nullptr);
  } catch (const hresult_error &e) {
    printf("open dir: %ls\n", e.message().c_str());
  } catch (const std::exception &e) {
    printf("open dir: %s\n", e.what());
  }
}