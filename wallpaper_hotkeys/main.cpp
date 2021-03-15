
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

    // Get all base paths we need
    LPWSTR CurrentDirectoryPath = new WCHAR[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, CurrentDirectoryPath);
    LPWSTR UserDocumentsPath = new WCHAR[MAX_PATH];
    SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &UserDocumentsPath);
    LPWSTR WindowsPath = new WCHAR[MAX_PATH];
    SHGetKnownFolderPath(FOLDERID_Windows, KF_FLAG_DEFAULT, NULL, &WindowsPath);
    
    // Create default values for ini variables
    LPWSTR DefaultWallpaperDefaultPath = new WCHAR[MAX_PATH];
    wcscpy_s(DefaultWallpaperDefaultPath, MAX_PATH, WindowsPath);
    wcscat_s(DefaultWallpaperDefaultPath, MAX_PATH, L"\\web\\wallpaper\\windows\\img0.jpg");
    
    LPWSTR DefaultWallpaperSlideshowFolderPath = new WCHAR[MAX_PATH];
    wcscpy_s(DefaultWallpaperSlideshowFolderPath, MAX_PATH, UserDocumentsPath);
    wcscat_s(DefaultWallpaperSlideshowFolderPath, MAX_PATH, L"\\Wallpapers\\");

    // Set up ini path and write default values if ini not present
    LPWSTR INIPath = new WCHAR[MAX_PATH];
    wcscpy_s(INIPath, MAX_PATH, CurrentDirectoryPath);
    wcscat_s(INIPath, MAX_PATH, L"\\wallpaper_hotkeys.ini");

    DWORD dwAttrib = GetFileAttributes(INIPath);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES) {
        WritePrivateProfileString(L"settings", L"ShowMessageBoxEnter", L"1", INIPath);
        WritePrivateProfileString(L"settings", L"ShowMessageBoxExit", L"1", INIPath);
        WritePrivateProfileString(L"settings", L"WallpaperDefaultPath", DefaultWallpaperDefaultPath, INIPath);
        WritePrivateProfileString(L"settings", L"WallpaperSlideshowFolderPath", DefaultWallpaperSlideshowFolderPath, INIPath);
    }

    // Read variables from ini file, write default values if individual values are not present
    LPWSTR ShowMessageBoxEnter = new WCHAR[8];
    GetPrivateProfileString(L"settings", L"ShowMessageBoxEnter", NULL, ShowMessageBoxEnter, 8, INIPath);
    LPWSTR ShowMessageBoxExit = new WCHAR[8];
    GetPrivateProfileString(L"settings", L"ShowMessageBoxExit", NULL, ShowMessageBoxExit, 8, INIPath);

    LPWSTR WallpaperSlideshowFolderPath = new WCHAR[MAX_PATH];
    GetPrivateProfileString(L"settings", L"WallpaperSlideshowFolderPath", NULL, WallpaperSlideshowFolderPath, MAX_PATH, INIPath);
    if (WallpaperSlideshowFolderPath[0] == 0) {
        WritePrivateProfileString(L"settings", L"WallpaperSlideshowFolderPath", DefaultWallpaperSlideshowFolderPath, INIPath);
        wcscpy_s(WallpaperSlideshowFolderPath, MAX_PATH, DefaultWallpaperSlideshowFolderPath);
    }
    LPWSTR WallpaperDefaultPath = new WCHAR[MAX_PATH];
    GetPrivateProfileString(L"settings", L"WallpaperDefaultPath", NULL, WallpaperDefaultPath, MAX_PATH, INIPath);
    if (WallpaperDefaultPath[0] == 0) {
        WritePrivateProfileString(L"settings", L"WallpaperDefaultPath", DefaultWallpaperDefaultPath, INIPath);
        wcscpy_s(WallpaperDefaultPath, MAX_PATH, DefaultWallpaperDefaultPath);
    }

    // Registering those hotkeys...
    RegisterHotKey(NULL, ADVANCE_SLIDESHOW_ID, HOTKEY_MODS, ADVANCE_SLIDESHOW_KEY);

    RegisterHotKey(NULL, WALLPAPER_FOLDER_ID, HOTKEY_MODS, WALLPAPER_FOLDER_KEY);
    RegisterHotKey(NULL, WALLPAPER_DEFAULT_ID, HOTKEY_MODS, WALLPAPER_DEFAULT_KEY);

    RegisterHotKey(NULL, SELF_DESTRUCT_ID, HOTKEY_MODS, SELF_DESTRUCT_KEY);

    // Notify user that the program is running
    if (ShowMessageBoxEnter[0] == L'1') {
        MessageBox(NULL, L"Wallpaper Hotkeys is running in the background, use CTRL+ALT+Q to exit, edit ini file to remove this message", L"Wallpaper Hotkeys", MB_OK);
    }

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
                    CreateDirectory(WallpaperSlideshowFolderPath, NULL);

                    IShellItem* WallpapersFolder;
                    SHCreateItemFromParsingName(WallpaperSlideshowFolderPath, nullptr, IID_PPV_ARGS(&WallpapersFolder));

                    IShellItemArray* WallpapersArray;
                    SHCreateShellItemArrayFromShellItem(WallpapersFolder, IID_PPV_ARGS(&WallpapersArray));

                    pDesktopWallpaper->SetSlideshow(WallpapersArray);
                } break;

                case SELF_DESTRUCT_ID:
                {
                    // Notify the user about the exit
                    if (ShowMessageBoxExit[0] == L'1') {
                        MessageBox(NULL, L"Wallpaper Hotkeys has stopped running", L"Wallpaper Hotkeys", MB_OK);
                    }

                    return EXIT_SUCCESS;
                } break;

            }
    
        }

    }
    
    return EXIT_SUCCESS;
}
