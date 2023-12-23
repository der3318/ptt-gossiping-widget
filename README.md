
## 📻 Windows PTT Gossiping Widget

![platform](https://img.shields.io/badge/platform%20toolset-140+-blue.svg)
[![sdk](https://img.shields.io/badge/windows%20app%20sdk-1.4-pink.svg)](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/downloads#windows-app-sdk-14)
![flavor](https://img.shields.io/badge/flavor-win%20x64-brightgreen.svg)
![size](https://img.shields.io/badge/size-316%20KB-yellow.svg)
![license](https://img.shields.io/badge/license-MIT%20%28inherited%29-blueviolet.svg)

A Windows 11 tool that shows the trending posts (title, link and preview) of [Taiwan's PTT Gossiping bulletin board](https://www.ptt.cc/bbs/Gossiping/index.html), demonstrating the implementation of [Windows widgets experience](https://learn.microsoft.com/en-us/windows/apps/develop/widgets/widget-providers), built on top of [official SDK widget cpp sample](https://github.com/microsoft/WindowsAppSDK-Samples/tree/main/Samples/Widgets/cpp-win32-packaged):

![Demo.png](https://github.com/der3318/ptt-gossiping-widget/blob/main/Images/Demo.png)


### Setup & Use

For Windows x64 machines, prebuilt binaries are available. Enables the developer mode in Windows Settings, and make sure the device has [Micosoft Visual C++ Redistributable (x64)](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist) installed.

Find and download the files in the repository's release page.

* Double click `windowsappruntimeinstall-x64.exe` to install the core app runtime.
* Then, right click on the file `SampleWidgetProviderAppPackage_1.0.0.0_x64.cer`, and choose "Install Certificate". Select location "Local Machine" and place it into "Trusted Root Certification Authorities".
* Lastly, double click and install `SampleWidgetProviderAppPackage_1.0.0.0_x64.msixbundle`.

If everything goes well, the wiget should be pinnable/activatable in the widget panel (whose entry is at the bottom left side) by clicking the "+" (add widgets) icon.


### Cleanup & Uninstall

The MSIX app package is removable in "Settings > Apps > Installed apps". Certificate can be manually deleted in "Windows+R: certlm.msc", too. Some temporary files may be left in `C:\Users\Public\Documents\`, and it's safe to wipe them out.


### Build & Deploy

To modify, customize or build additional flavors, Visual Studio 2022 is the suggested IDE to use. Follow the [docs](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment) to setup required workloads.

Open the solution file `SampleWidgets.sln` to develop new features and changes. Click "delopy" on the project "SampleWidgetProviderAppPackage".

![Deploy.png](https://github.com/der3318/ptt-gossiping-widget/blob/main/Images/Deploy.png)

Or use "Publish > Create App Packages..." to generate a MSIX installer bundle as well as a self signed certificate.


### References

* https://docs.microsoft.com/windows/apps/windows-app-sdk/
* https://learn.microsoft.com/windows/apps/design/widgets/
* https://github.com/pnp/AdaptiveCards-Templates/tree/main/samples/yammer-liveevents
