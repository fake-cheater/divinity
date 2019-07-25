#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" 
#include "Interfaces.h"
#include "CRC32.h"
#include <fstream>
#include "XorStr.hpp"
/*
 /$$   /$$                                   /$$
| $$$ | $$                                  | $$
| $$$$| $$  /$$$$$$  /$$$$$$/$$$$   /$$$$$$ | $$  /$$$$$$   /$$$$$$$ /$$$$$$$
| $$ $$ $$ |____  $$| $$_  $$_  $$ /$$__  $$| $$ /$$__  $$ /$$_____//$$_____/
| $$  $$$$  /$$$$$$$| $$ \ $$ \ $$| $$$$$$$$| $$| $$$$$$$$|  $$$$$$|  $$$$$$
| $$\  $$$ /$$__  $$| $$ | $$ | $$| $$_____/| $$| $$_____/ \____  $$\____  $$
| $$ \  $$|  $$$$$$$| $$ | $$ | $$|  $$$$$$$| $$|  $$$$$$$ /$$$$$$$//$$$$$$$/
|__/  \__/ \_______/|__/ |__/ |__/ \_______/|__/ \_______/|_______/|_______/
*/
//credits to Freak for the source
//credits to Nightmare for being the dev for divinity, and for the jitter desync
//credits to me aka Nameless for editing the menu add added features

#define WINDOW_WIDTH 635  // 635
#define WINDOW_HEIGHT 645 // 645
mirror_window options::menu;
struct Config_t {
	int id;
	std::string name;
};
std::vector<Config_t> configs;
typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void MsgX(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); 	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}
void save_callback()
{
	int should_save = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "divinity\\cfg\\";
	config_directory += configs[should_save].name; config_directory += ".xml";
	GUI.SaveWindowState(&options::menu, XorStr(config_directory.c_str()));
	interfaces::cvar->ConsoleColorPrintf(Color(140, 10, 250, 255), "divinity ");
	std::string uremam = "Saved configuration.     \n";
	MsgX(uremam.c_str());
}
void load_callback()
{
	int should_load = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "divinity\\cfg\\";
	config_directory += configs[should_load].name; config_directory += ".xml";
	GUI.LoadWindowState(&options::menu, XorStr(config_directory.c_str()));
	interfaces::cvar->ConsoleColorPrintf(Color(140, 10, 250, 255), "divinity ");
	std::string uremam = "Loaded configuration.     \n";
	MsgX(uremam.c_str());
}

void list_configs() {
	configs.clear();
	options::menu.ColorsTab.ConfigListBox.ClearItems();
	std::ifstream file_in;
	file_in.open("divinity\\cfg\\divinity_configs.txt");
	if (file_in.fail()) {
		std::ofstream("divinity\\cfg\\divinity_configs.txt");
		file_in.open("divinity\\cfg\\divinity_configs.txt");
	}
	int line_count;
	while (!file_in.eof()) {
		Config_t config;
		file_in >> config.name;
		config.id = line_count;
		configs.push_back(config);
		line_count++;
		options::menu.ColorsTab.ConfigListBox.AddItem(config.name);
	}
	file_in.close();
	if (configs.size() > 7) options::menu.ColorsTab.ConfigListBox.AddItem(" ");
}

void add_config() {
	std::fstream file;
	file.open("divinity\\cfg\\divinity_configs.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("divinity\\cfg\\divinity_configs.txt");
		file.open("divinity\\cfg\\divinity_configs.txt", std::fstream::app);
	}
	file << std::endl << options::menu.ColorsTab.NewConfigName.getText();
	file.close();
	list_configs();
	int should_add = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "divinity\\cfg\\";
	config_directory += options::menu.ColorsTab.NewConfigName.getText(); config_directory += ".xml";
	GUI.SaveWindowState(&options::menu, XorStr(config_directory.c_str()));
	options::menu.ColorsTab.NewConfigName.SetText("");
}

void remove_config() {
	int should_remove = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "divinity\\cfg\\";
	config_directory += configs[should_remove].name; config_directory += ".xml";
	std::remove(config_directory.c_str());
	std::ofstream ofs("divinity\\cfg\\divinity_configs.txt", std::ios::out | std::ios::trunc);
	ofs.close();
	std::fstream file;
	file.open("divinity\\cfg\\divinity_configs.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("divinity\\cfg\\divinity_configs.txt");
		file.open("divinity\\cfg\\divinity_configs.txt", std::fstream::app);
	}
	for (int i = 0; i < configs.size(); i++) {
		if (i == should_remove) continue;
		Config_t config = configs[i];
		file << std::endl << config.name;
	}
	file.close();
	list_configs();
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = interfaces::cvar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	interfaces::engine->ClientCmd_Unrestricted("cl_fullupdate");
}

void UnLoadCallbk()
{
	DoUnload = true;
}
void mirror_window::Setup()
{
	SetPosition(1100, 250);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("");
	RegisterTab(&aimbot);
	//RegisterTab(&LegitBotTab);
	RegisterTab(&visuals);
	RegisterTab(&misc);
	RegisterTab(&ColorsTab);
	RegisterTab(&skin);
	//RegisterTab(&playerlist);

	RECT Client = GetClientArea();
	Client.bottom -= 29;
	aimbot.Setup();
	//LegitBotTab.Setup();
	visuals.Setup();
	misc.Setup();
	ColorsTab.Setup();
	skin.Setup();
	//playerlist.Setup();

#pragma endregion
}
void CAimbotTab::Setup()
{
	SetTitle("A");
	//separating shit yes
#pragma region Aimbot
	AimbotGroup.SetPosition(4, 25); // 15, 25
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(270, 315);
	AimbotGroup.AddTab(CGroupTab("Main", 1));
	AimbotGroup.AddTab(CGroupTab("Lag Comp", 2));
	AimbotGroup.AddTab(CGroupTab("BodyAim", 3));

	//	AimbotGroup.AddTab(CGroupTab("Debug", 5));
	//	AimbotGroup.AddTab(CGroupTab("Resolver Mods", 4));
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl(1, "Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl(1, "Automatic Fire", this, &AimbotAutoFire);

	//AutoWall.SetFileId("aim_autofire");
	//AimbotGroup.PlaceLabledControl(1, "Auto Wall", this, &AutoWall);

	//AimbotKey.SetFileId("aa_1thirpbind");
	//AimbotGroup.PlaceLabledControl(1, "Aimbot Key", this, &AimbotKey);
	//AimbotKeyBind.SetFileId("aim_aimbotkey");
	//AimbotGroup.PlaceLabledControl(1, "", this, &AimbotKeyBind);

	AimbotSilentAim.SetFileId("aim_silent");
	AimbotGroup.PlaceLabledControl(1, "Silent Aim", this, &AimbotSilentAim);

	AccuracyAutoScope.SetFileId("acc_scope");
	AimbotGroup.PlaceLabledControl(1, "Automatic Scope", this, &AccuracyAutoScope);

	//AimbotFov.SetFileId("aim_fov");
	//AimbotFov.SetBoundaries(0.f, 180.f);
	//AimbotFov.extension = XorStr("°");
	//AimbotFov.SetValue(0.f);
	//AimbotGroup.PlaceLabledControl(1, "Maximum Fov", this, &AimbotFov);

	AccuracyHitchance.SetFileId("base_hc");
	AccuracyHitchance.SetBoundaries(0.f, 100.f);
	AccuracyHitchance.SetValue(20);

	AccuracyMinimumDamage.SetFileId("base_md");
	AccuracyMinimumDamage.SetBoundaries(0.f, 100.f);
	AccuracyMinimumDamage.SetValue(20);

	preso.SetFileId("acc_zeusisgay");
	preso.AddItem("Off");
	preso.AddItem("Default");
	preso.AddItem("Down");
	preso.AddItem("Up");
	preso.AddItem("Zero");
	preso.AddItem("Automatic");
	AimbotGroup.PlaceLabledControl(1, "Pitch Resolver", this, &preso);

	resolver.SetFileId("acc_aaa");
	resolver.AddItem("Off");
	resolver.AddItem("Divinity");
	resolver.AddItem("Nameless");
	//	resolver.AddItem("experimental");
	AimbotGroup.PlaceLabledControl(1, "Resolver", this, &resolver);

	flip180.SetFileId("flip180");
	AimbotGroup.PlaceLabledControl(1, "Override", this, &flip180);

	/*
	TargetPointscale.SetFileId("acc_hitbox_Scale");
	TargetPointscale.SetBoundaries(0, 100);
	TargetPointscale.SetValue(50);
	TargetPointscale.extension = ("%%");
	AimbotGroup.PlaceLabledControl(2, "Hitbox Scale", this, &TargetPointscale);
	*/

	lag_pred.SetFileId("lag_pred");
	lag_pred.AddItem("Off");
	lag_pred.AddItem("Classic");
	lag_pred.AddItem("Genuine");
	AimbotGroup.PlaceLabledControl(2, "Position Adjustment", this, &lag_pred);

	delay_shot.SetFileId("delay_shot");
	delay_shot.AddItem("Off");
	delay_shot.AddItem("Sim-Time");
	delay_shot.AddItem("Lag Compensation");
	delay_shot.AddItem("Refine Shot");
	AimbotGroup.PlaceLabledControl(2, "Delay Shot", this, &delay_shot);

	extrapolation.SetFileId("acc_extra_P_lation");
	AimbotGroup.PlaceLabledControl(2, "Extrapolation", this, &extrapolation);

	baim_fake.SetFileId("bodyaim_fake");
	AimbotGroup.PlaceLabledControl(3, "If Fake", this, &baim_fake); // if we have to resort to a brute

	baim_fakewalk.SetFileId("bodyaim_fakewalk");
	AimbotGroup.PlaceLabledControl(3, "If Slow Motion", this, &baim_fakewalk);

	baim_inair.SetFileId("bodyaim_inair");
	AimbotGroup.PlaceLabledControl(3, "If In Air", this, &baim_inair);

	BaimIfUnderXHealth.SetFileId("acc_BaimIfUnderXHealth");
	BaimIfUnderXHealth.SetBoundaries(0, 100);
	BaimIfUnderXHealth.extension = XorStr("HP");
	BaimIfUnderXHealth.SetValue(0);
	AimbotGroup.PlaceLabledControl(3, "If HP Lower Than", this, &BaimIfUnderXHealth);

	bigbaim.SetFileId("acc_bigbaim");
	AimbotGroup.PlaceLabledControl(3, "Baim On Key", this, &bigbaim);
	
	//bigheadaim.SetFileId("acc_bigbaim");
	//AimbotGroup.PlaceLabledControl(3, "Head Only Key", this, &bigheadaim);

	multipoint.SetText("Multipoint");
	multipoint.SetPosition(286, 25); // 15, 230
	multipoint.SetSize(326, 315);
	multipoint.AddTab(CGroupTab("Settings", 1));
	RegisterControl(&multipoint);

	Multienable.SetFileId("multipoint_enable");
	multipoint.PlaceLabledControl(1, "Toggle Multipoint", this, &Multienable);

	Multival2.SetFileId("hitbox_scale_head");
	Multival2.SetBoundaries(0.1, 100);
	Multival2.SetValue(20);
	Multival2.extension = XorStr("%%");
	multipoint.PlaceLabledControl(1, "Head Multipoint", this, &Multival2);

	Multival4.SetFileId("hitbox_scale_upperbody");
	Multival4.SetBoundaries(0.1, 100);
	Multival4.SetValue(20);
	Multival4.extension = XorStr("%%");
	multipoint.PlaceLabledControl(1, "Upper Body Multipoint", this, &Multival4);

	Multival.SetFileId("hitbox_scale_body");
	Multival.SetBoundaries(0.1, 100);
	Multival.SetValue(20);
	Multival.extension = XorStr("%%");
	multipoint.PlaceLabledControl(1, "Lower Body Multipoint", this, &Multival);

	MultiVal3.SetFileId("hitbox_scale_legs");
	MultiVal3.SetBoundaries(0.1, 100);
	MultiVal3.SetValue(20);
	MultiVal3.extension = XorStr("%%");
	multipoint.PlaceLabledControl(1, "Leg Multipoint", this, &MultiVal3);

	/*
	prefer_head.SetFileId("acc_prefer_head");
	prefer_head.items.push_back(dropdownboxitem(false, XorStr("If Moving")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("Lower body Is Unhittable")));
	prefer_head.items.push_back(dropdownboxitem(false, XorStr("No Fake")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("Head Is Visible")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("")));
	AimbotGroup.PlaceLabledControl(4, "Prefer Head Aim Factors", this, &prefer_head);
	*/
	//	toggledebug.SetFileId("debugtoggle");
	//	AimbotGroup.PlaceLabledControl(5, "Print Debug Info In Console", this, &toggledebug);


	// -<--------------------------------------------------------------->- //
	/*
	legit_mode.SetFileId("aim_legit_toggle");
	AimbotGroup.PlaceLabledControl(4, "Enable", this, &legit_mode);

	legit_trigger.SetFileId("aim_legit_trigger");
	AimbotGroup.PlaceLabledControl(4, "Trigger", this, &legit_trigger);

	legit_trigger_key.SetFileId("aim_legit_trigger_key");
	AimbotGroup.PlaceLabledControl(4, "Trigger Key", this, &legit_trigger_key);

	apply_smooth.SetFileId("aim_legit_apply_smooth");
	AimbotGroup.PlaceLabledControl(4, "Apply Smoothness", this, &apply_smooth); */
	// -<--------------------------------------------------------------->- //

	weapongroup.SetText("Weapon Configurations");
	weapongroup.SetPosition(4, 350); // 15, 230
	weapongroup.SetSize(608, 225);//180
	weapongroup.AddTab(CGroupTab("Auto Sniper", 1));
	weapongroup.AddTab(CGroupTab("Pistols", 2));
	weapongroup.AddTab(CGroupTab("Scout", 3));
	weapongroup.AddTab(CGroupTab("Awp", 4));
	weapongroup.AddTab(CGroupTab("SMG", 5));
	weapongroup.AddTab(CGroupTab("Others", 6));
	RegisterControl(&weapongroup);

	/*
	target_auto.SetFileId("tgt_hitbox_auto");
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(1, "HitScan", this, &target_auto);

	custom_hitscan.SetFileId("auto_hitscan_auto");
	weapongroup.PlaceLabledControl(1, "Automatic HitScan", this, &custom_hitscan);

	hc_auto.SetFileId("auto_hitchance");
	hc_auto.SetBoundaries(0, 100);
	hc_auto.SetValue(25);
	hc_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Hitchance", this, &hc_auto);

	md_auto.SetFileId("auto_minimumdamage");
	md_auto.SetBoundaries(0, 100);
	md_auto.SetValue(25);
	md_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Minimum Damage", this, &md_auto);

	//	preset_auto.SetFileId("auto_automatic_cfg");
	//	weapongroup.PlaceLabledControl(1, "Automatic Auto Sniper Configuration", this, &preset_auto);
	//----------------------------------------------------------------------

	target_pistol.SetFileId("tgt_hitbox_pistol");
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(2, "HitScan", this, &target_pistol);

	hc_pistol.SetFileId("pistol_hitchance");
	hc_pistol.SetBoundaries(0, 100);
	hc_pistol.SetValue(25);
	hc_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Hitchance", this, &hc_pistol);

	md_pistol.SetFileId("pistol_minimumdamage");
	md_pistol.SetBoundaries(0, 100);
	md_pistol.SetValue(25);
	md_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Minimum Damage", this, &md_pistol);


	//	preset_pistol.SetFileId("pistol_automatic_cfg");
	//	weapongroup.PlaceLabledControl(2, "Automatic Pistol Configuration", this, &preset_pistol);

	//----------------------------------------------------------------------

	target_scout.SetFileId("tgt_hitbox_scout");
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(3, "HitScan", this, &target_scout);

	hc_scout.SetFileId("scout_hitchance");
	hc_scout.SetBoundaries(0, 100);
	hc_scout.SetValue(25);
	hc_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Hitchance", this, &hc_scout);

	md_scout.SetFileId("scout_minimumdamage");
	md_scout.SetBoundaries(0, 100);
	md_scout.SetValue(25);
	md_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Minimum Damage", this, &md_scout);

	//	headonly_if_vis_scout.SetFileId("headonly_if_vis_scout");
	//	weapongroup.PlaceLabledControl(3, "Headshot Only If Hittable", this, &headonly_if_vis_scout);

	//	preset_scout.SetFileId("scout_automatic_cfg");
	//	weapongroup.PlaceLabledControl(3, "Automatic Scout Configuration", this, &preset_scout);
	//----------------------------------------------------------------------

	target_awp.SetFileId("tgt_hitbox_awp");
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(4, "HitScan", this, &target_awp);

	hc_awp.SetFileId("awp_hitchance");
	hc_awp.SetBoundaries(0, 100);
	hc_awp.SetValue(25);
	hc_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Hitchance", this, &hc_awp);

	md_awp.SetFileId("awp_minimumdamage");
	md_awp.SetBoundaries(0, 125);
	md_awp.SetValue(25);
	md_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Minimum Damage", this, &md_awp);

	min_damage_enemy_hp_awp.SetFileId("tgt_min_damage_enemy_hp_awp");
	weapongroup.PlaceLabledControl(4, "Minimum Damage Based Off Enemy Health", this, &min_damage_enemy_hp_awp);

	//----------------------------------------------------------------------

	target_smg.SetFileId("tgt_hitbox_smg");
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(5, "HitScan", this, &target_smg);

	hc_smg.SetFileId("smg_hitchance");
	hc_smg.SetBoundaries(0, 100);
	hc_smg.SetValue(25);
	hc_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Hitchance", this, &hc_smg);

	md_smg.SetFileId("smg_minimumdamage");
	md_smg.SetBoundaries(0, 100);
	md_smg.SetValue(25);
	md_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Minimum Damage", this, &md_smg);

	//----------------------------------------------------------------------

	target_otr.SetFileId("tgt_hitbox_otr");
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(6, "HitScan", this, &target_otr);

	hc_otr.SetFileId("otr_hitchance");
	hc_otr.SetBoundaries(0, 100);
	hc_otr.SetValue(25);
	hc_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Hitchance", this, &hc_otr);

	md_otr.SetFileId("otr_minimumdamage");
	md_otr.SetBoundaries(0, 100);
	md_otr.SetValue(25);
	md_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Minimum Damage", this, &md_otr);

	//	headonly_if_vis_otr.SetFileId("headonly_if_vis_otr");
	//	weapongroup.PlaceLabledControl(5, "Headshot Only If Hittable", this, &headonly_if_vis_otr);

	//	preset_otr.SetFileId("otr_automatic_cfg");
	//	weapongroup.PlaceLabledControl(5, "Automatic  Weapon Configuration", this, &preset_otr);

	*/

	/*		target_auto.SetFileId("tgt_hitbox_auto");
	target_auto.AddItem("Head");
	target_auto.AddItem("Neck");
	target_auto.AddItem("Chest");
	target_auto.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(1, "Hitbox Priority", this, &target_auto); */

	target_auto2.SetFileId("tgt_hitscan_autosniper");
	target_auto2.AddItem("Off");
	target_auto2.AddItem("Minimal");
	target_auto2.AddItem("Essential");
	target_auto2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(1, "Hitscan", this, &target_auto2);

	hc_auto.SetFileId("auto_hitchance");
	hc_auto.SetBoundaries(0, 100);
	hc_auto.SetValue(25);
	hc_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Hitchance", this, &hc_auto);

	md_auto.SetFileId("auto_minimumdamage");
	md_auto.SetBoundaries(0, 100);
	md_auto.SetValue(25);
	md_auto.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(1, "Minimum Damage", this, &md_auto);

	//----------------------------------------------------------------------

	/*		target_pistol.SetFileId("tgt_hitbox_pistol");
	target_pistol.AddItem("Head");
	target_pistol.AddItem("Neck");
	target_pistol.AddItem("Chest");
	target_pistol.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(2, "Hitbox Priority", this, &target_pistol); */

	target_pistol2.SetFileId("tgt_hitscan_pistol");
	target_pistol2.AddItem("Off");
	target_pistol2.AddItem("Minimal");
	target_pistol2.AddItem("Essential");
	target_pistol2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(2, "Hitscan", this, &target_pistol2);

	hc_pistol.SetFileId("pistol_hitchance");
	hc_pistol.SetBoundaries(0, 100);
	hc_pistol.SetValue(25);
	hc_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Hitchance", this, &hc_pistol);

	md_pistol.SetFileId("pistol_minimumdamage");
	md_pistol.SetBoundaries(0, 100);
	md_pistol.SetValue(25);
	md_pistol.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(2, "Minimum Damage", this, &md_pistol);


	//----------------------------------------------------------------------

	/*		target_scout.SetFileId("tgt_hitbox_scout");
	target_scout.AddItem("Head");
	target_scout.AddItem("Neck");
	target_scout.AddItem("Chest");
	target_scout.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(3, "Hitbox Priority", this, &target_scout); */

	target_scout2.SetFileId("tgt_hitscan_scout");
	target_scout2.AddItem("Off");
	target_scout2.AddItem("Minimal");
	target_scout2.AddItem("Essential");
	target_scout2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(3, "Hitscan", this, &target_scout2);

	hc_scout.SetFileId("scout_hitchance");
	hc_scout.SetBoundaries(0, 100);
	hc_scout.SetValue(25);
	hc_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Hitchance", this, &hc_scout);

	md_scout.SetFileId("scout_minimumdamage");
	md_scout.SetBoundaries(0, 100);
	md_scout.SetValue(25);
	md_scout.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(3, "Minimum Damage", this, &md_scout);

	//----------------------------------------------------------------------

	/*		target_awp.SetFileId("tgt_hitbox_awp");
	target_awp.AddItem("Head");
	target_awp.AddItem("Neck");
	target_awp.AddItem("Chest");
	target_awp.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(4, "Hitbox Priority", this, &target_awp); */

	target_awp2.SetFileId("tgt_hitscan_awp");
	target_awp2.AddItem("Off");
	target_awp2.AddItem("Minimal");
	target_awp2.AddItem("Essential");
	target_awp2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(4, "Hitscan", this, &target_awp2);

	hc_awp.SetFileId("awp_hitchance");
	hc_awp.SetBoundaries(0, 100);
	hc_awp.SetValue(25);
	hc_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Hitchance", this, &hc_awp);

	md_awp.SetFileId("awp_minimumdamage");
	md_awp.SetBoundaries(0, 100);
	md_awp.SetValue(25);
	md_awp.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(4, "Minimum Damage", this, &md_awp);


	//----------------------------------------------------------------------

	/*		target_otr.SetFileId("tgt_hitbox_otr");
	target_otr.AddItem("Head");
	target_otr.AddItem("Neck");
	target_otr.AddItem("Chest");
	target_otr.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(5, "Hitbox Priority", this, &target_otr); */

	target_smg2.SetFileId("tgt_hitscan_smg");
	target_smg2.AddItem("Off");
	target_smg2.AddItem("Minimal");
	target_smg2.AddItem("Essential");
	target_smg2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(5, "Hitscan", this, &target_smg2);

	hc_smg.SetFileId("otr_hitchance");
	hc_smg.SetBoundaries(0, 100);
	hc_smg.SetValue(25);
	hc_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Hitchance", this, &hc_smg);

	md_smg.SetFileId("otr_minimumdamage");
	md_smg.SetBoundaries(0, 100);
	md_smg.SetValue(25);
	md_smg.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(5, "Minimum Damage", this, &md_smg);


	target_otr2.SetFileId("tgt_hitscan_otr");
	target_otr2.AddItem("Off");
	target_otr2.AddItem("Minimal");
	target_otr2.AddItem("Essential");
	target_otr2.AddItem("Maximal");
	weapongroup.PlaceLabledControl(6, "Hitscan", this, &target_otr2);

	hc_otr.SetFileId("otr_hitchance");
	hc_otr.SetBoundaries(0, 100);
	hc_otr.SetValue(25);
	hc_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Hitchance", this, &hc_otr);

	md_otr.SetFileId("otr_minimumdamage");
	md_otr.SetBoundaries(0, 100);
	md_otr.SetValue(25);
	md_otr.extension = XorStr("dmg");
	weapongroup.PlaceLabledControl(6, "Minimum Damage", this, &md_otr);


#pragma endregion  AntiAim controls get setup in here
}

void CLegitBotTab::Setup()
{


	SetTitle("B");
	AimbotGroup.SetText("Legitbot"); //fuck this
	AimbotGroup.SetPosition(4, 25);
	AimbotGroup.SetSize(608, 315);
	AimbotGroup.AddTab(CGroupTab("Aimbot", 1));
	AimbotGroup.AddTab(CGroupTab("Misc", 2));

	RegisterControl(&AimbotGroup);
	RegisterControl(&Active);
	Active.SetFileId("active");
	AimbotGroup.PlaceLabledControl(1, "Enable", this, &Active);

	AimbotEnable.SetFileId("legitaimbotenable");
	AimbotGroup.PlaceLabledControl(1, "Aimbot", this, &AimbotEnable);

	AimbotKeyPress.SetFileId("legitaimbotleyress");
	AimbotKeyPress.SetState(true);
	//AimbotGroup.PlaceLabledControl(1, "on key", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("legitaimbotaimkey");
	AimbotGroup.PlaceLabledControl(1, "Aimbot Key", this, &AimbotKeyBind);

	//AimbotAutoFire2.SetFileId("aim_autofire");
	//AimbotGroup.PlaceLabledControl(1, "auto fire", this, &AimbotAutoFire2);

	//AimbotFriendlyFire.SetFileId("aim_friendfire");
	//AimbotGroup.PlaceLabledControl(1, "friendly fire", this, &AimbotFriendlyFire);

	//AimbotAutoPistol.SetFileId("aim_apistol");
	//AimbotGroup.PlaceLabledControl(1, "Auto Pistol", this, &AimbotAutoPistol);

	AimbotSmokeCheck.SetFileId("aimbotsmokecheck");
	AimbotGroup.PlaceLabledControl(1, "Smoke Check", this, &AimbotSmokeCheck);

	legitbacktrack.SetFileId("legitbacktrack");
	AimbotGroup.PlaceLabledControl(2, "Backtrack", this, &legitbacktrack);

	legitbacktrackticks.SetFileId("legitbacktrackticks");
	legitbacktrackticks.SetBoundaries(0.0f, 12.0f);
	AimbotGroup.PlaceLabledControl(2, "Backtrack Ticks", this, &legitbacktrackticks);

	//StandAloneRCS.SetFileId("standalonercs");
	//AimbotGroup.PlaceLabledControl(2, "standalone rcs", this, &StandAloneRCS);

	//StandAloneRCSamount.SetFileId("standalonercsamount");
	//StandAloneRCSamount.SetBoundaries(0.0f, 2.0f);
	//AimbotGroup.PlaceLabledControl(2, "standalone rcs amount", this, &StandAloneRCSamount);

	visualizefov.SetFileId("visualizefov");
	AimbotGroup.PlaceLabledControl(2, "Visualize Fov", this, &visualizefov);

	/*ImGui::Text("Triggerbot Options");
	ImGui::Checkbox("Enabled", &yakuza::config.TriggerEnabled);
	ImGui::Hotkey("##TRIGGERKEY", &yakuza::config.triggerbotkey);
	ImGui::Checkbox("Hitboxes", &yakuza::config.triggerbones);
	if (yakuza::config.triggerbones == 1)
	{
		ImGui::Selectable("Head", &yakuza::config.triggerhead);
		ImGui::Selectable("Body", &yakuza::config.triggerbody);
		ImGui::Selectable("Arms", &yakuza::config.triggerarms);
		ImGui::Selectable("Legs", &yakuza::config.triggerlegs);
	}
	ImGui::Spacing();

	ImGui::SliderInt("  > delay", &yakuza::config.triggerdelay, 0, 20);
	//	ImGui::SliderFloat("Trigger hitchance", &g_Options.trigger_hc, 0, 100, "%.0f");
	}*/


	//visualizebacktrack.SetFileId("visualizebacktrack");
	//AimbotGroup.PlaceLabledControl(2, "visualize backtrack", this, &visualizebacktrack);







	//----------------------solid kys---------------------//

	weapongroup.SetText("Weapon Configs");
	weapongroup.SetPosition(4, 350);
	weapongroup.SetSize(608, 225);
	weapongroup.AddTab(CGroupTab("Rifle", 1));
	weapongroup.AddTab(CGroupTab("Pistol", 2));
	weapongroup.AddTab(CGroupTab("Sniper", 3));
	weapongroup.AddTab(CGroupTab("Smg", 4));
	RegisterControl(&weapongroup);
	RegisterControl(&Active);

	////rifle////
	WeaponMainHitbox.SetFileId("riflehitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	//WeaponMainHitbox.AddItem("All");
	weapongroup.PlaceLabledControl(1, "Hitbox", this, &WeaponMainHitbox);


	WeaponMainSpeed.SetFileId("riflespeed");
	WeaponMainSpeed.SetBoundaries(0.001f, 50.000f);
	weapongroup.PlaceLabledControl(1, "Speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("riflefov");
	WeaponMainFoV.SetBoundaries(0.00f, 10.00f);
	weapongroup.PlaceLabledControl(1, "Field Of View", this, &WeaponMainFoV);


	//WeaponMainRecoil.SetFileId("riflercs");
	//WeaponMainRecoil.SetBoundaries(0.00f, 2.00f);
	//weapongroup.PlaceLabledControl(1, "rcs", this, &WeaponMainRecoil);


	WeaponMainInacc.SetFileId("riflerandomize");
	WeaponMainInacc.SetBoundaries(0.00f, 15.00f);
	weapongroup.PlaceLabledControl(1, "Randomize", this, &WeaponMainInacc);




	// --- Pistols --- //
	WeaponPistHitbox.SetFileId("pistolhitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	//WeaponPistHitbox.AddItem("All");
	weapongroup.PlaceLabledControl(2, "Hitbox", this, &WeaponPistHitbox);


	WeaponPistSpeed.SetFileId("pistolspeed");
	WeaponPistSpeed.SetBoundaries(0.001f, 50.000f);
	weapongroup.PlaceLabledControl(2, "Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pistolfov");
	WeaponPistFoV.SetBoundaries(0.00f, 10.00f);
	weapongroup.PlaceLabledControl(2, "Field Of View", this, &WeaponPistFoV);


	WeaponPistPSilent.SetFileId("pistolpsilent");
	weapongroup.PlaceLabledControl(2, "Perfect Silent", this, &WeaponPistPSilent);


	// --- Sniper --- //
	WeaponSnipHitbox.SetFileId("sniperhitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	//WeaponSnipHitbox.AddItem("all");
	weapongroup.PlaceLabledControl(3, "Hitbox", this, &WeaponSnipHitbox);


	WeaponSnipSpeed.SetFileId("sniperspeed");
	WeaponSnipSpeed.SetBoundaries(0.001f, 50.000f);
	weapongroup.PlaceLabledControl(3, "Speed", this, &WeaponSnipSpeed);


	WeaponSnipFoV.SetFileId("sniperfov");
	WeaponSnipFoV.SetBoundaries(0.00f, 10.00f);
	weapongroup.PlaceLabledControl(3, "Field Of View", this, &WeaponSnipFoV);

	WeaponSnipPSilent.SetFileId("sniperpsilent");
	weapongroup.PlaceLabledControl(3, "Perfect Silent", this, &WeaponSnipPSilent);
	// --- SMG --- //
	WeaponMpHitbox.SetFileId("mphitbox");
	WeaponMpHitbox.AddItem("Head");
	WeaponMpHitbox.AddItem("Neck");
	WeaponMpHitbox.AddItem("Chest");
	WeaponMpHitbox.AddItem("Stomach");
	//WeaponMpHitbox.AddItem("All");
	weapongroup.PlaceLabledControl(4, "Hitbox", this, &WeaponMpHitbox);

	WeaponMpSpeed.SetFileId("mpspeed");
	WeaponMpSpeed.SetBoundaries(0.001f, 50.000f);
	weapongroup.PlaceLabledControl(4, "Speed", this, &WeaponMpSpeed);


	WeaponMpFoV.SetFileId("mpfov");
	WeaponMpFoV.SetBoundaries(0.00f, 10.00f);
	weapongroup.PlaceLabledControl(4, "Field Of View", this, &WeaponMpFoV);

	//WeaponMpRecoil.SetFileId("mprcs");
	//WeaponMpRecoil.SetBoundaries(0.00f, 2.00f);
	//weapongroup.PlaceLabledControl(4, "rcs", this, &WeaponMpRecoil);

	weaponmpinacc.SetFileId("mprandomize");
	weaponmpinacc.SetBoundaries(0.00f, 15.00f);
	weapongroup.PlaceLabledControl(4, "Randomize", this, &weaponmpinacc);

}

void CVisualTab::Setup()
{

	SetTitle("C");
#pragma region Options
	OptionsGroup.SetText("ESP");
	OptionsGroup.SetPosition(4, 30);
	OptionsGroup.SetSize(270, 545); //412
	OptionsGroup.AddTab(CGroupTab("Main", 1));
	OptionsGroup.AddTab(CGroupTab("Filters", 2));
	OptionsGroup.AddTab(CGroupTab("Chams", 3));
	OptionsGroup.AddTab(CGroupTab("Misc", 4));
	RegisterControl(&OptionsGroup);
	RegisterControl(&Active);

	Active.SetFileId("active");
	OptionsGroup.PlaceLabledControl(1, "Activate ESP", this, &Active);

	OptionsBox.SetFileId("opt_box");
	OptionsBox.AddItem("Off");
	OptionsBox.AddItem("Default");
	OptionsBox.AddItem("Genuine");
	OptionsGroup.PlaceLabledControl(1, "Box", this, &OptionsBox);

	OptionsHealth.SetFileId("opt_hp");
	OptionsHealth.AddItem("Off");
	OptionsHealth.AddItem("Default");
	OptionsHealth.AddItem("Battery");
	OptionsHealth.AddItem("Bottom");

	OptionsGroup.PlaceLabledControl(1, "Health", this, &OptionsHealth);

	OptionsArmor.SetFileId("otr_armor");
	OptionsArmor.AddItem("Off");
	OptionsArmor.AddItem("Default");
	OptionsArmor.AddItem("Battery");
	OptionsArmor.AddItem("Bottom");
	OptionsGroup.PlaceLabledControl(1, "Armor Bar", this, &OptionsArmor);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl(1, "Name", this, &OptionsName);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl(1, "Info", this, &OptionsInfo);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl(1, "Skeleton", this, &OptionsSkeleton);

	Weapons.SetFileId("kysquecest");
	OptionsGroup.PlaceLabledControl(1, "Weapons", this, &Weapons);

	Ammo.SetFileId("urmomsucksass");
	OptionsGroup.PlaceLabledControl(1, "Ammo Bar", this, &Ammo);

	//OffscreenESP.SetFileId("otr_offscreenESP");
	//OptionsGroup.PlaceLabledControl(1, "Offscreen ESP", this, &OffscreenESP);

	GlowZ.SetFileId("opt_glowz");
	GlowZ.SetValue(0.f);
	GlowZ.SetBoundaries(0.f, 100.f);
	GlowZ.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Enemy Glow", this, &GlowZ);

	team_glow.SetFileId("opt_team_glow");
	team_glow.SetValue(0.f);
	team_glow.SetBoundaries(0.f, 100.f);
	team_glow.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Team Glow", this, &team_glow);

	Glowz_lcl.SetFileId("opt_glowz_lcl");
	Glowz_lcl.SetValue(0.f);
	Glowz_lcl.SetBoundaries(0.f, 100.f);
	Glowz_lcl.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Local Glow", this, &Glowz_lcl);

	plistwindow.SetFileId("espwindow");
	OptionsGroup.PlaceLabledControl(1, "ESP Preview", this, &espwindow);

	//	debug_esp.SetFileId("opt_debug_Esp");
	//	OptionsGroup.PlaceLabledControl(1, "Debug Esp", this, &debug_esp);

	//	FiltersAll.SetFileId("ftr_all");
	//	OptionsGroup.PlaceLabledControl(2, "All", this, &FiltersAll);

	show_players.SetFileId("ftr_players");
	show_players.SetState(true);
	OptionsGroup.PlaceLabledControl(2, "Players", this, &show_players);

	show_team.SetFileId("ftr_enemyonly");
	OptionsGroup.PlaceLabledControl(2, "Show Team", this, &show_team);

	FiltersNades.SetFileId("ftr_nades");
	OptionsGroup.PlaceLabledControl(2, "Nades", this, &FiltersNades);

	FiltersC4.SetFileId("ftr_c4");
	OptionsGroup.PlaceLabledControl(2, "C4", this, &FiltersC4);

	//	show_hostage.SetFileId("ftr_hostage");
	//	OptionsGroup.PlaceLabledControl(2, "Hostage", this, &show_hostage);

	optimize.SetFileId("ftr_optimize");
	OptionsGroup.PlaceLabledControl(2, "Optimize Graphics", this, &optimize);

	WeaponFilterName.SetFileId("ftr_weapon_toggle");
	OptionsGroup.PlaceLabledControl(2, "Dropped Weapon Name", this, &WeaponFilterName);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersWeapons.AddItem("Off");
	FiltersWeapons.AddItem("Default");
	FiltersWeapons.AddItem("Genuine");
	FiltersWeapons.AddItem("Corners");
	OptionsGroup.PlaceLabledControl(2, "Dropped Weapon Box", this, &FiltersWeapons);

	//	asus_type.SetFileId("asus_wall_type");
	//	asus_type.AddItem("Props Only");
	//	asus_type.AddItem("Walls and Props");
	//	OptionsGroup.PlaceLabledControl(2, "Asus Type", this, &asus_type);


	//----------------------------------------------//

	ChamsEnemy.SetFileId("chams_enenmy_selection");
	ChamsEnemy.AddItem("Off");
	ChamsEnemy.AddItem("Visible Only");
	ChamsEnemy.AddItem("Always");
	OptionsGroup.PlaceLabledControl(3, "Enemies", this, &ChamsEnemy); // *1

	ChamsTeamVis.SetFileId("chams_team_selection");
	ChamsTeamVis.AddItem("Off");
	ChamsTeamVis.AddItem("Visible Only");
	ChamsTeamVis.AddItem("Always");
	OptionsGroup.PlaceLabledControl(3, "Team", this, &ChamsTeamVis);

	visible_chams_type.SetFileId("otr_visiblechams_type");
	visible_chams_type.AddItem("Normal");
	visible_chams_type.AddItem("Flat"); 
	visible_chams_type.AddItem("Pulse");
	visible_chams_type.AddItem("Crystal");
	visible_chams_type.AddItem("Glass");
	OptionsGroup.PlaceLabledControl(3, "Visible Chams Type", this, &visible_chams_type);

	invisible_chams_type.SetFileId("otr_invisiblechams_type");
	invisible_chams_type.AddItem("Normal");
	invisible_chams_type.AddItem("Flat"); 
	invisible_chams_type.AddItem("Pulse");
	invisible_chams_type.AddItem("Crystal");
	invisible_chams_type.AddItem("Glass");
	OptionsGroup.PlaceLabledControl(3, "Invisible Chams Type", this, &invisible_chams_type);

	ChamsLocal.SetFileId("chams_local");
	OptionsGroup.PlaceLabledControl(3, "Apply Local Chams", this, &ChamsLocal);

	/*fakelag_ghost.SetFileId("chams_desyncyaw");
	OptionsGroup.PlaceLabledControl(3, "Server Angle Ghost", this, &fakelag_ghost);*/

	localmaterial.SetFileId("esp_localscopedmat");
	localmaterial.AddItem("Default");
	localmaterial.AddItem("Clear");
	localmaterial.AddItem("Cham");
	localmaterial.AddItem("Wireframe");
	localmaterial.AddItem("LSD");
	localmaterial.AddItem("Glass");
	localmaterial.AddItem("Pulse");
	OptionsGroup.PlaceLabledControl(3, "Scoped Materials", this, &localmaterial);

	HandCHAMS.SetFileId("chams_local_hand");
	HandCHAMS.AddItem("Off");
	HandCHAMS.AddItem("Simple");
	HandCHAMS.AddItem("Wireframe");
	HandCHAMS.AddItem("Golden");
	HandCHAMS.AddItem("Glass");
	HandCHAMS.AddItem("Crystal");
	HandCHAMS.AddItem("Pulse");
	OptionsGroup.PlaceLabledControl(3, "Hand Chams", this, &HandCHAMS);

	GunCHAMS.SetFileId("chams_local_weapon");
	GunCHAMS.AddItem("Off");
	GunCHAMS.AddItem("Simple");
	GunCHAMS.AddItem("Wireframe");
	GunCHAMS.AddItem("Golden");
	GunCHAMS.AddItem("Glass");
	GunCHAMS.AddItem("Crystal");
	GunCHAMS.AddItem("Pulse");
	OptionsGroup.PlaceLabledControl(3, "Weapon Chams", this, &GunCHAMS);

	SleeveChams.SetFileId("remove_Sleeve");
	SleeveChams.AddItem("Off");
	SleeveChams.AddItem("Simple");
	SleeveChams.AddItem("Wireframe");
	SleeveChams.AddItem("Golden");
	SleeveChams.AddItem("Glass");
	SleeveChams.AddItem("Crystal");
	SleeveChams.AddItem("Pulse");
	SleeveChams.AddItem("Invisible");
	OptionsGroup.PlaceLabledControl(3, "Sleeve Chams", this, &SleeveChams);

	/*
	fakelag_ghost.SetFileId("otr_fakelag_ghost");
	fakelag_ghost.AddItem("Off");
	fakelag_ghost.AddItem("Default");
	fakelag_ghost.AddItem("Pulse");
	OptionsGroup.PlaceLabledControl(3, "Fake Lag Ghost", this, &fakelag_ghost);
	*/


	transparency.SetFileId("esp_transparency");
	transparency.SetBoundaries(0, 100);
	transparency.SetValue(20);
	transparency.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(3, "Scoped Transparency", this, &transparency);


	hand_transparency.SetFileId("esp_hand_transparency");
	hand_transparency.SetBoundaries(0, 100);
	hand_transparency.SetValue(20);
	hand_transparency.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(3, "Arm Transparency", this, &hand_transparency);

	gun_transparency.SetFileId("esp_gun_transparency");
	gun_transparency.SetBoundaries(0, 100);
	gun_transparency.SetValue(20);
	gun_transparency.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(3, "Gun Transparency", this, &gun_transparency);

	sleeve_transparency.SetFileId("esp_sleeve_transparency");
	sleeve_transparency.SetBoundaries(0, 100);
	sleeve_transparency.SetValue(20);
	sleeve_transparency.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(3, "Sleeve Transparency", this, &sleeve_transparency);

	blend_local.SetFileId("esp_teamblend");
	blend_local.SetBoundaries(0, 100);
	blend_local.SetValue(75);
	blend_local.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(3, "Local Player Chams Blend", this, &blend_local);

	//SniperCrosshair.SetFileId("SniperCrosshair");
	//OptionsGroup.PlaceLabledControl(3, "Sniper Crosshair", this, &SniperCrosshair);

	//bulletbeam.SetFileId("otr_bullet_beam");
	//OptionsGroup.PlaceLabledControl(3, "Bullet Beam", this, &bulletbeam);

	//BulletTrace.SetFileId("otr_bullet_tracers_local");
	//OptionsGroup.PlaceLabledControl(3, "Local Bullet Tracers", this, &BulletTrace);

	//BulletTrace_enemy.SetFileId("otr_bullet_tracers_enemy");
	//OptionsGroup.PlaceLabledControl(3, "Enemy Bullet Tracers", this, &BulletTrace_enemy);


	/*SpreadCrosshair.SetFileId(XorStr("v_spreadcrosshair"));
	SpreadCrosshair.AddItem("Off");
	SpreadCrosshair.AddItem("Standard");
	SpreadCrosshair.AddItem("Color");
	SpreadCrosshair.AddItem("Rainbow");
	SpreadCrosshair.AddItem("Rainbow Rotate");
	OptionsGroup.PlaceLabledControl(4, XorStr("Spread Crosshair"), this, &SpreadCrosshair);*/

	/*	SpreadCrossSize.SetFileId("otr_spreadcross_size");
	SpreadCrossSize.SetBoundaries(1.f, 100.f); //we should take smth like 650 as max so i guess *6.5?
	SpreadCrossSize.extension = XorStr("%%");
	SpreadCrossSize.SetValue(45.f);
	OptionsGroup.PlaceLabledControl(3, "Size", this, &SpreadCrossSize); */

	crosshair.SetFileId("otr_crosshair");
	OptionsGroup.PlaceLabledControl(4, "Crosshair", this, &crosshair);

	OtherNoScope.SetFileId("otr_noscope");
	OptionsGroup.PlaceLabledControl(4, "Remove Scope", this, &OtherNoScope);

	RemoveZoom.SetFileId("otr_remv_zoom");
	OptionsGroup.PlaceLabledControl(4, "Remove Zoom", this, &RemoveZoom);

	nosmoke.SetFileId("otr_nosmoke");
	OptionsGroup.PlaceLabledControl(4, "Remove Smoke", this, &nosmoke);
	
	OtherNoFlash.SetFileId("otr_noflash");
	OptionsGroup.PlaceLabledControl(4, "Remove Flash", this, &OtherNoFlash);

	/*	nosmoke_slider.SetFileId("otr_nosmoke_alpha");
	nosmoke_slider.SetBoundaries(0, 100);
	nosmoke_slider.extension = ("%%");
	nosmoke_slider.SetValue(10);
	OptionsGroup.PlaceLabledControl(4, "Smoke Alpha", this, &nosmoke_slider); */

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OptionsGroup.PlaceLabledControl(4, "No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherThirdperson.SetFileId("aa_1thirpbind");
	OptionsGroup.PlaceLabledControl(4, "Thirdperson", this, &OtherThirdperson);
	ThirdPersonKeyBind.SetFileId("aa_thirpbind");
	OptionsGroup.PlaceLabledControl(4, "", this, &ThirdPersonKeyBind);

	flashAlpha.SetFileId("otr_stolen_from_punknown_muahahaha");
	flashAlpha.SetBoundaries(0, 100);
	flashAlpha.extension = XorStr("%%");
	flashAlpha.SetValue(10);
	OptionsGroup.PlaceLabledControl(4, "Flash Alpha", this, &flashAlpha);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(30.f, 120.f);
	OtherViewmodelFOV.SetValue(90.f);
	OptionsGroup.PlaceLabledControl(4, "Viewmodel FOV", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 50.f);
	OtherFOV.SetValue(0.f);
	OptionsGroup.PlaceLabledControl(4, "Override FOV", this, &OtherFOV);

	override_viewmodel.SetFileId("otr_override_viewmodel_offset");
	OptionsGroup.PlaceLabledControl(4, "Override Viewmodel Offset", this, &override_viewmodel);

	offset_x.SetFileId("otr_offset_x");
	offset_x.SetBoundaries(-12, 12);
	offset_x.SetValue(2.5);
	OptionsGroup.PlaceLabledControl(4, "Offset X", this, &offset_x);

	offset_y.SetFileId("otr_offset_Y");
	offset_y.SetBoundaries(-12, 12);
	offset_y.SetValue(2.0);
	OptionsGroup.PlaceLabledControl(4, "Offset Y", this, &offset_y);

	offset_z.SetFileId("otr_offset_z");
	offset_z.SetBoundaries(-12, 12);
	offset_z.SetValue(-2.0);
	OptionsGroup.PlaceLabledControl(4, "Offset Z", this, &offset_z);

	owo_slider.SetFileId("owo_slider");
	owo_slider.SetBoundaries(0, 100);
	owo_slider.SetValue(100);
	owo_slider.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(4, XorStr("Brightness"), this, &owo_slider);

	//----------------------------------------------//

	OptionsGroup2.SetText("Other");
	OptionsGroup2.AddTab(CGroupTab("Main", 1));
	OptionsGroup2.AddTab(CGroupTab("Indicator", 2));
	OptionsGroup2.SetPosition(286, 30); // 225, 30
	OptionsGroup2.SetSize(326, 243);
	RegisterControl(&OptionsGroup2);


	/*
	beamtime.SetFileId("otr_beamtime");
	beamtime.SetBoundaries(1.f, 5.f);
	beamtime.SetValue(2.f);
	OptionsGroup.PlaceLabledControl(4, "Beam Time", this, &beamtime);
	*/

	OtherHitmarker.SetFileId("otr_hitmarker");
	OptionsGroup2.PlaceLabledControl(1, "Hitmarker", this, &OtherHitmarker);

	killfeed.SetFileId("otr_killfeed");
	OptionsGroup2.PlaceLabledControl(1, "Preserve Killfeed", this, &killfeed);

	leftknife.SetFileId("leftknife");
	OptionsGroup2.PlaceLabledControl(1, "Left Hand Knife", this, &leftknife);

	cheatinfo.SetFileId("cheatinfox");
	OptionsGroup2.PlaceLabledControl(1, "Debug Info", this, &cheatinfo);

	CompRank.SetFileId("otr_reveal__rank");
	OptionsGroup2.PlaceLabledControl(1, "Rank Reveal", this, &CompRank);

	watermark.SetFileId("otr_watermark");
	watermark.SetState(true);
	OptionsGroup2.PlaceLabledControl(1, "Watermark", this, &watermark);

	BulletImpacts.SetFileId("opt_bulletimpacts");
	OptionsGroup2.PlaceLabledControl(1, "Bullet Impacts", this, &BulletImpacts);

	DebugLagComp.SetFileId(XorStr("lagcompensationyes"));
	OptionsGroup2.PlaceLabledControl(1, XorStr("Draw Lag Compensation"), this, &DebugLagComp);

	BackTrackBones2.SetFileId(XorStr("spookybonesOwOomg"));
	OptionsGroup2.PlaceLabledControl(1, XorStr("Backtrack"), this, &BackTrackBones2);

	OtherEntityGlow.SetFileId("otr_world_ent_glow");
	OptionsGroup2.PlaceLabledControl(1, "World Entity Glow", this, &OtherEntityGlow);

	logs.SetFileId("otr_skeetpaste");
	logs.AddItem("Off");
	logs.AddItem("Default");
	logs.AddItem("Coloured");
	OptionsGroup2.PlaceLabledControl(1, "Event Log", this, &logs);

	damageindi.SetFileId("DamageIndicator");
	OptionsGroup2.PlaceLabledControl(2, "Damage Indicator", this, &damageindi);

	FakeDuckIndicator.SetFileId("otr_FakeDuckIndicator");
	FakeDuckIndicator.AddItem("Off");
	FakeDuckIndicator.AddItem("Classic");
	FakeDuckIndicator.AddItem("Genuine");
	OptionsGroup2.PlaceLabledControl(2, "FakeDuck Indicator", this, &FakeDuckIndicator);

	LCIndicator.SetFileId("otr_LCIndicator");
	LCIndicator.AddItem("Off");
	LCIndicator.AddItem("Classic");
	LCIndicator.AddItem("Genuine");
	OptionsGroup2.PlaceLabledControl(2, "LagComp Indicator", this, &LCIndicator);

	fake_indicator.SetFileId("otr_desync_indicator");
	fake_indicator.AddItem("Off");
	fake_indicator.AddItem("Classic");
	fake_indicator.AddItem("Genuine");
	OptionsGroup2.PlaceLabledControl(2, "Desync Indicator", this, &fake_indicator);

	manualaa_type.SetFileId("manualaa");
	manualaa_type.AddItem("Off");
	manualaa_type.AddItem("Single Arrow");
	manualaa_type.AddItem("All Arrows");
	OptionsGroup2.PlaceLabledControl(2, "Manual Indicator", this, &manualaa_type);


	worldgroup.SetText("World");
	worldgroup.AddTab(CGroupTab("Main", 1));
	//worldgroup.AddTab(CGroupTab("Sky", 2));
	worldgroup.SetPosition(286, 280);  // 225, 285
	worldgroup.SetSize(326, 295);
	RegisterControl(&worldgroup);
	colmodupdate.SetFileId("otr_night");
	worldgroup.PlaceLabledControl(1, "Force update Materials", this, &colmodupdate);

	customskies.SetFileId("otr_skycustom");
	customskies.AddItem("Default");
	customskies.AddItem("Night");
	customskies.AddItem("NoSky");
	customskies.AddItem("Galaxy");

	worldgroup.PlaceLabledControl(1, "Change Sky", this, &customskies);

	colmod.SetFileId("night_amm");
	colmod.SetBoundaries(000.000f, 100.00f);
	colmod.extension = XorStr("%%");
	colmod.SetValue(020.0f);
	worldgroup.PlaceLabledControl(1, "Brightness Percentage", this, &colmod);

	asusamount.SetFileId("otr_asusprops");
	asusamount.SetBoundaries(1.f, 100.f);
	asusamount.extension = XorStr("%%");
	asusamount.SetValue(95.f);
	worldgroup.PlaceLabledControl(1, "Asus Percantage", this, &asusamount);

	ModulateSkyBox.SetFileId(XorStr("sky_box_color_enable"));
	worldgroup.PlaceLabledControl(1, XorStr("Enable Sky Color Changer"), this, &ModulateSkyBox);

	sky_r.SetFileId("sky_r");
	sky_r.SetBoundaries(0.f, 25.f);
	sky_r.SetValue(10.f);
	worldgroup.PlaceLabledControl(1, "Sky: Red", this, &sky_r);

	sky_g.SetFileId("sky_g");
	sky_g.SetBoundaries(0.f, 25.f);
	sky_g.SetValue(1.f);
	worldgroup.PlaceLabledControl(1, "Sky: Green", this, &sky_g);

	sky_b.SetFileId("sky_b");
	sky_b.SetBoundaries(0.f, 25.f);
	sky_b.SetValue(20.f);
	worldgroup.PlaceLabledControl(1, "Sky: Blue", this, &sky_b);

#pragma endregion Setting up the Other controls
}
void CMiscTab::Setup()
{
	/*
	__  __ _____  _____  _____
	|  \/  |_   _|/ ____|/ ____|
	| \  / | | | | (___ | |
	| |\/| | | |  \___ \| |
	| |  | |_| |_ ____) | |____
	|_|  |_|_____|_____/ \_____|


	*/
	SetTitle("D");
#pragma region Other
	OtherGroup.SetText("Miscellaneous");
	OtherGroup.AddTab(CGroupTab("Main", 1));
	//OtherGroup.AddTab(CGroupTab("BuyBot", 2));
	OtherGroup.SetPosition(4, 30);
	OtherGroup.SetSize(270, 545);
	RegisterControl(&OtherGroup);


	infinite_duck.SetFileId("infinteduck");
	OtherGroup.PlaceLabledControl(1, "Infinite Duck", this, &infinite_duck);

	namechangerr.SetFileId("misc_namechangerrrr");
	namechangerr.AddItem("None");
	namechangerr.AddItem("Fake Kick");
	namechangerr.AddItem("Name Stealer");
	namechangerr.AddItem("Divinity");
	OtherGroup.PlaceLabledControl(1, "Namechanger", this, &namechangerr);

	ChatSpam.SetFileId("misc_chatspam");
	ChatSpam.AddItem("None");
	ChatSpam.AddItem("Divinity");
	OtherGroup.PlaceLabledControl(1, "Chat Spam", this, &ChatSpam);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceLabledControl(1, "BunnyHop", this, &OtherAutoJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherGroup.PlaceLabledControl(1, "Air Strafe", this, &OtherAutoStrafe);

	autojump_type.SetFileId("misc_autojump_type");
	autojump_type.AddItem("Normal");
	autojump_type.AddItem("Always On");
	OtherGroup.PlaceLabledControl(1, "BunnyHop Type", this, &autojump_type);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.AddItem("Anti Untrusted");
	OtherSafeMode.AddItem("Anti VAC Kick");
	OtherSafeMode.AddItem("Danger Zone");
	OtherSafeMode.AddItem("Unrestricted (!)");
	OtherGroup.PlaceLabledControl(1, "Safety Mode", this, &OtherSafeMode);

	hitmarker_sound.SetFileId("hitmarker_sound");
	hitmarker_sound.AddItem("Off");
	hitmarker_sound.AddItem("Cod");
	hitmarker_sound.AddItem("Gamesense");
	hitmarker_sound.AddItem("Bubble");
	hitmarker_sound.AddItem("Bameware");
	hitmarker_sound.AddItem("Bell"); // diiiing	
									 //	hitmarker_sound.AddItem("Light Switch");
	hitmarker_sound.AddItem("User Custom (csgo sound folder)");
	OtherGroup.PlaceLabledControl(1, "Hitmarker Sound", this, &hitmarker_sound);

	/*Radar.SetFileId("Radar");
	OtherGroup.PlaceLabledControl(1, "Draw Radar", this, &Radar);

	RadarX.SetFileId("misc_radar_xax1");
	RadarX.SetBoundaries(0, 1920);
	RadarX.SetValue(0);
	OtherGroup.PlaceLabledControl(1, "X-Axis", this, &RadarX);

	RadarY.SetFileId("misc_radar_yax2");
	RadarY.SetBoundaries(0, 1080);
	RadarY.SetValue(0);
	OtherGroup.PlaceLabledControl(1, "Y-Axis", this, &RadarY);*/

	killsay.SetFileId("misc_killsay");
	killsay.items.push_back(dropdownboxitem(false, XorStr("On Kill")));
	killsay.items.push_back(dropdownboxitem(false, XorStr("On Death")));
	OtherGroup.PlaceLabledControl(1, "Trash Talk", this, &killsay);

	ClanTag.SetFileId("otr_clantg");
	OtherGroup.PlaceLabledControl(1, XorStr("Clan Tag Spammer"), this, &ClanTag);

	//ClanTag.SetFileId("misc_clantg");
	//OtherGroup.PlaceLabledControl(1, XorStr("Gamesense Clan Tag"), this, &ClanTag2);

	fakeping.SetFileId("misc_fakeping");
	OtherGroup.PlaceLabledControl(1, "Fake Ping", this, &fakeping);

	fakepingvalue.SetFileId("misc_fakepingvalue");
	fakepingvalue.SetBoundaries(0.00f, 200.00f);
	OtherGroup.PlaceLabledControl(1, "Fake Ping Value", this, &fakepingvalue);

	buybot_primary.SetFileId("buybot_primary");
	buybot_primary.AddItem("Off");
	buybot_primary.AddItem("Auto-Sniper");
	buybot_primary.AddItem("Scout");
	buybot_primary.AddItem("Awp");
	buybot_primary.AddItem("Ak-47 / M4");
	buybot_primary.AddItem("Aug / Sg553");
	buybot_primary.AddItem("Mp9 / Mac10");
	OtherGroup.PlaceLabledControl(1, "Primary Weapon", this, &buybot_primary);

	buybot_secondary.SetFileId("buybot_secondary");
	buybot_secondary.AddItem("Off");
	buybot_secondary.AddItem("Dual Berretas");
	buybot_secondary.AddItem("Revolver / Desert Eagle");
	buybot_secondary.AddItem("Five-Seven / Cz75");
	OtherGroup.PlaceLabledControl(1, "Secondary Weapon", this, &buybot_secondary);

	buybot_otr.SetFileId("buybot_other");
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("kevlar")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("he-grenade")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("flashbang")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("smoke grenade")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("molotov")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("zeus")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("defuse-kit")));
	OtherGroup.PlaceLabledControl(1, "Others", this, &buybot_otr);


	AntiAimGroup.SetPosition(286, 30);
	AntiAimGroup.SetText("Anti-Aimbot Angles");
	AntiAimGroup.SetSize(326, 545);
	AntiAimGroup.AddTab(CGroupTab("Main", 1));
	AntiAimGroup.AddTab(CGroupTab("Fakelag", 2));
	AntiAimGroup.AddTab(CGroupTab("Misc", 3));
	//AntiAimGroup.AddTab(CGroupTab("Desync", 4));
	RegisterControl(&AntiAimGroup);
	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl(1, "Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("Off");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("Up");
	AntiAimPitch.AddItem("Jitter");
	AntiAimPitch.AddItem("Random");
	AntiAimPitch.AddItem("Zero");
	AntiAimGroup.PlaceLabledControl(1, "Pitch", this, &AntiAimPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("Off");
	AntiAimYaw.AddItem("Backward");
	AntiAimYaw.AddItem("Manual");
	AntiAimYaw.AddItem("Crooked");
	AntiAimYaw.AddItem("Freestanding");
	AntiAimYaw.AddItem("Jitter 180");
	AntiAimYaw.AddItem("Random Lowerbody");
	//	AntiAimYaw.AddItem("Twist");
	AntiAimGroup.PlaceLabledControl(1, "Standing Yaw", this, &AntiAimYaw);

	stand_jitter.SetFileId("c_addjitter_stand");
	stand_jitter.SetBoundaries(0.f, 90.f);
	stand_jitter.SetValue(0.f);
	AntiAimGroup.PlaceLabledControl(1, "Jitter", this, &stand_jitter);

	AntiAimYawrun.SetFileId("aa_y2");
	AntiAimYawrun.AddItem("Off");
	AntiAimYawrun.AddItem("Backward");
	AntiAimYawrun.AddItem("Manual");
	AntiAimYawrun.AddItem("Crooked");
	AntiAimYawrun.AddItem("Freestanding");
	AntiAimYawrun.AddItem("180 Jitter");
	AntiAimYawrun.AddItem("Random Lowerbody");
	AntiAimGroup.PlaceLabledControl(1, "Moving Yaw", this, &AntiAimYawrun);

	move_jitter.SetFileId("c_addjitter_move");
	move_jitter.SetBoundaries(0.f, 90.f);
	move_jitter.SetValue(0.f);
	AntiAimGroup.PlaceLabledControl(1, "Jitter", this, &move_jitter);

	AntiAimYaw3.SetFileId("aa_y3");
	AntiAimYaw3.AddItem("Off");
	AntiAimYaw3.AddItem("Backward");
	AntiAimYaw3.AddItem("Manual");
	AntiAimYaw3.AddItem("Crooked");
	AntiAimYaw3.AddItem("Freestanding");
	AntiAimYaw3.AddItem("180 Jitter");
	AntiAimYaw3.AddItem("Random Lowerbody");
	AntiAimGroup.PlaceLabledControl(1, "In Air Yaw", this, &AntiAimYaw3);

	desync_aa_stand.SetFileId("v_desync_aa_stand");
	AntiAimGroup.PlaceLabledControl(1, "Standing Desync", this, &desync_aa_stand);

	desync_type_stand.SetFileId("desync_type_stand");
	desync_type_stand.AddItem("Static");
	desync_type_stand.AddItem("Jitter");
	desync_type_stand.AddItem("Manual Stretch");
	desync_type_stand.AddItem("Extend");
	desync_type_stand.AddItem("Random");
	AntiAimGroup.PlaceLabledControl(1, "Standing Type", this, &desync_type_stand);

	//	desync_range_stand.SetFileId("desync_range_standing");
	//	desync_range_stand.SetBoundaries(0, 58);
	//	desync_range_stand.SetValue(40);
	//	AntiAimGroup.PlaceLabledControl(1, "Standing Range", this, &desync_range_stand);

	//	desync_swapsides_stand.SetFileId("desync_swapsides_stand");
	//	AntiAimGroup.PlaceLabledControl(1, "Swap Sides", this, &desync_swapsides_stand);

	desync_aa_move.SetFileId("v_desync_aa_move");
	AntiAimGroup.PlaceLabledControl(1, "Moving Desync", this, &desync_aa_move);

	desync_type_move.SetFileId("desync_type_moving");
	desync_type_move.AddItem("Static");
	desync_type_move.AddItem("Jitter");
	desync_type_move.AddItem("Manual Stretch");
	desync_type_move.AddItem("Extend");
	desync_type_move.AddItem("Random");
	AntiAimGroup.PlaceLabledControl(1, "Moving Type", this, &desync_type_move);

	//	desync_range_move.SetFileId("desync_range_move");
	//	desync_range_move.SetBoundaries(0, 58);
	//	desync_range_move.SetValue(40);
	//	AntiAimGroup.PlaceLabledControl(1, "Moving Range", this, &desync_range_move);

		//	air_desync.SetFileId("v_air_desync");
	//	AntiAimGroup.PlaceLabledControl(1, "Air Desync", this, &air_desync);

	desync_twist_onshot.SetFileId("desync_twist_onshot");
	AntiAimGroup.PlaceLabledControl(1, "On Shot Desync", this, &desync_twist_onshot);

	//	pitch_up.SetFileId("pitch_up");
	//	AntiAimGroup.PlaceLabledControl(1, "Pitch Flick", this, &pitch_up);

	antilby.SetFileId("otr_meh");
	//	antilby.AddItem("Off");
	//	antilby.AddItem("One Flick");
	//	antilby.AddItem("Two Flicks");
	//	antilby.AddItem("Relative");
	AntiAimGroup.PlaceLabledControl(1, "Anti-LBY", this, &antilby);

	disable_on_dormant.SetFileId("disable_on_dormant");
	AntiAimGroup.PlaceLabledControl(1, "Disable On Dormant", this, &disable_on_dormant);

	//	BreakLBYDelta2.SetFileId("b_antilby2");
	//	BreakLBYDelta2.SetBoundaries(-180, 180);
	//	BreakLBYDelta2.SetValue(90);
	//	AntiAimGroup.PlaceLabledControl(2, "Anti-LBY First Flick", this, &BreakLBYDelta2);

	//	BreakLBYDelta.SetFileId("b_antilby");
	//	BreakLBYDelta.SetBoundaries(-180, 180);
	//	BreakLBYDelta.SetValue(-90);
	//	AntiAimGroup.PlaceLabledControl(1, "Anti-LBY Range", this, &BreakLBYDelta);

	//	freerange.SetFileId("freestanding_range");
	//	freerange.SetBoundaries(0, 90);
	//	freerange.SetValue(35);
	//	AntiAimGroup.PlaceLabledControl(1, "Freestanding Value", this, &freerange);

	//preset_aa.SetFileId("preset_aa");
	//AntiAimGroup.PlaceLabledControl(1, "Pre-set AntiAim", this, &preset_aa);

	//choked_shot.SetFileId("choke_shot");
	//AntiAimGroup.PlaceLabledControl(1, "Choke Shot", this, &choked_shot);
	//-<------------------------------------->-//

	FakelagStand.SetFileId("fakelag_stand_val");
	FakelagStand.SetBoundaries(1, 14);
	FakelagStand.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Standing", this, &FakelagStand);

	FakelagMove.SetFileId("fakelag_move_val");
	FakelagMove.SetBoundaries(1, 14);
	FakelagMove.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Moving", this, &FakelagMove);

	Fakelagjump.SetFileId("fakelag_jump_val");
	Fakelagjump.SetBoundaries(1, 14);
	Fakelagjump.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag In Air", this, &Fakelagjump);

	fl_spike.SetFileId("fakelag_spike");
	fl_spike.AddItem("Off");
	fl_spike.AddItem("Default");
	fl_spike.AddItem("Enemy Sight");
	fl_spike.AddItem("Divinity Adaptive");
	fl_spike.AddItem("Aimware Adaptive");
	fl_spike.AddItem("Velocity Based");
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Factor", this, &fl_spike);

	fakelag_key.SetFileId("fakelag_onkey");
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Spike Key", this, &fakelag_key);

	FakelagBreakLC.SetFileId("fakelag_breaklc");
	AntiAimGroup.PlaceLabledControl(2, "Break Lag Compensation", this, &FakelagBreakLC);

	//	auto_fakelag.SetFileId("fakelag_auto");
	//	AntiAimGroup.PlaceLabledControl(2, "Dynamic Fakelag", this, &auto_fakelag);

	fake_crouch.SetFileId("fake_crouch");
	AntiAimGroup.PlaceLabledControl(3, "Fake Duck", this, &fake_crouch);

	fake_crouch_key.SetFileId("fake_crouch_key");
	AntiAimGroup.PlaceLabledControl(3, "Fake Duck Key", this, &fake_crouch_key);

	minimal_walk.SetFileId("minimal_walk");
	AntiAimGroup.PlaceLabledControl(3, "Slow Motion Key", this, &minimal_walk);

	QuickStop.SetFileId("acc_quickstop");
	QuickStop.AddItem("Off");
	QuickStop.AddItem("Default");
	QuickStop.AddItem("Slow Motion");
	AntiAimGroup.PlaceLabledControl(3, "Quickstop", this, &QuickStop);

	QuickCrouch.SetFileId("acc_quickcrouch");
	AntiAimGroup.PlaceLabledControl(3, "Quickcrouch", this, &QuickCrouch);

	manualleft.SetFileId("otr_keybasedleft");
	AntiAimGroup.PlaceLabledControl(3, "Manual Right", this, &manualleft);

	manualright.SetFileId("otr_keybasedright");
	AntiAimGroup.PlaceLabledControl(3, "Manual Left", this, &manualright);

	manualback.SetFileId("otr_keybasedback");
	AntiAimGroup.PlaceLabledControl(3, "Manual Back", this, &manualback);

	manualfront.SetFileId("otr_manualfrontk");
	AntiAimGroup.PlaceLabledControl(3, "Manual Front", this, &manualfront);

	//fw.SetFileId("fakewalk_key");
	//AntiAimGroup.PlaceLabledControl(3, "FakeWalk Key", this, &fw);


	randlbyr.SetFileId("b_randlbyr");
	randlbyr.SetBoundaries(20, 180);
	randlbyr.SetValue(60);
	AntiAimGroup.PlaceLabledControl(3, "Random Lowerbody Ammount", this, &randlbyr);
	//-<------------------------------------->-//
}

void CSkinTab::Setup()
{
	SetTitle("E");
	knifegroup.SetText("Knives");
	knifegroup.SetPosition(4, 30);
	//knifegroup.AddTab(CGroupTab("Terrorist", 1));
	//knifegroup.AddTab(CGroupTab("Counter-Terrorist", 2));
	knifegroup.SetSize(608, 545);
	RegisterControl(&knifegroup);

#pragma region Terrorist ( to be replaced with team based stuff )
	t_knife_index.SetFileId("t_knife_index");
	t_knife_index.AddItem("Off");
	t_knife_index.AddItem("Bayonet");
	t_knife_index.AddItem("M9 Bayonet");
	t_knife_index.AddItem("Butterfly");
	t_knife_index.AddItem("Flip");
	t_knife_index.AddItem("Gut");
	t_knife_index.AddItem("Karambit");
	t_knife_index.AddItem("Huntsman");
	t_knife_index.AddItem("Falchion");
	t_knife_index.AddItem("Bowie");
	t_knife_index.AddItem("Shadow");
	t_knife_index.AddItem("Talon");
	t_knife_index.AddItem("Stiletto");
	t_knife_index.AddItem("Ursus");
	knifegroup.PlaceLabledControl(0, XorStr("Model"), this, &t_knife_index);

	t_knife_wear.SetFileId("t_knife_seed");
	t_knife_wear.SetBoundaries(1, 100);
	t_knife_wear.SetValue(1);
	t_knife_wear.extension = XorStr("%%");
	knifegroup.PlaceLabledControl(0, XorStr("Wear"), this, &t_knife_wear);

	t_knife_skin_id.SetFileId("t_knife_skin");
	t_knife_skin_id.AddItem("Default");
	t_knife_skin_id.AddItem("Ruby");
	t_knife_skin_id.AddItem("Sapphire");
	t_knife_skin_id.AddItem("Black Pearl");
	t_knife_skin_id.AddItem("Doppler");
	t_knife_skin_id.AddItem("Fade");
	t_knife_skin_id.AddItem("Marble Fade");
	t_knife_skin_id.AddItem("Gamma Doppler");
	t_knife_skin_id.AddItem("Emerald");
	t_knife_skin_id.AddItem("Slaughter");
	t_knife_skin_id.AddItem("Whiteout");
	t_knife_skin_id.AddItem("Ultraviolet");
	t_knife_skin_id.AddItem("Lore (M9)");
	knifegroup.PlaceLabledControl(0, XorStr("Skin"), this, &t_knife_skin_id);


	//-----------------
	/*
	snipergroup.SetText("Snipers");
	snipergroup.SetPosition(284, 30);
	snipergroup.AddTab(CGroupTab("Auto", 1));
	snipergroup.AddTab(CGroupTab("Bolt Action", 2));
	snipergroup.SetSize(270, 170);
	RegisterControl(&snipergroup);

	t_sniperSCAR_skin_id.SetFileId("t_scar20_skin");
	t_sniperSCAR_skin_id.AddItem("Default");
	t_sniperSCAR_skin_id.AddItem("Crimson Web");
	t_sniperSCAR_skin_id.AddItem("Splash Jam");
	t_sniperSCAR_skin_id.AddItem("Emerald");
	t_sniperSCAR_skin_id.AddItem("Cardiac");
	t_sniperSCAR_skin_id.AddItem("Cyrex");
	t_sniperSCAR_skin_id.AddItem("Whiteout");
	t_sniperSCAR_skin_id.AddItem("The Fuschia Is Now");
	snipergroup.PlaceLabledControl(1, XorStr("Scar20"), this, &t_sniperSCAR_skin_id);

	t_sniperSCAR_wear.SetFileId("t_sniperSCAR_wear");
	t_sniperSCAR_wear.SetBoundaries(1, 100);
	t_sniperSCAR_wear.SetValue(1);
	t_sniperSCAR_wear.extension = XorStr("%%");
	snipergroup.PlaceLabledControl(1, XorStr("Wear"), this, &t_sniperSCAR_wear);

	// --



	// --

	t_sniperAWP_skin_id.SetFileId("t_AWP_skin");
	t_sniperAWP_skin_id.AddItem("Default");
	t_sniperAWP_skin_id.AddItem("Dragon Lore");
	t_sniperAWP_skin_id.AddItem("Pink DDPAT");
	t_sniperAWP_skin_id.AddItem("Asiimov");
	t_sniperAWP_skin_id.AddItem("Redline");
	t_sniperAWP_skin_id.AddItem("Medusa");
	t_sniperAWP_skin_id.AddItem("Hyper Beast");
	t_sniperAWP_skin_id.AddItem("Whiteout");
	snipergroup.PlaceLabledControl(3, XorStr("Skin"), this, &t_sniperAWP_skin_id);

	t_sniperAWP_wear.SetFileId("t_sniperAWP_wear");
	t_sniperAWP_wear.SetBoundaries(1, 100);
	t_sniperAWP_wear.SetValue(1);
	t_sniperAWP_wear.extension = XorStr("%%");
	snipergroup.PlaceLabledControl(3, XorStr("Wear"), this, &t_sniperAWP_wear);
	*/
}

void CColorTab::Setup()
{
	SetTitle("F");
#pragma region Visual Colors
	ColorsGroup.SetText("Settings");
	ColorsGroup.SetPosition(4, 30);
	ColorsGroup.AddTab(CGroupTab("ESP", 1));
	ColorsGroup.AddTab(CGroupTab("Menu", 2));
	//	ColorsGroup.AddTab(CGroupTab("Misc", 6));
	ColorsGroup.SetSize(270, 545);
	RegisterControl(&ColorsGroup);

	/*---------------------- COL ----------------------*/
	/*---------------------- COL ----------------------*/
	/*---------------------- COL ----------------------*/

	NameCol.SetFileId(XorStr("player_espname_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Name"), this, &NameCol);

	BoxCol.SetFileId(XorStr("player_espbox_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Box"), this, &BoxCol);;

	Skeleton.SetFileId(XorStr("player_skeleton_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Skeleton Enemy"), this, &Skeleton);

	Skeletonteam.SetFileId(XorStr("player_skeletonteam_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Skeleton Team"), this, &Skeletonteam);

	GlowEnemy.SetFileId(XorStr("player_glowenemy_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Enemy"), this, &GlowEnemy);

	GlowTeam.SetFileId(XorStr("player_glowteam_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Team"), this, &GlowTeam);

	GlowOtherEnt.SetFileId(XorStr("player_glowother_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow World"), this, &GlowOtherEnt);

	GlowLocal.SetFileId(XorStr("player_glowlocal_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Local"), this, &GlowLocal);

	Weapons.SetFileId(XorStr("player_weapons_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Weapons Enemy"), this, &Weapons);

	Weaponsteam.SetFileId(XorStr("player_weapons_color_team"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Weapons Team"), this, &Weaponsteam);

	Ammo.SetFileId(XorStr("player_ammo_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Ammo Enemy"), this, &Ammo);

	//	Money.SetFileId(XorStr("player_money_color"));
	//	ColorsGroup.PlaceLabledControl(1, XorStr("Money"), this, &Money);

	ChamsLocal.SetFileId(XorStr("player_chamslocal_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Local"), this, &ChamsLocal);
	ChamsLocal.SetColor(71, 206, 229, 255);
	ChamsEnemyVis.SetFileId(XorStr("player_chamsEVIS_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Enemy"), this, &ChamsEnemyVis);
	ChamsEnemyVis.SetColor(71, 206, 229, 255);
	ChamsEnemyNotVis.SetFileId(XorStr("player_chamsENVIS_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Enemy (XQZ)"), this, &ChamsEnemyNotVis);
	ChamsEnemyNotVis.SetColor(241, 1, 254, 255);

	ChamsTeamVis.SetFileId(XorStr("player_ChamsTeamVis"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Team"), this, &ChamsTeamVis);
	ChamsTeamVis.SetColor(71, 206, 229, 255);

	ChamsTeamNotVis.SetFileId(XorStr("player_ChamsTeamNotVis"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Team (XQZ)"), this, &ChamsTeamNotVis);
	ChamsTeamNotVis.SetColor(71, 206, 229, 255);

	//	Bullettracer.SetFileId(XorStr("player_beam_color"));
//	ColorsGroup.PlaceLabledControl(1, XorStr("Bullet tracers"), this, &Bullettracer);

	scoped_c.SetFileId(XorStr("scope_c"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Local Scoped Material Color"), this, &scoped_c);

	misc_lagcomp.SetFileId(XorStr("misc_lagcomp"));
	misc_lagcomp.SetColor(250, 250, 250, 255);
	ColorsGroup.PlaceLabledControl(1, XorStr("Lag Compensation"), this, &misc_lagcomp);

	misc_lagcompBones.SetFileId(XorStr("misc_lagcompBones"));
	misc_lagcompBones.SetColor(250, 250, 250, 255);
	ColorsGroup.PlaceLabledControl(1, XorStr("Backtrack Bones"), this, &misc_lagcompBones);

	//spreadcrosscol.SetFileId(XorStr("weapon_spreadcross_col"));
	//ColorsGroup.PlaceLabledControl(1, XorStr("Spread Crosshair"), this, &spreadcrosscol);
	HandChamsCol.SetFileId(XorStr("handchams_col"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Hand Chams"), this, &HandChamsCol);
	GunChamsCol.SetFileId(XorStr("gunchams_col"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Weapon Chams"), this, &GunChamsCol);

	//	bullet_tracer.SetFileId("beam_color");
	//	ColorsGroup.PlaceLabledControl(1, XorStr("Bullet Impact"), this, &bullet_tracer);

	SleeveChams_col.SetFileId("player_chams_sleeves_color");
	ColorsGroup.PlaceLabledControl(1, XorStr("Sleeve Chams"), this, &SleeveChams_col);

	bomb_timer.SetFileId(XorStr("bomb_timer"));
	bomb_timer.SetColor(250, 10, 90, 230);
	ColorsGroup.PlaceLabledControl(1, XorStr("Bomb Timer"), this, &bomb_timer);

	console_colour.SetFileId("eventlog_color");
	ColorsGroup.PlaceLabledControl(1, XorStr("Event Log"), this, &console_colour);

	//	Offscreen.SetFileId(XorStr("player_offscreen_color"));
	//	ColorsGroup.PlaceLabledControl(1, XorStr("Offscreen"), this, &Offscreen);

	//	fakelag_ghost.SetFileId("player_fakelag_ghost");
	//	ColorsGroup.PlaceLabledControl(1, XorStr("Fakelag Ghost"), this, &fakelag_ghost);



	MenuBar.SetFileId(XorStr("menu_bar_mode"));
	MenuBar.AddItem("Default");
	MenuBar.AddItem("Coloured");
	MenuBar.AddItem("Disabled");
	ColorsGroup.PlaceLabledControl(2, XorStr("Menu Bar"), this, &MenuBar);

	menu_backdrop.SetFileId("owo_backdrop");
	ColorsGroup.PlaceLabledControl(2, XorStr("Menu Backdrop"), this, &menu_backdrop);

	outl_r2.SetFileId("outlred");
	outl_r2.SetBoundaries(0.f, 255.f);
	outl_r2.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Red", this, &outl_r);
	outl_g2.SetFileId("outlgreen");
	outl_g2.SetBoundaries(0.f, 255.f);
	outl_g2.SetValue(215.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Green", this, &outl_g);
	outl_b2.SetFileId("outlblue");
	outl_b2.SetBoundaries(0.f, 255.f);
	outl_b2.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Blue", this, &outl_b);
	inl_r2.SetFileId("inlred");
	inl_r2.SetBoundaries(0.f, 255.f);
	inl_r2.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Red", this, &inl_r);
	inl_g2.SetFileId("inlgreen");
	inl_g2.SetBoundaries(0.f, 255.f);
	inl_g2.SetValue(215.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Green", this, &inl_g);
	inl_b2.SetFileId("inlblue");
	inl_b2.SetBoundaries(0.f, 255.f);
	inl_b2.SetValue(225.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Blue", this, &inl_b);
	inr_r2.SetFileId("inrred");
	inr_r2.SetBoundaries(0.f, 255.f);
	inr_r2.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Red", this, &inr_r);

	inr_g2.SetFileId("inrgreen");
	inr_g2.SetBoundaries(0.f, 255.f);
	inr_g2.SetValue(215.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Green", this, &inr_g);

	inr_b2.SetFileId("inrblue");
	inr_b2.SetBoundaries(0.f, 255.f);
	inr_b2.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Blue", this, &inr_b);
	outr_r2.SetFileId("outrred");
	outr_r2.SetBoundaries(0.f, 255.f);
	outr_r2.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Red", this, &outr_r);
	outr_g2.SetFileId("outrgreen");
	outr_g2.SetBoundaries(0.f, 255.f);
	outr_g2.SetValue(215.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Green", this, &outr_g);
	outr_b2.SetFileId("outrblue");
	outr_b2.SetBoundaries(0.f, 255.f);
	outr_b2.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Blue", this, &outr_b);

	outl_r.SetFileId("outlred");
	outl_r.SetBoundaries(0.f, 255.f);
	outl_r.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Red", this, &outl_r);
	outl_g.SetFileId("outlgreen");
	outl_g.SetBoundaries(0.f, 255.f);
	outl_g.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Green", this, &outl_g);
	outl_b.SetFileId("outlblue");
	outl_b.SetBoundaries(0.f, 255.f);
	outl_b.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Left: Blue", this, &outl_b);
	inl_r.SetFileId("inlred");
	inl_r.SetBoundaries(0.f, 255.f);
	inl_r.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Red", this, &inl_r);
	inl_g.SetFileId("inlgreen");
	inl_g.SetBoundaries(0.f, 255.f);
	inl_g.SetValue(130.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Green", this, &inl_g);
	inl_b.SetFileId("inlblue");
	inl_b.SetBoundaries(0.f, 255.f);
	inl_b.SetValue(225.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Left: Blue", this, &inl_b);
	inr_r.SetFileId("inrred");
	inr_r.SetBoundaries(0.f, 255.f);
	inr_r.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Red", this, &inr_r);

	inr_g.SetFileId("inrgreen");
	inr_g.SetBoundaries(0.f, 255.f);
	inr_g.SetValue(130.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Green", this, &inr_g);

	inr_b.SetFileId("inrblue");
	inr_b.SetBoundaries(0.f, 255.f);
	inr_b.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Inner Right: Blue", this, &inr_b);
	outr_r.SetFileId("outrred");
	outr_r.SetBoundaries(0.f, 255.f);
	outr_r.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Red", this, &outr_r);
	outr_g.SetFileId("outrgreen");
	outr_g.SetBoundaries(0.f, 255.f);
	outr_g.SetValue(0.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Green", this, &outr_g);
	outr_b.SetFileId("outrblue");
	outr_b.SetBoundaries(0.f, 255.f);
	outr_b.SetValue(255.f);
	//ColorsGroup.PlaceLabledControl(2, "Outer Right: Blue", this, &outr_b);

	Menu.SetFileId(XorStr("menu_color"));
	Menu.SetColor(71, 206, 229, 255);
	//ColorsGroup.PlaceLabledControl(2, XorStr("Controls"), this, &Menu);


	//	misc_backtrackchams.SetFileId(XorStr("misc_backtrackchams"));
	//	misc_backtrackchams.SetColor(250, 250, 250, 255);
	//	ColorsGroup.PlaceLabledControl(5, XorStr("Backtrack Chams"), this, &misc_backtrackchams);

	ConfigGroup.SetText("Configs");
	ConfigGroup.SetPosition(286, 30);
	ConfigGroup.SetSize(326, 335);
	RegisterControl(&ConfigGroup); ConfigListBox.SetHeightInItems(7);
	list_configs();
	ConfigGroup.PlaceLabledControl(0, XorStr(""), this, &ConfigListBox);
	LoadConfig.SetText(XorStr("Load"));
	LoadConfig.SetCallback(&load_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &LoadConfig);
	SaveConfig.SetText(XorStr("Save"));
	SaveConfig.SetCallback(&save_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &SaveConfig);
	RemoveConfig.SetText(XorStr("Remove"));
	RemoveConfig.SetCallback(&remove_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &RemoveConfig);
	ConfigGroup.PlaceLabledControl(0, "", this, &NewConfigName);
	AddConfig.SetText(XorStr("Add"));
	AddConfig.SetCallback(&add_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &AddConfig);

	/*---------------------- OTHERS ----------------------*/
	/*---------------------- OTHERS ----------------------*/
	/*---------------------- OTHERS ----------------------*/

	OtherOptions.SetText("Other");
	OtherOptions.SetPosition(4, 373);
	OtherOptions.SetSize(270, 202);
	RegisterControl(&OtherOptions);


	//	quickstop_speed.SetFileId(XorStr("quickstop_speed"));
	//	quickstop_speed.SetBoundaries(1, 40);
	//	quickstop_speed.SetValue(30);
	//	OtherOptions.PlaceLabledControl(0, XorStr("Quick Stop Speed"), this, &quickstop_speed);
	//	BackTrackBones.SetFileId(XorStr("spookybonesOwO"));
	//	OtherOptions.PlaceLabledControl(0, XorStr("BackTrack Chams"), this, &BackTrackBones);
	// your fps will look beyond the gates of the next life and will raise their middle fingers for having caused their suicide

	OtherOptions2.SetText("Extra");
	OtherOptions2.SetPosition(286, 373);
	OtherOptions2.SetSize(326, 202);
	RegisterControl(&OtherOptions2);

	unload.SetText("Unload Cheat");
	unload.SetCallback(UnLoadCallbk);
	OtherOptions2.PlaceLabledControl(0, "", this, &unload);

	//OtherOptions2.SetText("Mods");
	//OtherOptions2.SetPosition(286, 373);
	//OtherOptions2.SetSize(323, 202);
	//RegisterControl(&OtherOptions2);

	//	experimental_backtrack.SetFileId(XorStr("experimental_backtrack"));
	//	OtherOptions2.PlaceLabledControl(0, XorStr("Experimental Position Adjustment"), this, &experimental_backtrack);
}

void options::SetupMenu()
{
	menu.Setup();
	GUI.RegisterWindow(&menu);
	GUI.BindWindow(VK_INSERT, &menu);
}
void options::DoUIFrame()
{
	GUI.Update();
	GUI.Draw();
}

