
#include <Windows.h>
#include <ShObjIdl.h>

inline void wallpaper_set(LPWSTR WallpaperPath);
inline void slideshow_set(LPWSTR SlideshowFolderPath);

inline void move_advanced_slide(int VK_LEFT_RIGHT);

BOOL ControlHandler(DWORD ControlEvent);

// -------------------------------------------------------------------------
IDesktopWallpaper *pDesktopWallpaper = nullptr;

size_t advanced_slide_num = 0;

WCHAR WindowsWallPath[] = L"c:\\windows\\web\\wallpaper\\windows\\img0.jpg";
WCHAR MyWallFolderPath[] = L"c:\\users\\aleksandar\\documents\\wallpapers\\";
WCHAR MyOriginalFolderPath[] = L"c:\\users\\aleksandar\\documents\\wallpapers\\originals\\";

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CmdLine, int nCmdShow)
{
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ControlHandler, TRUE);

    CoInitialize(nullptr);
    CoCreateInstance(__uuidof(DesktopWallpaper), nullptr,
                  CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));

// Things this code will do
#define ADV_SLIDE_ID 623

#define RIGHT_SLIDE_ID 624
#define LEFT_SLIDE_ID 625

#define WALLPAPER_FOLDER_ID 1201
#define ORIGINALS_FOLDER_ID 1202
#define DEFAULT_DESKTOP_ID 1200

#define SELF_DESTRUCT 9999

// Keys that need to be held
#define HOTKEY_MODS (MOD_ALT | MOD_CONTROL)

    // Registering those hotkeys...
    RegisterHotKey(NULL, ADV_SLIDE_ID, HOTKEY_MODS, 'A');
    
    RegisterHotKey(NULL, RIGHT_SLIDE_ID, HOTKEY_MODS, VK_RIGHT);
    RegisterHotKey(NULL, LEFT_SLIDE_ID, HOTKEY_MODS, VK_LEFT);

    RegisterHotKey(NULL, WALLPAPER_FOLDER_ID, HOTKEY_MODS, 'S');
    RegisterHotKey(NULL, DEFAULT_DESKTOP_ID, HOTKEY_MODS, 'D');
    RegisterHotKey(NULL, ORIGINALS_FOLDER_ID, HOTKEY_MODS, 'O');

    RegisterHotKey(NULL, SELF_DESTRUCT, HOTKEY_MODS, 'Q');

    // Main loop
    MSG MSG = { 0 };
    while (GetMessage(&MSG, NULL, 0, 0) != 0) {

        // We only care about this...
        if (MSG.message == WM_HOTKEY) {

            switch (MSG.wParam) {
                
                case ADV_SLIDE_ID:
                {
                    pDesktopWallpaper->AdvanceSlideshow(NULL, DSD_FORWARD);
                } break;

                case RIGHT_SLIDE_ID:
                {
                    move_advanced_slide(VK_RIGHT);
                } break;
                
                case LEFT_SLIDE_ID:
                {
                    move_advanced_slide(VK_LEFT);
                } break;

                case DEFAULT_DESKTOP_ID:
                {
                    wallpaper_set(WindowsWallPath);
                } break;

                case ORIGINALS_FOLDER_ID: 
                {
                    slideshow_set(MyOriginalFolderPath);
                } break;

                case WALLPAPER_FOLDER_ID:
                {
                    slideshow_set(MyWallFolderPath);
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

BOOL ControlHandler(DWORD ControlEvent) 
{
    switch (ControlEvent) {
        // User wants to shutdown
        case CTRL_SHUTDOWN_EVENT:
        {
            wallpaper_set(WindowsWallPath);
            return FALSE;
        } break;

        // User wants to logoff
        case CTRL_LOGOFF_EVENT: 
        {
            wallpaper_set(WindowsWallPath);
            return FALSE;
        } break;
    }

    return FALSE;
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

inline void move_advanced_slide(int VK_LEFT_RIGHT) 
{

    if (VK_LEFT_RIGHT == VK_RIGHT) {
        
    } else if (VK_LEFT_RIGHT == VK_LEFT) {
        
    }

}
