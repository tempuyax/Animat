IDS_TIMER   	   	Equ 1
IDM_RGNSHOWHIDE    	Equ 100
IDM_EXIT           	Equ 101
IDS_MYBITMAP	   	equ 102	
IDS_BMPREGION       equ 103 

SKINCRAFTER Struct
       SkinCrafterHeader  SIZEL	<?>     ;ukuran region
       SkinCrafterNumByte DD     ?      ;32bit {128,4,0,0} jumlah RECT region+Region Header
SKINCRAFTER EndS

CStr szClassName, "WinAnimate"     		;Creates szPngDemoApp string in .const

TopXY proto :dword, :dword

Object WinAnimate, 10002, SdiApp	    ;Objek Turunan dari SdiApp dgn (ID >= 10002)
  	RedefineMethod  Init          		;redefinisi Metode Init  
  	RedefineMethod  Done          		;redefinisi Metode Done  
    VirtualMethod   Calxy 
	StaticMethod    Startup       		;Startup method redefinition to registration
  	
  	VirtualEvent    OnCreate,   		WM_CREATE
  	VirtualEvent    OnCommand,  		WM_COMMAND
  	VirtualEvent    OnLButtonDown,  	WM_LBUTTONDOWN
  	VirtualEvent    OnRButtonDown,  	WM_RBUTTONDOWN
  	VirtualEvent    OnPaint,    		WM_PAINT
    VirtualEvent    OnDestroy,          WM_DESTROY
  	VirtualEvent    OnKeyDown,  		WM_KEYDOWN
  	VirtualEvent    OnTimer,	  		WM_TIMER
  
	DefineVariable 	rc,					RECT,		{}
	DefineVariable	hBmp, 	 			HBITMAP, 	0
	DefineVariable	hdcBmp, 	  		HDC,     	0
	DefineVariable	hOldBmp,   			HBITMAP, 	0
	DefineVariable	hdcRgn,		  		HDC, 		0
	DefineVariable	hRegionOld,			HDC, 		0
	DefineVariable	ptTmp,     			POINT,  	{}
	DefineVariable	ptRgn,     			POINT,  	{}
	DefineVariable	RsrcHand,  			DWord, 		0
	DefineVariable	RsrcPoint, 			DWord, 		0
	DefineVariable	RsrcSize,  			DWord, 		0
	DefineVariable	hRegion,   			DWord,   	0
	DefineVariable	hMenu,      		HWND,    	0
	DefineVariable	pt,         		POINT,   	{}
	DefineVariable	fChek,      		DWord,   	MF_UNCHECKED  ;init value
	DefineVariable	fTimer,     		DWord,    	TRUE
ObjectEnd                         		;Ends object definition

.code
; ==================================================================================================
;    WinAnimate implementation
; ==================================================================================================

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.Init
; Purpose:   Initalizes the SDI application object, when an instance is created.
;            This mirrors Iczelions "WinMain" proceedure (mid section)
;            Note: The inherited "Run" method from SdiApp is equivalent to the last message loop
;                  section in Iczelions "WinMain". So we need not redefine this method, we will
;                  simply reuse the source as provided.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.Init, uses esi
    SetObject esi                                       ;esi = This Object Instance
    ACall esi.Init

	invoke CreateWindowEx, 0, Addr szClassName, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, esi
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.Done
; Purpose:   
; Arguments: None.
; Return:    

Method WinAnimate.Done, uses esi
    SetObject esi
;   ===============================
;	Bagian ini di isi Despose Object
;   ===============================
    ACall esi.Done
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnCreate
; Purpose:   Initalizes the SDI application object, when an instance is created.
;            This mirrors Iczelions "WinMain" proceedure (mid section)
;            Note: The inherited "Run" method from SdiApp is equivalent to the last message loop
;                  section in Iczelions "WinMain". So we need not redefine this method, we will
;                  simply reuse the source as provided.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnCreate, uses esi, wParam:dword, lParam:dword
	LOCAL hdc:HDC

    SetObject esi
	; create popup menu
	Mov [esi].hMenu, $invoke(CreatePopupMenu)
	@invoke	AppendMenu, [esi].hMenu, MF_STRING, IDM_RGNSHOWHIDE, "&UnSkined"
	@invoke	AppendMenu, [esi].hMenu, MF_STRING, IDM_EXIT, "&Exit"
	;bitmap load from resouce
    invoke LoadBitmap, hInstance, IDS_MYBITMAP
    .If Eax
    	Mov [esi].hBmp, Eax
    	;init device
     	Mov hdc, $invoke(GetDC, [esi].hWnd)
    	Mov [esi].hdcBmp, $invoke(CreateCompatibleDC, hdc)
    	Mov [esi].hOldBmp, $invoke(SelectObject, [esi].hdcBmp, [esi].hBmp)
    	invoke ReleaseDC, [esi].hWnd, hdc
		;===========================================================
    	;loading region from resource
		Mov [esi].RsrcHand, $invoke(FindResource, hInstance, IDS_BMPREGION, RT_RCDATA)
	    Mov [esi].RsrcPoint, $invoke(LoadResource, hInstance, [esi].RsrcHand)
		Mov [esi].RsrcSize, $invoke(SizeofResource, hInstance, [esi].RsrcHand)
		Mov [esi].RsrcPoint, $invoke(LockResource, [esi].RsrcPoint)
        ;Pass this Code to SkinCrafter Region Product
		Sub [esi].RsrcSize, SizeOf SKINCRAFTER ;ukuran dikurangi
		Add [esi].RsrcPoint, SizeOf SKINCRAFTER ;pointer ditambah
    	; --> CREATE REGION AND PASS IT TO OUR WINDOW
		Mov [esi].hRegion, $invoke(ExtCreateRegion, NULL, [esi].RsrcSize, [esi].RsrcPoint)
		invoke SetWindowRgn, [esi].hWnd, [esi].hRegion, TRUE
		;===========================================================
    	;Set Rect rc to frame size
    	Mov [esi].rc.right, 64      ;set window size cx(width)
    	Mov [esi].rc.bottom, 64     ;set window size cy(height)
    	;Centering Display
		invoke GetSystemMetrics, SM_CXSCREEN
		invoke TopXY, [esi].rc.right, Eax
		Mov [esi].rc.left, Eax
		invoke GetSystemMetrics, SM_CYSCREEN
		invoke TopXY, [esi].rc.bottom, Eax
		Mov [esi].rc.top, Eax
    	;Set Window Size & show window
    	invoke SetWindowPos, [esi].hWnd, HWND_TOP, [esi].rc.left, [esi].rc.top, [esi].rc.right, [esi].rc.bottom, SWP_SHOWWINDOW
    	;Window Update client area, acsses mss WM_PAINT
    	invoke UpdateWindow, [esi].hWnd
    	; Init timer
   	  	invoke SetTimer, [esi].hWnd, IDS_TIMER, 50, NULL
	   	xor eax,eax
   	.endif  	
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnCommand
; Purpose:   Registers the object class with the OS.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnCommand, uses esi, wParam:dword, lParam:dword
    SetObject esi
    mov eax, wParam
   .If ax == IDM_RGNSHOWHIDE 
    	.If [esi].fChek == MF_UNCHECKED
            Mov [esi].fChek, MF_CHECKED
            invoke CheckMenuItem, [esi].hMenu, IDM_RGNSHOWHIDE, [esi].fChek
			invoke SetWindowRgn, [esi].hWnd, NULL, TRUE ; set  window region
        .ElseIf [esi].fChek == MF_CHECKED
            Mov [esi].fChek, MF_UNCHECKED
            invoke CheckMenuItem, [esi].hMenu, IDM_RGNSHOWHIDE, [esi].fChek
	    	Mov [esi].hRegion, $invoke(ExtCreateRegion, NULL, [esi].RsrcSize, [esi].RsrcPoint)
		    invoke OffsetRgn, [esi].hRegion, [esi].ptRgn.x, [esi].ptRgn.y
			invoke SetWindowRgn, [esi].hWnd, [esi].hRegion, TRUE ; set  window region
        .EndIf
    .elseIf ax == IDM_EXIT
    	invoke DestroyWindow, [esi].hWnd
    .EndIf
	Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnLbuttonDown
; Purpose:   Mouse Tombol Kiri.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnLButtonDown, uses esi, wParam:dword, lParam:dword
    SetObject esi
    invoke SendMessage, [esi].hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL
    Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnRbuttonDown
; Purpose:   Mouse Tombol Kanan.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnRButtonDown, uses esi, wParam:dword, lParam:dword
    SetObject esi
	invoke	GetCursorPos, Addr [esi].pt
	invoke	SetForegroundWindow, [esi].hWnd
	invoke	TrackPopupMenu, [esi].hMenu, TPM_RIGHTALIGN, [esi].pt.x, [esi].pt.y, NULL, [esi].hWnd, NULL
	invoke	PostMessage, [esi].hWnd, WM_NULL, 0, 0
	Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnPaint
; Purpose:   Registers the object class with the OS.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnPaint, uses esi, wParam:dword, lParam:dword
	local ps:PAINTSTRUCT

    SetObject esi
    invoke BeginPaint, [esi].hWnd, Addr ps
    invoke BitBlt, ps.hdc, 0, 0, [esi].rc.right, [esi].rc.bottom, [esi].hdcBmp, [esi].ptTmp.x, [esi].ptTmp.y, SRCCOPY
    invoke EndPaint, [esi].hWnd, Addr ps
	Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnDestroy
; Purpose:   Event procedure for WM_DESTROY message.
; Arguments: Arg1: First message parameter.
;            Arg2: Second message parameter.
; Return:    Zero if handled.

Method WinAnimate.OnDestroy, uses esi, wParam:dword, lParam:dword
    SetObject esi
	.If [esi].hBmp
		Invoke SelectObject, [esi].hdcBmp, [esi].hOldBmp
		Invoke DeleteObject, [esi].hBmp
		Invoke DeleteDC, [esi].hdcBmp
		Invoke FreeResource, [esi].RsrcPoint
		Invoke KillTimer, [esi].hWnd, IDS_TIMER
	.EndIf
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnKeydown
; Purpose:   Keyboard command
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnKeyDown, uses esi, wParam:dword, lParam:dword
    SetObject esi
	;Exit Window
	.If wParam == VK_ESCAPE
		invoke DestroyWindow, [esi].hWnd
	.ElseIf wParam == VK_SPACE
    ;Runing Test Step by step....
		.If [esi].fTimer == TRUE
	        invoke KillTimer, [esi].hWnd, IDS_TIMER
	        Mov [esi].fTimer, FALSE
	    .EndIf
    	invoke SendMessage, [esi].hWnd, WM_TIMER, NULL, NULL
   ;Running it on Realtime....
	.ElseIf wParam == VK_RETURN
		.If [esi].fTimer == FALSE
    	  	invoke SetTimer, [esi].hWnd, IDS_TIMER, 50, NULL
	        Mov [esi].fTimer, TRUE
	    .EndIf
	.EndIf
	Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.OnTimer
; Purpose:   Animation Running Realtime
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.OnTimer, uses esi, wParam:dword, lParam:dword
    SetObject esi
    OCall esi.Calxy
	.If [esi].fChek == MF_UNCHECKED
    	Mov [esi].hRegion, $invoke(ExtCreateRegion, NULL, [esi].RsrcSize, [esi].RsrcPoint)
	    invoke OffsetRgn, [esi].hRegion, [esi].ptRgn.x, [esi].ptRgn.y
		invoke SetWindowRgn, [esi].hWnd, [esi].hRegion, TRUE ; set  window region
    .ElseIf [esi].fChek == MF_CHECKED
		invoke SetWindowRgn, [esi].hWnd, NULL, TRUE ; set  window region
    .EndIf
    invoke RedrawWindow, [esi].hWnd, NULL, NULL, RDW_INVALIDATE ;draw bitmap again
	Xor Eax, Eax
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.Startup
; Purpose:   Registers the object class with the OS.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.Startup
	Local wc:WNDCLASSEX

	Mov wc.cbSize, SizeOf WNDCLASSEX
	Mov wc.style, 0 
	m2m wc.lpfnWndProc, $MethodAddr(WinAnimate.WndProc)
	Mov wc.cbClsExtra, 0
	Mov wc.cbWndExtra, 0
	m2m wc.hInstance, hInstance
	Mov wc.hIcon, 0
	Mov wc.hCursor, $invoke(LoadCursor, NULL, IDC_ARROW)
	Mov wc.hbrBackground, COLOR_BTNFACE + 1
	Mov wc.lpszMenuName, 0
	Mov wc.lpszClassName, Offset szClassName
	Mov wc.hIconSm, 0

	invoke RegisterClassEx, Addr wc
MethodEnd
	
;NOTE:
;    ukuran tiap frame = (64 x 64 pixel)
;    ukuran total frame yg bersusun = (640 x 384)
;    ukuran frame bitmap = ukuran frame region
;    ukuran total bitmap = ukuran total region
;    susunan frame = cx x cy = 10 x 6
;
;METHODE:
;	Frame Bitmap Ditampilkan 1 per 1 dgn menambah x dan y (BITLT source)
;   Frame Region Ditampilkan 1 per 1 dgn mengurangi x dan y (OffsetRgn x y)

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    WinAnimate.Calxy
; Purpose:   Registers the object class with the OS.
; Arguments: None.
; Return:    Nothing.

Method WinAnimate.Calxy, uses esi
    SetObject esi
	Add [esi].ptTmp.x, 64
	Sub [esi].ptRgn.x, 64
	.If [esi].ptTmp.x >= 640 && [esi].ptRgn.x <= -640 ;calculation x
	    ;reset xy  bitmap frame
	    Mov [esi].ptTmp.x, 0
	    Add [esi].ptTmp.y, 64
	    ;reset xy region frame
	    Mov [esi].ptRgn.x, 0
	    Sub [esi].ptRgn.y, 64
	    .If [esi].ptTmp.y >= 384 && [esi].ptRgn.x <= -384 ;calculation y
	        Mov [esi].ptTmp.y, 0 ;reset y back to 0
	        Mov [esi].ptRgn.y, 0 ;reset y back to 0
	    .EndIf
	.EndIf
MethodEnd

TopXY Proc wDim:DWord, sDim:DWord
    Shr sDim, 1      ; divide screen dimension by 2
    shr wDim, 1      ; divide window dimension by 2
    mov eax, wDim    ; copy window dimension into eax
    sub sDim, eax    ; sub half win dimension from half screen dimension
    mov eax, sDim
    ret
TopXY EndP






