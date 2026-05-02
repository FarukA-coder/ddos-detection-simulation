; ============================================================
;  DDoS Detection Simulation - Inno Setup Installer Script
;  Professional installer for Windows
; ============================================================

#define MyAppName "DDoS Detection Simulation"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Faruk - OOP Final Project"
#define MyAppExeName "DDoSDetectionSimulation.exe"
#define MyAppURL "https://github.com/faruk"

[Setup]
AppId={{B5E8F9A2-7C3D-4E6A-8F1B-2D9E4A7C5B3F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=..\build\installer
OutputBaseFilename=DDoS_Detection_Setup_v{#MyAppVersion}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
SetupIconFile=..\assets\icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

; Visual settings
WizardImageFile=..\assets\banner.bmp
WizardSmallImageFile=..\assets\icon.bmp

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "..\build\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}\doc"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
procedure InitializeWizard();
begin
  WizardForm.WelcomeLabel2.Caption :=
    'This will install DDoS Detection Simulation v1.0.0 on your computer.' + #13#10 +
    #13#10 +
    'A professional network security monitoring tool that simulates ' +
    'DDoS attacks and demonstrates real-time detection algorithms.' + #13#10 +
    #13#10 +
    'OOP Final Project by Faruk' + #13#10 +
    #13#10 +
    'Click Next to continue.';
end;
