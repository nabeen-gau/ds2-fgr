#include <dwmapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Link the DWM API for the dark title bar
#pragma comment(lib, "dwmapi.lib")

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define BTN_HEIGHT 30
#define MARGIN_Y 10

HWND hSeedEdit;
HWND hBeginBtn;
HWND hItemCheck;
HWND hStatusLabel;
HBRUSH hDarkBrush;

// will be linked during compilation
int start_randomization(BOOL load_item_rando, int seed); // returns 0 on success 1 on failure
void init_rando();
void deinit_rando();

DWORD WINAPI RandomizeTask(LPVOID lpParam) {
	HWND hwnd = (HWND)lpParam;

	char seedStr[16];
    GetWindowText(hSeedEdit, seedStr, sizeof(seedStr));
    int seed = atoi(seedStr);
	BOOL load_item_rando = (SendMessage(hItemCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
	if (load_item_rando) {
		DWORD fileAttr = GetFileAttributes("..\\randomizer\\DS2SRandomizer.exe");
        
        if (fileAttr == INVALID_FILE_ATTRIBUTES) {
            // File is missing! Show dialog, unlock button, and abort thread.
            MessageBox(hwnd, 
                       "Item rando is not installed. Ensure DS2SRandomizer.exe is in the (GameFolder)/randomizer/ folder. Uncheck the load item randomizer button to proceed.", 
                       "Missing File Error", 
                       MB_ICONERROR | MB_OK);
                       
			SetWindowText(hStatusLabel, "Status: Ready");
            EnableWindow(hBeginBtn, TRUE);
            return 1; // Exit thread early
        }
	}
	if (start_randomization(load_item_rando, seed) == 0) {
		SetWindowText(hStatusLabel, "Status: Randomization Complete!");
	} else {
		SetWindowText(hStatusLabel, "Status: Randomization Failed!");
	}
	EnableWindow(hBeginBtn, TRUE);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 1. Force dark mode on the title bar (Windows 10/11)
            BOOL dark = TRUE;
            DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));

			int currentY = (WINDOW_HEIGHT / 2) - 100;
			int chkWidth = 275;
            int chkX = (WINDOW_WIDTH - chkWidth) / 2;

            // 2. Checkbox for item randomizer
            hItemCheck = CreateWindow("BUTTON", "Include Item and Enemy Randomizer", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                         10, 10, chkWidth, BTN_HEIGHT, hwnd, NULL, NULL, NULL);

			currentY += BTN_HEIGHT + MARGIN_Y;

			int editWidth = 150, randBtnWidth = 100, gap = 20;
            int totalSeedWidth = editWidth + gap + randBtnWidth;
            int seedX = (WINDOW_WIDTH - totalSeedWidth) / 2;
            
            // 3. Text box for seed and Randomize Seed button
            hSeedEdit = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 
                                     seedX, currentY, editWidth, BTN_HEIGHT, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "New Seed", WS_CHILD | WS_VISIBLE, 
                         seedX + editWidth + gap, currentY, randBtnWidth, BTN_HEIGHT, hwnd, (HMENU)1, NULL, NULL);

			currentY += BTN_HEIGHT + (MARGIN_Y * 2);

			int beginWidth = 220;
            int beginX = (WINDOW_WIDTH - beginWidth) / 2;
            
            // 4. Begin randomize button
            hBeginBtn = CreateWindow("BUTTON", "Start Randomization", WS_CHILD | WS_VISIBLE, 
                         beginX, currentY, beginWidth, 45, hwnd, (HMENU)2, NULL, NULL);

			hStatusLabel = CreateWindow("STATIC", "Status: Ready", WS_CHILD | WS_VISIBLE | SS_CENTER, 
                                    0, WINDOW_HEIGHT - 80, WINDOW_WIDTH, 20, hwnd, NULL, NULL, NULL);

			char savedSeed[16];
            GetPrivateProfileString("Config", "Seed", "666666", savedSeed, sizeof(savedSeed), ".\\settings.ini");
            SetWindowText(hSeedEdit, savedSeed);

            int savedCheck = GetPrivateProfileInt("Config", "ItemRando", 0, ".\\settings.ini");
            if (savedCheck == 1) {
                SendMessage(hItemCheck, BM_SETCHECK, BST_CHECKED, 0);
            }

            break;
		}
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { 
                // Randomize seed button clicked
                char seedStr[16];
				int bigRand = (rand() << 15) | rand();
                sprintf(seedStr, "%d", bigRand % 999999999);
                SetWindowText(hSeedEdit, seedStr);
            } else if (LOWORD(wParam) == 2) { 
				EnableWindow(hBeginBtn, FALSE);
				SetWindowText(hStatusLabel, "Status: Randomizing...");
                // Begin randomize button clicked
                CreateThread(NULL, 0, RandomizeTask, (LPVOID)hwnd, 0, NULL);
            }
            break;

        case WM_CTLCOLORSTATIC:
            // Minimalist dark theme formatting for the checkbox text background
            SetTextColor((HDC)wParam, RGB(255, 255, 255));
            SetBkColor((HDC)wParam, RGB(30, 30, 30));
            return (LRESULT)hDarkBrush;

        case WM_DESTROY: {
			char currentSeed[16];
            GetWindowText(hSeedEdit, currentSeed, sizeof(currentSeed));
            WritePrivateProfileString("Config", "Seed", currentSeed, ".\\settings.ini");

            BOOL isChecked = (SendMessage(hItemCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
            WritePrivateProfileString("Config", "ItemRando", isChecked ? "1" : "0", ".\\settings.ini");
            PostQuitMessage(0);
		 } break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main() {
    // Seed the random number generator
    srand((unsigned)time(NULL));

	init_rando();

    // Create the dark background brush once
    hDarkBrush = CreateSolidBrush(RGB(30, 30, 30));

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = hDarkBrush;
    wc.lpszClassName = "RandApp";
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
    RegisterClass(&wc);

    // Create a fixed-size, non-resizable window
    HWND hwnd = CreateWindow("RandApp", "DS2S Fog Gate Randomizer by nabeen", 
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, 
                             100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, wc.hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hDarkBrush);
	deinit_rando();
    return 0;
}
