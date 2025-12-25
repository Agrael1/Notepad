#include "notepad.hpp"
struct WGL_WindowData { HDC hDC; };
static int64_t fwop = 0;
// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;
static int              g_Width;
static int              g_Height;
ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.02f, 0.00f);

// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);

//
void ResetDeviceWGL();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int main(int argc, char** argv) {
    ImDrawListSplitter JEApp;
    std::wstring WindowTitle = L"Notepad";
    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, WindowTitle.data(), nullptr };
    wc.hIcon = LoadIconW(wc.hInstance, MAKEINTRESOURCEW(102));
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, WindowTitle.data(), WS_OVERLAPPEDWINDOW | WS_EX_TOOLWINDOW | WS_EX_NOPARENTNOTIFY, 100, 80, 800, 600, nullptr, nullptr, wc.hInstance, nullptr);
    ::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
    if (!CreateDeviceWGL(hwnd, &g_MainWindow))
    {
        CleanupDeviceWGL(hwnd, &g_MainWindow);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    wglMakeCurrent(g_MainWindow.hDC, g_hRC);
    //
    ::AnimateWindow(hwnd, 100, AW_BLEND);
    //
    ::UpdateWindow(hwnd);

    //
    IMGUI_CHECKVERSION();
    //
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    int fTextSize = 20;
    //ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_InitForOpenGL(hwnd);
    if (!ImGui_ImplOpenGL3_Init()) {
       debug_log("[Fatal Error] OGL3 NOT SUPPORT ON YOU GPU");
    }
    else {

       debug_log("(OGL3)::InitToWIN32 OK!");

    }
   ImFont* fontCurrent =  io.Fonts->AddFontFromFileTTF("SFMono-Bold.ttf", 20.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());//
   while (!render_flags)
   {
       if (argv[1]) {
           pNotepad->sFileName = argv[1];
           pNotepad->ToolsUI.OpenFile(pNotepad->sFileName,&pNotepad->sBufferText);
       }
       MSG msg;
       while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
       {
           ::TranslateMessage(&msg);
           ::DispatchMessage(&msg);
           if (msg.message == WM_QUIT)
               exit(0);
           //  done = true;
           if (msg.message == WM_DESTROY)
               exit(0);
       }
       // Start the Dear ImGui frame//
       ImGui_ImplOpenGL3_NewFrame();
       ImGui_ImplWin32_NewFrame();
       ImGui::NewFrame();
       pNotepad->styleImGui();
       ImGui::Begin("Hello", NULL, ImGuiWindowFlag);
       //
       ImGui::SetNextWindowContentSize({ 300,100 });
       if (ImGui::BeginPopup("#SaveFile", ImGuiWindowFlags_NoTitleBar)) {
           if(!pNotepad->bIsFileSaved)
           {
               ImGui::SetCursorPosX(20); ImGui::Text("Save text to File");
               ImGui::Separator();
               ImGui::SetCursorPosX(20);
               ImGui::InputText("FileName", &pNotepad->sFileName);
               if (pNotepad->sFileName.empty()) {
                   ImGui::SetCursorPosX(20); ImGui::Text("FileName  is Empty!");
               }
               if (pNotepad->sBufferText.empty()) {
                   ImGui::SetCursorPosX(20); ImGui::Text("Data: file empty!");
               }
               if (!pNotepad->sBufferText.empty() && !pNotepad->sFileName.empty()) {
                   ImGui::SetCursorPosX(20); if (ImGui::Button("Save", { 100,30 })) {
                       pNotepad->ToolsUI.SaveFile(pNotepad->sFileName, pNotepad->sBufferText);
                       ImGui::CloseCurrentPopup();
                   }
               }
           }
           if (pNotepad->bIsFileSaved)
           {
               static float fTimer = 0;
               fTimer += 200 * io.DeltaTime;
               if (fTimer >= 40) {
                   ImGui::CloseCurrentPopup();
                   fTimer = 0;
                   pNotepad->bIsFileSaved = false;
               }
               pNotepad->ToolsUI.textCenter(std::format("file: {} Saved!!", pNotepad->sFileName).c_str(), { 300,100 });
               ImGui::Text(std::format("file: {} Saved!!", pNotepad->sFileName).c_str());
           }
           ImGui::EndPopup();
       }
       if (ImGui::BeginPopup("#NewFile", ImGuiWindowFlags_NoTitleBar)) {
           ImGui::SetCursorPosX(20); ImGui::Text("Create New File");
           ImGui::Separator();
           ImGui::SetCursorPosX(20);
           ImGui::InputText("FileName", &pNotepad->sFileName);
           if (pNotepad->sFileName.empty()) {
               ImGui::SetCursorPosX(20); ImGui::Text("FileName  is Empty!");
           }
           ImGui::SetCursorPosX(20); if (ImGui::Button("Create", { 100,30 })) {
           ImGui::CloseCurrentPopup();
           }
           ImGui::SameLine();
           if (ImGui::Button("Open File", { 100,30 })) {
               File::data_ptr->OpenFileDialog(pNotepad->sFileName);
               pNotepad->ToolsUI.OpenFile(pNotepad->sFileName, &pNotepad->sBufferText);
               ImGui::CloseCurrentPopup();
           }
           ImGui::EndPopup();
       }

       //
       pNotepad->commands();
       pNotepad->NUI.WellcomeFrame(fontCurrent);
       pNotepad->ScanFilesUI();
       if (i64ItemIdClicked == 1 && i64MenuSelected == 1) {
           ImGui::OpenPopup("FileEdit");
       }

       ImGui::BeginMainMenuBar();
       
        ImGui::SetCursorPosX(20); ImGui::Text("Help - [F1] file: %s",pNotepad->sFileName.c_str()); ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetItemRectMax().x + 20); ImGui::Text("size: %lld", pNotepad->sBufferText.size());
     
       ImGui::EndMainMenuBar();

       pNotepad->NUI.fileEdit(fontCurrent);
      
       ImVec2 MenuBarSize = ImGui::GetWindowSize();
       ImGui::SetWindowSize(fWindowSize);
       ImGui::SetWindowPos({ 0,26 });
       //pNotepad->ToolsUI.ItemCenterX((fWindowSize.x), fWindowSize); ImGui::SetCursorPosY(-4);
       ImGui::SetCursorPosY(-4); ImGui::SetCursorPosX(-1);
       ImGui::PushStyleColor(ImGuiCol_Border, pNotepad->rgba(0, 0, 0, 0));
       ImGui::InputTextMultiline("text", &pNotepad->sBufferText, { (fWindowSize.x),(fWindowSize.y - 30) }, ImGuiInputTextFlags_ElideLeft);
       ImGui::PopStyleColor();
       ImGui::TextColored(pNotepad->rgba({100, 110, 246, 255}), "size: %lld", pNotepad->sBufferText.size());
        ImGui::End();
        glViewport(0, 0, g_Width,g_Height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present
        ::SwapBuffers(g_MainWindow.hDC);

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceWGL(hwnd, &g_MainWindow);
    wglDeleteContext(g_hRC);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 3;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cAlphaBits = 8;
    pfd.cColorBits = 32;
    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = ImGuiCreateDevice(data->hDC, 3, 3);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            g_Width = LOWORD(lParam);
            g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        exit(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}////////////////////////////////////