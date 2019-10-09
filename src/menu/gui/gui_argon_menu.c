/*  
 * Copyright (c) 2018 Guillem96
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 #include "power/max17050.h"
 
#include <string.h>
#include "menu/gui/gui_argon_menu.h"
#include "menu/gui/gui_menu.h"
#include "menu/gui/gui_menu_pool.h"
#include "core/custom-gui.h"
#include "gfx/di.h"
#include "gfx/gfx.h"
#include <stdio.h>

#include "utils/types.h"
#include "utils/fs_utils.h"
#include "utils/dirlist.h"
#include "utils/util.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"

#include "mem/heap.h"

#define COLUMNS 4  
#define ROWS 2
#define ELEM_SIZE 230
#define MARGIN_TOP 100
#define MARGIN_LEFT 46
#define MAX_CHAR 100 typedef char string[MAX_CHAR+1];
//static int tool_menu_rSD(void* param);
//static int findLastIndex(char* str, char* x);
char *gobacknow(char* sd_path) ;

static int tool_extr_rSD(void* param);
static int tool_reboot_rcm(void* param);
static int tool_power_off(void* param);
static int tool_menu_rem(void* param);
int tool_theme(char* param);
static int tool_emu(u32 param);
//static int zbackup(char* param);
static int tool_Menus(u32 param);
int tool_dir(char *param);
int tool_servises(u32 param);
int tool_servises_on(char* title);
int tool_servises_off(char* title);
int tool_Themes_on(char* cfw);
int tool_filete(char* fil);
int tool_file_act(u32 fil);
int memloader(u32 fil);
int Incognito(char* order);
int Autoboot(u32 fil);

int bat_show(u32 percent);
void serv_display(gui_menu_t* menu,char* titleid,char* name);
void help_display(void);
//menus
u64 main_menu = 0;
u32 submenu = 0;
u32 filemgr = 0;

//sub menus
u32 permsubY = 0;
u32 permsubX = 0;
u32 sub_buttonW = 0;
u32 sub_buttonH = 0;

//some indicators
u32 onemi = 0;
u32 retir = 0;

//services
u32 servX = 30;
u32 servY = 80;
u32 sepaserv = 70;

//dinamic directory
char *directory = "";
char *filete = "";
char *filpay = "3333333333";
char Sversion[1];

//help Switch
u32 help_switch = 1;

void gui_init_argon_boot(void)
{
    /* Init pool for menu */
    gui_menu_pool_init();

    gui_menu_t* menu = gui_menu_create("ArgonNX");
	display_backlight_brightness(100, 1000);
	//show display without icons
		
    gui_menu_open2(menu);
	if (!sd_mount()){BootStrapNX();}//check sd

	if (sd_file_exists("NEXT/autobootecho.txt")& (!sd_file_exists ("NEXT/inc.return")))
	msleep(2000);
	


	//set archive bit 
	 f_chmod("switch/mercury", AM_ARC, AM_ARC);

	//waith user input
    bool cancel_auto_chainloading = btn_read() & BTN_VOL_UP;
    if ((!cancel_auto_chainloading) & (!sd_file_exists ("NEXT/inc.return")))
	{

		//autoboot
		char *str;
		if (g_sd_mounted){
			void *buf;
			buf = sd_file_read("NEXT/autobootecho.txt");
			str = buf;
			Sversion[0] = str[0];
		}
		if(strstr(Sversion,"A") != NULL)
		launch_payload("NEXT/payloads/Atmosphere.bin");

		if(strstr(Sversion,"R") != NULL)
		launch_payload("NEXT/payloads/ReiNX.bin");

		if(strstr(Sversion,"S") != NULL)
		launch_payload("NEXT/payloads/SXOS.bin");
	}
	f_unlink("NEXT/payload.bin");
	if (btn_read() & BTN_VOL_DOWN)
	f_unlink("NEXT/autobootecho.txt");
	
//	f_rename("/Emutendo","/NEXT/Emuni");
	f_unlink("auto.bak");
gui_menu_pool_cleanup();
if (!sd_mount()){BootStrapNX();}//check sd
gui_init_argon_menu();
}

/* Generate entries dynamically */
static void generate_payloads_entries(char* payloads, gui_menu_t* menu)
{
    if (payloads == NULL)
    {
        g_gfx_con.scale = 4;
        gfx_con_setpos(&g_gfx_con, 140, 250);
        gfx_printf(&g_gfx_con, "Payloads directory is empty...\n");
        
        g_gfx_con.scale = 3;
        gfx_con_setpos(&g_gfx_con, 110, 370);
        gfx_printf(&g_gfx_con, "Place your payloads inside \"%s\"", PAYLOADS_DIR);

        return;
    }

    u32 i = 0;//payload name
	u32 f = 0;//entry number
    /* For each payload generate its logo, its name and its path */
    while(payloads[i * 256])
    {
        char* payload_path = (char*)malloc(256);
        payload_full_path(&payloads[i * 256], payload_path);
        
        char payload_logo[256];
        payload_logo_path(&payloads[i * 256], payload_logo);
        const char* payload_wo_bin = str_replace(&payloads[i * 256], ".bin", "");
		
		u32 row = f;
        u32 col = f % COLUMNS;
        u32 x = g_gfx_ctxt.width / COLUMNS * col + MARGIN_LEFT;
        u32 y = 140 / ROWS * row + MARGIN_TOP + MARGIN_TOP;

	if((retir == 2) & (strstr(payload_wo_bin,"reinx") != NULL))
	{
	f++;
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Reinx-off.bmp"),x, y, 200 , 200, NULL, NULL));
	}else{
		if(strstr(payload_wo_bin,"Stock") != NULL){
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Stock.bmp"),1010, 300, 200 , 100,(int (*)(void *))launch_payload, (void*)payload_path));
		}else{
		
			gui_menu_append_entry(menu, 
				gui_create_menu_entry(payload_wo_bin,
										sd_file_read(payload_logo), 
										x, y,
										200, 200,
										(int (*)(void *))launch_payload, (void*)payload_path));
				f++;						
			}
		}
		
//        gfx_printf(&g_gfx_con, "-%s\n-X%d\n-Y%d\n-E%d\n\n\n",payload_wo_bin, x, y, i);
	
	i++;
    }
//        gfx_swap_buffer(&g_gfx_ctxt);
//		btn_wait();
}

/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
    /* Init pool for menu */
    gui_menu_pool_init();
gui_menu_t* menu = gui_menu_create("ArgonNX");
if (!sd_mount()){BootStrapNX();}//check sd

		if (sd_file_exists("NEXT/inc.return"))
		{
			main_menu = 6;
			f_unlink("NEXT/inc.return");
		}
	//low icons Y
	u64 iconrowSEP = 150;
	u64 iconrowX = 50;
	u64 iconrowY = 600;

	//main menu 0-------------------------------------
	if(main_menu == 0)
	{

		//Create emummc icon
		u32 buttonH = 75;
		u32 buttonW = 200;
		u32 buttonX = 950;
		u32 buttonY = 0;

		if (sd_file_exists("emummc/emummc.ini"))
		{

		char *str = sd_file_read("emummc/emummc.ini");
		if(retir == 0)
		{
			if(strstr(str,"emummc_") != NULL)
			{
			str = str_replace(str, "emummc_", "");
			u32 size = strlen(str)-1;
			sd_save_to_file(str,size,"emummc/emummc.ini");

			}
			if(strstr(str," ") != NULL)
			{
			str = str_replace(str, " ", "");
			u32 size = strlen(str)-1;
			sd_save_to_file(str,size,"emummc/emummc.ini");
			}
		retir = 1;
			if(strstr(str,"enabled=1") != NULL)
			{retir = 2;}
		}
			if(retir == 2)
			{
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bon.bmp"),buttonX, buttonY, buttonW , buttonH,(int (*)(void *))tool_emu, (void*)0)); //- 80, - 500
			}else{
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/boff.bmp"),buttonX, buttonY, buttonW , buttonH,(int (*)(void *))tool_emu, (void*)1)); //- 80, - 500
			}
		}


		//generate main menu
		if (sd_file_exists ("/NEXT/Dinamy"))
		{
		//dinamic entries 
		generate_payloads_entries(dirlist(PAYLOADS_DIR, "*.bin", false), menu);
		}else{
			//fixed entries cfw
			u32 myicons = 220;
			u32 xcorrect = 150;
			u32 xcorrectSEP = 350;
			
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Atmosphere.bmp"), xcorrect, myicons, 300, 300,(int (*)(void *))launch_payload, (void*)"/NEXT/payloads/Atmosphere.bin"));
			xcorrect = xcorrect+xcorrectSEP;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/SXOS.bmp"), xcorrect, myicons, 300 , 300,(int (*)(void *))launch_payload, (void*)"/NEXT/payloads/SXOS.bin"));
			xcorrect = xcorrect+xcorrectSEP;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Stock.bmp"),xcorrect, myicons, 300 , 300,(int (*)(void *))launch_payload, (void*)"/NEXT/payloads/Stock.bin"));
		}
		
	//generate down Icons
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/zHekate.bmp"), iconrowX, iconrowY, 120, 120,(int (*)(void *))launch_payload, (void*)"/NEXT/payloads/zHekate.bin"));
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Incognito.bmp"),iconrowX, iconrowY, 120 , 120,(int (*)(void *))tool_Menus, (void*)6));		
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/memloader.bmp"),iconrowX, iconrowY, 120 , 120,(int (*)(void *))tool_Menus, (void*)3));
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/Lockpick_RCM.bmp"),iconrowX, iconrowY, 120 , 120,(int (*)(void *))launch_payload, (void*)"/NEXT/payback/Lockpick_RCM.bin"));
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/rcm.bmp"),iconrowX, iconrowY, 120, 120, tool_reboot_rcm, NULL));
	iconrowX = iconrowX +iconrowSEP;	
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/power.bmp"),iconrowX, iconrowY, 120, 120, tool_power_off, NULL));


		
		if (sd_file_exists ("/switchroot_android/coreboot.rom")|| sd_file_exists ("/switchroot_android/coreboot.bin"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/Android.bmp"),1, 620, 100, 100, (int (*)(void *))launch_payload, (void*)"/switchroot_android/coreboot.bin"));
	
		//activate dev tools
		if (sd_file_exists ("/NEXT/Extratools.flag"))
		{
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/SD.bmp"),1200, 230, 70, 70, tool_extr_rSD, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/themes.bmp"),1200, 330, 70, 70, tool_menu_rem, NULL));		
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/gear.bmp"),1200, 430, 70, 70, (int (*)(void *))tool_Menus, (void*)1));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/file-browser.bmp"),1200, 530, 70, 70,(int (*)(void *))tool_Menus, (void*)2));
		}
	}

//Menu 1
	if(main_menu == 1)
	{
	//generate menu2
//	u64 iconrowX = 80;
//	u64 iconrowY = 200;


	//generate themes Dinamy
	u32 tm_iconsY = 5;
	u32 tm_iconsX = 260;
	u32 tm_ajust = 90;
	char* tm_directory = "NEXT/Atheme";
	char* folder = listfol(tm_directory, "*", true);
			u32 r = 0;
			u32 w = 0;
		while(folder[r * 256])
		{
			if(strlen(&folder[r * 256]) <= 100){			
			char* source_fol = (char*)malloc(256);
			strcpy(source_fol, tm_directory);
			strcat(source_fol, "/");
			strcat(source_fol, &folder[r * 256]);
			char* source_icon = (char*)malloc(256);
			strcpy(source_icon, source_fol);
			strcat(source_icon, "/icon.bmp");
			if(sd_file_exists(source_icon))
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read(source_icon),tm_iconsX, tm_iconsY, 70, 70, (int (*)(void *))tool_theme, (void*)source_fol)); 
			tm_iconsX = tm_iconsX+tm_ajust;
			w++;
			}
		if(w == 9)
		break;
		r++;
		}
		//draw autoboot
		if (sd_file_exists ("NEXT/autobootecho.txt"))
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/Auton.bmp"),1050, 130, 200 , 75,(int (*)(void *))Autoboot, (void*)0));
		else
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/Autoff.bmp"),1050, 130, 200 , 75,(int (*)(void *))Autoboot, (void*)1));



		//theme Activator for cfw
		if (!sd_file_exists ("atmosphere/titles/0100000000001000/fsmitm.flag")||!sd_file_exists ("ReiNX/titles/0100000000001000/fsmitm.flag")||!sd_file_exists ("sxos/titles/0100000000001000/fsmitm.flag"))
		{
			u32 temX = 1050;
			u32 temY = 290;
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Themes", temX+20, temY+80, 150, 100, NULL, NULL));
				if (!sd_file_exists ("atmosphere/titles/0100000000001000/fsmitm.flag"))
				{
				temY = temY + 100;
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"atmosphere"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("AMS",temX+30, temY+30, 150, 100, NULL, NULL));
				}
				
				if (!sd_file_exists ("ReiNX/titles/0100000000001000/fsmitm.flag"))
				{
				temY = temY + 100;
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"ReiNX"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("ReiNX",temX+30, temY+30, 150, 100, NULL, NULL));
				}
				
				if (!sd_file_exists ("sxos/titles/0100000000001000/fsmitm.flag"))
				{
				temY = temY + 100;
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-off.bmp"),temX, temY, 200, 75,(int (*)(void *))tool_Themes_on, (void*)"sxos"));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("sxos",temX+30, temY+30, 150, 100, NULL, NULL));
				}

		}


		// Create SXOS transfer button
		if (sd_file_exists ("emuMMC/EF00/eMMC/00")&sd_file_exists ("emuMMC/EF00/eMMC/boot0")&sd_file_exists ("emuMMC/EF00/eMMC/boot1"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/swap.bmp"),380, 450, 200 , 75,(int (*)(void *))tool_emu, (void*)66));
		if (sd_file_exists ("sxos/emunand/full.00.bin")&sd_file_exists ("sxos/emunand/boot0.bin")&sd_file_exists ("sxos/emunand/boot1.bin"))
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/Transfer.bmp"),380, 450, 200 , 75,(int (*)(void *))tool_emu, (void*)33)); //- 80, - 500
		if (!sd_file_exists("emummc/emummc.ini"))
		{
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/link_hide.bmp"),680, 450, 200 , 75,(int (*)(void *))tool_emu, (void*)99)); 
		}
	

		//services
		servY = 60;//mandatory
		servX = 10;
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Services",servX+30, servY-10, 150, 100, NULL, NULL));
		serv_display(menu,"420000000000000E","FTP");
		serv_display(menu,"0100000000000352","Emuiio"); 
		serv_display(menu,"4200000000000010","Lan Play");
		serv_display(menu,"0100000000000FAF","HDI");
		serv_display(menu,"420000000000000B","sysplay"); 
		serv_display(menu,"00FF0000636C6BFF","sys-clk");
		serv_display(menu,"0100000000534C56","ReverseNX"); 
		serv_display(menu,"0100000000000069","ReiSpoof");

		//call menu 0
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 120, 120,(int (*)(void *))tool_Menus, (void*)0));
	}

	//Menu 2
	if(main_menu == 2)
	{
			char* files = listfil(directory, "*", true);
			char* folder = listfol(directory, "*", true);				
				u32 r = 0;
				u32 w = 0;
				u32 i = 0;
				u32 f = 0;
				u32 y = 60;
				u32 x = 10;
				u32 space = 50;
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sd-root.bmp"),270, 10, 70, 70,(int (*)(void *))tool_Menus, (void*)33));
				if(strlen(directory) >= 1)
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sd-back.bmp"),370, 10, 70, 70,(int (*)(void *))tool_file_act, (void*)99));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(directory, strlen(directory)*8-40+50, 68, 150, 100, NULL, NULL));
			
			while(folder[r * 256])
			{
			if(f >= 22){
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,680, 150, 100, NULL, NULL));
			break;}
				if(strlen(&folder[r * 256]) <= 100){			
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/folder.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_dir, (void*)&folder[r * 256]));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(&folder[r * 256], x+strlen(&folder[r * 256])*8-40, y+35, 150, 100, NULL, NULL));

				y = y + space;
				
				f++;
				i++;
					if(i == 11){
					y = 60;
					x = 700;
					i = 0;
					}
				}
			r++;
			}

			while(files[w * 256])
			{
			if(f >= 22){
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,680, 150, 100, NULL, NULL));
			break;}
			if(strlen(&files[w * 256]) <= 100){
					//files Icons
					if(strstr(&files[w * 256],".bin") != NULL)
					{
						gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/files-bin.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
					}else{
						if(strstr(&files[w * 256],".txt") != NULL||strstr(&files[w * 256],".ini") != NULL||strstr(&files[w * 256],".conf") != NULL)
						{
							gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/files-txt.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
						}else{
							if(strstr(&files[w * 256],".png") != NULL||strstr(&files[w * 256],".bmp") != NULL||strstr(&files[w * 256],".jpg") != NULL)
								gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/files-img.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
							else
								gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/files.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
						}
					}
					

				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(&files[w * 256], x+strlen(&files[w * 256])*8-40, y+35, 150, 100, NULL, NULL));
				y = y + space;

			f++;			
			i++;			
					
					if(i == 11){
					y = 60;
					x = 600;
					i = 0;
					}	

				}
			w++;
			}
				if(strlen(filete) >= 1)
				{
				u32 actX = 800;
				u32 actY = 1;
				gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/del.bmp"),actX, actY, 70, 70, (int (*)(void *))tool_file_act, (void*)0));
				gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(filete, actX, actY+70, 150, 100, NULL, NULL));
				}
			//call menu 0
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 120, 120,(int (*)(void *))tool_Menus, (void*)0));
	}
		 
	//Menu 3
	if(main_menu == 3)
	{
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Menu de MemLoader", 600,1, 150, 100, NULL, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/memloader_boot0.bmp"),80, 200, 200 , 200,(int (*)(void *))memloader, (void*)0));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/memloader_boot1.bmp"),380, 200, 200 , 200,(int (*)(void *))memloader, (void*)1));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/memloader_emmc.bmp"),680, 200, 200 , 200,(int (*)(void *))memloader, (void*)2));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/memloader_sd.bmp"),980, 200, 200 , 200,(int (*)(void *))memloader, (void*)3));
		//call
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 120, 120,(int (*)(void *))tool_Menus, (void*)0));

	}
	
		//Menu 4
	if(main_menu == 4)
	{
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Pagina de Ayuda 1/2...", 600, 1, 150, 100, NULL, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/1-help.bmp"),50, 30, 1200, 650,(int (*)(void *))tool_Menus, (void*)5));
			//call menu 0
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 120, 120,(int (*)(void *))tool_Menus, (void*)0));

	}
		//Menu 5
	if(main_menu == 5)
	{
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Pagina de Ayuda 2/2...", 600,1, 150, 100, NULL, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/2-help.bmp"),50, 30, 1200, 650,(int (*)(void *))tool_Menus, (void*)4));
			//call menu 0
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 70, 70,(int (*)(void *))tool_Menus, (void*)0));

	}

		//Menu 6
	if(main_menu == 6)
	{
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Menu de Incognito", 600,1, 150, 100, NULL, NULL));
		
		u32 xcol = 90;
		
		u32 rowinc = 0+xcol;
		u32 colinc = 150;
		u32 rowsepar = 50;
		u32 colsepar = 100;

		if (!sd_file_exists("emummc/emummc.ini"))
		{
			rowinc = 550;
			rowsepar = 0;

		}		
		
		rowinc = rowinc + rowsepar;
		colinc = colinc + colsepar;
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc0.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"backup_sysnand"));
		
		
		rowinc = rowinc + rowsepar;
		colinc = colinc + colsepar;
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc1.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"incognito_sysnand"));
		
		
		rowinc = rowinc + rowsepar;
		colinc = colinc + colsepar;
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc2.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"restore_sysnand"));
		
		if (sd_file_exists("emummc/emummc.ini"))
		{
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("SysNand", 170,220, 150, 100, NULL, NULL));
			gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("EmuNand", 970,220, 150, 100, NULL, NULL));
			
			rowinc = 900+xcol;
			colinc = 150;
			rowsepar = 50;
			colsepar = 100;
		
			rowinc = rowinc - rowsepar;
			colinc = colinc + colsepar;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc3.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"backup_emunand"));

			rowinc = rowinc - rowsepar;
			colinc = colinc + colsepar;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc4.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"incognito_emunand"));

			rowinc = rowinc - rowsepar;
			colinc = colinc + colsepar;
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/inc5.bmp"),rowinc, colinc, 200, 75,(int (*)(void *))Incognito, (void*)"restore_emunand"));
		}

		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/arrowl.bmp"),100, iconrowY, 120, 120,(int (*)(void *))tool_Menus, (void*)0));

	}

/* Generate permanent entries**/
/*
if (help_switch == 0)
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/logos/logoSD.bmp"),1, 630, 100, 100,(int (*)(void *)) help_display, NULL));
else
help_switch = 0;
*/
if (sd_file_exists ("/NEXT/Help.flag"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/help.bmp"),10, 650, 70, 70, (int (*)(void *))tool_Menus, (void*)4));


gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read(""),1, 1, 35, 35, (int (*)(void *))bat_show,(void*)145));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read(""),1240, 680, 35, 35, (int (*)(void *))screenshot, NULL));
//battery
u32 battPercent = 0;
max17050_get_property(MAX17050_RepSOC, (int *)&battPercent);		
u32 battimgper = (battPercent >> 8) & 0xFF;
u32 batY = 1;
u32 batX = 1210;
if ((battimgper <= 100) & (battimgper >= 76))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bat1.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 75) & (battimgper >= 51))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bat2.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 50) & (battimgper >= 26))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bat3.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if ((battimgper <= 25) & (battimgper >= 3))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bat4.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));
if (battimgper <= 2)
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/bat5.bmp"),batX, batY, 70, 35,(int (*)(void *))bat_show, (void*)battimgper));

    /* Start menu */
    gui_menu_open(menu);

    /* Clear all entries and menus */
    gui_menu_pool_cleanup();
}

static int tool_reboot_rcm(void* param)
{
   reboot_rcm();
    return 0;
}

static int tool_power_off(void* param)
{
    gui_menu_pool_cleanup();
    power_off();
    return 0;
}

static int tool_extr_rSD(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
	gfx_swap_buffer(&g_gfx_ctxt);
	display_backlight_brightness(100, 1000);
	g_gfx_con.scale = 3;
    gfx_con_setpos(&g_gfx_con, 160, 100);
    gfx_printf(&g_gfx_con, "Ya puedes extraer la SD, Al terminar\n");
    gfx_con_setpos(&g_gfx_con, 230, 130);
    gfx_printf(&g_gfx_con, "Pon la SD y presiona POWER\n\n");
    gfx_con_setpos(&g_gfx_con, 110, 600);
    gfx_printf(&g_gfx_con, "You can now extract the SD, When you finish\n");
    gfx_con_setpos(&g_gfx_con, 230, 630);
    gfx_printf(&g_gfx_con, "Put the SD and press POWER\n");
    gfx_swap_buffer(&g_gfx_ctxt);
	btn_wait_timeout(10000, BTN_POWER);
	display_backlight_brightness(0, 1000);
BootStrapNX();
    return 0;
}


static int tool_emu(u32 status)
{
if (!sd_mount()){BootStrapNX();}//check sd
	
	//give sxos emu to ams
	if(status == 33)
	{
	f_unlink("sxos/eMMC");
	display_backlight_brightness(1, 1000);
	f_mkdir("emuMMC");
	f_mkdir("emuMMC/EF00");
	f_rename("/sxos/emunand","/emuMMC/EF00/eMMC");
	f_rename("/Emutendo","/emuMMC/EF00/Nintendo");
	
	f_rename("emuMMC/EF00/eMMC/boot0.bin","emuMMC/EF00/eMMC/boot0");
	f_rename("emuMMC/EF00/eMMC/boot1.bin","emuMMC/EF00/eMMC/boot1");

	f_rename("emuMMC/EF00/eMMC/full.00.bin","emuMMC/EF00/eMMC/00");
	f_rename("emuMMC/EF00/eMMC/full.01.bin","emuMMC/EF00/eMMC/01");
	f_rename("emuMMC/EF00/eMMC/full.02.bin","emuMMC/EF00/eMMC/02");
	f_rename("emuMMC/EF00/eMMC/full.03.bin","emuMMC/EF00/eMMC/03");
	f_rename("emuMMC/EF00/eMMC/full.04.bin","emuMMC/EF00/eMMC/04");
	f_rename("emuMMC/EF00/eMMC/full.05.bin","emuMMC/EF00/eMMC/05");
	f_rename("emuMMC/EF00/eMMC/full.06.bin","emuMMC/EF00/eMMC/06");
	f_rename("emuMMC/EF00/eMMC/full.07.bin","emuMMC/EF00/eMMC/07");
	f_rename("emuMMC/EF00/eMMC/full.08.bin","emuMMC/EF00/eMMC/08");
	f_rename("emuMMC/EF00/eMMC/full.09.bin","emuMMC/EF00/eMMC/09");
	f_rename("emuMMC/EF00/eMMC/full.10.bin","emuMMC/EF00/eMMC/10");
	f_rename("emuMMC/EF00/eMMC/full.11.bin","emuMMC/EF00/eMMC/11");
	f_rename("emuMMC/EF00/eMMC/full.12.bin","emuMMC/EF00/eMMC/12");
	f_rename("emuMMC/EF00/eMMC/full.13.bin","emuMMC/EF00/eMMC/13");
	f_rename("emuMMC/EF00/eMMC/full.14.bin","emuMMC/EF00/eMMC/14");
		if (sd_file_exists ("emuMMC/EF00/eMMC/00")&sd_file_exists ("emuMMC/EF00/eMMC/boot0")&sd_file_exists ("emuMMC/EF00/eMMC/boot1"))
		{
			f_unlink("emummc/emummc.bak");
			f_rename("emummc/emummc.ini","emummc/emummc.bak");
			f_mkdir("emummc");
			FIL fp;
			f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[emummc]\n", &fp);
			f_puts("enabled=1\n", &fp);
			f_puts("sector=0x0\n", &fp);
			f_puts("path=emuMMC/EF00\n", &fp);
			f_puts("id=0x0000\n", &fp);
			f_puts("nintendo_path=emuMMC/EF00/Nintendo\n", &fp);
			f_close(&fp);
			sd_save_to_file("", 0, "emuMMC/EF00/file_based");
		}

		gui_init_argon_menu();
	return 0;
	}

	//return emunand to sxos
	if(status == 66)
	{
	f_unlink("sxos/emunand");
	display_backlight_brightness(1, 1000);
	f_rename("/emuMMC/EF00/eMMC","/sxos/emunand");
	
	f_rename("sxos/emunand/boot0","sxos/emunand/boot0.bin");
	f_rename("sxos/emunand/boot1","sxos/emunand/boot1.bin");

	f_rename("sxos/emunand/00","sxos/emunand/full.00.bin");
	f_rename("sxos/emunand/01","sxos/emunand/full.01.bin");
	f_rename("sxos/emunand/02","sxos/emunand/full.02.bin");
	f_rename("sxos/emunand/03","sxos/emunand/full.03.bin");
	f_rename("sxos/emunand/04","sxos/emunand/full.04.bin");
	f_rename("sxos/emunand/05","sxos/emunand/full.05.bin");
	f_rename("sxos/emunand/06","sxos/emunand/full.06.bin");
	f_rename("sxos/emunand/07","sxos/emunand/full.07.bin");
	f_rename("sxos/emunand/08","sxos/emunand/full.08.bin");
	f_rename("sxos/emunand/09","sxos/emunand/full.09.bin");
	f_rename("sxos/emunand/10","sxos/emunand/full.10.bin");
	f_rename("sxos/emunand/11","sxos/emunand/full.11.bin");
	f_rename("sxos/emunand/12","sxos/emunand/full.12.bin");
	f_rename("sxos/emunand/13","sxos/emunand/full.13.bin");
	f_rename("sxos/emunand/14","sxos/emunand/full.14.bin");
	f_rename("/emuMMC/EF00/Nintendo","/Emutendo");
		if (sd_file_exists ("sxos/emunand/full.00.bin")&sd_file_exists ("sxos/emunand/boot0.bin")&sd_file_exists ("sxos/emunand/boot1.bin"))
		{
			f_unlink("emummc/emummc.ini");
			f_rename("emummc/emummc.bak","emummc/emummc.ini");
			f_unlink("emuMMC/EF00/file_based");
			f_unlink("/emuMMC/EF00");
			f_unlink("emummc");
			retir = 0;
		}

		gui_init_argon_menu();
	return 0;
	}

	
	//link sxos hide partition to ams
	if(status == 99)
	{
	display_backlight_brightness(1, 1000);
			f_mkdir("emummc");
			FIL fp;
			f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[emummc]\n", &fp);
			f_puts("enabled=1\n", &fp);
			f_puts("sector=0x2\n", &fp);
			f_puts("nintendo_path=Emutendo\n", &fp);
			f_close(&fp);
		gui_init_argon_menu();
	return 0;
	}

	




	if(status == 1)
	{
	char *str1 = sd_file_read("emummc/emummc.ini");
	char* payload_wo_bin = str_replace(str1, "enabled=0", "enabled=1");
	FIL op;
	f_open(&op, "emummc/emummc.ini", FA_READ);
	u32 size = f_size(&op);
	f_close(&op);
	sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
	retir = 2;
	}

	if(status == 0)
	{
	char *str1 = sd_file_read("emummc/emummc.ini");
	char* payload_wo_bin = str_replace(str1, "enabled=1", "enabled=0");
	FIL op;
	f_open(&op, "emummc/emummc.ini", FA_READ);
	u32 size = f_size(&op);
	f_close(&op);
	sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
	retir = 1;
	}

/*	
if (sd_file_exists ("emummc/emummc.ini"))
f_rename("emummc/emummc.ini.bak","emummc/emummc.ini");
*/
	gui_init_argon_menu();
return 0;
}

static int tool_Menus(u32 param)
{
if (!sd_mount()){BootStrapNX();}//check sd

if(param == 33){
directory = "";
gui_init_argon_menu();
return 0;
}
display_backlight_brightness(50, 1000);
//check if is the same menu
if(main_menu == param)
return 0;

if(param == 0)
{
directory = "";
filete = "";
}
//set menu number
main_menu = param;

//gui_menu_pool_cleanup();
gui_init_argon_menu();
return 0;
}

int tool_servises_on(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(50, 1000);
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("",0,path);
		strcpy(path, "ReiNX/titles/");
		strcat(path, title);
		strcat(path, "/boot2.flag");
		sd_save_to_file("",0,path);
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags");
		f_mkdir(path);
		strcat(path, "/boot2.flag");
		sd_save_to_file("",0,path);
gui_init_argon_menu();
return 0;
}

int tool_servises_off(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(50, 1000);
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
		strcpy(path, "ReiNX/titles/");
		strcat(path, title);
		strcat(path, "/boot2.flag");
		f_unlink(path);
		strcpy(path, "sxos/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);

gui_init_argon_menu();
return 0;
}

int tool_Themes_on(char* cfw)
{
if (!sd_mount()){BootStrapNX();}//check sd
char* path = (char*)malloc(256);
		strcpy(path, cfw);
		strcat(path, "/titles/0100000000001000/fsmitm.flag");
		sd_save_to_file("",0,path);
gui_init_argon_menu();
return 0;
}

//file mgr
int tool_dir(char *param)
{
if (!sd_mount()){BootStrapNX();}//check sd



if(strlen(directory) <= 1)
{
directory = param;
}else{
strcat(directory, "/");
strcat(directory, param);

}
gui_init_argon_menu();
return 0;
}
//hacer doble toque
int tool_filete(char* fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
if(strlen(directory) <= 1)
{
filete = fil;
}else{
    char tmp[256];
    strcpy(tmp, directory);
    strcat(tmp, "/");
	strcat(tmp, fil);
filete = tmp;		
}
	
//	if(filete == filpay)
	if(strstr(filete,filpay) != NULL)
	{
		if(strstr(filete,".bin") != NULL)
		{
		tool_file_act(1);
		}
		if(strstr(filete,".BIN") != NULL)
		{
		tool_file_act(1);
		}
		
		if(strstr(filete,".txt") != NULL)
		{
		tool_file_act(2);
		}
		if(strstr(filete,".ini") != NULL)
		{
		tool_file_act(2);
		}
		if(strstr(filete,".conf") != NULL)
		{
		tool_file_act(2);
		}
	}
filpay = filete;
gui_init_argon_menu();
return 0;
}

int tool_file_act(u32 fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
	if(fil == 0)
	{
	f_unlink(filete);
	free(filete);
	filete = "";
	}
	
	if(fil == 1)
	{
	launch_payload(filete);
	}
	
	if(fil == 2)
	{
	//launch_payload(filete);
		gfx_swap_buffer(&g_gfx_ctxt);
		g_gfx_con.scale = 2;
		gfx_con_setpos(&g_gfx_con, 1, 1);
		gfx_printf(&g_gfx_con, "\nText Reader\n",filete);
		gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);
		gfx_printf(&g_gfx_con, "\n%s...\n",filete);
		gfx_printf(&g_gfx_con, "\n-------------------------------------------------------------------------------------------------------------------\n",filete);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		g_gfx_con.scale = 2;
		gfx_printf(&g_gfx_con, "\n%s",sd_file_read(filete));
	gfx_swap_buffer(&g_gfx_ctxt);
	gui_menu_pool_init();
    gui_menu_t* menu = gui_menu_create("ArgonNX");
    gui_menu_open3(menu);

	}
	//Go back directory
	if(fil == 99)
	{
	
	//identify last '/' caracter
	char *outFilename = directory;
	u32 sdPathLen = strlen(directory);
	char recip[0];
		while(true)
		{
		recip[0] = outFilename[sdPathLen--];
		if(strstr(recip,"/") != NULL){
		break;}
		
		
		if(sdPathLen <= 0){
		directory = "";
		break;}
		}
	
		if(strlen(directory) >= 1)
		{
			//take position of last / and contruct the variable before hem
			char name[100];
//			gfx_swap_buffer(&g_gfx_ctxt);
			gfx_printf(&g_gfx_con, "-\n\n\n\n\n\n\n");
			for (u32 i = 0;sdPathLen >= i;i++)
			{
				name[i] = outFilename[i];
				gfx_printf(&g_gfx_con, "name-%s-%d-%d\n",name,i,sdPathLen);
			}
			directory = name;
			//logs 
			/*
				gfx_con_setpos(&g_gfx_con, 600, 10);
				gfx_printf(&g_gfx_con, "dir-%s-\n",directory);
				
				gfx_con_setpos(&g_gfx_con, 600, 25);
				gfx_printf(&g_gfx_con, "name-%s-\n",name);
				gfx_con_setpos(&g_gfx_con, 600, 40);
				gfx_printf(&g_gfx_con, "dir-%s-\n",directory);
	gfx_swap_buffer(&g_gfx_ctxt);
	btn_wait();			
				*/
		}
	}
	
	
gui_init_argon_menu();
return 0;
}

static int tool_menu_rem(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
 gfx_printf(&g_gfx_con, "loading\n");
//gui_menu_pool_cleanup();
	f_unlink("/atmosphere/titles/0100000000001000/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001000/fsmitm.flag");
	f_unlink("/SXOS/titles/0100000000001000/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001000/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000000034-OFF/exefs.nsp");
	f_unlink("/sxos/titles/0100000000000034-OFF");
	f_rename("/sxos/titles/0100000000000034", "/sxos/titles/0100000000000034-OFF");
	f_unlink("/sxos/titles/0100000000000034/exefs.nsp");
	//this is a panic option so i will disable the servises also
	char* folder_ams = listfol("/atmosphere/titles", "*", true);
			u32 r = 0;
		while(folder_ams[r * 256])
		{
			if(strlen(&folder_ams[r * 256]) <= 100){			
			char* source_ams = (char*)malloc(256);
			strcpy(source_ams, "/atmosphere/titles/");
			strcat(source_ams, &folder_ams[r * 256]);
			strcat(source_ams, "/flags/boot2.flag");
			f_unlink(source_ams);
			}
		r++;
		}

	char* folder_rei = listfol("/ReiNX/titles", "*", true);
			u32 w = 0;
		while(folder_rei[w * 256])
		{
			if(strlen(&folder_rei[w * 256]) <= 100){			
			char* source_rei = (char*)malloc(256);
			strcpy(source_rei, "/ReiNX/titles/");
			strcat(source_rei, &folder_rei[w * 256]);
			strcat(source_rei, "/boot2.flag");
			f_unlink(source_rei);
			}
		w++;
		}
		
	char* folder_sxos = listfol("/sxos/titles", "*", true);
			u32 e = 0;
		while(folder_sxos[e * 256])
		{
			if(strlen(&folder_sxos[e * 256]) <= 100){			
			char* source_sxos = (char*)malloc(256);
			strcpy(source_sxos, "/sxos/titles/");
			strcat(source_sxos, &folder_sxos[e * 256]);
			strcat(source_sxos, "/flags/boot2.flag");
			f_unlink(source_sxos);
			}
		e++;
		}

	//reactivate ams critical serv
	sd_save_to_file("", 0, "/atmosphere/titles/0100000000000032/flags/boot2.flag");
	sd_save_to_file("", 0, "/atmosphere/titles/0100000000000037/flags/boot2.flag");
    sd_save_to_file("", 0, "fixer.del");
launch_payload("payload.bin");
return 0;
}

//Themes///// copy
int tool_theme(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(1, 1000);
copyfileparam(param,"background.bmp","NEXT/background.bmp");

char* sourse_folder = (char*)malloc(256);	
			strcpy(sourse_folder, param);
			strcat(sourse_folder, "/Icons");
copy_folder(sourse_folder, "NEXT/Icons");
			strcpy(sourse_folder, param);
			strcat(sourse_folder, "/logos");
copy_folder(sourse_folder, "NEXT/logos");
//display_backlight_brightness(100, 1000);
gui_init_argon_menu();
return 0;
}

//Memloader
int memloader(u32 fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
	//boot0
	if(fil == 0)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=NEXT/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=NEXT/uboot/ums_emmc_boot0.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("maxMemoryFreq=200\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("NEXT/payback/memloader.bin");
	}

	//boot1
	if(fil == 1)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=NEXT/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=NEXT/uboot/ums_emmc_boot1.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("maxMemoryFreq=-1600\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("NEXT/payback/memloader.bin");
	}

	//emmc
	if(fil == 2)
	{
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=NEXT/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0006e13f\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=NEXT/uboot/ums_emmc.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("NEXT/payback/memloader.bin");
	}
	
	if(fil == 3)
	{
	//sd
			FIL fp;
			f_open(&fp, "auto.bak", FA_WRITE | FA_CREATE_ALWAYS);
			f_puts("[load:PH_0]\n", &fp);
			f_puts("if=NEXT/uboot/u-boot.elf\n", &fp);
			f_puts("skip=0x00010000\n", &fp);
			f_puts("count=0x0007c1bf\n", &fp);
			f_puts("dst=0x80110000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[load:script]\n", &fp);
			f_puts("if=NEXT/uboot/ums_sd.scr.img\n", &fp);
			f_puts("dst=0x80100000\n", &fp);
			f_puts("\n", &fp);
			f_puts("[boot:ENTRY]\n", &fp);
			f_puts("pc=0x80110000\n", &fp);
			f_puts("pwroffHoldTime=4\n", &fp);
			f_puts("\n", &fp);
			f_close(&fp);
			launch_payload("NEXT/payback/memloader.bin");
	}
return 0;
}

int bat_show(u32 percent)
{
	if(percent <= 100)
	{
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0xFFCCCCCC, 0xFF191414);
		gfx_con_setpos(&g_gfx_con, 1215, 10);
        gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22,percent,0xFFCCCCCC);
//      gfx_printf(&g_gfx_con, "%k%d%%%k", 0xFF00FF22,percent,0xFFCCCCCC);
		gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
		
	}
	
	if(percent == 145)
	{
		g_gfx_con.scale = 10;
		gfx_con_setpos(&g_gfx_con, 12, 400);
        gfx_printf(&g_gfx_con, "ArgonNX v0.3\nBy %kKronos2308%k", 0xFF00FF22,0xFFCCCCCC);
	}
return 0;
}

void serv_display(gui_menu_t* menu,char* titleid,char* name) 
{
if (!sd_mount()){BootStrapNX();}//check sd
		char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, titleid);
		strcat(path, "/exefs.nsp");

		char* flagpath = (char*)malloc(256);
		strcpy(flagpath, "atmosphere/titles/");
		strcat(flagpath, titleid);
		strcat(flagpath, "/flags/boot2.flag");

	if (sd_file_exists(path))
	{
		if (sd_file_exists(flagpath))
		{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-on.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_off, (void*)titleid));
		}else{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-off.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_on, (void*)titleid));
		}
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(name,servX+30, servY+30, 150, 100, NULL, NULL));
	servY = servY + sepaserv;
	}
}

void help_display(void) 
{
if (!sd_mount()){BootStrapNX();}//check sd
help_switch++;
gui_init_argon_menu();
}

int Incognito(char* order)
{
	if (!sd_mount()){BootStrapNX();}//check sd
			char* incog = (char*)malloc(256);
			strcpy(incog, "NEXT/");
			strcat(incog, order);
			strcat(incog, ".inc");
			sd_save_to_file("", 0, incog);
			sd_save_to_file("", 0, "NEXT/inc.return");
			launch_payload("NEXT/payback/Incognito_RCM.bin");
			return 0;
}
int Autoboot(u32 fil)
{
	if (!sd_mount()){BootStrapNX();}//check sd
	if(fil == 0)
		f_unlink("NEXT/autobootecho.txt");
	
	if(fil == 1)
		sd_save_to_file("", 0, "NEXT/autobootecho.txt");
	
	gui_init_argon_menu();
return 0;
};


/*
void update_script(gui_menu_t* menu,char* titleid,char* name) 
{

char* payloads = listfil(directory, filet, true);
    u32 i = 0;
    while(payloads[i * 256])
    {
char* payloadpath = (char*)malloc(256);
			if(strlen(&payloads[i * 256]) <= 100){			
			strcpy(payloadpath, directory);
			strcat(payloadpath, "/");
			strcat(payloadpath, &payloads[i * 256]);
			f_unlink(payloadpath);
			}
	i++;
    }
f_unlink(directory);



		char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, titleid);
		strcat(path, "/exefs.nsp");

		char* flagpath = (char*)malloc(256);
		strcpy(flagpath, "atmosphere/titles/");
		strcat(flagpath, titleid);
		strcat(flagpath, "/flags/boot2.flag");

	if (sd_file_exists(path))
	{
		if (sd_file_exists(flagpath))
		{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-on.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_off, (void*)titleid));
		}else{
	gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/NEXT/Icons/sw-off.bmp"),servX, servY, 200, 75,(int (*)(void *))tool_servises_on, (void*)titleid));
		}
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(name,servX+30, servY+30, 150, 100, NULL, NULL));
	}
	servY = servY + sepaserv;
}

*/
/*
//	char *sd_path = "makunga/mitril/tormente";
	char *outFilename = sd_path;
	u32 sdPathLen = strlen(sd_path);
	char recip[0];
	while(true)
	{

	recip[0] = outFilename[sdPathLen--];
//	gfx_printf(&g_gfx_con, "-%s\n",recip);

	if(strstr(recip,"/") != NULL){
//	gfx_printf(&g_gfx_con, "1-%s-%d\n",recip,sdPathLen);
	break;}
	
	if(sdPathLen <= 0){
//	gfx_printf(&g_gfx_con, "2\n",recip);
	return 0;
	break;}

	}
	char name[100];
	u32 i = 0;
	
		while(true)
	{
		name[i] = outFilename[i];
		i++;
		if(sdPathLen+1 <= i)
		break;
	}
//	gfx_printf(&g_gfx_con, "%s",name);
	sd_path = name;
	return sd_path;
}






        // Traverse from right 
        for (int i = strlen(str) - 1; i >= 0; i--) 
 //           if (str[i] == x)
			if(strstr(str[i],x) != NULL)
                return i; 
        return -1; 


static int zbackup(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(1, 1000);
sd_save_to_file("", 0, param);
launch_payload("NEXT/payloads/zbackup.bin");
return 0;
}


static int tool_menu_rSD(void* param)
{
	sd_mount();
		if (g_sd_mounted)
		{
		launch_payload("payload.bin");
		}

return 0;
}
*/

