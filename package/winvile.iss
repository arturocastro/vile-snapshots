; $Header: /users/source/archives/vile.vcs/package/RCS/winvile.iss,v 1.3 2005/03/13 23:40:43 tom Exp $
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; Notes:
;	Rather than apply environment to all users, add a registry lookup for the default value of environment settings.
;	Uninstalling an environment variable isn't completed until I logout/login again.  The old value isn't in the registry.
;	System environment variables appear to override the user environment variables.  Add a check and don't try to conflict.
;
; TODO:
;	during install, check for system environment variables.  If found, migrate them to the selected flavor
;	(which is not system environment variables!), and require a reboot to get rid of them.
;
;	components' disablenouninstallwarning could be used to implement selective uninstall, but looks clumsy
;
;	the skipifdoesntexist flag on the [Run] does not appear to work.
;
;	optional task: add a "Send To" task for winvile
;
;	optional task: register winvile as an OLE server (RegisterServer function).  Do this via a shortcut.
;	optional task: unregister winvile as an OLE server
;
;	install wvwrap and visvile (separate install, or component to add-on)
;	install vile.exe (separate install)

[Setup]
AppName=WinVile
AppVerName=WinVile 9.5
AppPublisher=Thomas E. Dickey
AppPublisherURL=http://invisible-island.net/vile/
AppSupportURL=http://invisible-island.net/vile/
AppUpdatesURL=http://invisible-island.net/vile/
DefaultDirName={pf}\VI Like Emacs
DefaultGroupName=WinVile
AllowNoIcons=yes
LicenseFile=..\COPYING
Compression=lzma
SolidCompression=yes

[Components]
Name: main; Description: The WinVile executable; types: full custom compact
Name: help; Description: Vile's help-file; types: full custom compact
Name: docs; Description: Extra documentation; types: full custom
Name: macros; Description: Useful macros; types: full custom compact
Name: filters; Description: External filter executables (WinVile has most of these built-in); types: full custom

[Tasks]
Name: register_hklm; Description: Register for all users on this machine; GroupDescription: Configuration Settings; Components: filters macros; Flags: unchecked exclusive
Name: register_hkcu; Description: Register for the current user; GroupDescription: Configuration Settings; Components: filters macros; Flags: exclusive
Name: register_vars; Description: Environment Variables (current user only); GroupDescription: Configuration Settings; Components: filters macros; Flags: unchecked exclusive
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Components: main; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Components: main; Flags: unchecked

[Dirs]
Name: {app}\bin; Components: main
Name: {app}\doc; Components: docs
Name: {app}\macros; Components: macros
Name: {app}\filters; Components: filters

[Files]
Source: ..\bin\winvile-ole.exe; DestDir: {app}\bin; DestName: winvile.exe; Components: main; Flags: ignoreversion
Source: ..\vile.hlp; DestDir: {app}\bin; Components: docs help; Flags: ignoreversion
Source: ..\README; DestDir: {app}; DestName: README.txt; Components: docs main; Flags: isreadme
Source: ..\doc\*.doc; Destdir: {app}\doc; Components: docs; Flags: ignoreversion
Source: ..\macros\*.rc; DestDir: {app}\macros; Components: macros; AfterInstall: myPostMacros; Flags: ignoreversion recursesubdirs
Source: ..\filters\*.rc; DestDir: {app}\macros; Components: macros; AfterInstall: myPostMacros; Flags: ignoreversion
Source: c:\vile\*.exe; DestDir: {app}\filters; Components: filters; AfterInstall: myPostFilters; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\WinVile; Filename: {app}\bin\winvile.exe; Components: main; Flags: createonlyiffileexists
Name: {group}\{cm:UninstallProgram,WinVile}; Filename: {uninstallexe}
Name: {userdesktop}\WinVile; Filename: {app}\bin\winvile.exe; Components: main; Tasks: desktopicon; Flags: createonlyiffileexists
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinVile; Filename: {app}\bin\winvile.exe; Components: main; Tasks: quicklaunchicon; Flags: createonlyiffileexists

[Run]
Filename: {app}\bin\winvile.exe; Description: {cm:LaunchProgram,WinVile}; Flags: nowait postinstall skipifsilent skipifdoesntexist

[UninstallDelete]
Type: files; Name: {app}\macros\vile.rc
Type: dirifempty; Name: {app}\macros
Type: dirifempty; Name: {app}

[Code]
// Add the given string to the front of the environment variable ValueName,
// unless it is already there.
procedure addDirToEnv(const RootKey: Integer; const SubKeyName, ValueName, toAdd: String);
var
	Current : string;
	Testing : string;
	Updated : string;
	Actual  : string;
begin
	Updated := ExpandConstant(toAdd);
	if RegQueryStringValue(RootKey, SubKeyName, ValueName, Current) then
		begin
		// Successfully read the value
		Actual := Updated;
		if Length(Current) >= Length(Actual) then
			begin
			Testing := Copy(Current, 1, Length(Actual));
			if CompareStr(Testing, Actual) = 0 then
				begin
				Log('Directory ' + toAdd + ' is already in ' + ValueName)
				Updated := '';
				end
			else
				begin
				if Length(Current) > 0 then
					Updated := Updated + ';' + Current
				end;
			end
		else
			begin
			if Length(Current) > 0 then
				Updated := Updated + ';' + Current
			end;
		end;
	if Length(Updated) > 0 then
	begin
		RegWriteStringValue(RootKey, SubKeyName, ValueName, Updated);
		Log('Added ' + toAdd + ' to ' + ValueName);
		// MsgBox('addDirToEnv: ' #13#13 + ValueName + '="' + Updated + '"', mbInformation, MB_OK)
	end;
end;

// Remove the given string from the environment variable ValueName, assuming
// it is at the front of the variable, and (like the PATH variable) delimited
// by a semicolon from any other directory(s).
function removeDirFromEnv(const RootKey: Integer; const SubKeyName, ValueName, toRemove: String): Boolean;
var
	Current : string;
	Updated : string;
	Actual  : string;
begin
	Result := False;
	if RegQueryStringValue(RootKey, SubKeyName, ValueName, Current) then
	begin
		// Successfully read the value
		Actual := ExpandConstant(toRemove);
		if Length(Current) >= Length(Actual) then
		begin
			Updated := Copy(Current, 1, Length(Actual));
			if CompareStr(Updated, Actual) = 0 then
			begin
				Updated := Copy(Current, Length(Actual) + 1, Length(Current));
				Actual  := Copy(Updated, 1, 1);
				if CompareStr(Actual, ';') = 0 then
				begin
					Updated := Copy(Updated, 2, Length(Updated));
				end;
				if Length(Updated) = 0 then
					RegDeleteValue(RootKey, SubKeyName, ValueName)
				else
					RegWriteStringValue(RootKey, SubKeyName, ValueName, Updated);
				Result := True;
				Log('Removed ' + toRemove + ' from ' + ValueName);
				// MsgBox('removeDirFromEnv: ' #13#13 + ValueName + '="' + Updated + '"', mbInformation, MB_OK)
			end;
		end;
	end;
end;

// Installing environment variables is only useful for the current user,
// since the setup script does not remember what tasks were selected for
// an uninstall.  But make the root/sub keys functions to make this easier
// to change.
function environRootKey(): Integer;
begin
	Result := HKEY_CURRENT_USER;
end;

function appSubKey(): string;
begin
	Result := 'Software\VI Like Emacs\Environment';
end;

function envSubKey(): string;
begin
	Result := 'Environment';
end;

procedure add_startup_path(const RootKey: Integer; const SubKeyName: String);
begin
	addDirToEnv(RootKey, SubKeyName, 'VILE_STARTUP_PATH', '{app}\macros');
end;

procedure add_libdir_path(const RootKey: Integer; const SubKeyName: String);
begin
	addDirToEnv(RootKey, SubKeyName, 'VILE_LIBDIR_PATH', '{app}\filters');
end;

// This is called once per installed-file.
procedure myPostFilters();
begin
	if isTaskSelected('register_hklm') then
		add_libdir_path(HKEY_LOCAL_MACHINE, appSubKey())
	else if isTaskSelected('register_hkcu') then
		add_libdir_path(HKEY_CURRENT_USER, appSubKey())
	else if isTaskSelected('register_vars') then
		add_libdir_path(HKEY_CURRENT_USER, envSubKey());
end;

// This is called once per installed-file.
procedure myPostMacros();
var
	ThisFile : string;
	InitFile : string;
begin
	// MsgBox('myPostMacros: tasks <' + WizardSelectedTasks(false) + '>', mbInformation, MB_OK);
	if isTaskSelected('register_hklm') then
		add_startup_path(HKEY_LOCAL_MACHINE, appSubKey())
	else if isTaskSelected('register_hkcu') then
		add_startup_path(HKEY_CURRENT_USER, appSubKey())
	else if isTaskSelected('register_vars') then
		add_startup_path(HKEY_CURRENT_USER, envSubKey());
	ThisFile := CurrentFileName();
	// MsgBox('myPostMacros: <' + ThisFile + '>', mbInformation, MB_OK);
	if CompareStr(ThisFile, '{app}\macros\vileinit.rc') = 0 then
	begin
		InitFile := ExpandConstant('{app}\macros\vile.rc');
		SaveStringToFile(InitFile, 'source vileinit.rc' + #13#10, False);
		SaveStringToFile(InitFile, 'set cs=black' + #13#10, True);
	end;
end;

// On uninstall, we do not know which registry setting was selected during install, so we remove all.
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  case CurUninstallStep of
    usUninstall:
      begin
        // MsgBox('CurUninstallStepChanged:' #13#13 'Uninstall is about to start.', mbInformation, MB_OK)
        // ...insert code to perform pre-uninstall tasks here...
      end;
    usPostUninstall:
      begin
        // MsgBox('CurUninstallStepChanged:' #13#13 'Uninstall just finished.', mbInformation, MB_OK);
        removeDirFromEnv(HKEY_CURRENT_USER, appSubKey(), 'VILE_LIBDIR_PATH', '{app}\filters');
        removeDirFromEnv(HKEY_CURRENT_USER, appSubKey(), 'VILE_STARTUP_PATH', '{app}\macros');
        removeDirFromEnv(HKEY_CURRENT_USER, envSubKey(), 'VILE_LIBDIR_PATH', '{app}\filters');
        removeDirFromEnv(HKEY_CURRENT_USER, envSubKey(), 'VILE_STARTUP_PATH', '{app}\macros');
        removeDirFromEnv(HKEY_LOCAL_MACHINE, appSubKey(), 'VILE_LIBDIR_PATH', '{app}\filters');
        removeDirFromEnv(HKEY_LOCAL_MACHINE, appSubKey(), 'VILE_STARTUP_PATH', '{app}\macros');
        // ...insert code to perform post-uninstall tasks here...
      end;
  end;
end;
