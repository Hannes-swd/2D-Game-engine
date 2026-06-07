#define MyAppName "Voxira Game Engine"
#define MyAppVersion "1.0"

[Setup]
AppId={{A7F3B2C1-D4E5-4F60-8A9B-C0D1E2F34567}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\Voxira-Game-Engine
DisableProgramGroupPage=yes
OutputDir=Output
OutputBaseFilename=VoxiraSetup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ShowLanguageDialog=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\*";           DestDir: "{app}"; Flags: recursesubdirs createallsubdirs; Excludes: "build\*,.git\*,.claude\*,installer\*,trash\*,cmake\*"
Source: "build_helper.ps1"; DestDir: "{tmp}"

[Code]

function CMakeAvailable(): Boolean;
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{cmd}'), '/C cmake --version', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Result := (ResultCode = 0);
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  ScriptFile: String;
  AppDir: String;
begin
  if CurStep <> ssPostInstall then Exit;

  AppDir := ExpandConstant('{app}');

  if not CMakeAvailable() then
  begin
    MsgBox(
      'CMake wurde nicht gefunden.' + #13#10#13#10 +
      'Bitte CMake installieren von:' + #13#10 +
      '  https://cmake.org/download/' + #13#10#13#10 +
      'Dabei "Add CMake to the system PATH" auswaehlen.' + #13#10#13#10 +
      'Danach in diesem Ordner eine Kommandozeile oeffnen:' + #13#10 +
      '  ' + AppDir + #13#10 +
      'und ausfuehren:' + #13#10 +
      '  cmake -B build' + #13#10 +
      '  cmake --build build --config Release',
      mbError, MB_OK);
    ShellExec('open', 'https://cmake.org/download/', '', '', SW_SHOW, ewNoWait, ResultCode);
    Exit;
  end;

  // Copy PS1 to system temp so it survives after installer closes
  ScriptFile := GetEnv('TEMP') + '\voxira_build.ps1';
  FileCopy(ExpandConstant('{tmp}\build_helper.ps1'), ScriptFile, False);

  ShellExec(
    'open', 'powershell.exe',
    '-ExecutionPolicy Bypass -NoProfile -File "' + ScriptFile + '" -appDir "' + AppDir + '"',
    '', SW_SHOW, ewNoWait, ResultCode);
end;
