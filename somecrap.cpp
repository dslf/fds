#include <windows.h>

#define local_persist static 
#define global_variable static 
#define internal static 

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void
Win32ResizeDIBSection(int Width, int Height)
{

  if(BitmapMemory)
  {
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  }

  BitmapWidth = Width;
  BitmapHeight = Height;

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  int BytesPerPixel = 4;
  int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
  int WindowWidth = WindowRect->right - WindowRect->left;
  int WindowHeight = WindowRect->bottom - WindowRect->top;

  StretchDIBits(DeviceContext,
      // X, Y, Width, Height,
      // X, Y, Width, Height,
      0, 0, BitmapWidth, BitmapHeight,
      0, 0, WindowWidth, WindowHeight,
      BitmapMemory,
      &BitmapInfo,
      DIB_RGB_COLORS, SRCCOPY);

}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
  LRESULT Result = 0;

  switch(Message)
  {
    case WM_SIZE:
      {
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int Width = ClientRect.right - ClientRect.left;
        int Height = ClientRect.bottom - ClientRect.top;
        Win32ResizeDIBSection(Width, Height);
      } break;
    
    case WM_CLOSE:
      {
        Running = false;
      } break;

    case WM_DESTROY:
      {
        Running = false;
      } break;
    
    case WM_ACTIVATEAPP:
      {
      } break;
    case WM_PAINT:
      {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        int X = Paint.rcPaint.left;
        int Y = Paint.rcPaint.top;
        int Width = Paint.rcPaint.right - Paint.rcPaint.left;
        int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

        RECT ClientRect;
        GetClientRect(Window, &ClientRect);

        Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
        EndPaint(Window, &Paint);
      } break;

    default:
      {
        Result = DefWindowProc(Window, Message, WParam, LParam);
      } break;
  }

  return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
  WNDCLASS WindowClass = {};
  WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";
  if(RegisterClass(&WindowClass))
  {
    HWND WindowHandle = 
      CreateWindowEx(
          0,
          WindowClass.lpszClassName,
          "Handmade hero",
          WS_OVERLAPPEDWINDOW|WS_VISIBLE,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          0,
          0,
          Instance,
          0);
    if(WindowHandle)
    {
      Running = true;
      while(Running)
      { 
        MSG Message;
        BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
        if(MessageResult > 0)
        {
          TranslateMessage(&Message);
          DispatchMessage(&Message);
        }
        else
        {
          break;
        }
      }
    }
  }
  else
  {
    // logging
  }
  
  return(0);
}
