; ==================================================================================================
; Title:   WnAnimate.asm
; Author:  Pahor.m  @ September 2019
; Version: 1.0.0
; Purpose: Demonstration program 1 of ObjAsm32.
;		   Membuat Dinamis Region. 	
; ==================================================================================================

;Include & initialize standard modules
%include @Environ(OA32_PATH)\\Code\\Macros\\Model.inc

;Loads OOP files and OS related objects       
SysSetup OOP_WINDOWS ;, DEBUG(WND)                          

;Load or build the following objects
LoadObjects WinApp, SdiApp                                  ;Include SDI framework library

;Includes Demonstration object (insert .code)
include WinAnimate.inc                                      

;The application is an object called "WinAnimate"
start:                                                      ;Program entry point
    SysInit                                                 ;Runtime initialization of OOP model (Required)
    ;Registering of the class is carried out
    OCall TPL_WinAnimate::WinAnimate.Init                   ;Initialize the object data
    OCall TPL_WinAnimate::WinAnimate.Run                      
    OCall TPL_WinAnimate::WinAnimate.Done                   

    SysDone                                                 ;Runtime finalization of the OOP model (Required)
    invoke ExitProcess, 0                                   ;Exits program returning 0 to the OS
end start                                                   ;Code end and defines prg entry point
