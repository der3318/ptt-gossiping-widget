#pragma once
#include "WidgetImplBase.h"

class PttGossipingPost
{
public:
    std::wstring vote;
    std::wstring link;
    std::wstring name;
    std::wstring image;
    static bool Compare(const PttGossipingPost& lhs, const PttGossipingPost& rhs)
    {
        return (lhs.image.find(L"images.ptt.cc") == std::wstring::npos) && (rhs.image.find(L"images.ptt.cc") != std::wstring::npos);
    }
};

class PttGossipingWidget : public WidgetImplBase
{
public:
    // Initalize a widget with saved state
    PttGossipingWidget(winrt::hstring const& id, winrt::hstring const& state);

    void Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext) override;
    void OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs) override;
    winrt::hstring GetTemplateForWidget() override;
    winrt::hstring GetDataForWidget() override;
private:
    winrt::hstring GetDefaultTemplate();
    void RefreshPosts();
    std::vector<PttGossipingPost> _posts;
    std::optional<long long> _lastRefreshTimeInSec;
};