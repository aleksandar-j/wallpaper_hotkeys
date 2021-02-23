
#include <string>

#include <Windows.h>
#include <ShlObj.h>

// Things this code will do
constexpr int WALLPAPER_FOLDER_ID = 10;
constexpr UINT WALLPAPER_FOLDER_KEY = 'S';
constexpr int ADVANCE_SLIDESHOW_ID = 11;
constexpr UINT ADVANCE_SLIDESHOW_KEY = 'A';

constexpr int WALLPAPER_DEFAULT_ID = 1;
constexpr UINT WALLPAPER_DEFAULT_KEY = 'D';

constexpr int SELF_DESTRUCT_ID = 0;
constexpr UINT SELF_DESTRUCT_KEY = 'Q';

// Keys that need to be held
constexpr UINT HOTKEY_MODS = MOD_ALT | MOD_CONTROL;

int CALLBACK
WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR CmdLine, _In_ int nCmdShow)
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

    // Getting all necessary paths
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

    // Registering those hotkeys...
    RegisterHotKey(NULL, ADVANCE_SLIDESHOW_ID, HOTKEY_MODS, ADVANCE_SLIDESHOW_KEY);

    RegisterHotKey(NULL, WALLPAPER_FOLDER_ID, HOTKEY_MODS, WALLPAPER_FOLDER_KEY);
    RegisterHotKey(NULL, WALLPAPER_DEFAULT_ID, HOTKEY_MODS, WALLPAPER_DEFAULT_KEY);

    RegisterHotKey(NULL, SELF_DESTRUCT_ID, HOTKEY_MODS, SELF_DESTRUCT_KEY);

    // Notify user that the program is running
    MessageBox(NULL, L"Wallpaper Hotkeys is running in the background, use CTRL+ALT+Q to exit", L"Wallpaper Hotkeys", MB_OK);

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
                    // If Documents\\Wallpaper folder does not exist, make it
                    CreateDirectory(WallpaperFolderPath, NULL);

                    IShellItem* WallpapersFolder;
                    SHCreateItemFromParsingName(WallpaperFolderPath, nullptr, IID_PPV_ARGS(&WallpapersFolder));

                    IShellItemArray* WallpapersArray;
                    SHCreateShellItemArrayFromShellItem(WallpapersFolder, IID_PPV_ARGS(&WallpapersArray));

                    pDesktopWallpaper->SetSlideshow(WallpapersArray);
                } break;

                case SELF_DESTRUCT_ID:
                {
                    // Notify the user about the exit
                    MessageBox(NULL, L"Wallpaper Hotkeys has stopped running", L"Wallpaper Hotkeys", MB_OK);
                    
                    return EXIT_SUCCESS;
                } break;

            }
    
        }

    }
    
    return EXIT_SUCCESS;
}
