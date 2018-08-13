; Warp Rogue install script
;

;--------------------------------

; The name of the installer
Name "Warp Rogue"

; The file to write
OutFile "wrogue-0.8.0-i586.exe"

; Adds an XP manifest to the installer
XPStyle on

; Turns off Window Icon
WindowIcon off

; The default installation directory
InstallDir "$PROGRAMFILES\Warp Rogue"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Warp Rogue" "Install_Dir"


;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Warp Rogue"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r C:\wrogue\*.*

  ; Write the installation path into the registry
  WriteRegStr HKLM "Software\Warp Rogue" "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Warp Rogue" "DisplayName" "Warp Rogue"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Warp Rogue" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Warp Rogue" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Warp Rogue" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Warp Rogue"
  CreateShortCut "$SMPROGRAMS\Warp Rogue\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Warp Rogue\Warp Rogue.lnk" "$INSTDIR\wrogue.exe" "" "$INSTDIR\wrogue.exe" 0

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Warp Rogue"
  DeleteRegKey HKLM "Software\Warp Rogue"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Warp Rogue\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Warp Rogue"
  RMDir /r "$INSTDIR"

SectionEnd
