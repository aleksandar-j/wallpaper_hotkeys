
#include <string>

#include <Windows.h>
#include <ShlObj.h>

// Things this code will do
#define WALLPAPER_FOLDER_ID 10
#define WALLPAPER_FOLDER_KEY 'S'
#define ADVANCE_SLIDESHOW_ID 11
#define ADVANCE_SLIDESHOW_KEY 'A'

#define WALLPAPER_DEFAULT_ID 1
#define WALLPAPER_DEFAULT_KEY 'D'

#define SELF_DESTRUCT_ID 0
#define SELF_DESTRUCT_KEY 'Q'

// Keys that need to be held
#define HOTKEY_MODS (MOD_ALT | MOD_CONTROL)

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int nCmdShow)
{
    HRESULT COMInitializeRes = CoInitializeEx(nullptr, 0);
    if (COMInitializeRes != S_OK) {
        return EXIT_FAILURE;
    }

    IDesktopWallpaper* pDesktopWallpaper;
    HRESULT DesktopWallpaperRes = CoCreateInstance(CLSID_DesktopWallpaper, nullptr,
                                                CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
    if (DesktopWallpaperRes != S_OK) {
        return EXIT_FAILURE;
    }

    // Getting all the necessary paths
    LPWSTR UserDocumentsPath = new WCHAR[MAX_PATH];
    SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &UserDocumentsPath);
    LPWSTR WindowsPath = new WCHAR[MAX_PATH];
    SHGetKnownFolderPath(FOLDERID_Windows, KF_FLAG_DEFAULT, NULL, &WindowsPath);

    std::wstring WallpaperDefaultPath_mem{ WindowsPath };
    WallpaperDefaultPath_mem += L"\\web\\wallpaper\\windows\\img0.jpg";
    std::wstring WallpaperFolderPath_mem{ UserDocumentsPath };
    WallpaperFolderPath_mem += L"\\Wallpapers\\";

    LPWSTR WallpaperDefaultPath = const_cast<LPWSTR>(WallpaperDefaultPath_mem.c_str());
    LPWSTR WallpaperFolderPath = const_cast<LPWSTR>(WallpaperFolderPath_mem.c_str());

    // If Documents\\Wallpaper folder does not exist, make it
    CreateDirectory(WallpaperFolderPath, NULL);

    // Registering those hotkeys...
    RegisterHotKey(NULL, ADVANCE_SLIDESHOW_ID, HOTKEY_MODS, ADVANCE_SLIDESHOW_KEY);

    RegisterHotKey(NULL, WALLPAPER_FOLDER_ID, HOTKEY_MODS, WALLPAPER_FOLDER_KEY);
    RegisterHotKey(NULL, WALLPAPER_DEFAULT_ID, HOTKEY_MODS, WALLPAPER_DEFAULT_KEY);

    RegisterHotKey(NULL, SELF_DESTRUCT_ID, HOTKEY_MODS, SELF_DESTRUCT_KEY);

    // Main loop
    MSG MSG;
    while (GetMessage(&MSG, NULL, 0, 0) != 0) {

        // We only care about hotkeys...
        if (MSG.message == WM_HOTKEY) {

            switch (MSG.wParam) {
                
                case ADVANCE_SLIDESHOW_ID:
                {
                    pDesktopWallpaper->AdvanceSlideshow(NULL, DSD_FORWARD);
                } break;

                case WALLPAPER_DEFAULT_ID:
                {
                    pDesktopWallpaper->SetWallpaper(NULL, WallpaperDefaultPath);
                } break;

                case WALLPAPER_FOLDER_ID:
                {
                    IShellItem* WallpapersFolder;
                    SHCreateItemFromParsingName(WallpaperFolderPath, nullptr, IID_PPV_ARGS(&WallpapersFolder));

                    IShellItemArray* WallpapersArray;
                    SHCreateShellItemArrayFromShellItem(WallpapersFolder, IID_PPV_ARGS(&WallpapersArray));

                    pDesktopWallpaper->SetSlideshow(WallpapersArray);
                } break;

                case SELF_DESTRUCT_ID:
                {
                    return EXIT_SUCCESS;
                } break;

            }
    
        }

    }
    
    return EXIT_SUCCESS;
}
