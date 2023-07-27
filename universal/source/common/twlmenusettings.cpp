#include "common/bootstrappaths.h"
#include "common/twlmenusettings.h"
#include "common/flashcard.h"
#include "common/inifile.h"
#include "common/systemdetails.h"
#include "myDSiMode.h"

#include <nds/arm9/dldi.h>
#include <string.h>

const char *charUnlaunchBg;
int *removeLauncherPatchesPtr;

TWLSettings::TWLSettings()
{
	romfolder[0] = "sd:/";
	romfolder[1] = "fat:/";

	pagenum[0] = 0;
	pagenum[1] = 0;

	cursorPosition[0] = 0;
	cursorPosition[1] = 0;

	consoleModel = EDSiRetail;
	languageSet = false;
	regionSet = false;

	logging = false;
	guiLanguage = ELangDefault;
	currentLanguage = ELangDefault;
	titleLanguage = ELangDefault;
	fps = 60;
	macroMode = false;
	colorMode = 0;
	// blfLevel = 0;
	sleepMode = true;
	kioskMode = false;
	dsiWareExploit = EExploitNone;
	wifiLed = true;
	wifiLedVer = 0;
	powerLedColor = false;
	sdRemoveDetect = true;
	showMicroSd = false;
	gbar2DldiAccess = false;
	showMainMenu = false;
	showSelectMenu = false;
	theme = EThemeDSi;
	settingsMusic = ESMusicTheme;
	dsiMusic = EMusicRegular;
	boxArtColorDeband = false;

	gbaBooter = isDSiMode() ? EGbaGbar2 : EGbaNativeGbar2;
	colEmulator = EColSegaColecoDS;
	sgEmulator = EColSegaColecoDS;
	cpcEmulator = ECpcAmEDS;
	mdEmulator = EMegaDriveHybrid;
	//snesEmulator = true;
	updateRecentlyPlayedList = true;
	sortMethod = ESortAlphabetical;
	showDirectories = true;
	showHidden = false;
	showBoxArt = 0;
	animateDsiIcons = true;
	showCustomIcons = true;
	preventDeletion = false;
	sysRegion = ERegionDefault;
	launcherApp = -1;
	secondaryAccess = false;
	previousUsedDevice = !sys().isRunFromSD();
	secondaryDevice = !sys().isRunFromSD();
	fcSaveOnSd = false;

	flashcard = EDSTTClone;
	slot1LaunchMethod = EDirect;

	dsiSplash = isDSiMode();
	dsiSplashAutoSkip = false;
	nintendoLogoColor = 1;
	showlogo = true;
	longSplashJingle = false;
	autorun = false;
	autostartSlot1 = false;

	show12hrClock = true;

	r4_theme = "unused";
	dsi_theme = "dark";
	_3ds_theme = "light";

	gbaBorder = "default.png";
	unlaunchBg = "default.gif";
	removeLauncherPatches = 2; // 2 == 'Default', keep splash/sound but allow the rest of the patches
	font = "default";
	useThemeFont = true;
	dsClassicCustomFont = false;

	dontShowClusterWarning = false;
	ignoreBlacklists = false;

	slot1AccessSD = false;
	slot1SCFGUnlock = false;
	slot1TouchMode = false;
	ezFlashRam = false;
	limitedMode = 0;
	dontShowDSiWareInDSModeWarning = false;
	dsiWareBooter = EDSiWareBootstrap;
	dsiWareToSD = true;
	newSnesEmuVer = false;
	smsGgInRam = false;
	esrbRatingScreen = false;

	// ak_viewMode = EViewInternal;
	// ak_scrollSpeed = EScrollFast;
	// ak_theme = "zelda";
	// ak_zoomIcons = true;

	useBootstrap = true;
	btsrpBootloaderDirect = false;
	bootstrapFile = EReleaseBootstrap;

	slot1Launched = false;
	launchType[0] = ENoLaunch;
	launchType[1] = ENoLaunch;
	homebrewBootstrap = EReleaseBootstrap;
	homebrewHasWide = false;

	//screenScaleSize = 0;
	wideScreen = false;

	gameLanguage = ELangDefault;
	gameRegion = ERegionDefault;
	useRomRegion = true;
	extendedMemory = 0;
	forceSleepPatch = false;
	soundFreq = EFreq32KHz;
}

void TWLSettings::loadSettings()
{
	if (!sys().isRunFromSD()) {
		settingsinipath = DSIMENUPP_INI_FC; // Fallback to .ini path on flashcard, if TWLMenu++ is not found on SD card
	}

	CIniFile settingsini(settingsinipath);

	// UI settings.
	romfolder[0] = settingsini.GetString("SRLOADER", "ROM_FOLDER", romfolder[0]);
	romfolder[1] = settingsini.GetString("SRLOADER", "SECONDARY_ROM_FOLDER", romfolder[1]);
	if (sdFound() && (strncmp(romfolder[0].c_str(), "sd:", 3) != 0 || access(romfolder[0].c_str(), F_OK) != 0)) {
		romfolder[0] = "sd:/";
	}
	if (flashcardFound() && (strncmp(romfolder[1].c_str(), "fat:", 4) != 0 || access(romfolder[1].c_str(), F_OK) != 0)) {
		romfolder[1] = "fat:/";
	}

	romPath[0] = settingsini.GetString("SRLOADER", "ROM_PATH", romPath[0]);
	romPath[1] = settingsini.GetString("SRLOADER", "SECONDARY_ROM_PATH", romPath[1]);
	if (strncmp(romPath[0].c_str(), "sd:", 3) != 0) {
		romPath[0] = "";
	}
	if (strncmp(romPath[1].c_str(), "fat:", 4) != 0) {
		romPath[1] = "";
	}

	pagenum[0] = settingsini.GetInt("SRLOADER", "PAGE_NUMBER", pagenum[0]);
	pagenum[1] = settingsini.GetInt("SRLOADER", "SECONDARY_PAGE_NUMBER", pagenum[1]);

	cursorPosition[0] = settingsini.GetInt("SRLOADER", "CURSOR_POSITION", cursorPosition[0]);
	cursorPosition[1] = settingsini.GetInt("SRLOADER", "SECONDARY_CURSOR_POSITION", cursorPosition[1]);

	consoleModel = (TConsoleModel)settingsini.GetInt("SRLOADER", "CONSOLE_MODEL", consoleModel);
	languageSet = settingsini.GetInt("SRLOADER", "LANGUAGE_SET", languageSet);
	regionSet = settingsini.GetInt("SRLOADER", "REGION_SET", regionSet);

	// Customizable UI settings.
	logging = settingsini.GetInt("SRLOADER", "LOGGING", logging);
	guiLanguage = (TLanguage)settingsini.GetInt("SRLOADER", "LANGUAGE", guiLanguage);
	currentLanguage = guiLanguage;
	titleLanguage = (TLanguage)settingsini.GetInt("SRLOADER", "TITLELANGUAGE", titleLanguage);
	fps = settingsini.GetInt("SRLOADER", "FRAME_RATE", fps);
	macroMode = settingsini.GetInt("SRLOADER", "MACRO_MODE", macroMode);
	colorMode = settingsini.GetInt("SRLOADER", "COLOR_MODE", colorMode);
	// blfLevel = settingsini.GetInt("SRLOADER", "BLUE_LIGHT_FILTER_LEVEL", blfLevel);
	sleepMode = settingsini.GetInt("SRLOADER", "SLEEP_MODE", sleepMode);
	kioskMode = settingsini.GetInt("SRLOADER", "KIOSK_MODE", kioskMode);
	dsiWareExploit = (TExploit)settingsini.GetInt("SRLOADER", "DSIWARE_EXPLOIT", dsiWareExploit);
	wifiLed = settingsini.GetInt("SRLOADER", "WIFI_LED", wifiLed);
	wifiLedVer = settingsini.GetInt("SRLOADER", "WIFI_LED_VER", wifiLedVer);
	if (wifiLedVer == 0) {
		wifiLed = true; // Set to enable by default
		wifiLedVer = 1;
	}
	powerLedColor = settingsini.GetInt("SRLOADER", "POWER_LED_COLOR", powerLedColor);
	sdRemoveDetect = settingsini.GetInt("SRLOADER", "SD_REMOVE_DETECT", sdRemoveDetect);
	showMicroSd = settingsini.GetInt("SRLOADER", "SHOW_MICROSD", showMicroSd);
	gbar2DldiAccess = settingsini.GetInt("SRLOADER", "GBAR2_DLDI_ACCESS", gbar2DldiAccess);
	showMainMenu = settingsini.GetInt("SRLOADER", "SHOW_MAIN_MENU", showMainMenu);
	showSelectMenu = settingsini.GetInt("SRLOADER", "SHOW_SELECT_MENU", showSelectMenu);
	theme = (TTheme)settingsini.GetInt("SRLOADER", "THEME", theme);
	settingsMusic = (TSettingsMusic)settingsini.GetInt("SRLOADER", "SETTINGS_MUSIC", settingsMusic);
	dsiMusic = (TDSiMusic)settingsini.GetInt("SRLOADER", "DSI_MUSIC", dsiMusic);
	boxArtColorDeband = settingsini.GetInt("SRLOADER", "PHOTO_BOXART_COLOR_DEBAND", boxArtColorDeband);

	if (sys().isRegularDS()) {
		gbaBooter = (TGbaBooter)settingsini.GetInt("SRLOADER", "SHOW_GBA", gbaBooter);
		if (gbaBooter == 0) // 0 (don't show) is deprecated
			gbaBooter = EGbaNativeGbar2;
	} else {
		gbaBooter = EGbaGbar2;
	}
	colEmulator = (TColSegaEmulator)settingsini.GetInt("SRLOADER", "SHOW_COL", colEmulator);
	if (colEmulator == 0) // 0 (don't show) is deprecated
		colEmulator = EColSegaColecoDS;
	sgEmulator = (TColSegaEmulator)settingsini.GetInt("SRLOADER", "SHOW_SG", sgEmulator);
	if (sgEmulator == 0) // 0 (don't show) is deprecated
		sgEmulator = EColSegaColecoDS;
	cpcEmulator = (TCpcEmulator)settingsini.GetInt("SRLOADER", "SHOW_CPC", cpcEmulator);
	if (cpcEmulator == 0) // 0 (don't show) is deprecated
		cpcEmulator = ECpcAmEDS;
	if (!(isDSiMode() && (access("sd:/", F_OK) == 0) && sys().arm7SCFGLocked())) {
		mdEmulator = (TMegaDriveEmulator)settingsini.GetInt("SRLOADER", "SHOW_MDGEN", mdEmulator);
		if (mdEmulator == 0) // 0 (don't show) is deprecated
			mdEmulator = EMegaDriveHybrid;
	} else {
		mdEmulator = EMegaDrivePico; // Use only PicoDriveTWL
	}
	//snesEmulator = settingsini.GetInt("SRLOADER", "SNES_EMULATOR", snesEmulator);
	updateRecentlyPlayedList = settingsini.GetInt("SRLOADER", "UPDATE_RECENTLY_PLAYED_LIST", updateRecentlyPlayedList);
	sortMethod = (TSortMethod)settingsini.GetInt("SRLOADER", "SORT_METHOD", sortMethod);
	showDirectories = settingsini.GetInt("SRLOADER", "SHOW_DIRECTORIES", showDirectories);
	showHidden = settingsini.GetInt("SRLOADER", "SHOW_HIDDEN", showHidden);
	showBoxArt = settingsini.GetInt("SRLOADER", "SHOW_BOX_ART", showBoxArt);
	if (!dsiFeatures() && showBoxArt == 2) // Reset to 1 if not in DSi mode
		showBoxArt = 1;
	animateDsiIcons = settingsini.GetInt("SRLOADER", "ANIMATE_DSI_ICONS", animateDsiIcons);
	showCustomIcons = settingsini.GetInt("SRLOADER", "SHOW_CUSTOM_ICONS", showCustomIcons);
	preventDeletion = settingsini.GetInt("SRLOADER", "PREVENT_ROM_DELETION", preventDeletion);
	sysRegion = (TRegion)settingsini.GetInt("SRLOADER", "SYS_REGION", sysRegion);
	if (consoleModel < 2) {
		launcherApp = settingsini.GetInt("SRLOADER", "LAUNCHER_APP", launcherApp);
	}
	secondaryAccess = settingsini.GetInt("SRLOADER", "SECONDARY_ACCESS", secondaryAccess);
	previousUsedDevice = settingsini.GetInt("SRLOADER", "PREVIOUS_USED_DEVICE", previousUsedDevice);
	secondaryDevice = bothSDandFlashcard() ? settingsini.GetInt("SRLOADER", "SECONDARY_DEVICE", secondaryDevice) : flashcardFound();
	fcSaveOnSd = settingsini.GetInt("SRLOADER", "FC_SAVE_ON_SD", fcSaveOnSd);
	settingsini.GetStringVector("SRLOADER", "BLOCKED_EXTENSIONS", blockedExtensions, ':');

	flashcard = settingsini.GetInt("SRLOADER", "FLASHCARD", flashcard);
	slot1LaunchMethod = (TSlot1LaunchMethod)settingsini.GetInt("SRLOADER", "SLOT1_LAUNCHMETHOD", slot1LaunchMethod);

	dsiSplash = settingsini.GetInt("SRLOADER", "DSI_SPLASH", dsiSplash);
	dsiSplashAutoSkip = settingsini.GetInt("SRLOADER", "DSI_SPLASH_AUTO_SKIP", dsiSplashAutoSkip);
	nintendoLogoColor = settingsini.GetInt("SRLOADER", "NINTENDO_LOGO_COLOR", nintendoLogoColor);
	showlogo = settingsini.GetInt("SRLOADER", "SHOWLOGO", showlogo);
	longSplashJingle = settingsini.GetInt("SRLOADER", "LONG_SPLASH_JINGLE", longSplashJingle);
	autorun = settingsini.GetInt("SRLOADER", "AUTORUNGAME", autorun);
	autostartSlot1 = settingsini.GetInt("SRLOADER", "AUTORUNSLOT1", autostartSlot1);

	show12hrClock = settingsini.GetInt("SRLOADER", "SHOW_12H_CLOCK", show12hrClock);

	r4_theme = settingsini.GetString("SRLOADER", "R4_THEME", r4_theme);
	dsi_theme = settingsini.GetString("SRLOADER", "DSI_THEME", dsi_theme);
	_3ds_theme = settingsini.GetString("SRLOADER", "3DS_THEME", _3ds_theme);

	gbaBorder = settingsini.GetString("SRLOADER", "GBA_BORDER", gbaBorder);
	unlaunchBg = settingsini.GetString("SRLOADER", "UNLAUNCH_BG", unlaunchBg);
	charUnlaunchBg = unlaunchBg.c_str();
	removeLauncherPatches = settingsini.GetInt("SRLOADER", "UNLAUNCH_LAUNCHER_PATCHES", removeLauncherPatches);
	removeLauncherPatchesPtr = &removeLauncherPatches;
	font = settingsini.GetString("SRLOADER", "FONT", font);
	useThemeFont = settingsini.GetInt("SRLOADER", "USE_THEME_FONT", useThemeFont);
	dsClassicCustomFont = settingsini.GetInt("SRLOADER", "DS_CLASSIC_CUSTOM_FONT", dsClassicCustomFont);

	dontShowClusterWarning = settingsini.GetInt("SRLOADER", "DONT_SHOW_CLUSTER_WARNING", dontShowClusterWarning);
	ignoreBlacklists = settingsini.GetInt("SRLOADER", "IGNORE_BLACKLISTS", ignoreBlacklists);

	slot1AccessSD = settingsini.GetInt("SRLOADER", "SLOT1_ENABLESD", slot1AccessSD);
	slot1SCFGUnlock = settingsini.GetInt("SRLOADER", "SLOT1_SCFG_UNLOCK", slot1SCFGUnlock);
	slot1TouchMode = settingsini.GetInt("SRLOADER", "SLOT1_TOUCH_MODE", slot1TouchMode);
	ezFlashRam = settingsini.GetInt("SRLOADER", "EZ_FLASH_RAM", ezFlashRam);
	limitedMode = settingsini.GetInt("SRLOADER", "LIMITED_MODE", limitedMode);
	dontShowDSiWareInDSModeWarning = settingsini.GetInt("SRLOADER", "DONT_SHOW_DSIWARE_IN_DS_MODE_WARNING", dontShowDSiWareInDSModeWarning);
	dsiWareBooter = (TDSiWareBooter)settingsini.GetInt("SRLOADER", "DSIWARE_BOOTER", dsiWareBooter);
	dsiWareToSD = settingsini.GetInt("SRLOADER", "DSIWARE_TO_SD", dsiWareToSD);
	newSnesEmuVer = settingsini.GetInt("SRLOADER", "NEW_SNES_EMU_VER", newSnesEmuVer);
	smsGgInRam = settingsini.GetInt("SRLOADER", "SMS_GG_IN_RAM", smsGgInRam);
	esrbRatingScreen = settingsini.GetInt("SRLOADER", "ESRB_RATING_SCREEN", esrbRatingScreen);

	// ak_viewMode = settingsini.GetInt("SRLOADER", "AK_VIEWMODE", ak_viewMode);
	// ak_scrollSpeed = settingsini.GetInt("SRLOADER", "AK_SCROLLSPEED", ak_scrollSpeed);
	// ak_theme = settingsini.GetString("SRLOADER", "AK_THEME", ak_theme);
	// ak_zoomIcons = settingsini.GetInt("SRLOADER", "AK_ZOOM_ICONS", ak_zoomIcons);

	if (!(io_dldi_data->ioInterface.features & FEATURE_SLOT_GBA)) {
		useBootstrap = settingsini.GetInt("SRLOADER", "USE_BOOTSTRAP", useBootstrap);
	} else {
		useBootstrap = true;
	}
	btsrpBootloaderDirect = settingsini.GetInt("SRLOADER", "BOOTSTRAP_BOOTLOADER_DIRECT", btsrpBootloaderDirect);
	bootstrapFile = (TBootstrapFile)settingsini.GetInt("SRLOADER", "BOOTSTRAP_FILE", bootstrapFile);

	dsiWareSrlPath = settingsini.GetString("SRLOADER", "DSIWARE_SRL", dsiWareSrlPath);
	dsiWarePubPath = settingsini.GetString("SRLOADER", "DSIWARE_PUB", dsiWarePubPath);
	dsiWarePrvPath = settingsini.GetString("SRLOADER", "DSIWARE_PRV", dsiWarePrvPath);
	slot1Launched = settingsini.GetInt("SRLOADER", "SLOT1_LAUNCHED", slot1Launched);
	launchType[0] = (TLaunchType)settingsini.GetInt("SRLOADER", "LAUNCH_TYPE", launchType[0]);
	launchType[1] = (TLaunchType)settingsini.GetInt("SRLOADER", "SECONDARY_LAUNCH_TYPE", launchType[1]);
	homebrewArg[0] = settingsini.GetString("SRLOADER", "HOMEBREW_ARG", homebrewArg[0]);
	homebrewArg[1] = settingsini.GetString("SRLOADER", "SECONDARY_HOMEBREW_ARG", homebrewArg[1]);
	homebrewBootstrap = settingsini.GetInt("SRLOADER", "HOMEBREW_BOOTSTRAP", homebrewBootstrap);
	homebrewHasWide = settingsini.GetInt("SRLOADER", "HOMEBREW_HAS_WIDE", 0);

	//screenScaleSize = settingsini.GetInt("TWL_FIRM", "SCREENSCALESIZE", screenScaleSize);
	wideScreen = settingsini.GetInt("SRLOADER", "WIDESCREEN", wideScreen);

	// Default nds-bootstrap settings
	gameLanguage = (TLanguage)settingsini.GetInt("NDS-BOOTSTRAP", "LANGUAGE", gameLanguage);
	gameRegion = (TRegion)settingsini.GetInt("NDS-BOOTSTRAP", "REGION", gameRegion);
	useRomRegion = settingsini.GetInt("NDS-BOOTSTRAP", "USE_ROM_REGION", useRomRegion);
	extendedMemory = settingsini.GetInt("NDS-BOOTSTRAP", "EXTENDED_MEMORY", extendedMemory);
	forceSleepPatch = settingsini.GetInt("NDS-BOOTSTRAP", "FORCE_SLEEP_PATCH", forceSleepPatch);
	soundFreq = (TSoundFreq)settingsini.GetInt("NDS-BOOTSTRAP", "SOUND_FREQ", soundFreq);
}

void TWLSettings::saveSettings()
{
	CIniFile settingsini(settingsinipath);

	// UI settings.
	settingsini.SetString("SRLOADER", "ROM_FOLDER", romfolder[0]);
	settingsini.SetString("SRLOADER", "SECONDARY_ROM_FOLDER", romfolder[1]);

	settingsini.SetString("SRLOADER", "ROM_PATH", romPath[0]);
	settingsini.SetString("SRLOADER", "SECONDARY_ROM_PATH", romPath[1]);

	settingsini.SetInt("SRLOADER", "PAGE_NUMBER", pagenum[0]);
	settingsini.SetInt("SRLOADER", "SECONDARY_PAGE_NUMBER", pagenum[1]);

	settingsini.SetInt("SRLOADER", "CURSOR_POSITION", cursorPosition[0]);
	settingsini.SetInt("SRLOADER", "SECONDARY_CURSOR_POSITION", cursorPosition[1]);

	settingsini.SetInt("SRLOADER", "CONSOLE_MODEL", consoleModel);
	settingsini.SetInt("SRLOADER", "LANGUAGE_SET", languageSet);
	settingsini.SetInt("SRLOADER", "REGION_SET", regionSet);

	// Customizable UI settings.
	settingsini.SetInt("SRLOADER", "LOGGING", logging);
	settingsini.SetInt("SRLOADER", "LANGUAGE", guiLanguage);
	settingsini.SetInt("SRLOADER", "TITLELANGUAGE", titleLanguage);
	settingsini.SetInt("SRLOADER", "FRAME_RATE", fps);
	settingsini.SetInt("SRLOADER", "MACRO_MODE", macroMode);
	settingsini.SetInt("SRLOADER", "COLOR_MODE", colorMode);
	// settingsini.SetInt("SRLOADER", "BLUE_LIGHT_FILTER_LEVEL", blfLevel);
	settingsini.SetInt("SRLOADER", "DSIWARE_EXPLOIT", dsiWareExploit);
	settingsini.SetInt("SRLOADER", "WIFI_LED", wifiLed);
	settingsini.SetInt("SRLOADER", "WIFI_LED_VER", wifiLedVer);
	settingsini.SetInt("SRLOADER", "POWER_LED_COLOR", powerLedColor);
	settingsini.SetInt("SRLOADER", "SD_REMOVE_DETECT", sdRemoveDetect);
	settingsini.SetInt("SRLOADER", "SHOW_MICROSD", showMicroSd);
	settingsini.SetInt("SRLOADER", "GBAR2_DLDI_ACCESS", gbar2DldiAccess);
	settingsini.SetInt("SRLOADER", "SHOW_MAIN_MENU", showMainMenu);
	settingsini.SetInt("SRLOADER", "SHOW_SELECT_MENU", showSelectMenu);
	settingsini.SetInt("SRLOADER", "THEME", theme);
	settingsini.SetInt("SRLOADER", "SETTINGS_MUSIC", settingsMusic);
	settingsini.SetInt("SRLOADER", "DSI_MUSIC", dsiMusic);
	settingsini.SetInt("SRLOADER", "PHOTO_BOXART_COLOR_DEBAND", boxArtColorDeband);

	if (sys().isRegularDS()) {
		settingsini.SetInt("SRLOADER", "SHOW_GBA", gbaBooter);
	}
	settingsini.SetInt("SRLOADER", "SHOW_COL", colEmulator);
	settingsini.SetInt("SRLOADER", "SHOW_SG", sgEmulator);
	settingsini.SetInt("SRLOADER", "SHOW_CPC", cpcEmulator);
	if (!(isDSiMode() && (access("sd:/", F_OK) == 0) && sys().arm7SCFGLocked())) {
		settingsini.SetInt("SRLOADER", "SHOW_MDGEN", mdEmulator);
	}
	// settingsini.SetInt("SRLOADER", "SNES_EMULATOR", snesEmulator);
	settingsini.SetInt("SRLOADER", "UPDATE_RECENTLY_PLAYED_LIST", updateRecentlyPlayedList);
	settingsini.SetInt("SRLOADER", "SORT_METHOD", sortMethod);
	settingsini.SetInt("SRLOADER", "SHOW_DIRECTORIES", showDirectories);
	settingsini.SetInt("SRLOADER", "SHOW_HIDDEN", showHidden);
	settingsini.SetInt("SRLOADER", "SHOW_BOX_ART", showBoxArt);
	settingsini.SetInt("SRLOADER", "ANIMATE_DSI_ICONS", animateDsiIcons);
	settingsini.SetInt("SRLOADER", "SHOW_CUSTOM_ICONS", showCustomIcons);
	settingsini.SetInt("SRLOADER", "PREVENT_ROM_DELETION", preventDeletion);
	settingsini.SetInt("SRLOADER", "SYS_REGION", sysRegion);
	if (consoleModel < 2) {
		settingsini.SetInt("SRLOADER", "LAUNCHER_APP", launcherApp);
	}
	settingsini.SetInt("SRLOADER", "SECONDARY_ACCESS", secondaryAccess);
	settingsini.SetInt("SRLOADER", "PREVIOUS_USED_DEVICE", previousUsedDevice);
	if (bothSDandFlashcard()) {
		settingsini.SetInt("SRLOADER", "SECONDARY_DEVICE", secondaryDevice);
	}
	settingsini.SetInt("SRLOADER", "FC_SAVE_ON_SD", fcSaveOnSd);

	settingsini.SetInt("SRLOADER", "FLASHCARD", flashcard);
	settingsini.SetInt("SRLOADER", "SLOT1_LAUNCHMETHOD", slot1LaunchMethod);

	settingsini.SetInt("SRLOADER", "DSI_SPLASH", dsiSplash);
	settingsini.SetInt("SRLOADER", "DSI_SPLASH_AUTO_SKIP", dsiSplashAutoSkip);
	settingsini.SetInt("SRLOADER", "NINTENDO_LOGO_COLOR", nintendoLogoColor);
	settingsini.SetInt("SRLOADER", "SHOWLOGO", showlogo);
	settingsini.SetInt("SRLOADER", "LONG_SPLASH_JINGLE", longSplashJingle);
	settingsini.SetInt("SRLOADER", "AUTORUNGAME", autorun);
	settingsini.SetInt("SRLOADER", "AUTORUNSLOT1", autostartSlot1);

	settingsini.SetInt("SRLOADER", "SHOW_12H_CLOCK", show12hrClock);

	settingsini.SetString("SRLOADER", "R4_THEME", r4_theme);
	settingsini.SetString("SRLOADER", "DSI_THEME", dsi_theme);
	settingsini.SetString("SRLOADER", "3DS_THEME", _3ds_theme);

	settingsini.SetString("SRLOADER", "GBA_BORDER", gbaBorder);
	settingsini.SetString("SRLOADER", "UNLAUNCH_BG", unlaunchBg);
	settingsini.SetInt("SRLOADER", "UNLAUNCH_LAUNCHER_PATCHES", removeLauncherPatches);
	settingsini.SetString("SRLOADER", "FONT", font);
	settingsini.SetInt("SRLOADER", "USE_THEME_FONT", useThemeFont);
	settingsini.SetInt("SRLOADER", "DS_CLASSIC_CUSTOM_FONT", dsClassicCustomFont);

	settingsini.SetInt("SRLOADER", "DONT_SHOW_CLUSTER_WARNING", dontShowClusterWarning);
	settingsini.SetInt("SRLOADER", "IGNORE_BLACKLISTS", ignoreBlacklists);

	settingsini.SetInt("SRLOADER", "SLOT1_ENABLESD", slot1AccessSD);
	settingsini.SetInt("SRLOADER", "SLOT1_SCFG_UNLOCK", slot1SCFGUnlock);
	settingsini.SetInt("SRLOADER", "SLOT1_TOUCH_MODE", slot1TouchMode);
	settingsini.SetInt("SRLOADER", "LIMITED_MODE", limitedMode);
	settingsini.SetInt("SRLOADER", "DONT_SHOW_DSIWARE_IN_DS_MODE_WARNING", dontShowDSiWareInDSModeWarning);
	settingsini.SetInt("SRLOADER", "DSIWARE_BOOTER", dsiWareBooter);
	settingsini.SetInt("SRLOADER", "DSIWARE_TO_SD", dsiWareToSD);
	settingsini.SetInt("SRLOADER", "SMS_GG_IN_RAM", smsGgInRam);
	settingsini.SetInt("SRLOADER", "ESRB_RATING_SCREEN", esrbRatingScreen);

	// settingsini.SetInt("SRLOADER", "AK_VIEWMODE", ak_viewMode);
	// settingsini.SetInt("SRLOADER", "AK_SCROLLSPEED", ak_scrollSpeed);
	// settingsini.SetString("SRLOADER", "AK_THEME", ak_theme);
	// settingsini.SetInt("SRLOADER", "AK_ZOOM_ICONS", ak_zoomIcons);

	if (!(io_dldi_data->ioInterface.features & FEATURE_SLOT_GBA)) {
		settingsini.SetInt("SRLOADER", "USE_BOOTSTRAP", useBootstrap);
	}
	settingsini.SetInt("SRLOADER", "BOOTSTRAP_BOOTLOADER_DIRECT", btsrpBootloaderDirect);
	settingsini.SetInt("SRLOADER", "BOOTSTRAP_FILE", bootstrapFile);

	settingsini.SetString("SRLOADER", "DSIWARE_SRL", dsiWareSrlPath);
	settingsini.SetString("SRLOADER", "DSIWARE_PUB", dsiWarePubPath);
	settingsini.SetString("SRLOADER", "DSIWARE_PRV", dsiWarePrvPath);
	settingsini.SetInt("SRLOADER", "SLOT1_LAUNCHED", slot1Launched);
	settingsini.SetInt("SRLOADER", "LAUNCH_TYPE", launchType[0]);
	settingsini.SetInt("SRLOADER", "SECONDARY_LAUNCH_TYPE", launchType[1]);
	settingsini.SetString("SRLOADER", "HOMEBREW_ARG", homebrewArg[0]);
	settingsini.SetString("SRLOADER", "SECONDARY_HOMEBREW_ARG", homebrewArg[0]);
	settingsini.SetInt("SRLOADER", "HOMEBREW_BOOTSTRAP", homebrewBootstrap);
	settingsini.SetInt("SRLOADER", "HOMEBREW_HAS_WIDE", 0);

	// settingsini.SetInt("TWL_FIRM", "SCREENSCALESIZE", screenScaleSize);
	settingsini.SetInt("SRLOADER", "WIDESCREEN", wideScreen);

	// Default nds-bootstrap settings
	settingsini.SetInt("NDS-BOOTSTRAP", "LANGUAGE", gameLanguage);
	settingsini.SetInt("NDS-BOOTSTRAP", "REGION", gameRegion);
	settingsini.SetInt("NDS-BOOTSTRAP", "USE_ROM_REGION", useRomRegion);
	settingsini.SetInt("NDS-BOOTSTRAP", "EXTENDED_MEMORY", extendedMemory);
	settingsini.SetInt("NDS-BOOTSTRAP", "FORCE_SLEEP_PATCH", forceSleepPatch);
	settingsini.SetInt("NDS-BOOTSTRAP", "SOUND_FREQ", soundFreq);

	settingsini.SaveIniFileModified(settingsinipath);
}

TWLSettings::TLanguage TWLSettings::getGuiLanguage()
{
	if (currentLanguage == ELangDefault) {
		extern bool useTwlCfg;
		return (TLanguage)(useTwlCfg ? *(u8*)0x02000406 : PersonalData->language);
	}
	return (TLanguage)currentLanguage;
}

TWLSettings::TLanguage TWLSettings::getGameLanguage()
{
	if (gameLanguage == ELangDefault) {
		extern bool useTwlCfg;
		return (TLanguage)(useTwlCfg ? *(u8*)0x02000406 : PersonalData->language);
	}
	return (TLanguage)gameLanguage;
}

TWLSettings::TLanguage TWLSettings::getTitleLanguage()
{
	if (titleLanguage == ELangDefault) {
		return (TLanguage)PersonalData->language;
	}
	return (TLanguage)titleLanguage;
}

std::string TWLSettings::getGuiLanguageString()
{
	switch (getGuiLanguage()) {
		case TWLSettings::ELangJapanese:
			return "ja";
		case TWLSettings::ELangEnglish:
		default:
			return "en";
		case TWLSettings::ELangFrench:
			return "fr";
		case TWLSettings::ELangGerman:
			return "de";
		case TWLSettings::ELangItalian:
			return "it";
		case TWLSettings::ELangSpanish:
			return "es";
		case TWLSettings::ELangChineseS:
			return "zh-CN";
		case TWLSettings::ELangKorean:
			return "ko";
		case TWLSettings::ELangChineseT:
			return "zh-TW";
		case TWLSettings::ELangPolish:
			return "pl";
		case TWLSettings::ELangPortuguese:
			return "pt";
		case TWLSettings::ELangRussian:
			return "ru";
		case TWLSettings::ELangSwedish:
			return "sv";
		case TWLSettings::ELangDanish:
			return "da";
		case TWLSettings::ELangTurkish:
			return "tr";
		case TWLSettings::ELangUkrainian:
			return "uk";
		case TWLSettings::ELangHungarian:
			return "hu";
		case TWLSettings::ELangNorwegian:
			return "no";
		case TWLSettings::ELangHebrew:
			return "he";
		case TWLSettings::ELangDutch:
			return "nl";
		case TWLSettings::ELangIndonesian:
			return "id";
		case TWLSettings::ELangGreek:
			return "el";
		case TWLSettings::ELangBulgarian:
			return "bg";
		case TWLSettings::ELangRomanian:
			return "ro";
		case TWLSettings::ELangArabic:
			return "ar";
		case TWLSettings::ELangPortugueseBrazil:
			return "pt-BR";
		case TWLSettings::ELangVietnamese:
			return "vi";
		case TWLSettings::ELangValencian:
			return "val";
		case TWLSettings::ELangCatalan:
			return "ca";
		case TWLSettings::ELangRyukyuan:
			return "ry";
	}
}

bool TWLSettings::rtl()
{
	switch (currentLanguage) {
		case ELangHebrew:
		case ELangArabic:
			return true;
		default:
			return false;
	}
}

TWLSettings::TRegion TWLSettings::getGameRegion()
{
	if (dsiFeatures() && gameRegion == ERegionDefault) {
		extern bool useTwlCfg;
		if (!useTwlCfg) {
			return ERegionUSA;
		}

		u8 twlCfgCountry = *(u8*)0x02000405;
		if (twlCfgCountry == 0x01) {
			return ERegionJapan;
		} else if (twlCfgCountry == 0xA0) {
			return ERegionChina;
		} else if (twlCfgCountry == 0x88) {
			return ERegionKorea;
		} else if (twlCfgCountry == 0x41 || twlCfgCountry == 0x5F) {
			return ERegionAustralia;
		} else if ((twlCfgCountry >= 0x08 && twlCfgCountry <= 0x34) || twlCfgCountry == 0x99 || twlCfgCountry == 0xA8) {
			return ERegionUSA;
		} else if (twlCfgCountry >= 0x40 && twlCfgCountry <= 0x70) {
			return ERegionEurope;
		}
		return ERegionUSA;
	}
	return (TRegion)gameRegion;
}
