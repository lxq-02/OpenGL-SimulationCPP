﻿// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProject1.h"
#include"Canvas.h"
#include<math.h>
#include"Image.h"


#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND    hWnd;
int     wWidth = 800;
int     wHeight = 600;
HDC     hDC;
HDC     hMem;

GT::Canvas* _canvas = NULL;
GT::Image* _image = NULL;
GT::Image* _bkImage = NULL;
GT::Image* _zoomImage = NULL;
GT::Image* _zoomImageSimple = NULL;

float _angle = 0.0f;
float _xCam = 0;
float _zRun = 0;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Render();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));


    /*===========创建绘图用的位图========*/
    void* buffer = 0;

    hDC = GetDC(hWnd);
    hMem = ::CreateCompatibleDC(hDC);

    BITMAPINFO  bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = wWidth;
    bmpInfo.bmiHeader.biHeight = wHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgr
    bmpInfo.bmiHeader.biSizeImage = 0;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;

    HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&buffer, 0, 0);//在这里创建buffer的内存
    SelectObject(hMem, hBmp);

    memset(buffer, 0, wWidth * wHeight * 4); //清空buffer为0

    /*===========创建绘图用的位图========*/

    _canvas = new GT::Canvas(wWidth, wHeight, buffer);
    _image = GT::Image::readFromFile("res/carma.png");
    _zoomImage = GT::Image::zoomImage(_image, 3, 3);
    _zoomImageSimple = GT::Image::zoomImageSimple(_image, 3, 3);
    // _image->setAlpha(0.5);
    _bkImage = GT::Image::readFromFile("res/bk.jpg");


    MSG msg;

    // 主消息循环:
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Render();
    }

    return (int)msg.wParam;
}





void Render()
{
    _canvas->clear();

    GT::Point ptArray[] =
    {
        {0.0f,0.0f,0.0f,       GT::RGBA(255,0,0) , GT::floatV2(0,0)},
        {300.0f,0.0f,0.0f,    GT::RGBA(0,255,0) , GT::floatV2(1.0,0)},
        {300.0f,300.0f,0.0f,   GT::RGBA(0,0,255) , GT::floatV2(1.0,1.0)},

        {300.0f,0.0f,0.0f,       GT::RGBA(255,0,0) , GT::floatV2(0,0)},
        {300.0f,300.0f,0.0f,    GT::RGBA(0,255,0) , GT::floatV2(0.0,0)},
        {300.0f,0.0f,-500.0f,   GT::RGBA(0,0,255) , GT::floatV2(0.0,0.0)},
    };

    for (int i = 0; i < 6; i++)
    {

        glm::vec4 ptv4(ptArray[i].m_x, ptArray[i].m_y, ptArray[i].m_z, 1);


        glm::mat4 mMat(1.0f);
        mMat = glm::translate(mMat, glm::vec3(-300, 0, 0));

        glm::mat4 rMat(1.0f);
        rMat = glm::rotate(rMat, glm::radians(_angle), glm::vec3(0, 1, 0));

        //VP变换
        glm::mat4 vMat(1.0f);
        vMat = glm::lookAt(glm::vec3(0, 0, 1000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        glm::mat4 pMat(1.0f);
        pMat = glm::perspective(glm::radians(60.0f), (float)wWidth / (float)wHeight, 1.0f, 1000.0f);

        ptv4 = pMat * vMat * rMat * mMat * ptv4;


        ptArray[i].m_x = (ptv4.x / ptv4.w + 1.0) * (float)wWidth / 2.0;
        ptArray[i].m_y = (ptv4.y / ptv4.w + 1.0) * (float)wHeight / 2.;
        ptArray[i].m_z = ptv4.z / ptv4.w;
    }

    _angle += 0.2;
    _xCam += 5;
    _zRun += 2;

    _canvas->gtVertexPointer(2, GT::GT_FLOAT, sizeof(GT::Point), (GT::byte*)ptArray);
    _canvas->gtColorPointer(1, GT::GT_FLOAT, sizeof(GT::Point), (GT::byte*)&ptArray[0].m_color);
    _canvas->gtTexCoordPointer(1, GT::GT_FLOAT, sizeof(GT::Point), (GT::byte*)&ptArray[0].m_uv);

    _canvas->enableTexture(true);
    _canvas->bindTexture(_bkImage);

    _canvas->gtDrawArray(GT::GT_TRIANGLE, 0, 6);

    //在这里画到设备上，hMem相当于缓冲区
    BitBlt(hDC, 0, 0, wWidth, wHeight, hMem, 0, 0, SRCCOPY);
}




//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
        100, 100, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
