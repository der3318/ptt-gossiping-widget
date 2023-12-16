#include "pch.h"
#include "PttGossipingWidgetImpl.h"

#include <fstream>
#include <regex>

static std::string ConvertUTF16LE(const UINT codePage, const std::wstring& strUTF16LE)
{
    if (std::empty(strUTF16LE))
    {
        return std::string();
    }

    int sizeNeeded = WideCharToMultiByte(codePage, 0, std::data(strUTF16LE), (int)std::size(strUTF16LE), NULL, 0, NULL, NULL);
    std::string str(sizeNeeded, 0);
    (void)WideCharToMultiByte(codePage, 0, std::data(strUTF16LE), (int)std::size(strUTF16LE), &(str[0]), sizeNeeded, NULL, NULL);
    return str;
}

static std::wstring ComposeUTF16LE(const UINT codePage, const std::string& str)
{
    if (std::empty(str))
    {
        return std::wstring();
    }

    int sizeNeeded = MultiByteToWideChar(codePage, 0, std::data(str), (int)std::size(str), NULL, 0);
    std::wstring strUTF16LE(sizeNeeded, 0);
    (void)MultiByteToWideChar(codePage, 0, std::data(str), (int)std::size(str), &(strUTF16LE[0]), sizeNeeded);
    return strUTF16LE;
}

static std::wstring GetPttPageContent(const std::wstring& path)
{
    std::string response = "C:\\Users\\Public\\Documents\\PttRsp.txt";

    std::string command = "curl --cookie \"over18=1\" https://www.ptt.cc" + ConvertUTF16LE(CP_ACP, path) + " -o " + response;
    STARTUPINFOA startupInfo = {};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo = {};
    if (!CreateProcessA(NULL, &(command[0]), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
    {
        return std::wstring();
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    std::ifstream stream(response, std::ios::binary);
    stream.seekg(0, std::ios::end);
    std::string rspUTF8(stream.tellg(), 0);
    stream.seekg(0);
    stream.read(&(rspUTF8[0]), std::size(rspUTF8));
    stream.close();
    return ComposeUTF16LE(CP_UTF8, rspUTF8);
}

static std::wstring RegexSearch(const std::wstring& str, const std::wstring& re)
{
    std::wsmatch match = {};
    if (std::regex_search(str, match, std::wregex(re)))
    {
        return match[1].str();
    }
    return std::wstring();
}

void PttGossipingWidget::RefreshPosts()
{
    _posts.clear();

    std::wstring rsp = GetPttPageContent(L"/bbs/Gossiping/search?q=recommend%3A50");
    std::wstring::size_type start = rsp.find(L"\"nrec\"");
    while (start != std::wstring::npos)
    {
        std::wstring::size_type end = rsp.find(L"</a>", start + 1);
        std::wstring content = rsp.substr(start, end - start);
        PttGossipingPost post(RegexSearch(content, L">([^>]*?)</span>"), RegexSearch(content, L"href=\"(.*?)\">"), RegexSearch(content, L".html\">(.*)"), L"https://images.ptt.cc/ptt.jpg");
        std::wstring image = RegexSearch(GetPttPageContent(post.link), L"src=\"https://cache.ptt.cc/c/https/([^>]*?)\\?");
        if (!std::empty(image))
        {
            post.image = L"https://" + image;
        }
        _posts.emplace_back(std::move(post));
        start = rsp.find(L"\"nrec\"", end + 1);
    }
    std::sort(std::begin(_posts), std::end(_posts), PttGossipingPost::Compare);

    // mock data for debugging
    _posts.emplace_back(L"爆", L"/bbs/Gossiping/index.html", L"[公告] Sample Post #1", L"https://images.ptt.cc/ptt.jpg");
    _posts.emplace_back(L"10", L"/bbs/Gossiping/index.html", L"[問卦] Sample Post #2", L"https://images.ptt.cc/ptt.jpg");
    _posts.emplace_back(L"20", L"/bbs/Gossiping/index.html", L"Re: [問卦] Sample Post #2", L"https://images.ptt.cc/ptt.jpg");
    _posts.emplace_back(L"30", L"/bbs/Gossiping/index.html", L"Fw: [問卦] Sample Post #2", L"https://images.ptt.cc/ptt.jpg");
}

PttGossipingWidget::PttGossipingWidget(winrt::hstring const& id, winrt::hstring const& state) : WidgetImplBase(id, state)
{
    State(L"PttGossipingWidget");
    RefreshPosts();
}

void PttGossipingWidget::Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext)
{
    long long timeInSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (!_lastRefreshTimeInSec.has_value() || (timeInSec > (_lastRefreshTimeInSec.value() + 60LL)))
    {
        _lastRefreshTimeInSec = timeInSec;
        RefreshPosts();
        auto widgetData = GetDataForWidget();
        winrt::WidgetUpdateRequestOptions updateOptions{ Id() };
        updateOptions.Data(widgetData);
        updateOptions.CustomState(State());
        winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);
    }
}

// This function will be invoked when WidgetContext has changed.
void PttGossipingWidget::OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs)
{
    // This function is raised when
    // (Optional) There a several things that can be done here:
    // 1. If you need to adjust template/data for the new context (i.e. widget size has chaned) - you can do it here.
    // 2. Log this call for telemetry to monitor what size users choose the most.
}

winrt::hstring PttGossipingWidget::GetDefaultTemplate()
{
    auto uri = winrt::Uri(L"ms-appx:///Templates/PttGossipingWidgetTemplate.json");
    auto storageFile = winrt::StorageFile::GetFileFromApplicationUriAsync(uri).get();
    return winrt::FileIO::ReadTextAsync(storageFile).get();
}

winrt::hstring PttGossipingWidget::GetTemplateForWidget()
{
    // This widget has the same template for all the sizes/themes so we load it only once.
    static winrt::hstring widgetTemplate = GetDefaultTemplate();
    return widgetTemplate;
}

winrt::hstring PttGossipingWidget::GetDataForWidget()
{
    std::wstring format(LR"(
    {
        "posts": [
            {
                "status": "$status1",
                "link" : "$link1",
                "description" : "$description1",
                "image" : "$image1"
            },
            {
                "status": "$status2",
                "link" : "$link2",
                "description" : "$description2",
                "image" : "$image2"
            },
            {
                "status": "$status3",
                "link" : "$link3",
                "description" : "$description3",
                "image" : "$image3"
            },
            {
                "status": "$status4",
                "link" : "$link4",
                "description" : "$description4",
                "image" : "$image4"
            }
        ]
    })");
    for (auto i = 0; i < std::size(_posts); i++)
    {
        format = std::regex_replace(format, std::wregex(L"\\$status" + std::to_wstring(i + 1)), L"📈 推×" + _posts[i].vote);
        format = std::regex_replace(format, std::wregex(L"\\$link" + std::to_wstring(i + 1)), L"https://www.ptt.cc" + _posts[i].link);
        format = std::regex_replace(format, std::wregex(L"\\$description" + std::to_wstring(i + 1)), _posts[i].name);
        format = std::regex_replace(format, std::wregex(L"\\$image" + std::to_wstring(i + 1)), _posts[i].image);
    }
    return format.c_str();
}