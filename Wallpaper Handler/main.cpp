
#include <string>

#include <Windows.h>
#include <ShlObj.h>

inline void wallpaper_set(LPWSTR WallpaperPath);
inline void slideshow_set(LPWSTR SlideshowFolderPath);

// -------------------------------------------------------------------------
IDesktopWallpaper *pDesktopWallpaper = nullptr;

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int nCmdShow)
{
    CoInitialize(nullptr);
    CoCreateInstance(__uuidof(DesktopWallpaper), nullptr,
                  CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));

    // Getting all the necessary paths
    LPWSTR documents_path = new WCHAR[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, documents_path);

    std::wstring WindowsWallPath_mem = L"c:\\windows\\web\\wallpaper\\windows\\img0.jpg";
    std::wstring WallFolderPath_mem{ documents_path };
    WallFolderPath_mem += L"\\wallpapers\\";

    LPWSTR WindowsWallPath = const_cast<LPWSTR>(WindowsWallPath_mem.c_str());
    LPWSTR WallFolderPath = const_cast<LPWSTR>(WallFolderPath_mem.c_str());

// Things this code will do
#define ADV_SLIDE_ID 623

#define WALLPAPER_FOLDER_ID 1201
#define DEFAULT_DESKTOP_ID 1200

#define SELF_DESTRUCT 9999

// Keys that need to be held
#define HOTKEY_MODS (MOD_ALT | MOD_CONTROL)

    // Registering those hotkeys...
    RegisterHotKey(NULL, ADV_SLIDE_ID, HOTKEY_MODS, 'A');

    RegisterHotKey(NULL, WALLPAPER_FOLDER_ID, HOTKEY_MODS, 'S');
    RegisterHotKey(NULL, DEFAULT_DESKTOP_ID, HOTKEY_MODS, 'D');

    RegisterHotKey(NULL, SELF_DESTRUCT, HOTKEY_MODS, 'Q');

    // Main loop
    MSG MSG = { 0 };
    while (GetMessage(&MSG, NULL, 0, 0) != 0) {

        // We only care about hotkeys...
        if (MSG.message == WM_HOTKEY) {

            switch (MSG.wParam) {
                
                case ADV_SLIDE_ID:
                {
                    pDesktopWallpaper->AdvanceSlideshow(NULL, DSD_FORWARD);
                } break;

                case DEFAULT_DESKTOP_ID:
                {
                    wallpaper_set(WindowsWallPath);
                } break;

                case WALLPAPER_FOLDER_ID:
                {
                    slideshow_set(WallFolderPath);
                } break;

                case SELF_DESTRUCT:
                {
                    return EXIT_SUCCESS;
                } break;

            }
    
        }

    }
    
    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------
inline void wallpaper_set(LPWSTR WallpaperPath) 
{
    pDesktopWallpaper->SetWallpaper(NULL, WallpaperPath);
}

inline void slideshow_set(LPWSTR SlideshowFolderPath)
{
    IShellItem *WallpapersFolder;
    CoCreateInstance(CLSID_ShellItem, nullptr,
                        CLSCTX_ALL, IID_PPV_ARGS(&WallpapersFolder));

    SHCreateItemFromParsingName(SlideshowFolderPath, nullptr, IID_PPV_ARGS(&WallpapersFolder));

    IShellItemArray *WallpapersArray;
    SHCreateShellItemArrayFromShellItem(WallpapersFolder,
                            IID_IShellItemArray, (void**)&WallpapersArray);

    pDesktopWallpaper->SetSlideshow(WallpapersArray);
}
