#if defined DONT_REMOVE

    » Gamemode Xyronite by Luminouz
    
    > Credits: - Kalcor (For the SA:MP)
			   - Y_Less (For the YSI, sscanf, foreach)
			   - pBlueG (For the MySQL)
			   - ZeeX (For the IZCMD)
			   - Yashas (For the EVF2)
			   - SyS (For the samp-bcrypt)
			   - Southclaws (For the progress2)
			   - Incognito (For the streamer)
			   - LuminouZ (For the main Gamemode Scripter)
			   
    > NOTE: Please don't remove the credits!
    
    
    ===================== » Changelog ========================
    
    > Added Owner Vehicle System
    > Added Vehicle Insurance System
    > Added Dynamic Inventory System
    
#endif

/* Includes */
#include <open.mp>
#include <a_mysql>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_timers>
#include <samp_bcrypt>
#include <Pawn.CMD>
#include <EVF2>
#include <progress2>
#include <sscanf2>
#include <streamer>

/* Define & Macro */
#define MIXED_SPELLINGS
#define forex(%0,%1) for(new %0 = 0; %0 < %1; %0++)

#define FUNC::%0(%1) forward %0(%1); public %0(%1)

#define COLOR_YELLOW 			0xFFFF00FF
#define COLOR_SERVER 			0x00FFFFFF
#define COLOR_GREY   			0xAFAFAFFF
#define COLOR_PURPLE 			0xD0AEEBFF
#define COLOR_CLIENT 			0xC6E2FFFF
#define COLOR_WHITE  			0xFFFFFFFF
#define COLOR_LIGHTRED    		0xFF6347FF

#define MAX_CHARS 3

#define DATABASE_ADDRESS "localhost" //Change this to your Database Address
#define DATABASE_USERNAME "root" // Change this to your database username
#define DATABASE_PASSWORD "" //Change this to your database password
#define DATABASE_NAME "basic"

#if !defined BCRYPT_HASH_LENGTH
	#define BCRYPT_HASH_LENGTH 250
#endif

#if !defined BCRYPT_COST
	#define BCRYPT_COST 12
#endif

#define SendServerMessage(%0,%1) \
	SendClientMessageEx(%0, 0x00FFFFFF, "SERVER:{FFFFFF} "%1)

#define SendSyntaxMessage(%0,%1) \
	SendClientMessageEx(%0, COLOR_GREY, "USAGE:{FFFFFF} "%1)
	
#define SendErrorMessage(%0,%1) \
	SendClientMessageEx(%0, COLOR_GREY, "ERROR: "%1)
	
#define MAX_PLAYER_VEHICLE 			1000
#define MAX_INVENTORY 				20
#define MAX_BUSINESS                100
#define MAX_DROPPED_ITEMS  			1000
#define MAX_RENTAL                  20

/* Variable */

new MySQL:sqlcon;
new g_RaceCheck[MAX_PLAYERS char];
new PlayerChar[MAX_PLAYERS][MAX_CHARS][MAX_PLAYER_NAME + 1];
new tempUCP[64];

new PlayerText:ENERGYTD[MAX_PLAYERS][2];
new PlayerBar:ENERGYBAR[MAX_PLAYERS];

new PlayerText:SPEEDOTD[MAX_PLAYERS][4];
new PlayerBar:FUELBAR[MAX_PLAYERS];
new PlayerText:HEALTHTD[MAX_PLAYERS];
new PlayerText:KMHTD[MAX_PLAYERS];
new PlayerText:VEHNAMETD[MAX_PLAYERS];
new PlayerText:MSGTD[MAX_PLAYERS];

new g_aMaleSkins[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	30, 32, 33, 34, 35, 36, 37, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 57, 58, 59, 60,
	61, 62, 66, 68, 72, 73, 78, 79, 80, 81, 82, 83, 84, 94, 95, 96, 97, 98, 99, 100, 101, 102,
	103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
	121, 122, 123, 124, 125, 126, 127, 128, 132, 133, 134, 135, 136, 137, 142, 143, 144, 146,
	147, 153, 154, 155, 156, 158, 159, 160, 161, 162, 167, 168, 170, 171, 173, 174, 175, 176,
	177, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 200, 202, 203, 204, 206,
	208, 209, 210, 212, 213, 217, 220, 221, 222, 223, 228, 229, 230, 234, 235, 236, 239, 240,
	241, 242, 247, 248, 249, 250, 253, 254, 255, 258, 259, 260, 261, 262, 268, 272, 273, 289,
	290, 291, 292, 293, 294, 295, 296, 297, 299
};

new g_aFemaleSkins[] = {
    9, 10, 11, 12, 13, 31, 38, 39, 40, 41, 53, 54, 55, 56, 63, 64, 65, 69,
    75, 76, 77, 85, 88, 89, 90, 91, 92, 93, 129, 130, 131, 138, 140, 141,
    145, 148, 150, 151, 152, 157, 169, 178, 190, 191, 192, 193, 194, 195,
    196, 197, 198, 199, 201, 205, 207, 211, 214, 215, 216, 219, 224, 225,
    226, 231, 232, 233, 237, 238, 243, 244, 245, 246, 251, 256, 257, 263,
    298
};

/* Enums */

enum e_faction
{
	FACTION_LSPD,
	FACTION_LSES,
	FACTION_LSN,
	FACTION_LSG
};

enum inventoryData
{
	invExists,
	invID,
	invItem[32 char],
	invModel,
	invQuantity
};

new InventoryData[MAX_PLAYERS][MAX_INVENTORY][inventoryData];

	
enum e_InventoryItems
{
	e_InventoryItem[32],
	e_InventoryModel
};


new const g_aInventoryItems[][e_InventoryItems] =
{
	{"GPS", 18875},
	{"Cellphone", 18867},
	{"Medkit", 1580},
	{"Portable Radio", 19942},
	{"Mask", 19036},
	{"Snack", 2768},
	{"Water", 2958}
};

enum vCore
{
	vehFuel,
};
new VehCore[MAX_VEHICLES][vCore];
	

enum droppedItems
{
	droppedID,
	droppedItem[32],
	droppedPlayer[24],
	droppedModel,
	droppedQuantity,
	Float:droppedPos[3],
	droppedWeapon,
	droppedAmmo,
	droppedInt,
	droppedWorld,
	droppedObject,
	Text3D:droppedText3D
};

new DroppedItems[MAX_DROPPED_ITEMS][droppedItems];

enum vData
{
	vID,
	vOwner,
	vColor[2],
	vModel,
	vLocked,
	vInsurance,
	vInsuTime,
	vPlate[16],
	Float:vHealth,
	Float:vPos[4],
	vWorld,
	vInterior,
	vFuel,
	vVehicle,
	vDamage[4],
	VEHICLE_PANEL_STATUS:vPanelDamage, // Menggunakan tag VEHICLE_PANEL_STATUS
    VEHICLE_DOOR_STATUS:vDoorDamage,   // Menggunakan tag VEHICLE_DOOR_STATUS
    VEHICLE_LIGHT_STATUS:vLightDamage, // Menggunakan tag VEHICLE_LIGHT_STATUS
    VEHICLE_TIRE_STATUS:vTireDamage,    // Menggunakan tag VEHICLE_TIRE_STATUS
	bool:vExists,
	vRental,
	vRentTime,
};
new VehicleData[MAX_PLAYER_VEHICLE][vData];

enum
{
	DIALOG_REGISTER,
	DIALOG_LOGIN,
	DIALOG_MAKECHAR,
	DIALOG_ORIGIN,
	DIALOG_AGE,
	DIALOG_GENDER,
	DIALOG_CHARLIST,
	DIALOG_NONE,
	DIALOG_BIZBUY,
	DIALOG_INVENTORY,
	DIALOG_GIVEITEM,
	DIALOG_DROPITEM,
	DIALOG_USEITEM,
	DIALOG_GIVEAMOUNT,
	DIALOG_INVACTION,
	DIALOG_BUYSKINS,
	DIALOG_INSURANCE,
	DIALOG_BUYINSURANCE,
	DIALOG_RENTAL,
	DIALOG_RENTTIME,
	DIALOG_BIZMENU,
	DIALOG_BIZNAME,
	DIALOG_BIZPROD,
	DIALOG_BIZPRODSET,
	DIALOG_BIZPRICE,
	DIALOG_BIZPRICESET,
	DIALOG_BIZCARGO
};

enum e_player_data
{
	pID,
	pUCP[22],
	pName[MAX_PLAYER_NAME],
	Float:pPos[3],
	pWorld,
	pInterior,
	pSkin,
	pAge,
	pAttempt,
	pOrigin[32],
	pGender,
	bool:pMaskOn,
	pMaskID,
	bool:pSpawned,
	pChar,
	Float:pHealth,
	pEnergy,
	pMoney,
	pBank,
	pInBiz,
	pListitem,
	pStorageSelect,
	pAdmin,
	pAduty,
	pPhoneNumber,
	pCalline,
	pCredit,
	pCalling,
	pTarget,
	pSkinPrice,
	pVehKey,
	pFaction,
	pRenting,
};

new PlayerData[MAX_PLAYERS][e_player_data];

enum e_biz_data
{
	bizID,
	bizName[32],
	bizOwner,
	bizOwnerName[MAX_PLAYER_NAME],
	bool:bizExists,
	Float:bizInt[3],
	Float:bizExt[3],
	bizWorld,
	bizInterior,
	bizVault,
	bizPrice,
	bizLocked,
	bizFuel,
	bizProduct[7],
	bizType,
	bizStock,
	STREAMER_TAG_PICKUP:bizFuelPickup,
	STREAMER_TAG_3D_TEXT:bizFuelText,
	STREAMER_TAG_PICKUP:bizDeliverPickup,
	STREAMER_TAG_3D_TEXT:bizDeliverText,
	STREAMER_TAG_PICKUP:bizPickup,
	STREAMER_TAG_3D_TEXT_LABEL:bizText,
	STREAMER_TAG_CP:bizCP,
};

new BizData[MAX_BUSINESS][e_biz_data];
new ProductName[MAX_BUSINESS][7][24];

enum e_rental
{
	rentID,
	bool:rentExists,
	Float:rentPos[3],
	Float:rentSpawn[4],
	rentModel[2],
	rentPrice[2],
	STREAMER_TAG_3D_TEXT_LABEL:rentText,
	STREAMER_TAG_PICKUP:rentPickup,
};

new RentData[MAX_RENTAL][e_rental];

/* Functions */

stock GetElapsedTime(time, &hours, &minutes, &seconds)
{
	hours = 0;
	minutes = 0;
	seconds = 0;

	if (time >= 3600)
	{
		hours = (time / 3600);
		time -= (hours * 3600);
	}
	while (time >= 60)
	{
	    minutes++;
	    time -= 60;
	}
	return (seconds = time);
}

stock ShowMessage(playerid, const string[], time)//Time in Sec.
{
	new validtime = time*1000;

	PlayerTextDrawSetString(playerid, MSGTD[playerid], string);
	PlayerTextDrawShow(playerid, MSGTD[playerid]);
	SetTimerEx("HideMessage", validtime, false, "d", playerid);
	return 1;
}

FUNC::HideMessage(playerid)
{
	return PlayerTextDrawHide(playerid, MSGTD[playerid]);
}

stock Biz_GetCount(playerid)
{
	new count = 0;
	forex(i, MAX_BUSINESS) if(BizData[i][bizExists] && BizData[i][bizOwner] == PlayerData[playerid][pID])
	{
	    count++;
	}
	return count;
}

stock RandomEx(min, max)
{
	new rand = random(max-min)+min;
	return rand;
}

stock UpdatePlayerSkin(playerid, skinid)
{
	SetPlayerSkin(playerid, skinid);
	PlayerData[playerid][pSkin] = skinid;
}

stock StreamerConfig()
{
	Streamer_MaxItems(STREAMER_TYPE_OBJECT, 990000);
	Streamer_MaxItems(STREAMER_TYPE_MAP_ICON, 2000);
	Streamer_MaxItems(STREAMER_TYPE_PICKUP, 2000);
	for(new playerid = (GetMaxPlayers() - 1); playerid != -1; playerid--)
	{
		Streamer_DestroyAllVisibleItems(playerid, 0);
	}
	Streamer_VisibleItems(STREAMER_TYPE_OBJECT, 1000);
	return 1;
}

FUNC::OnPlayerUseItem(playerid, itemid, const name[])
{
	if(!strcmp(name, "Snack"))
	{
        if (PlayerData[playerid][pEnergy] > 90)
            return SendErrorMessage(playerid, "Energy milikmu sudah penuh.");

        PlayerData[playerid][pEnergy] += 10;
		Inventory_Remove(playerid, "Snack", 1);
		ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, 0, 0, 0, 0, 0, SYNC_ALL);
        SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s takes a snack and eats it.", ReturnName(playerid));
	}
	else if(!strcmp(name, "Water"))
	{
        if (PlayerData[playerid][pEnergy] > 90)
            return SendErrorMessage(playerid, "Energy milikmu sudah penuh.");

        PlayerData[playerid][pEnergy] += 10;
		Inventory_Remove(playerid, "Water", 1);
        SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s takes a water mineral and drinks it.", ReturnName(playerid));
	}
	return 1;
}

FUNC::Dropped_Load()
{
	new rows = cache_num_rows();
 	if(rows)
  	{
    	forex(i, rows)
		{
		    cache_get_value_name_int(i, "ID", DroppedItems[i][droppedID]);

			cache_get_value_name(i, "itemName", DroppedItems[i][droppedItem]);
			cache_get_value_name(i, "itemPlayer", DroppedItems[i][droppedPlayer]);

			cache_get_value_name_int(i, "itemModel", DroppedItems[i][droppedModel]);
			cache_get_value_name_int(i, "itemQuantity", DroppedItems[i][droppedQuantity]);
			cache_get_value_name_float(i, "itemX", DroppedItems[i][droppedPos][0]);
			cache_get_value_name_float(i, "itemY", DroppedItems[i][droppedPos][1]);
			cache_get_value_name_float(i, "itemZ", DroppedItems[i][droppedPos][2]);
			cache_get_value_name_int(i, "itemInt", DroppedItems[i][droppedInt]);
			cache_get_value_name_int(i, "itemWorld", DroppedItems[i][droppedWorld]);

			DroppedItems[i][droppedObject] = CreateDynamicObject(DroppedItems[i][droppedModel], DroppedItems[i][droppedPos][0], DroppedItems[i][droppedPos][1], DroppedItems[i][droppedPos][2], 0.0, 0.0, 0.0, DroppedItems[i][droppedWorld], DroppedItems[i][droppedInt]);
			DroppedItems[i][droppedText3D] = CreateDynamic3DTextLabel(DroppedItems[i][droppedItem], COLOR_SERVER, DroppedItems[i][droppedPos][0], DroppedItems[i][droppedPos][1], DroppedItems[i][droppedPos][2], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, DroppedItems[i][droppedWorld], DroppedItems[i][droppedInt]);
		}
		printf("[DROPITEM] Loaded %d Dropped items from database.", rows);
	}
	return 1;
}

stock Inventory_Clear(playerid)
{
	static
	    string[64];

	forex(i, MAX_INVENTORY)
	{
	    if (InventoryData[playerid][i][invExists])
	    {
	        InventoryData[playerid][i][invExists] = 0;
	        InventoryData[playerid][i][invModel] = 0;
	        InventoryData[playerid][i][invQuantity] = 0;
		}
	}
	format(string, sizeof(string), "DELETE FROM `inventory` WHERE `ID` = '%d'", PlayerData[playerid][pID]);
	return mysql_tquery(sqlcon, string);
}

stock Inventory_GetItemID(playerid, const item[])
{
	forex(i, MAX_INVENTORY)
	{
	    if (!InventoryData[playerid][i][invExists])
	        continue;

		if (!strcmp(InventoryData[playerid][i][invItem], item)) return i;
	}
	return -1;
}

stock Inventory_GetFreeID(playerid)
{
	if (Inventory_Items(playerid) >= 20)
		return -1;

	forex(i, MAX_INVENTORY)
	{
	    if (!InventoryData[playerid][i][invExists])
	        return i;
	}
	return -1;
}

stock Inventory_Items(playerid)
{
    new count;

    forex(i, MAX_INVENTORY) if (InventoryData[playerid][i][invExists]) {
        count++;
	}
	return count;
}

stock Inventory_Count(playerid, const item[])
{
	new itemid = Inventory_GetItemID(playerid, item);

	if (itemid != -1)
	    return InventoryData[playerid][itemid][invQuantity];

	return 0;
}

stock PlayerHasItem(playerid, const item[])
{
	return (Inventory_GetItemID(playerid, item) != -1);
}

stock Inventory_Set(playerid, const item[], model, amount)
{
	new itemid = Inventory_GetItemID(playerid, item);

	if (itemid == -1 && amount > 0)
		Inventory_Add(playerid, item, model, amount);

	else if (amount > 0 && itemid != -1)
	    Inventory_SetQuantity(playerid, item, amount);

	else if (amount < 1 && itemid != -1)
	    Inventory_Remove(playerid, item, -1);

	return 1;
}

stock Inventory_SetQuantity(playerid, const item[], quantity)
{
	new
	    itemid = Inventory_GetItemID(playerid, item),
	    string[128];

	if (itemid != -1)
	{
	    format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, PlayerData[playerid][pID], InventoryData[playerid][itemid][invID]);
	    mysql_tquery(sqlcon, string);

	    InventoryData[playerid][itemid][invQuantity] = quantity;
	}
	return 1;
}

stock Inventory_Remove(playerid, const item[], quantity = 1)
{
	new
		itemid = Inventory_GetItemID(playerid, item),
		string[128];

	if (itemid != -1)
	{
	    if (InventoryData[playerid][itemid][invQuantity] > 0)
	    {
	        InventoryData[playerid][itemid][invQuantity] -= quantity;
		}
		if (quantity == -1 || InventoryData[playerid][itemid][invQuantity] < 1)
		{
		    InventoryData[playerid][itemid][invExists] = false;
		    InventoryData[playerid][itemid][invModel] = 0;
		    InventoryData[playerid][itemid][invQuantity] = 0;

		    format(string, sizeof(string), "DELETE FROM `inventory` WHERE `ID` = '%d' AND `invID` = '%d'", PlayerData[playerid][pID], InventoryData[playerid][itemid][invID]);
	        mysql_tquery(sqlcon, string);
		}
		else if (quantity != -1 && InventoryData[playerid][itemid][invQuantity] > 0)
		{
			format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = `invQuantity` - %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, PlayerData[playerid][pID], InventoryData[playerid][itemid][invID]);
            mysql_tquery(sqlcon, string);
		}
		return 1;
	}
	return 0;
}

stock Inventory_Add(playerid, const item[], model, quantity = 1)
{
	new
		itemid = Inventory_GetItemID(playerid, item),
		string[128];

	if (itemid == -1)
	{
	    itemid = Inventory_GetFreeID(playerid);

	    if (itemid != -1)
	    {
	        InventoryData[playerid][itemid][invExists] = true;
	        InventoryData[playerid][itemid][invModel] = model;
	        InventoryData[playerid][itemid][invQuantity] = quantity;

	        strpack(InventoryData[playerid][itemid][invItem], item, 32 char);

			format(string, sizeof(string), "INSERT INTO `inventory` (`ID`, `invItem`, `invModel`, `invQuantity`) VALUES('%d', '%s', '%d', '%d')", PlayerData[playerid][pID], item, model, quantity);
			mysql_tquery(sqlcon, string, "OnInventoryAdd", "dd", playerid, itemid);
	        return itemid;
		}
		return -1;
	}
	else
	{
	    format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = `invQuantity` + %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, PlayerData[playerid][pID], InventoryData[playerid][itemid][invID]);
	    mysql_tquery(sqlcon, string);

	    InventoryData[playerid][itemid][invQuantity] += quantity;
	}
	return itemid;
}

FUNC::OnInventoryAdd(playerid, itemid)
{
	InventoryData[playerid][itemid][invID] = cache_insert_id();
	return 1;
}

FUNC::ShowInventory(playerid, targetid)
{
    if (!IsPlayerConnected(playerid))
	    return 0;

	new
	    items[MAX_INVENTORY],
		amounts[MAX_INVENTORY],
		str[512],
		string[352],
		count = 0;

	format(str, sizeof(str), "Name\tAmount\n");
	format(str, sizeof(str), "%s\nMoney\t%s", str, FormatNumber(GetMoney(targetid)));
    forex(i, 20)
	{
 		if (InventoryData[targetid][i][invExists])
        {
            count++;
   			items[i] = InventoryData[targetid][i][invModel];
   			amounts[i] = InventoryData[targetid][i][invQuantity];
   			strunpack(string, InventoryData[targetid][i][invItem]);
   			format(str, sizeof(str), "%s\n%s\t%d", str, string, amounts[i]);
		}
	}
	ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Data", str,  "Close", "");
	return 1;

}


FUNC::OpenInventory(playerid)
{
    if (!IsPlayerConnected(playerid))
	    return 0;

	new
	    items[MAX_INVENTORY],
		amounts[MAX_INVENTORY],
		str[512],
		string[256],
		count = 0;

	format(str, sizeof(str), "Name\tAmount\n");
    forex(i, 20)
	{
 		if (InventoryData[playerid][i][invExists])
        {
            count++;
   			items[i] = InventoryData[playerid][i][invModel];
   			amounts[i] = InventoryData[playerid][i][invQuantity];
   			strunpack(string, InventoryData[playerid][i][invItem]);
   			format(str, sizeof(str), "%s\n%s\t%d", str, string, amounts[i]);
		}
	}
	if(count)
	{
		ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS, "Inventory Data", str, "Select", "Close");
	}
	else
	{
	    ShowMessage(playerid, "~r~ERROR ~w~Tidak ada Item apapun di Inventory!", 3);
	}
	return 1;

}

DropItem(const item[], const player[], model, quantity, Float:x, Float:y, Float:z, interior, world, weaponid = 0, ammo = 0)
{
	new
	    query[300];

	forex(i, MAX_DROPPED_ITEMS) if (!DroppedItems[i][droppedModel])
	{
	    format(DroppedItems[i][droppedItem], 32, item);
	    format(DroppedItems[i][droppedPlayer], 24, player);

		DroppedItems[i][droppedModel] = model;
		DroppedItems[i][droppedQuantity] = quantity;
		DroppedItems[i][droppedWeapon] = weaponid;
  		DroppedItems[i][droppedAmmo] = ammo;
		DroppedItems[i][droppedPos][0] = x;
		DroppedItems[i][droppedPos][1] = y;
		DroppedItems[i][droppedPos][2] = z;

		DroppedItems[i][droppedInt] = interior;
		DroppedItems[i][droppedWorld] = world;

		DroppedItems[i][droppedObject] = CreateDynamicObject(model, x, y, z, 0.0, 0.0, 0.0, world, interior);

 		DroppedItems[i][droppedText3D] = CreateDynamic3DTextLabel(item, COLOR_SERVER, x, y, z, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, world, interior);

 		format(query, sizeof(query), "INSERT INTO `dropped` (`itemName`, `itemPlayer`, `itemModel`, `itemQuantity`, `itemWeapon`, `itemAmmo`, `itemX`, `itemY`, `itemZ`, `itemInt`, `itemWorld`) VALUES('%s', '%s', '%d', '%d', '%d', '%d', '%.4f', '%.4f', '%.4f', '%d', '%d')", item, player, model, quantity, weaponid, ammo, x, y, z, interior, world);
		mysql_tquery(sqlcon, query, "OnDroppedItem", "d", i);
		return i;
	}
	return -1;
}

DropPlayerItem(playerid, itemid, quantity = 1)
{
	if (itemid == -1 || !InventoryData[playerid][itemid][invExists])
	    return 0;

    new
		Float:x,
  		Float:y,
    	Float:z,
		Float:angle,
		string[32];

	strunpack(string, InventoryData[playerid][itemid][invItem]);

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, angle);

	DropItem(string, ReturnName(playerid), InventoryData[playerid][itemid][invModel], quantity, x, y, z - 0.9, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
 	Inventory_Remove(playerid, string, quantity);

	ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 4.1, 0, 0, 0, 0, 0, SYNC_ALL);
 	SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has dropped a \"%s\".", ReturnName(playerid), string);
	return 1;
}

FUNC::LoadPlayerItems(playerid)
{
	new name[128];
	new count = cache_num_rows();
	if(count > 0)
	{
	    forex(i, count)
	    {
	        InventoryData[playerid][i][invExists] = true;

	        cache_get_value_name_int(i, "invID", InventoryData[playerid][i][invID]);
	        cache_get_value_name_int(i, "invModel", InventoryData[playerid][i][invModel]);
	        cache_get_value_name_int(i, "invQuantity", InventoryData[playerid][i][invQuantity]);

	        cache_get_value_name(i, "invItem", name);

			strpack(InventoryData[playerid][i][invItem], name, 32 char);
		}
	}
	return 1;
}

GiveMoney(playerid, amount)
{
	PlayerData[playerid][pMoney] += amount;
	GivePlayerMoney(playerid, amount);
	return 1;
}

stock GetEnergy(playerid)
	return PlayerData[playerid][pEnergy];
	
stock GetMoney(playerid)
{
	return PlayerData[playerid][pMoney];
}

stock Rental_Create(playerid, veh1, veh2)
{
	new
	    Float:x,
	    Float:y,
	    Float:z;

	if (GetPlayerPos(playerid, x, y, z))
	{
		forex(i, MAX_RENTAL)
		{
		    if(!RentData[i][rentExists])
		    {
		        RentData[i][rentExists] = true;
		        RentData[i][rentModel][0] = veh1;
		        RentData[i][rentModel][1] = veh2;
		        RentData[i][rentPos][0] = x;
		        RentData[i][rentPos][1] = y;
		        RentData[i][rentPos][2] = z;
		        RentData[i][rentSpawn][0] = 0;
		        RentData[i][rentSpawn][1] = 0;
		        RentData[i][rentSpawn][2] = 0;
		        
		        Rental_Refresh(i);
		        mysql_tquery(sqlcon, "INSERT INTO `rental` (`Vehicle1`) VALUES(0)", "OnRentalCreated", "d", i);
		        return i;
			}
		}
	}
	return -1;
}

stock Business_Create(playerid, type, price)
{
	new
	    Float:x,
	    Float:y,
	    Float:z;

	if (GetPlayerPos(playerid, x, y, z))
	{
		forex(i, MAX_BUSINESS)
		{
	    	if (!BizData[i][bizExists])
		    {
    	        BizData[i][bizExists] = true;
        	    BizData[i][bizOwner] = -1;
            	BizData[i][bizPrice] = price;
            	BizData[i][bizType] = type;

				format(BizData[i][bizName], 32, "None Business");
				format(BizData[i][bizOwnerName], MAX_PLAYER_NAME, "No Owner");
    	        BizData[i][bizExt][0] = x;
    	        BizData[i][bizExt][1] = y;
    	        BizData[i][bizExt][2] = z;

				if (type == 1)
				{
                	BizData[i][bizInt][0] = 363.22;
                	BizData[i][bizInt][1] = -74.86;
                	BizData[i][bizInt][2] = 1001.50;
					BizData[i][bizInterior] = 10;
					format(ProductName[i][0], 24, "French Fries");
					format(ProductName[i][1], 24, "Mac n Cheese");
					format(ProductName[i][2], 24, "Fried Chicken");
				}
				else if (type == 2)
				{
                	BizData[i][bizInt][0] = 5.73;
                	BizData[i][bizInt][1] = -31.04;
                	BizData[i][bizInt][2] = 1003.54;
					BizData[i][bizInterior] = 10;
					format(ProductName[i][0], 24, "Chitato");
					format(ProductName[i][1], 24, "Danone Mineral");
					format(ProductName[i][2], 24, "Mask");
					format(ProductName[i][3], 24, "First Aid");
				}
				else if(type == 3)
				{
                	BizData[i][bizInt][0] = 207.55;
                	BizData[i][bizInt][1] = -110.67;
                	BizData[i][bizInt][2] = 1005.13;
					BizData[i][bizInterior] = 15;
					format(ProductName[i][0], 24, "Uniqlo Clothes");
				}
				else if(type == 4)
				{
                	BizData[i][bizInt][0] = -2240.7825;
                	BizData[i][bizInt][1] = 137.1855;
                	BizData[i][bizInt][2] = 1035.4141;
					BizData[i][bizInterior] = 6;
					format(ProductName[i][0], 24, "Huawei Mate");
					format(ProductName[i][1], 24, "GPS");
					format(ProductName[i][2], 24, "Walkie Talkie");
					format(ProductName[i][3], 24, "Electric Credit");
				}
				BizData[i][bizVault] = 0;
				BizData[i][bizStock] = 100;

				Business_Refresh(i);
				mysql_tquery(sqlcon, "INSERT INTO `business` (`bizOwner`) VALUES(0)", "OnBusinessCreated", "d", i);
				return i;
			}
		}
	}
	return -1;
}

stock Biz_IsOwner(playerid, id)
{
	if(!BizData[id][bizExists])
	    return 0;
	    
	if(BizData[id][bizOwner] == PlayerData[playerid][pID])
		return 1;
		
	return 0;
}

FUNC::OnRentalCreated(id)
{
	if (id == -1 || !RentData[id][rentExists])
	    return 0;

	RentData[id][rentID] = cache_insert_id();
	Rental_Save(id);

	return 1;
}

FUNC::OnBusinessCreated(bizid)
{
	if (bizid == -1 || !BizData[bizid][bizExists])
	    return 0;

	BizData[bizid][bizID] = cache_insert_id();
	BizData[bizid][bizWorld] = BizData[bizid][bizID]+1000;
	
	Business_Save(bizid);

	return 1;
}

FUNC::Rental_Load()
{
	new rows = cache_num_rows();
	if(rows)
	{
	    forex(i, rows)
	    {
	        RentData[i][rentExists] = true;
	        cache_get_value_name_int(i, "ID", RentData[i][rentID]);
	        cache_get_value_name_float(i, "PosX", RentData[i][rentPos][0]);
	        cache_get_value_name_float(i, "PosY", RentData[i][rentPos][1]);
	        cache_get_value_name_float(i, "PosZ", RentData[i][rentPos][2]);
	        cache_get_value_name_float(i, "SpawnX", RentData[i][rentSpawn][0]);
	        cache_get_value_name_float(i, "SpawnY", RentData[i][rentSpawn][1]);
	        cache_get_value_name_float(i, "SpawnZ", RentData[i][rentSpawn][2]);
	        cache_get_value_name_float(i, "SpawnA", RentData[i][rentSpawn][3]);
	        cache_get_value_name_int(i, "Vehicle1", RentData[i][rentModel][0]);
	        cache_get_value_name_int(i, "Vehicle2", RentData[i][rentModel][1]);
	        cache_get_value_name_int(i, "Price1", RentData[i][rentPrice][0]);
	        cache_get_value_name_int(i, "Price2", RentData[i][rentPrice][1]);
	        
	        Rental_Refresh(i);
		}
	}
	return 1;
}
FUNC::Business_Load()
{
	new rows = cache_num_rows(), str[128];
 	if(rows)
  	{
		forex(i, rows)
		{
		    BizData[i][bizExists] = true;
		    cache_get_value_name(i, "bizName", BizData[i][bizName]);
		    cache_get_value_name_int(i, "bizOwner", BizData[i][bizOwner]);
		    cache_get_value_name_int(i, "bizID", BizData[i][bizID]);
		    cache_get_value_name_float(i, "bizExtX", BizData[i][bizExt][0]);
		    cache_get_value_name_float(i, "bizExtY", BizData[i][bizExt][1]);
		    cache_get_value_name_float(i, "bizExtZ", BizData[i][bizExt][2]);
		    cache_get_value_name_float(i, "bizIntX", BizData[i][bizInt][0]);
		    cache_get_value_name_float(i, "bizIntY", BizData[i][bizInt][1]);
		    cache_get_value_name_float(i, "bizIntZ", BizData[i][bizInt][2]);
			forex(j, 7)
			{
				format(str, 32, "bizProduct%d", j + 1);
				cache_get_value_name_int(i, str, BizData[i][bizProduct][j]);
				format(str, 32, "bizProdName%d", j + 1);
				cache_get_value_name(i, str, ProductName[i][j]);
			}

			cache_get_value_name_int(i, "bizVault", BizData[i][bizVault]);
			cache_get_value_name_int(i, "bizPrice", BizData[i][bizPrice]);
			cache_get_value_name_int(i, "bizType", BizData[i][bizType]);
			cache_get_value_name_int(i, "bizWorld", BizData[i][bizWorld]);
			cache_get_value_name_int(i, "bizInterior", BizData[i][bizInterior]);
			cache_get_value_name_int(i, "bizType", BizData[i][bizType]);
			cache_get_value_name_int(i, "bizStock", BizData[i][bizStock]);
			cache_get_value_name_int(i, "bizFuel", BizData[i][bizFuel]);
			cache_get_value_name(i, "bizOwnerName", BizData[i][bizOwnerName]);
			Business_Refresh(i);
		}
	}
	return 1;
}
stock Business_Save(bizid)
{
	new
	    query[2048];

	mysql_format(sqlcon, query, sizeof(query), "UPDATE `business` SET `bizName` = '%s', `bizOwner` = '%d', `bizExtX` = '%f', `bizExtY` = '%f', `bizExtZ` = '%f', `bizIntX` = '%f', `bizIntY` = '%f', `bizIntZ` = '%f'",
		BizData[bizid][bizName],
		BizData[bizid][bizOwner],
		BizData[bizid][bizExt][0],
		BizData[bizid][bizExt][1],
		BizData[bizid][bizExt][2],
		BizData[bizid][bizInt][0],
		BizData[bizid][bizInt][1],
		BizData[bizid][bizInt][2]
	);
	forex(i, 7)
	{
		mysql_format(sqlcon, query, sizeof(query), "%s, `bizProduct%d` = '%d'", query, i + 1, BizData[bizid][bizProduct][i]);
	}
	forex(i, 7)
	{
		mysql_format(sqlcon, query, sizeof(query), "%s, `bizProdName%d` = '%s'", query, i + 1, ProductName[bizid][i]);
	}
	mysql_format(sqlcon, query, sizeof(query), "%s, `bizWorld` = '%d', `bizInterior` = '%d', `bizVault` = '%d', `bizType` = '%d', `bizStock` = '%d', `bizPrice` = '%d', `bizFuel` = '%d', `bizOwnerName` = '%s' WHERE `bizID` = '%d'",
		query,
		BizData[bizid][bizWorld],
		BizData[bizid][bizInterior],
		BizData[bizid][bizVault],
		BizData[bizid][bizType],
		BizData[bizid][bizStock],
		BizData[bizid][bizPrice],
		BizData[bizid][bizFuel],
		BizData[bizid][bizOwnerName],
		BizData[bizid][bizID]
	);
	return mysql_tquery(sqlcon, query);
}

stock GetBizType(type)
{
	new str[32];
	switch(type)
	{
	    case 1: str = "Fast Food";
	    case 2: str = "24/7";
	    case 3: str = "Clothes";
	    case 4: str = "Electronic";
	}
	return str;
}

FUNC::Rental_Refresh(id)
{
	if(id != -1 && RentData[id][rentExists])
	{
	    if(IsValidDynamic3DTextLabel(RentData[id][rentText]))
	        DestroyDynamic3DTextLabel(RentData[id][rentText]);
	        
		if(IsValidDynamicPickup(RentData[id][rentPickup]))
		    DestroyDynamicPickup(RentData[id][rentPickup]);
		    
		new string[156];
		format(string, sizeof(string), "[%d]\n{FFFFFF}Rental Point\n{FFFFFF}Use {FFFF00}/renthelp", id);
        RentData[id][rentText] = CreateDynamic3DTextLabel(string, COLOR_CLIENT, RentData[id][rentPos][0], RentData[id][rentPos][1], RentData[id][rentPos][2], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);
		RentData[id][rentPickup] = CreateDynamicPickup(1239, 23, RentData[id][rentPos][0], RentData[id][rentPos][1], RentData[id][rentPos][2], -1, -1);
	}
	return 1;
}

FUNC::Business_Refresh(bizid)
{
	if (bizid != -1 && BizData[bizid][bizExists])
	{
		if (IsValidDynamic3DTextLabel(BizData[bizid][bizText]))
		    DestroyDynamic3DTextLabel(BizData[bizid][bizText]);

		if (IsValidDynamicPickup(BizData[bizid][bizPickup]))
		    DestroyDynamicPickup(BizData[bizid][bizPickup]);

		if(IsValidDynamicCP(BizData[bizid][bizCP]))
		    DestroyDynamicCP(BizData[bizid][bizCP]);
		    
		new
		    string[256];

		if (BizData[bizid][bizOwner] == -1)
		{
			format(string, sizeof(string), "Type: {C6E2FF}%s\n{FFFFFF}Price: {C6E2FF}%s\n{FFFFFF}This business for sell", GetBizType(BizData[bizid][bizType]), FormatNumber(BizData[bizid][bizPrice]));
            BizData[bizid][bizText] = CreateDynamic3DTextLabel(string, -1, BizData[bizid][bizExt][0], BizData[bizid][bizExt][1], BizData[bizid][bizExt][2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);
		}
		else
		{
  			format(string, sizeof(string), "Name: %s{FFFFFF}\nStatus: {C6E2FF}%s{FFFFFF}\nType: {C6E2FF}%s", BizData[bizid][bizName], (!BizData[bizid][bizLocked]) ? ("{00FF00}Open{FFFFFF}") : ("{FF0000}Closed{FFFFFF}"), GetBizType(BizData[bizid][bizType]));
			BizData[bizid][bizText] = CreateDynamic3DTextLabel(string, -1, BizData[bizid][bizExt][0], BizData[bizid][bizExt][1], BizData[bizid][bizExt][2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);
		}
		BizData[bizid][bizCP] = CreateDynamicCP(BizData[bizid][bizExt][0], BizData[bizid][bizExt][1], BizData[bizid][bizExt][2], 1.0, -1, -1, -1, 2.0);
		BizData[bizid][bizPickup] = CreateDynamicPickup(19130, 23, BizData[bizid][bizExt][0], BizData[bizid][bizExt][1], BizData[bizid][bizExt][2], -1, -1);
	}
	return 1;
}

stock Vehicle_GetID(vehicleid)
{
	forex(i, MAX_PLAYER_VEHICLE) if (VehicleData[i][vExists] && VehicleData[i][vVehicle] == vehicleid)
	{
	    return i;
	}
	return -1;
}

stock Vehicle_Count(playerid)
{
	new count = 0;
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists] && VehicleData[i][vOwner] == PlayerData[playerid][pID])
	{
	    count++;
	}
	return count;
}

stock VehicleRental_Count(playerid)
{
	new count = 0;
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists] && VehicleData[i][vRental] != -1 && VehicleData[i][vOwner] == PlayerData[playerid][pID])
	{
	    count++;
	}
	return count;
}

stock Vehicle_Inside(playerid)
{
	new carid;

	if (IsPlayerInAnyVehicle(playerid) && (carid = Vehicle_GetID(GetPlayerVehicleID(playerid))) != -1)
	    return carid;

	return -1;
}
stock SetPlayerPosEx(playerid, Float:x, Float:y, Float:z)
{
	TogglePlayerControllable(playerid, false);
	SetPlayerPos(playerid, x, y, z);
	SetTimerEx("UnFreeze", 2000, false, "d", playerid);
}

FUNC::UnFreeze(playerid)
{
    TogglePlayerControllable(playerid, true);
}
stock ConvertHBEColor(value)
{
    new color;
    if(value >= 90 && value <= 100)
        color = 0x15a014FF;
    else if(value >= 80 && value < 90)
        color = 0x1b9913FF;
    else if(value >= 70 && value < 80)
        color = 0x1a7f08FF;
    else if(value >= 60 && value < 70)
        color = 0x326305FF;
    else if(value >= 50 && value < 60)
        color = 0x375d04FF;
    else if(value >= 40 && value < 50)
        color = 0x603304FF;
    else if(value >= 30 && value < 40)
        color = 0xd72800FF;
    else if(value >= 10 && value < 30)
        color = 0xfb3508FF;
    else if(value >= 0 && value < 10)
        color = 0xFF0000FF;
    else
        color = 0x15a014FF;

    return color;
}

stock ShowText(playerid, const text[], time)
{
	new total = time * 1000;
	new str[256];
	format(str, sizeof(str), "%s", text);
	GameTextForPlayer(playerid, str, total, 5);
	return 1;
}

new g_arrVehicleNames[][] = {
    "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perrenial", "Sentinel", "Dumper", "Firetruck", "Trashmaster",
    "Stretch", "Manana", "Infernus", "Voodoo", "Pony", "Mule", "Cheetah", "Ambulance", "Leviathan", "Moonbeam",
    "Esperanto", "Taxi", "Washington", "Bobcat", "Whoopee", "BF Injection", "Hunter", "Premier", "Enforcer",
    "Securicar", "Banshee", "Predator", "Bus", "Rhino", "Barracks", "Hotknife", "Trailer", "Previon", "Coach",
    "Cabbie", "Stallion", "Rumpo", "RC Bandit", "Romero", "Packer", "Monster", "Admiral", "Squalo", "Seasparrow",
    "Pizzaboy", "Tram", "Trailer", "Turismo", "Speeder", "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair",
    "Berkley's RC Van", "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider", "Glendale", "Oceanic",
    "Sanchez", "Sparrow", "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton",
    "Regina", "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher",
    "FBI Rancher", "Virgo", "Greenwood", "Jetmax", "Hotring", "Sandking", "Blista Compact", "Police Maverick",
    "Boxville", "Benson", "Mesa", "RC Goblin", "Hotring Racer A", "Hotring Racer B", "Bloodring Banger", "Rancher",
    "Super GT", "Elegant", "Journey", "Bike", "Mountain Bike", "Beagle", "Cropduster", "Stunt", "Tanker", "Roadtrain",
    "Nebula", "Majestic", "Buccaneer", "Shamal", "Hydra", "FCR-900", "NRG-500", "HPV1000", "Cement Truck", "Tow Truck",
    "Fortune", "Cadrona", "SWAT Truck", "Willard", "Forklift", "Tractor", "Combine", "Feltzer", "Remington", "Slamvan",
    "Blade", "Streak", "Freight", "Vortex", "Vincent", "Bullet", "Clover", "Sadler", "Firetruck", "Hustler", "Intruder",
    "Primo", "Cargobob", "Tampa", "Sunrise", "Merit", "Utility", "Nevada", "Yosemite", "Windsor", "Monster", "Monster",
    "Uranus", "Jester", "Sultan", "Stratum", "Elegy", "Raindance", "RC Tiger", "Flash", "Tahoma", "Savanna", "Bandito",
    "Freight Flat", "Streak Carriage", "Kart", "Mower", "Dune", "Sweeper", "Broadway", "Tornado", "AT-400", "DFT-30",
    "Huntley", "Stafford", "BF-400", "News Van", "Tug", "Trailer", "Emperor", "Wayfarer", "Euros", "Hotdog", "Club",
    "Freight Box", "Trailer", "Andromada", "Dodo", "RC Cam", "Launch", "LSPD", "SFPD", "LVPD",
    "Police Rancher", "Picador", "S.W.A.T", "Alpha", "Phoenix", "Glendale", "Sadler", "Luggage", "Luggage", "Stairs",
    "Boxville", "Tiller", "Utility Trailer"
};


GetVehicleModelByName(const name[])
{
	if(IsNumeric(name) && (strval(name) >= 400 && strval(name) <= 611))
		return strval(name);

	for (new i = 0; i < sizeof(g_arrVehicleNames); i ++)
	{
		if(strfind(g_arrVehicleNames[i], name, true) != -1)
		{
			return i + 400;
		}
	}
	return 0;
}

ReturnVehicleModelName(model)
{
	new
	    name[32] = "None";

    if (model < 400 || model > 611)
	    return name;

	format(name, sizeof(name), g_arrVehicleNames[model - 400]);
	return name;
}

stock GetVehicleSpeedKMH(vehicleid)
{
	new Float:speed_x, Float:speed_y, Float:speed_z, Float:temp_speed, round_speed;
	GetVehicleVelocity(vehicleid, speed_x, speed_y, speed_z);

	temp_speed = temp_speed = floatsqroot(((speed_x*speed_x) + (speed_y*speed_y)) + (speed_z*speed_z)) * 136.666667;

	round_speed = floatround(temp_speed);
	return round_speed;
}

stock GetFuel(vehicleid)
{
	return VehCore[vehicleid][vehFuel];
}
GetEngineStatus(vehicleid)
{
	static
	engine,
	lights,
	alarm,
	doors,
	bonnet,
	boot,
	objective;

	GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

	if(engine != 1)
		return 0;

	return 1;
}

stock CreatePlayerHUD(playerid)
{
	/* Energy */
	ENERGYTD[playerid][0] = CreatePlayerTextDraw(playerid, 571.000000, 134.000000, "_");
	PlayerTextDrawFont(playerid, ENERGYTD[playerid][0], TEXT_DRAW_FONT_1);
	PlayerTextDrawLetterSize(playerid, ENERGYTD[playerid][0], 0.595833, 4.250002);
	PlayerTextDrawTextSize(playerid, ENERGYTD[playerid][0], 298.500000, 75.000000);
	PlayerTextDrawSetOutline(playerid, ENERGYTD[playerid][0], 1);
	PlayerTextDrawSetShadow(playerid, ENERGYTD[playerid][0], 0);
	PlayerTextDrawAlignment(playerid, ENERGYTD[playerid][0], TEXT_DRAW_ALIGN_CENTRE);
	PlayerTextDrawColour(playerid, ENERGYTD[playerid][0], -1);
	PlayerTextDrawBackgroundColor(playerid, ENERGYTD[playerid][0], 255);
	PlayerTextDrawBoxColor(playerid, ENERGYTD[playerid][0], 135);
	PlayerTextDrawUseBox(playerid, ENERGYTD[playerid][0], 1);
	PlayerTextDrawSetProportional(playerid, ENERGYTD[playerid][0], 1);
	PlayerTextDrawSetSelectable(playerid, ENERGYTD[playerid][0], 0);

	ENERGYTD[playerid][1] = CreatePlayerTextDraw(playerid, 547.000000, 136.000000, "ENERGY");
	PlayerTextDrawFont(playerid, ENERGYTD[playerid][1], TEXT_DRAW_FONT_1);
	PlayerTextDrawLetterSize(playerid, ENERGYTD[playerid][1], 0.412499, 1.549999);
	PlayerTextDrawTextSize(playerid, ENERGYTD[playerid][1], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, ENERGYTD[playerid][1], 0);
	PlayerTextDrawSetShadow(playerid, ENERGYTD[playerid][1], 0);
	PlayerTextDrawAlignment(playerid, ENERGYTD[playerid][1], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, ENERGYTD[playerid][1], -168436481);
	PlayerTextDrawBackgroundColor(playerid, ENERGYTD[playerid][1], 255);
	PlayerTextDrawBoxColor(playerid, ENERGYTD[playerid][1], 50);
	PlayerTextDrawUseBox(playerid, ENERGYTD[playerid][1], 0);
	PlayerTextDrawSetProportional(playerid, ENERGYTD[playerid][1], 1);
	PlayerTextDrawSetSelectable(playerid, ENERGYTD[playerid][1], 0);
	
	/* Speedometer */
	SPEEDOTD[playerid][0] = CreatePlayerTextDraw(playerid, 572.000000, 372.000000, "_");
	PlayerTextDrawFont(playerid, SPEEDOTD[playerid][0], TEXT_DRAW_FONT_1);
	PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][0], 0.600000, 8.300003);
	PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][0], 298.500000, 135.000000);
	PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][0], 1);
	PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][0], 0);
	PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][0], TEXT_DRAW_ALIGN_CENTRE);
	PlayerTextDrawColour(playerid, SPEEDOTD[playerid][0], -1);
	PlayerTextDrawBackgroundColor(playerid, SPEEDOTD[playerid][0], 255);
	PlayerTextDrawBoxColor(playerid, SPEEDOTD[playerid][0], 135);
	PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][0], 1);
	PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][0], 1);
	PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][0], 0);

	SPEEDOTD[playerid][1] = CreatePlayerTextDraw(playerid, 519.000000, 412.000000, "FUEL:");
	PlayerTextDrawFont(playerid, SPEEDOTD[playerid][1], TEXT_DRAW_FONT_2);
	PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][1], 0.287500, 1.350000);
	PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][1], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][1], 0);
	PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][1], 0);
	PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][1], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, SPEEDOTD[playerid][1], -1061109505);
	PlayerTextDrawBackgroundColor(playerid, SPEEDOTD[playerid][1], 255);
	PlayerTextDrawBoxColor(playerid, SPEEDOTD[playerid][1], 50);
	PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][1], 0);
	PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][1], 1);
	PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][1], 0);

	SPEEDOTD[playerid][2] = CreatePlayerTextDraw(playerid, 519.000000, 396.000000, "HEALTH:");
	PlayerTextDrawFont(playerid, SPEEDOTD[playerid][2], TEXT_DRAW_FONT_2);
	PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][2], 0.287500, 1.350000);
	PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][2], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][2], 0);
	PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][2], 0);
	PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][2], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, SPEEDOTD[playerid][2], -1061109505);
	PlayerTextDrawBackgroundColor(playerid, SPEEDOTD[playerid][2], 255);
	PlayerTextDrawBoxColor(playerid, SPEEDOTD[playerid][2], 50);
	PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][2], 0);
	PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][2], 1);
	PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][2], 0);

	HEALTHTD[playerid] = CreatePlayerTextDraw(playerid, 572.000000, 396.000000, "--");
	PlayerTextDrawFont(playerid, HEALTHTD[playerid], TEXT_DRAW_FONT_2);
	PlayerTextDrawLetterSize(playerid, HEALTHTD[playerid], 0.287500, 1.350000);
	PlayerTextDrawTextSize(playerid, HEALTHTD[playerid], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, HEALTHTD[playerid], 0);
	PlayerTextDrawSetShadow(playerid, HEALTHTD[playerid], 0);
	PlayerTextDrawAlignment(playerid, HEALTHTD[playerid], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, HEALTHTD[playerid], -1061109505);
	PlayerTextDrawBackgroundColor(playerid, HEALTHTD[playerid], 255);
	PlayerTextDrawBoxColor(playerid, HEALTHTD[playerid], 50);
	PlayerTextDrawUseBox(playerid, HEALTHTD[playerid], 0);
	PlayerTextDrawSetProportional(playerid, HEALTHTD[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, HEALTHTD[playerid], 0);

	SPEEDOTD[playerid][3] = CreatePlayerTextDraw(playerid, 519.000000, 380.000000, "SPEED:");
	PlayerTextDrawFont(playerid, SPEEDOTD[playerid][3], TEXT_DRAW_FONT_2);
	PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][3], 0.287500, 1.350000);
	PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][3], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][3], 0);
	PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][3], 0);
	PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][3], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, SPEEDOTD[playerid][3], -1061109505);
	PlayerTextDrawBackgroundColour(playerid, SPEEDOTD[playerid][3], 255);
	PlayerTextDrawBoxColour(playerid, SPEEDOTD[playerid][3], 50);
	PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][3], false);
	PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][3], true);
	PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][3], false);

	KMHTD[playerid] = CreatePlayerTextDraw(playerid, 572.000000, 379.000000, "--");
	PlayerTextDrawFont(playerid, KMHTD[playerid], TEXT_DRAW_FONT_2);
	PlayerTextDrawLetterSize(playerid, KMHTD[playerid], 0.287500, 1.350000);
	PlayerTextDrawTextSize(playerid, KMHTD[playerid], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, KMHTD[playerid], 0);
	PlayerTextDrawSetShadow(playerid, KMHTD[playerid], 0);
	PlayerTextDrawAlignment(playerid, KMHTD[playerid], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, KMHTD[playerid], -1061109505);
	PlayerTextDrawBackgroundColour(playerid, KMHTD[playerid], 255);
	PlayerTextDrawBoxColour(playerid, KMHTD[playerid], 50);
	PlayerTextDrawUseBox(playerid, KMHTD[playerid], false);
	PlayerTextDrawSetProportional(playerid, KMHTD[playerid], true);
	PlayerTextDrawSetSelectable(playerid, KMHTD[playerid], false);

	VEHNAMETD[playerid] = CreatePlayerTextDraw(playerid, 519.000000, 362.000000, "--");
	PlayerTextDrawFont(playerid, VEHNAMETD[playerid], TEXT_DRAW_FONT_0);
	PlayerTextDrawLetterSize(playerid, VEHNAMETD[playerid], 0.408333, 1.500000);
	PlayerTextDrawTextSize(playerid, VEHNAMETD[playerid], 400.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, VEHNAMETD[playerid], 1);
	PlayerTextDrawSetShadow(playerid, VEHNAMETD[playerid], 0);
	PlayerTextDrawAlignment(playerid, VEHNAMETD[playerid], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, VEHNAMETD[playerid], -1061109505);
	PlayerTextDrawBackgroundColour(playerid, VEHNAMETD[playerid], 255);
	PlayerTextDrawBoxColour(playerid, VEHNAMETD[playerid], 50);
	PlayerTextDrawUseBox(playerid, VEHNAMETD[playerid], false);
	PlayerTextDrawSetProportional(playerid, VEHNAMETD[playerid], false);
	PlayerTextDrawSetSelectable(playerid, VEHNAMETD[playerid], false);
}

stock CreateGlobalTextDraw()
{

}
FormatNumber(number, const prefix[] = "$")
{
	static
		value[32],
		length;

	format(value, sizeof(value), "%d", (number < 0) ? (-number) : (number));

	if ((length = strlen(value)) > 3)
	{
		for (new i = length, l = 0; --i >= 0; l ++) {
		    if ((l > 0) && (l % 3 == 0)) strins(value, ",", i + 1);
		}
	}
	if (prefix[0] != 0)
	    strins(value, prefix, 0);

	if (number < 0)
		strins(value, "-", 0);

	return value;
}

stock KickEx(playerid)
{
	SaveData(playerid);
	SetTimerEx("KickTimer", 1000, false, "d", playerid);
}

FUNC::KickTimer(playerid)
{
	Kick(playerid);
}

FUNC::OnPlayerVehicleCreated(carid)
{
	if (carid == -1 || !VehicleData[carid][vExists])
	    return 0;

	VehicleData[carid][vID] = cache_insert_id();
	VehicleData[carid][vExists] = true;
	SaveVehicle(carid);
	return 1;
}

FUNC::Vehicle_GetStatus(carid)
{
	if(VehicleData[carid][vVehicle] != INVALID_VEHICLE_ID)
	{
		GetVehicleDamageStatus(VehicleData[carid][vVehicle], VehicleData[carid][vPanelDamage], VehicleData[carid][vDoorDamage], VehicleData[carid][vLightDamage], VehicleData[carid][vTireDamage]);

		GetVehicleHealth(VehicleData[carid][vVehicle], VehicleData[carid][vHealth]);
		VehicleData[carid][vFuel] = VehCore[VehicleData[carid][vVehicle]][vehFuel];
		VehicleData[carid][vWorld] = GetVehicleVirtualWorld(VehicleData[carid][vVehicle]);

		GetVehiclePos(VehicleData[carid][vVehicle], VehicleData[carid][vPos][0], VehicleData[carid][vPos][1], VehicleData[carid][vPos][2]);
		GetVehicleZAngle(VehicleData[carid][vVehicle],VehicleData[carid][vPos][3]);

	}
	return 1;
}

stock Vehicle_IsOwner(playerid, carid)
{
	if(PlayerData[playerid][pID] == -1)
		return 0;

	if(VehicleData[carid][vExists] && VehicleData[carid][vOwner] == PlayerData[playerid][pID])
		return 1;

	return 0;
}

stock Vehicle_HaveAccess(playerid, carid)
{
	if(PlayerData[playerid][pID] == -1)
		return 0;

	if(VehicleData[carid][vExists] && VehicleData[carid][vOwner] == PlayerData[playerid][pID] || PlayerData[playerid][pVehKey] == VehicleData[carid][vID])
		return 1;

	return 0;
}
FUNC::UnloadPlayerVehicle(playerid)
{
 	forex(i,MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists])
	{
		if(VehicleData[i][vOwner] == PlayerData[playerid][pID])
		{
		    Vehicle_GetStatus(i);
		    
			new cQuery[2512];
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "UPDATE `vehicle` SET ");
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehX`='%f', ", cQuery, VehicleData[i][vPos][0]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehY`='%f', ", cQuery, VehicleData[i][vPos][1]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehZ`='%f', ", cQuery, VehicleData[i][vPos][2]+0.1);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehA`='%f', ", cQuery, VehicleData[i][vPos][3]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehOwner`='%d', ", cQuery, VehicleData[i][vOwner]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehModel`='%d', ", cQuery, VehicleData[i][vModel]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehColor1`='%d', ", cQuery, VehicleData[i][vColor][0]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehColor2`='%d', ", cQuery, VehicleData[i][vColor][1]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehHealth`='%f', ", cQuery, VehicleData[i][vHealth]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehPanelDamage`='%d', ", cQuery, VehicleData[i][vPanelDamage]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehDoorDamage`='%d', ", cQuery, VehicleData[i][vDoorDamage]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehLightDamage`='%d', ", cQuery, VehicleData[i][vLightDamage]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehTireDamage`='%d', ", cQuery, VehicleData[i][vTireDamage]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInterior`='%d', ", cQuery, VehicleData[i][vInterior]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehWorld`='%d', ", cQuery, VehicleData[i][vWorld]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehFuel`='%d', ", cQuery, VehicleData[i][vFuel]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehPlate`='%s', ", cQuery, VehicleData[i][vPlate]);
		    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehRental`='%d', ", cQuery, VehicleData[i][vRental]);
		    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehRentalTime`='%d', ", cQuery, VehicleData[i][vRentTime]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehLocked`='%d', ", cQuery, VehicleData[i][vLocked]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInsurance`='%d', ", cQuery, VehicleData[i][vInsurance]);
            mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInsuTime`='%d' ", cQuery, VehicleData[i][vInsuTime]);
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "%sWHERE `vehID` = %d", cQuery, VehicleData[i][vID]);
			mysql_query(sqlcon, cQuery, true);


			if(VehicleData[i][vVehicle] != INVALID_VEHICLE_ID)
			{
				DestroyVehicle(VehicleData[i][vVehicle]);
			}
			VehicleData[i][vExists] = false;
			
		}
	}
	return 1;
}

stock VehicleRental_Create(ownerid, modelid, Float:x, Float:y, Float:z, Float:angle, time, rentid)
{
    forex(i, MAX_PLAYER_VEHICLE)
	{
		if (!VehicleData[i][vExists])
   		{
   		    VehicleData[i][vExists] = true;

            VehicleData[i][vModel] = modelid;
            VehicleData[i][vOwner] = ownerid;

			format(VehicleData[i][vPlate], 16, "RENTAL");

            VehicleData[i][vPos][0] = x;
            VehicleData[i][vPos][1] = y;
            VehicleData[i][vPos][2] = z;
            VehicleData[i][vPos][3] = angle;

			VehicleData[i][vInsurance] = 0;
			VehicleData[i][vInsuTime] = 0;

            VehicleData[i][vColor][0] = random(126);

            VehicleData[i][vColor][1] = random(126);

            VehicleData[i][vLocked] = false;

			VehicleData[i][vFuel] = 100;
			VehicleData[i][vHealth] = 1000.0;

			VehicleData[i][vRental] = rentid;
			VehicleData[i][vRentTime] = time;
			
			VehicleData[i][vVehicle] = CreateVehicle(VehicleData[i][vModel], VehicleData[i][vPos][0], VehicleData[i][vPos][1], VehicleData[i][vPos][2], VehicleData[i][vPos][3], VehicleData[i][vColor][0], VehicleData[i][vColor][1], 60000);
		    VehCore[VehicleData[i][vVehicle]][vehFuel] = VehicleData[i][vFuel];
		    SetVehicleNumberPlate(VehicleData[i][vVehicle], VehicleData[i][vPlate]);

            mysql_tquery(sqlcon, "INSERT INTO `vehicle` (`vehModel`) VALUES(0)", "OnPlayerVehicleCreated", "d", i);
            return i;
		}
	}
	return -1;
}

stock Vehicle_Delete(carid)
{
    if (carid != -1 && VehicleData[carid][vExists])
	{
	    new
	        string[64];

		format(string, sizeof(string), "DELETE FROM `vehicle` WHERE `vehID` = '%d'", VehicleData[carid][vID]);
		mysql_tquery(sqlcon, string);

		if (IsValidVehicle(VehicleData[carid][vVehicle]))
			DestroyVehicle(VehicleData[carid][vVehicle]);

        VehicleData[carid][vExists] = false;
	    VehicleData[carid][vID] = 0;
	    VehicleData[carid][vOwner] = -1;
	    VehicleData[carid][vVehicle] = INVALID_VEHICLE_ID;
	    VehicleData[carid][vRental] = -1;
	}
	return 1;
}

stock Vehicle_Create(ownerid, modelid, Float:x, Float:y, Float:z, Float:angle, color1, color2)
{
    forex(i, MAX_PLAYER_VEHICLE)
	{
		if (!VehicleData[i][vExists])
   		{
   		    VehicleData[i][vExists] = true;
   		    
            VehicleData[i][vModel] = modelid;
            VehicleData[i][vOwner] = ownerid;

			
			format(VehicleData[i][vPlate], 16, "NONE");
			
            VehicleData[i][vPos][0] = x;
            VehicleData[i][vPos][1] = y;
            VehicleData[i][vPos][2] = z;
            VehicleData[i][vPos][3] = angle;

			VehicleData[i][vInsurance] = 3;
			VehicleData[i][vInsuTime] = 0;
			
            VehicleData[i][vColor][0] = color1;

            VehicleData[i][vColor][1] = color2;
            
            VehicleData[i][vLocked] = false;

			VehicleData[i][vFuel] = 100;
			VehicleData[i][vHealth] = 1000.0;
			VehicleData[i][vRentTime] = 0;
			VehicleData[i][vRental] = -1;
			VehicleData[i][vVehicle] = CreateVehicle(VehicleData[i][vModel], VehicleData[i][vPos][0], VehicleData[i][vPos][1], VehicleData[i][vPos][2], VehicleData[i][vPos][3], VehicleData[i][vColor][0], VehicleData[i][vColor][1], 60000);
		    VehCore[VehicleData[i][vVehicle]][vehFuel] = VehicleData[i][vFuel];
		    SetVehicleNumberPlate(VehicleData[i][vVehicle], VehicleData[i][vPlate]);

            mysql_tquery(sqlcon, "INSERT INTO `vehicle` (`vehModel`) VALUES(0)", "OnPlayerVehicleCreated", "d", i);
            return i;
		}
	}
	return -1;
}


stock GetFreeVehicleID()
{
	forex(x,MAX_PLAYER_VEHICLE)
	{
		if(!VehicleData[x][vExists]) return x;
	}
	return -1;
}

FUNC::LoadPlayerVehicle(playerid)
{
	new query[128];
	mysql_format(sqlcon, query, sizeof(query), "SELECT * FROM `vehicle` WHERE `vehOwner` = %d", PlayerData[playerid][pID]);
	mysql_query(sqlcon, query, true);
	new count = cache_num_rows();
	if(count > 0)
	{
		forex(z,count)
		{
		    new i = GetFreeVehicleID();
		    
			VehicleData[i][vExists] = true;
			cache_get_value_name_int(z, "vehID", VehicleData[i][vID]);
			cache_get_value_name_int(z, "vehOwner", VehicleData[i][vOwner]);
			cache_get_value_name_int(z, "vehLocked", VehicleData[i][vLocked]);
			cache_get_value_name_float(z, "vehX", VehicleData[i][vPos][0]);
			cache_get_value_name_float(z, "vehY", VehicleData[i][vPos][1]);
			cache_get_value_name_float(z, "vehZ", VehicleData[i][vPos][2]);
			cache_get_value_name_float(z, "vehA", VehicleData[i][vPos][3]);
            cache_get_value_name_float(z, "vehHealth", VehicleData[i][vHealth]);
            cache_get_value_name_int(z, "vehModel", VehicleData[i][vModel]);
			cache_get_value_name_int(z, "vehPanelDamage", VehicleData[i][vPanelDamage]);
			cache_get_value_name_int(z, "vehDoorDamage", VehicleData[i][vDoorDamage]);
			cache_get_value_name_int(z, "vehLightDamage", VehicleData[i][vLightDamage]);
			cache_get_value_name_int(z, "vehTireDamage", VehicleData[i][vTireDamage]);
            cache_get_value_name_int(z, "vehInterior", VehicleData[i][vInterior]);
            cache_get_value_name_int(z, "vehWorld", VehicleData[i][vWorld]);
            cache_get_value_name_int(z, "vehColor1", VehicleData[i][vColor][0]);
            cache_get_value_name_int(z, "vehColor2", VehicleData[i][vColor][1]);
            cache_get_value_name_int(z, "vehFuel", VehicleData[i][vFuel]);
            cache_get_value_name_int(z, "vehInsurance", VehicleData[i][vInsurance]);
            cache_get_value_name_int(z, "vehInsuTime", VehicleData[i][vInsuTime]);
            cache_get_value_name(z, "vehPlate", VehicleData[i][vPlate]);
            cache_get_value_name_int(z, "vehRental", VehicleData[i][vRental]);
            cache_get_value_name_int(z, "vehRentalTime", VehicleData[i][vRentTime]);
            
			if(VehicleData[i][vInsuTime] == 0)
			{
			   // printf("PosX: %.1f | PosY: %.1f |  PosZ: %.1f | Model: %d", VehicleData[i][vPos][0], VehicleData[i][vPos][1], VehicleData[i][vPos][2], VehicleData[i][vModel]);
			    printf("[VEHICLE] Loaded %d player vehicle from: %s(%d)", count, GetName(playerid), playerid);

				VehicleData[i][vVehicle] = CreateVehicle(VehicleData[i][vModel], VehicleData[i][vPos][0], VehicleData[i][vPos][1], VehicleData[i][vPos][2], VehicleData[i][vPos][3], VehicleData[i][vColor][0], VehicleData[i][vColor][1], 60000);
				SetVehicleNumberPlate(VehicleData[i][vVehicle], VehicleData[i][vPlate]);
				SetVehicleVirtualWorld(VehicleData[i][vVehicle], VehicleData[i][vWorld]);
				LinkVehicleToInterior(VehicleData[i][vVehicle], VehicleData[i][vInterior]);
				VehCore[VehicleData[i][vVehicle]][vehFuel] = VehicleData[i][vFuel];

				if(VehicleData[i][vHealth] < 350.0)
				{
					SetVehicleHealth(VehicleData[i][vVehicle], 350.0);
				}
				else
				{
					SetVehicleHealth(VehicleData[i][vVehicle], VehicleData[i][vHealth]);
				}
				UpdateVehicleDamageStatus(VehicleData[i][vVehicle], VehicleData[i][vPanelDamage], VehicleData[i][vDoorDamage], VehicleData[i][vLightDamage], VehicleData[i][vTireDamage]);
				if(VehicleData[i][vVehicle] != INVALID_VEHICLE_ID)
				{
					if(VehicleData[i][vLocked] == 1)
					{
						SwitchVehicleDoors(VehicleData[i][vVehicle], true);
					}
					else
					{
						SwitchVehicleDoors(VehicleData[i][vVehicle], false);
					}
				}
			}
		}
	}
	return 1;
}

FUNC::OnPlayerVehicleRespawn(i)
{
	VehicleData[i][vVehicle] = CreateVehicle(VehicleData[i][vModel], VehicleData[i][vPos][0], VehicleData[i][vPos][1], VehicleData[i][vPos][2], VehicleData[i][vPos][3], VehicleData[i][vColor][0], VehicleData[i][vColor][1], 60000);
	SetVehicleNumberPlate(VehicleData[i][vVehicle], VehicleData[i][vPlate]);
	SetVehicleVirtualWorld(VehicleData[i][vVehicle], VehicleData[i][vWorld]);
	LinkVehicleToInterior(VehicleData[i][vVehicle], VehicleData[i][vInterior]);
	VehCore[VehicleData[i][vVehicle]][vehFuel] = VehicleData[i][vFuel];

	if(VehicleData[i][vHealth] < 350.0)
	{
		SetVehicleHealth(VehicleData[i][vVehicle], 350.0);
	}
	else
	{
		SetVehicleHealth(VehicleData[i][vVehicle], VehicleData[i][vHealth]);
	}
	UpdateVehicleDamageStatus(VehicleData[i][vVehicle], VehicleData[i][vPanelDamage], VehicleData[i][vDoorDamage], VehicleData[i][vLightDamage], VehicleData[i][vTireDamage]);
	if(VehicleData[i][vVehicle] != INVALID_VEHICLE_ID)
	{
		if(VehicleData[i][vLocked] == 1)
		{
			SwitchVehicleDoors(VehicleData[i][vVehicle], true);
		}
		else
		{
			SwitchVehicleDoors(VehicleData[i][vVehicle], false);
		}
	}
    return 1;
}

stock Rental_Save(id)
{
	print("Rental_Save");
	new query[1052];
	mysql_format(sqlcon, query, sizeof(query), "UPDATE `rental` SET ");
	mysql_format(sqlcon, query, sizeof(query), "%s`PosX`='%f', ", query, RentData[id][rentPos][0]);
	mysql_format(sqlcon, query, sizeof(query), "%s`PosY`='%f', ", query, RentData[id][rentPos][1]);
	mysql_format(sqlcon, query, sizeof(query), "%s`PosZ`='%f', ", query, RentData[id][rentPos][2]);
	mysql_format(sqlcon, query, sizeof(query), "%s`SpawnX`='%f', ", query, RentData[id][rentSpawn][0]);
	mysql_format(sqlcon, query, sizeof(query), "%s`SpawnY`='%f', ", query, RentData[id][rentSpawn][1]);
	mysql_format(sqlcon, query, sizeof(query), "%s`SpawnZ`='%f', ", query, RentData[id][rentSpawn][2]);
	mysql_format(sqlcon, query, sizeof(query), "%s`SpawnA`='%f', ", query, RentData[id][rentSpawn][3]);
	mysql_format(sqlcon, query, sizeof(query), "%s`Vehicle1`='%d', ", query, RentData[id][rentModel][0]);
	mysql_format(sqlcon, query, sizeof(query), "%s`Vehicle2`='%d', ", query, RentData[id][rentModel][1]);
	mysql_format(sqlcon, query, sizeof(query), "%s`Price1`='%d', ", query, RentData[id][rentModel][0]);
	mysql_format(sqlcon, query, sizeof(query), "%s`Price2`='%d' ", query, RentData[id][rentModel][1]);
	mysql_format(sqlcon, query, sizeof(query), "%sWHERE `ID` = '%d'", query, RentData[id][rentID]);
	mysql_query(sqlcon, query, true);
	return 1;
}

stock SaveVehicle(i)
{
	Vehicle_GetStatus(i);

	new cQuery[2512];
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "UPDATE `vehicle` SET ");
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehX`='%f', ", cQuery, VehicleData[i][vPos][0]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehY`='%f', ", cQuery, VehicleData[i][vPos][1]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehZ`='%f', ", cQuery, VehicleData[i][vPos][2]+0.1);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehA`='%f', ", cQuery, VehicleData[i][vPos][3]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehOwner`='%d', ", cQuery, VehicleData[i][vOwner]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehColor1`='%d', ", cQuery, VehicleData[i][vColor][0]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehColor2`='%d', ", cQuery, VehicleData[i][vColor][1]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehModel`='%d', ", cQuery, VehicleData[i][vModel]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehHealth`='%f', ", cQuery, VehicleData[i][vHealth]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehPanelDamage`='%d', ", cQuery, VehicleData[i][vPanelDamage]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehDoorDamage`='%d', ", cQuery, VehicleData[i][vDoorDamage]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehLightDamage`='%d', ", cQuery, VehicleData[i][vLightDamage]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehTireDamage`='%d', ", cQuery, VehicleData[i][vTireDamage]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInterior`='%d', ", cQuery, VehicleData[i][vInterior]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehWorld`='%d', ", cQuery, VehicleData[i][vWorld]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehFuel`='%d', ", cQuery, VehicleData[i][vFuel]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehLocked`='%d', ", cQuery, VehicleData[i][vLocked]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehPlate`='%s', ", cQuery, VehicleData[i][vPlate]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehRental`='%d', ", cQuery, VehicleData[i][vRental]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehRentalTime`='%d', ", cQuery, VehicleData[i][vRentTime]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInsurance`='%d', ", cQuery, VehicleData[i][vInsurance]);
    mysql_format(sqlcon, cQuery, sizeof(cQuery), "%s`vehInsuTime`='%d' ", cQuery, VehicleData[i][vInsuTime]);
	mysql_format(sqlcon, cQuery, sizeof(cQuery), "%sWHERE `vehID` = %d", cQuery, VehicleData[i][vID]);
	mysql_query(sqlcon, cQuery, true);
	
	return 1;
}


ReturnName(playerid)
{
    static
        name[MAX_PLAYER_NAME + 1];

    GetPlayerName(playerid, name, sizeof(name));
    if(PlayerData[playerid][pMaskOn])
    {
        format(name, sizeof(name), "Mask_#%d", PlayerData[playerid][pMaskID]);
	}
	else
	{
	    for (new i = 0, len = strlen(name); i < len; i ++)
		{
	        if (name[i] == '_') name[i] = ' ';
		}
	}
    return name;
}

stock GetName(playerid)
{
	new name[MAX_PLAYER_NAME];
 	GetPlayerName(playerid,name,sizeof(name));
	return name;
}

Database_Connect()
{
	sqlcon = mysql_connect(DATABASE_ADDRESS,DATABASE_USERNAME,DATABASE_PASSWORD,DATABASE_NAME);

	if(mysql_errno(sqlcon) != 0)
	{
	    print("[MySQL] - Connection Failed!");
	    SetGameModeText("Xyronite | Connection Failed!");
	}
	else
	{
		print("[MySQL] - Connection Estabilished!");
		SetGameModeText("Xyronite | UCP System");
	}
}

stock IsRoleplayName(const player[])
{
    forex(n,strlen(player))
    {
        if (player[n] == '_' && player[n+1] >= 'A' && player[n+1] <= 'Z') return 1;
        if (player[n] == ']' || player[n] == '[') return 0;
	}
    return 0;
}

stock IsPlayerNearPlayer(playerid, targetid, Float:radius)
{
	static
		Float:fX,
		Float:fY,
		Float:fZ;

	GetPlayerPos(targetid, fX, fY, fZ);

	return (GetPlayerInterior(playerid) == GetPlayerInterior(targetid) && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(targetid)) && IsPlayerInRangeOfPoint(playerid, radius, fX, fY, fZ);
}

stock SendNearbyMessage(playerid, Float:radius, color, const str[], {Float,_}:...)
{
	static
	    args,
	    start,
	    end,
	    string[144]
	;
	#emit LOAD.S.pri 8
	#emit STOR.pri args

	if (args > 16)
	{
		#emit ADDR.pri str
		#emit STOR.pri start

	    for (end = start + (args - 16); end > start; end -= 4)
		{
	        #emit LREF.pri end
	        #emit PUSH.pri
		}
		#emit PUSH.S str
		#emit PUSH.C 144
		#emit PUSH.C string

		#emit LOAD.S.pri 8
		#emit CONST.alt 4
		#emit SUB
		#emit PUSH.pri

		#emit SYSREQ.C format
		#emit LCTRL 5
		#emit SCTRL 4

        foreach (new i : Player)
		{
			if (IsPlayerNearPlayer(i, playerid, radius) && PlayerData[i][pSpawned])
			{
  				SendClientMessage(i, color, string);
			}
		}
		return 1;
	}
	foreach (new i : Player)
	{
		if (IsPlayerNearPlayer(i, playerid, radius) && PlayerData[i][pSpawned])
		{
			SendClientMessage(i, color, str);
		}
	}
	return 1;
}

stock SendClientMessageEx(playerid, colour, const text[], va_args<>)
{
    new str[145];
    va_format(str, sizeof(str), text, va_start<3>);
    return SendClientMessage(playerid, colour, str);
}

stock CheckAccount(playerid)
{
	new query[256];
	format(query, sizeof(query), "SELECT * FROM `PlayerUCP` WHERE `UCP` = '%s' LIMIT 1;", GetName(playerid));
	mysql_tquery(sqlcon, query, "CheckPlayerUCP", "d", playerid);
	return 1;
}

FUNC::PlayerCheck(playerid, rcc)
{
	if(rcc != g_RaceCheck{playerid})
	    return Kick(playerid);
	    
	CheckAccount(playerid);
	return true;
}

FUNC::CheckPlayerUCP(playerid)
{
	new rows = cache_num_rows();
	new str[256];
	if (rows)
	{
	    cache_get_value_name(0, "UCP", tempUCP[playerid]);
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), PlayerData[playerid][pAttempt]);
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to Xyronite", str, "Login", "Exit");
	}
	else
	{
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Create Password: {FF00FF}(Input Below)", GetName(playerid), PlayerData[playerid][pAttempt]);
		ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to Xyronite", str, "Register", "Exit");
	}
	return 1;
}

stock SetupPlayerData(playerid)
{
    SetSpawnInfo(playerid, NO_TEAM, PlayerData[playerid][pSkin], 1642.1681, -2333.3689, 13.5469, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    SpawnPlayer(playerid);
    GiveMoney(playerid, 150);
    return 1;
}

stock SaveData(playerid)
{
	new query[2512];
	if(PlayerData[playerid][pSpawned])
	{
		GetPlayerHealth(playerid, PlayerData[playerid][pHealth]);
		GetPlayerPos(playerid, PlayerData[playerid][pPos][0], PlayerData[playerid][pPos][1], PlayerData[playerid][pPos][2]);

		mysql_format(sqlcon, query, sizeof(query), "UPDATE `characters` SET ");
		mysql_format(sqlcon, query, sizeof(query), "%s`PosX`='%f', ", query, PlayerData[playerid][pPos][0]);
        mysql_format(sqlcon, query, sizeof(query), "%s`PosY`='%f', ", query, PlayerData[playerid][pPos][1]);
        mysql_format(sqlcon, query, sizeof(query), "%s`PosZ`='%f', ", query, PlayerData[playerid][pPos][2]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Health`='%f', ", query, PlayerData[playerid][pHealth]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`World`='%d', ", query, GetPlayerVirtualWorld(playerid));
	    mysql_format(sqlcon, query, sizeof(query), "%s`Interior`='%d', ", query, GetPlayerInterior(playerid));
	    mysql_format(sqlcon, query, sizeof(query), "%s`Age`='%d', ", query, PlayerData[playerid][pAge]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Origin`='%s', ", query, PlayerData[playerid][pOrigin]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Gender`='%d', ", query, PlayerData[playerid][pGender]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Skin`='%d', ", query, PlayerData[playerid][pSkin]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Energy`='%d', ", query, PlayerData[playerid][pEnergy]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`AdminLevel`='%d', ", query, PlayerData[playerid][pAdmin]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`InBiz`='%d', ", query, PlayerData[playerid][pInBiz]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`Money`='%d', ", query, PlayerData[playerid][pMoney]);
	    mysql_format(sqlcon, query, sizeof(query), "%s`UCP`='%s' ", query, PlayerData[playerid][pUCP]);
	    mysql_format(sqlcon, query, sizeof(query), "%sWHERE `pID` = %d", query, PlayerData[playerid][pID]);
		mysql_query(sqlcon, query, true);
	}
	return 1;
}

FUNC::LoadCharacterData(playerid)
{
	cache_get_value_name_int(0, "pID", PlayerData[playerid][pID]);
	cache_get_value_name(0, "Name", PlayerData[playerid][pName]);
	cache_get_value_name_float(0, "PosX", PlayerData[playerid][pPos][0]);
	cache_get_value_name_float(0, "PosY", PlayerData[playerid][pPos][1]);
	cache_get_value_name_float(0, "PosZ", PlayerData[playerid][pPos][2]);
	cache_get_value_name_float(0, "Health", PlayerData[playerid][pHealth]);
	cache_get_value_name_int(0, "Interior", PlayerData[playerid][pInterior]);
	cache_get_value_name_int(0, "World", PlayerData[playerid][pWorld]);
	cache_get_value_name_int(0, "Age", PlayerData[playerid][pAge]);
	cache_get_value_name(0, "Origin", PlayerData[playerid][pOrigin]);
	cache_get_value_name_int(0, "Gender", PlayerData[playerid][pGender]);
	cache_get_value_name_int(0, "Skin", PlayerData[playerid][pSkin]);
	cache_get_value_name(0, "UCP", PlayerData[playerid][pUCP]);
	cache_get_value_name_int(0, "Energy", PlayerData[playerid][pEnergy]);
	cache_get_value_name_int(0, "AdminLevel", PlayerData[playerid][pAdmin]);
	cache_get_value_name_int(0, "InBiz", PlayerData[playerid][pInBiz]);
	cache_get_value_name_int(0, "Money", PlayerData[playerid][pMoney]);
	
	new invQuery[256];
    format(invQuery, sizeof(invQuery), "SELECT * FROM `inventory` WHERE `ID` = '%d'", PlayerData[playerid][pID]);
	mysql_tquery(sqlcon, invQuery, "LoadPlayerItems", "d", playerid);
	
    SetSpawnInfo(playerid, NO_TEAM, PlayerData[playerid][pSkin], PlayerData[playerid][pPos][0], PlayerData[playerid][pPos][1], PlayerData[playerid][pPos][2], 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    SpawnPlayer(playerid);
    SendServerMessage(playerid, "Successfully loaded your characters database!");
    LoadPlayerVehicle(playerid);
    return 1;
}

FUNC::HashPlayerPassword(playerid, hashid)
{
	new
		query[256],
		hash[BCRYPT_HASH_LENGTH];

    bcrypt_get_hash(hash, sizeof(hash));

	GetPlayerName(playerid, tempUCP[playerid], MAX_PLAYER_NAME + 1);

	format(query,sizeof(query),"INSERT INTO `PlayerUCP` (`UCP`, `Password`) VALUES ('%s', '%s')", tempUCP[playerid], hash);
	mysql_tquery(sqlcon, query);

    SendServerMessage(playerid, "Your UCP is successfully registered!");
    CheckAccount(playerid);
	return 1;
}

ShowCharacterList(playerid)
{
	new name[256], count, sgstr[128];

	for (new i; i < MAX_CHARS; i ++) if(PlayerChar[playerid][i][0] != EOS)
	{
	    format(sgstr, sizeof(sgstr), "%s\n", PlayerChar[playerid][i]);
		strcat(name, sgstr);
		count++;
	}
	if(count < MAX_CHARS)
		strcat(name, "< Create Character >");

	ShowPlayerDialog(playerid, DIALOG_CHARLIST, DIALOG_STYLE_LIST, "Character List", name, "Select", "Quit");
	return 1;
}

FUNC::LoadCharacter(playerid)
{
	for (new i = 0; i < MAX_CHARS; i ++)
	{
		PlayerChar[playerid][i][0] = EOS;
	}
	for (new i = 0; i < cache_num_rows(); i ++)
	{
		cache_get_value_name(i, "Name", PlayerChar[playerid][i]);
	}
  	ShowCharacterList(playerid);
  	return 1;
}

FUNC::OnPlayerPasswordChecked(playerid, bool:success)
{
	new str[256];
    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), PlayerData[playerid][pAttempt]);
    
	if(!success)
	{
	    if(PlayerData[playerid][pAttempt] < 5)
	    {
		    PlayerData[playerid][pAttempt]++;
	        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to Xyronite", str, "Login", "Exit");
			return 1;
		}
		else
		{
		    SendServerMessage(playerid, "Kamu telah salah memasukan password sebanyak {FFFF00}5 kali!");
		    KickEx(playerid);
			return 1;
		}
	}
	new query[256];
	format(query, sizeof(query), "SELECT `Name` FROM `characters` WHERE `UCP` = '%s' LIMIT %d;", GetName(playerid), MAX_CHARS);
	mysql_tquery(sqlcon, query, "LoadCharacter", "d", playerid);
	return 1;
}

FUNC::InsertPlayerName(playerid, const name[])
{
	new count = cache_num_rows(), query[145], Cache:execute;
	if(count > 0)
	{
        ShowPlayerDialog(playerid, DIALOG_MAKECHAR, DIALOG_STYLE_INPUT, "Create Character", "ERROR: This name is already used by the other player!\nInsert your new Character Name\n\nExample: Finn_Xanderz, Javier_Cooper etc.", "Create", "Back");
	}
	else
	{
		mysql_format(sqlcon,query,sizeof(query),"INSERT INTO `characters` (`Name`,`UCP`) VALUES('%e','%e')",name,GetName(playerid));
		execute = mysql_query(sqlcon, query);
		PlayerData[playerid][pID] = cache_insert_id();
	 	cache_delete(execute);
	 	SetPlayerName(playerid, name);
		format(PlayerData[playerid][pName], MAX_PLAYER_NAME, name);
	 	ShowPlayerDialog(playerid, DIALOG_AGE, DIALOG_STYLE_INPUT, "Character Age", "Please Insert your Character Age", "Continue", "Cancel");
	}
	return 1;
}

stock IsEngineVehicle(vehicleid)
{
	static const g_aEngineStatus[] = {
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
	    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0
	};
    new modelid = GetVehicleModel(vehicleid);

    if (modelid < 400 || modelid > 611)
        return 0;

    return (g_aEngineStatus[modelid - 400]);
}

stock IsSpeedoVehicle(vehicleid)
{
	if (GetVehicleModel(vehicleid) == 509 || GetVehicleModel(vehicleid) == 510 || GetVehicleModel(vehicleid) == 481 || !IsEngineVehicle(vehicleid)) {
	    return 0;
	}
	return 1;
}

FUNC::EngineStatus(playerid, vehicleid)
{
	if(!GetEngineStatus(vehicleid))
	{
		new Float: f_vHealth;
		GetVehicleHealth(vehicleid, f_vHealth);
		if(f_vHealth < 350.0)
			return SendErrorMessage(playerid, "This vehicle is damaged!");

		if(VehCore[vehicleid][vehFuel] <= 0)
			return SendErrorMessage(playerid, "There is no fuel on this vehicle!");

		SwitchVehicleEngine(vehicleid, true);
		ShowText(playerid, "Engine turned ~g~ON", 3);
	}
	else
	{
		SwitchVehicleEngine(vehicleid, false);
		ShowText(playerid, "Engine turned ~r~OFF", 3);
		SwitchVehicleLight(vehicleid, false);
	}
	return 1;
}

stock ResetVariable(playerid)
{
	for (new i = 0; i != MAX_INVENTORY; i ++)
	{
	    InventoryData[playerid][i][invExists] = false;
	    InventoryData[playerid][i][invModel] = 0;
	    InventoryData[playerid][i][invQuantity] = 0;
	}
	PlayerData[playerid][pEnergy] = 100;
	PlayerData[playerid][pMoney] = 0;
	PlayerData[playerid][pInBiz] = -1;
	PlayerData[playerid][pListitem] = -1;
	PlayerData[playerid][pAttempt] = 0;
	PlayerData[playerid][pCalling] = INVALID_PLAYER_ID;
	PlayerData[playerid][pSpawned] = false;
	return 1;
}

ProxDetector(Float: f_Radius, playerid, const string[],col1,col2,col3,col4,col5)
{
		new
			Float: f_playerPos[3];

		GetPlayerPos(playerid, f_playerPos[0], f_playerPos[1], f_playerPos[2]);
		foreach(new i : Player)
		{
			if(GetPlayerVirtualWorld(i) == GetPlayerVirtualWorld(playerid) && GetPlayerInterior(i) == GetPlayerInterior(playerid))
			{
				if(IsPlayerInRangeOfPoint(i, f_Radius / 16, f_playerPos[0], f_playerPos[1], f_playerPos[2])) {
					SendClientMessage(i, col1, string);
				}
				else if(IsPlayerInRangeOfPoint(i, f_Radius / 8, f_playerPos[0], f_playerPos[1], f_playerPos[2])) {
					SendClientMessage(i, col2, string);
				}
				else if(IsPlayerInRangeOfPoint(i, f_Radius / 4, f_playerPos[0], f_playerPos[1], f_playerPos[2])) {
					SendClientMessage(i, col3, string);
				}
				else if(IsPlayerInRangeOfPoint(i, f_Radius / 2, f_playerPos[0], f_playerPos[1], f_playerPos[2])) {
					SendClientMessage(i, col4, string);
				}
				else if(IsPlayerInRangeOfPoint(i, f_Radius, f_playerPos[0], f_playerPos[1], f_playerPos[2])) {
					SendClientMessage(i, col5, string);
				}
			}
			else SendClientMessage(i, col1, string);
		}
		return 1;
}

/* Gamemode Start! */

main()
{
	print("[ Xyronite Gamemode Loaded ]");
}

public OnGameModeInit()
{
	Database_Connect();
	CreateGlobalTextDraw();
	DisableInteriorEnterExits();
	EnableStuntBonusForAll(false);
	ManualVehicleEngineAndLights();
	StreamerConfig();
	/* Load from Database */
	mysql_tquery(sqlcon, "SELECT * FROM `business`", "Business_Load");
	mysql_tquery(sqlcon, "SELECT * FROM `dropped`", "Dropped_Load", "");
	mysql_tquery(sqlcon, "SELECT * FROM `rental`", "Rental_Load", "");
	return 1;
}

public OnGameModeExit()
{
	return 1;
}

public OnPlayerConnect(playerid)
{
	g_RaceCheck{playerid} ++;
	ResetVariable(playerid);
	CreatePlayerHUD(playerid);
	SetPlayerPos(playerid, 155.3337, -1776.4384, 14.8978+5.0);
	SetPlayerCameraPos(playerid, 155.3337, -1776.4384, 14.8978);
	SetPlayerCameraLookAt(playerid, 156.2734, -1776.0850, 14.2128);
	InterpolateCameraLookAt(playerid, 156.2734, -1776.0850, 14.2128, 156.2713, -1776.0797, 14.7078, 5000, CAMERA_MOVE);
	SetTimerEx("PlayerCheck", 1000, false, "ii", playerid, g_RaceCheck{playerid});
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	UnloadPlayerVehicle(playerid);
	SaveData(playerid);
	return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER)
	{
	    new vehicleid = GetPlayerVehicleID(playerid);
	    new pvid = Vehicle_Inside(playerid);
	    new time[3];
	    if(IsSpeedoVehicle(vehicleid))
	    {
	        forex(i, 4)
	        {
	            PlayerTextDrawShow(playerid, SPEEDOTD[playerid][i]);
			}
			PlayerTextDrawShow(playerid, KMHTD[playerid]);
			PlayerTextDrawShow(playerid, VEHNAMETD[playerid]);
			PlayerTextDrawShow(playerid, HEALTHTD[playerid]);
			FUELBAR[playerid] = CreatePlayerProgressBar(playerid, 520.000000, 433.000000, 110.000000, 7.000000, 9109759, 100.000000, 0);
		}
		if(pvid != -1 && VehicleData[pvid][vRental] != -1)
		{
		    GetElapsedTime(VehicleData[pvid][vRentTime], time[0], time[1], time[2]);
		    SendClientMessageEx(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Sisa rental {00FFFF}%s {FFFFFF}milikmu adalah {FFFF00}%02d jam %02d menit %02d detik", GetVehicleName(vehicleid), time[0], time[1], time[2]);
		}
	}
	if(oldstate == PLAYER_STATE_DRIVER)
	{
        forex(i, 4)
        {
            PlayerTextDrawHide(playerid, SPEEDOTD[playerid][i]);
		}
		PlayerTextDrawHide(playerid, KMHTD[playerid]);
		PlayerTextDrawHide(playerid, VEHNAMETD[playerid]);
		PlayerTextDrawHide(playerid, HEALTHTD[playerid]);
		DestroyPlayerProgressBar(playerid, FUELBAR[playerid]);
	}
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(dialogid == DIALOG_BIZPRICE)
	{
	    if(response)
	    {
			new str[256];
	        PlayerData[playerid][pListitem] = listitem;
	        format(str, sizeof(str), "{FFFFFF}Current Product Price: %s\n{FFFFFF}Silahkan masukan harga baru untuk product {00FFFF}%s", FormatNumber(BizData[PlayerData[playerid][pInBiz]][bizProduct][listitem]), ProductName[PlayerData[playerid][pInBiz]][listitem]);
	        ShowPlayerDialog(playerid, DIALOG_BIZPRICESET, DIALOG_STYLE_INPUT, "Set Product Price", str, "Set", "Close");
		}
		// else
		    // cmd_biz(playerid, "menu");
	}
	if(dialogid == DIALOG_BIZPROD)
	{
	    if(response)
	    {
			new str[256];
	        PlayerData[playerid][pListitem] = listitem;
	        format(str, sizeof(str), "{FFFFFF}Current Product Name: %s\n{FFFFFF}Silahkan masukan nama baru untuk product {00FFFF}%s", ProductName[PlayerData[playerid][pInBiz]][listitem], ProductName[PlayerData[playerid][pInBiz]][listitem]);
	        ShowPlayerDialog(playerid, DIALOG_BIZPRODSET, DIALOG_STYLE_INPUT, "Set Product Name", str, "Set", "Close");
		}
		// else
		//     cmd_biz(playerid, "menu");
	}
	if(dialogid == DIALOG_BIZPRODSET)
	{
	    if(response)
	    {
	        if(strlen(inputtext) < 1 || strlen(inputtext) > 24)
	            return SendErrorMessage(playerid, "Invalid Product name!");

			new id = PlayerData[playerid][pInBiz];
			new slot = PlayerData[playerid][pListitem];
			SendClientMessageEx(playerid, COLOR_SERVER, "BIZ: {FFFFFF}Kamu telah mengubah nama product dari {00FFFF}%s {FFFFFF}menjadi {00FFFF}%s", ProductName[id][slot], inputtext);
			format(ProductName[id][slot], 24, inputtext);
			// cmd_biz(playerid, "menu");
			Business_Save(id);
		}
	}
	if(dialogid == DIALOG_BIZPRICESET)
	{
	    if(response)
	    {
	        if(strval(inputtext) < 1)
	            return SendErrorMessage(playerid, "Invalid Product price!");
	            
			new id = PlayerData[playerid][pInBiz];
			new slot = PlayerData[playerid][pListitem];
			SendClientMessageEx(playerid, COLOR_SERVER, "BIZ: {FFFFFF}Kamu telah mengubah harga product dari {009000}%s {FFFFFF}menjadi {009000}%s", FormatNumber(BizData[id][bizProduct][slot]), FormatNumber(strval(inputtext)));
			BizData[id][bizProduct][slot] = strval(inputtext);
			// cmd_biz(playerid, "menu");
			Business_Save(id);
		}
	}
	if(dialogid == DIALOG_BIZMENU)
	{
	    if(response)
	    {
	        if(listitem == 0)
	        {
	            SetProductName(playerid);
			}
			if(listitem == 1)
			{
			    SetProductPrice(playerid);
			}
			if(listitem == 2)
			{
				new str[256];
				format(str, sizeof(str), "{FFFFFF}Current Biz Name: %s\n{FFFFFF}Silahkan masukan nama Business mu yang baru:\n\n{FFFFFF}Note: Max 24 Huruf!", BizData[PlayerData[playerid][pInBiz]][bizName]);
				ShowPlayerDialog(playerid, DIALOG_BIZNAME, DIALOG_STYLE_INPUT, "Business Name", str, "Set", "Close");
			}
		}
	}
	if(dialogid == DIALOG_RENTAL)
	{
	    if(response)
	    {
	        new rentid = PlayerData[playerid][pRenting];
	        if(GetMoney(playerid) < RentData[rentid][rentPrice][listitem])
	            return SendErrorMessage(playerid, "Kamu tidak memiliki cukup uang!");
	            
			new str[256];
			format(str, sizeof(str), "{FFFFFF}Berapa jam kamu ingin menggunakan kendaraan Rental ini ?\n{FFFFFF}Maksimal adalah {FFFF00}4 jam\n\n{FFFFFF}Harga per Jam: {009000}$%d", RentData[rentid][rentPrice][listitem]);
			ShowPlayerDialog(playerid, DIALOG_RENTTIME, DIALOG_STYLE_INPUT, "{FFFFFF}Rental Time", str, "Rental", "Close");
			PlayerData[playerid][pListitem] = listitem;
		}
	}
	if(dialogid == DIALOG_RENTTIME)
	{
	    if(response)
	    {
	        new id = PlayerData[playerid][pRenting];
	        new slot = PlayerData[playerid][pListitem];
			new time = strval(inputtext);
			if(time < 1 || time > 4)
			{
				new str[256];
				format(str, sizeof(str), "{FFFFFF}Berapa jam kamu ingin menggunakan kendaraan Rental ini ?\n{FFFFFF}Maksimal adalah {FFFF00}4 jam\n\n{FFFFFF}Harga per Jam: {009000}$%d", RentData[id][rentPrice][listitem]);
				ShowPlayerDialog(playerid, DIALOG_RENTTIME, DIALOG_STYLE_INPUT, "{FFFFFF}Rental Time", str, "Rental", "Close");
				return 1;
			}
			GiveMoney(playerid, -RentData[id][rentPrice][slot] * time);
			SendClientMessageEx(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Kamu telah menyewa {00FFFF}%s {FFFFFF}untuk %d Jam seharga {009000}$%d", GetVehicleModelName(RentData[id][rentModel][slot]), time, RentData[id][rentPrice][slot] * time);
            VehicleRental_Create(PlayerData[playerid][pID], RentData[id][rentModel][slot], RentData[id][rentSpawn][0], RentData[id][rentSpawn][1], RentData[id][rentSpawn][2], RentData[id][rentSpawn][3], time*3600, PlayerData[playerid][pRenting]);
		}
	}
	if(dialogid == DIALOG_BUYSKINS)
	{
	    if(response)
	    {
	        GiveMoney(playerid, -PlayerData[playerid][pSkinPrice]);
			SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(PlayerData[playerid][pSkinPrice]), ProductName[PlayerData[playerid][pInBiz]][0]);
			BizData[PlayerData[playerid][pInBiz]][bizStock]--;
			if(PlayerData[playerid][pGender] == 1)
			{
				UpdatePlayerSkin(playerid, g_aMaleSkins[listitem]);
			}
			else
			{
				UpdatePlayerSkin(playerid, g_aFemaleSkins[listitem]);
			}
		}
	}
	if(dialogid == DIALOG_DROPITEM)
	{
	    if(response)
	    {
			new
			    itemid = PlayerData[playerid][pListitem],
			    string[32],
				str[356];

			strunpack(string, InventoryData[playerid][itemid][invItem]);

			if (response)
			{
			    if (isnull(inputtext))
			        return format(str, sizeof(str), "Drop Item", "Item: %s - Quantity: %d\n\nPlease specify how much of this item you wish to drop:", string, InventoryData[playerid][itemid][invQuantity]),
					ShowPlayerDialog(playerid, DIALOG_DROPITEM, DIALOG_STYLE_INPUT, "Drop Item", str, "Drop", "Cancel");

				if (strval(inputtext) < 1 || strval(inputtext) > InventoryData[playerid][itemid][invQuantity])
				    return format(str, sizeof(str), "ERROR: Insufficient amount specified.\n\nItem: %s - Quantity: %d\n\nPlease specify how much of this item you wish to drop:", string, InventoryData[playerid][itemid][invQuantity]),
					ShowPlayerDialog(playerid, DIALOG_DROPITEM, DIALOG_STYLE_INPUT, "Drop Item", str, "Drop", "Cancel");

				DropPlayerItem(playerid, itemid, strval(inputtext));
			}
		}
	}
	if(dialogid == DIALOG_GIVEITEM)
	{
		if (response)
		{
		    static
		        userid = -1,
				itemid = -1,
				string[32];

			if (sscanf(inputtext, "u", userid))
			    return ShowPlayerDialog(playerid, DIALOG_GIVEITEM, DIALOG_STYLE_INPUT, "Give Item", "Please enter the name or the ID of the player:", "Submit", "Cancel");

			if (userid == INVALID_PLAYER_ID)
			    return ShowPlayerDialog(playerid, DIALOG_GIVEITEM, DIALOG_STYLE_INPUT, "Give Item", "ERROR: Invalid player specified.\n\nPlease enter the name or the ID of the player:", "Submit", "Cancel");

		    if (!IsPlayerNearPlayer(playerid, userid, 6.0))
				return ShowPlayerDialog(playerid, DIALOG_GIVEITEM, DIALOG_STYLE_INPUT, "Give Item", "ERROR: You are not near that player.\n\nPlease enter the name or the ID of the player:", "Submit", "Cancel");

		    if (userid == playerid)
				return ShowPlayerDialog(playerid, DIALOG_GIVEITEM, DIALOG_STYLE_INPUT, "Give Item", "ERROR: You can't give items to yourself.\n\nPlease enter the name or the ID of the player:", "Submit", "Cancel");

			itemid = PlayerData[playerid][pListitem];

			if (itemid == -1)
			    return 0;

			strunpack(string, InventoryData[playerid][itemid][invItem]);

			if (InventoryData[playerid][itemid][invQuantity] == 1)
			{
			    new id = Inventory_Add(userid, string, InventoryData[playerid][itemid][invModel]);

			    if (id == -1)
					return SendErrorMessage(playerid, "That player doesn't have anymore inventory slots.");

			    SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s takes out a \"%s\" and gives it to %s.", ReturnName(playerid), string, ReturnName(userid));
			    SendServerMessage(userid, "%s has given you \"%s\" (added to inventory).", ReturnName(playerid), string);

				Inventory_Remove(playerid, string);
			    //Log_Write("logs/give_log.txt", "[%s] %s (%s) has given a %s to %s (%s).", ReturnDate(), ReturnName(playerid), PlayerData[playerid][pIP], string, ReturnName(userid, 0), PlayerData[userid][pIP]);
	  		}
			else
			{
				new str[152];
				format(str, sizeof(str), "Item: %s (Amount: %d)\n\nPlease enter the amount of this item you wish to give %s:", string, InventoryData[playerid][itemid][invQuantity], ReturnName(userid));
			    ShowPlayerDialog(playerid, DIALOG_GIVEAMOUNT, DIALOG_STYLE_INPUT, "Give Item", str, "Give", "Cancel");
			    PlayerData[playerid][pTarget] = userid;
			}
		}
	}
	if(dialogid == DIALOG_GIVEAMOUNT)
	{
		if (response && PlayerData[playerid][pTarget] != INVALID_PLAYER_ID)
		{
		    new
		        userid = PlayerData[playerid][pTarget],
		        itemid = PlayerData[playerid][pListitem],
				string[32],
				str[352];

			strunpack(string, InventoryData[playerid][itemid][invItem]);

			if (isnull(inputtext))
				return format(str, sizeof(str), "Item: %s (Amount: %d)\n\nPlease enter the amount of this item you wish to give %s:", string, InventoryData[playerid][itemid][invQuantity], ReturnName(userid)),
				ShowPlayerDialog(playerid, DIALOG_GIVEAMOUNT, DIALOG_STYLE_INPUT, "Give Item", str, "Give", "Cancel");

			if (strval(inputtext) < 1 || strval(inputtext) > InventoryData[playerid][itemid][invQuantity])
			    return format(str, sizeof(str), "ERROR: You don't have that much.\n\nItem: %s (Amount: %d)\n\nPlease enter the amount of this item you wish to give %s:", string, InventoryData[playerid][itemid][invQuantity], ReturnName(userid)),
				ShowPlayerDialog(playerid, DIALOG_GIVEAMOUNT, DIALOG_STYLE_INPUT, "Give Item", str, "Give", "Cancel");

	        new id = Inventory_Add(userid, string, InventoryData[playerid][itemid][invModel], strval(inputtext));

		    if (id == -1)
				return SendErrorMessage(playerid, "That player doesn't have anymore inventory slots.");

		    SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s takes out a \"%s\" and gives it to %s.", ReturnName(playerid), string, ReturnName(userid));
		    SendServerMessage(userid, "%s has given you \"%s\" (added to inventory).", ReturnName(playerid), string);

			Inventory_Remove(playerid, string, strval(inputtext));
		  //  Log_Write("logs/give_log.txt", "[%s] %s (%s) has given %d %s to %s (%s).", ReturnDate(), ReturnName(playerid), PlayerData[playerid][pIP], strval(inputtext), string, ReturnName(userid, 0), PlayerData[userid][pIP]);
		}
	}
	if(dialogid == DIALOG_INVACTION)
	{
	    if(response)
	    {
		    new
				itemid = PlayerData[playerid][pListitem],
				string[64],
				str[256];

		    strunpack(string, InventoryData[playerid][itemid][invItem]);

		    switch (listitem)
		    {
		        case 0:
		        {
		            CallLocalFunction("OnPlayerUseItem", "dds", playerid, itemid, string);
		        }
		        case 1:
		        {
				    if(!strcmp(string, "Cellphone"))
				        return SendErrorMessage(playerid, "You can't do that on this item!");

				    if(!strcmp(string, "GPS"))
				        return SendErrorMessage(playerid, "You can't do that on this item!");
				        
					PlayerData[playerid][pListitem] = itemid;
					ShowPlayerDialog(playerid, DIALOG_GIVEITEM, DIALOG_STYLE_INPUT, "Give Item", "Please enter the name or the ID of the player:", "Submit", "Cancel");
		        }
		        case 2:
		        {
		            if (IsPlayerInAnyVehicle(playerid))
		                return SendErrorMessage(playerid, "You can't drop items right now.");

				    if(!strcmp(string, "Cellphone"))
				        return SendErrorMessage(playerid, "You can't do that on this item!");

				    if(!strcmp(string, "GPS"))
				        return SendErrorMessage(playerid, "You can't do that on this item!");

					else if (InventoryData[playerid][itemid][invQuantity] == 1)
						DropPlayerItem(playerid, itemid);

					else
						format(str, sizeof(str), "Item: %s - Quantity: %d\n\nPlease specify how much of this item you wish to drop:", string, InventoryData[playerid][itemid][invQuantity]),
						ShowPlayerDialog(playerid, DIALOG_DROPITEM, DIALOG_STYLE_INPUT, "Drop Item", str, "Drop", "Cancel");
				}
			}
		}
	}
    if(dialogid == DIALOG_INVENTORY)
    {
        if(response)
        {
		    new
		        name[48];

            strunpack(name, InventoryData[playerid][listitem][invItem]);
            PlayerData[playerid][pListitem] = listitem;

			switch (PlayerData[playerid][pStorageSelect])
			{
			    case 0:
			    {
		            format(name, sizeof(name), "%s (%d)", name, InventoryData[playerid][listitem][invQuantity]);
		            ShowPlayerDialog(playerid, DIALOG_INVACTION, DIALOG_STYLE_LIST, name, "Use Item\nGive Item\nDrop Item", "Select", "Cancel");
				}
			}
		}
	}
	if(dialogid == DIALOG_BIZBUY)
	{
	    if(response)
	    {
	        new bid = PlayerData[playerid][pInBiz], price, prodname[34];
	        if(bid != -1)
	        {
	            price = BizData[bid][bizProduct][listitem];
				prodname = ProductName[bid][listitem];
	            if(GetMoney(playerid) < price)
	                return SendErrorMessage(playerid, "You don't have enough money!");
	                
				if(BizData[bid][bizStock] < 1)
					return SendErrorMessage(playerid, "This business is out of stock.");
					
				switch(BizData[bid][bizType])
				{
				    case 1:
				    {
						if(listitem == 0)
						{
						    if(GetEnergy(playerid) >= 100)
						        return SendErrorMessage(playerid, "Your energy is already full!");

							PlayerData[playerid][pEnergy] += 20;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 1)
						{
						    if(GetEnergy(playerid) >= 100)
						        return SendErrorMessage(playerid, "Your energy is already full!");

							PlayerData[playerid][pEnergy] += 40;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 2)
						{
						    if(GetEnergy(playerid) >= 100)
						        return SendErrorMessage(playerid, "Your energy is already full!");

							PlayerData[playerid][pEnergy] += 15;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
					}
					case 2:
					{
					    if(listitem == 0)
					    {
							Inventory_Add(playerid, "Snack", 2768, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 1)
						{
							Inventory_Add(playerid, "Water", 2958, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 2)
						{
							Inventory_Add(playerid, "Mask", 19036, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 3)
						{
							Inventory_Add(playerid, "Medkit", 1580, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
					}
					case 3:
					{
					    new gstr[1012];
					    if(PlayerData[playerid][pGender] == 1)
					    {
					        forex(i, sizeof(g_aMaleSkins))
					        {
					            format(gstr, sizeof(gstr), "%s%i\n", gstr, g_aMaleSkins[i]);
							}
							ShowPlayerDialog(playerid, DIALOG_BUYSKINS, DIALOG_STYLE_LIST, "Purchase Clothes", gstr, "Select", "Close");
						}
						else
						{
					        forex(i, sizeof(g_aFemaleSkins))
					        {
					            format(gstr, sizeof(gstr), "%s%i\n", gstr, g_aFemaleSkins[i]);
							}
							ShowPlayerDialog(playerid, DIALOG_BUYSKINS, DIALOG_STYLE_LIST, "Purchase Clothes", gstr, "Select", "Close");
						}
					}
					case 4:
					{
					    if(listitem == 0)
						{
						    if(PlayerHasItem(playerid, "Cellphone"))
						        return SendErrorMessage(playerid, "Kamu sudah memiliki Cellphone!");
						        
							PlayerData[playerid][pPhoneNumber] = PlayerData[playerid][pID]+RandomEx(13158, 98942);
							Inventory_Add(playerid, "Cellphone", 18867, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
					    if(listitem == 1)
						{
						    if(PlayerHasItem(playerid, "GPS"))
						        return SendErrorMessage(playerid, "Kamu sudah memiliki GPS!");

							Inventory_Add(playerid, "GPS", 18875, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
					    if(listitem == 2)
						{
						    if(PlayerHasItem(playerid, "Portable Radio"))
						        return SendErrorMessage(playerid, "Kamu sudah memiliki Portable Radio!");

							Inventory_Add(playerid, "Portable Radio", 19942, 1);
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 3)
						{
							PlayerData[playerid][pCredit] += 50;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
					}
				}
			}
		}
	}
	if(dialogid == DIALOG_REGISTER)
	{
	    if(!response)
	        return Kick(playerid);

		new str[256];
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Create Password: {FF00FF}(Input Below)", GetName(playerid), PlayerData[playerid][pAttempt]);

        if(strlen(inputtext) < 7)
			return ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to Xyronite", str, "Register", "Exit");

        if(strlen(inputtext) > 32)
			return ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to Xyronite", str, "Register", "Exit");

        bcrypt_hash(playerid, "HashPlayerPassword", inputtext, BCRYPT_COST);
	}
	if(dialogid == DIALOG_LOGIN)
	{
	    if(!response)
	        return Kick(playerid);
	        
        if(strlen(inputtext) < 1)
        {
			new str[256];
            format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), PlayerData[playerid][pAttempt]);
            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to Xyronite", str, "Login", "Exit");
            return 1;
		}
		new pwQuery[256], hash[BCRYPT_HASH_LENGTH];
		mysql_format(sqlcon, pwQuery, sizeof(pwQuery), "SELECT Password FROM PlayerUCP WHERE UCP = '%e' LIMIT 1", GetName(playerid));
		mysql_query(sqlcon, pwQuery);
		
        cache_get_value_name(0, "Password", hash, sizeof(hash));
        
        bcrypt_verify(playerid, "OnPlayerPasswordChecked", inputtext, hash);

	}
    if(dialogid == DIALOG_CHARLIST)
    {
		if(response)
		{
			if (PlayerChar[playerid][listitem][0] == EOS)
				return ShowPlayerDialog(playerid, DIALOG_MAKECHAR, DIALOG_STYLE_INPUT, "Create Character", "Insert your new Character Name\n\nExample: Finn_Xanderz, Javier_Cooper etc.", "Create", "Exit");

			PlayerData[playerid][pChar] = listitem;
			SetPlayerName(playerid, PlayerChar[playerid][listitem]);

			new cQuery[256];
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "SELECT * FROM `characters` WHERE `Name` = '%s' LIMIT 1;", PlayerChar[playerid][PlayerData[playerid][pChar]]);
			mysql_tquery(sqlcon, cQuery, "LoadCharacterData", "d", playerid);
			
		}
	}
	if(dialogid == DIALOG_MAKECHAR)
	{
	    if(response)
	    {
		    if(strlen(inputtext) < 1 || strlen(inputtext) > 24)
				return ShowPlayerDialog(playerid, DIALOG_MAKECHAR, DIALOG_STYLE_INPUT, "Create Character", "Insert your new Character Name\n\nExample: Finn_Xanderz, Javier_Cooper etc.", "Create", "Back");

			if(!IsRoleplayName(inputtext))
				return ShowPlayerDialog(playerid, DIALOG_MAKECHAR, DIALOG_STYLE_INPUT, "Create Character", "Insert your new Character Name\n\nExample: Finn_Xanderz, Javier_Cooper etc.", "Create", "Back");

			new characterQuery[178];
			mysql_format(sqlcon, characterQuery, sizeof(characterQuery), "SELECT * FROM `characters` WHERE `Name` = '%s'", inputtext);
			mysql_tquery(sqlcon, characterQuery, "InsertPlayerName", "ds", playerid, inputtext);

		    format(PlayerData[playerid][pUCP], 22, GetName(playerid));
		}
	}
	if(dialogid == DIALOG_AGE)
	{
		if(response)
		{
			if(strval(inputtext) >= 70)
			    return ShowPlayerDialog(playerid, DIALOG_AGE, DIALOG_STYLE_INPUT, "Character Age", "ERROR: Cannot more than 70 years old!", "Continue", "Cancel");

			if(strval(inputtext) < 13)
			    return ShowPlayerDialog(playerid, DIALOG_AGE, DIALOG_STYLE_INPUT, "Character Age", "ERROR: Cannot below 13 Years Old!", "Continue", "Cancel");

			PlayerData[playerid][pAge] = strval(inputtext);
			ShowPlayerDialog(playerid, DIALOG_ORIGIN, DIALOG_STYLE_INPUT, "Character Origin", "Please input your Character Origin:", "Continue", "Quit");
		}
		else
		{
		    ShowPlayerDialog(playerid, DIALOG_AGE, DIALOG_STYLE_INPUT, "Character Age", "Please Insert your Character Age", "Continue", "Cancel");
		}
	}
	if(dialogid == DIALOG_ORIGIN)
	{
	    if(!response)
	        return ShowPlayerDialog(playerid, DIALOG_ORIGIN, DIALOG_STYLE_INPUT, "Character Origin", "Please input your Character Origin:", "Continue", "Quit");

		if(strlen(inputtext) < 1)
		    return ShowPlayerDialog(playerid, DIALOG_ORIGIN, DIALOG_STYLE_INPUT, "Character Origin", "Please input your Character Origin:", "Continue", "Quit");

        format(PlayerData[playerid][pOrigin], 32, inputtext);
        ShowPlayerDialog(playerid, DIALOG_GENDER, DIALOG_STYLE_LIST, "Character Gender", "Male\nFemale", "Continue", "Cancel");
	}
	if(dialogid == DIALOG_GENDER)
	{
	    if(!response)
	        return ShowPlayerDialog(playerid, DIALOG_GENDER, DIALOG_STYLE_LIST, "Character Gender", "Male\nFemale", "Continue", "Cancel");

		if(listitem == 0)
		{
			PlayerData[playerid][pGender] = 1;
			PlayerData[playerid][pSkin] = 240;
			PlayerData[playerid][pHealth] = 100.0;
			SetupPlayerData(playerid);
		}
		if(listitem == 1)
		{
			PlayerData[playerid][pGender] = 2;
			PlayerData[playerid][pSkin] = 172;
			PlayerData[playerid][pHealth] = 100.0;
			SetupPlayerData(playerid);
			
		}
	}
	return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
	return 1;
}

public OnPlayerSpawn(playerid)
{
	if(!PlayerData[playerid][pSpawned])
	{
	    PlayerData[playerid][pSpawned] = true;
	    GivePlayerMoney(playerid, PlayerData[playerid][pMoney]);
	    SetPlayerHealth(playerid, PlayerData[playerid][pHealth]);
	    SetPlayerSkin(playerid, PlayerData[playerid][pSkin]);
	    SetPlayerVirtualWorld(playerid, PlayerData[playerid][pWorld]);
		SetPlayerInterior(playerid, PlayerData[playerid][pInterior]);
		PlayerTextDrawShow(playerid, ENERGYTD[playerid][0]);
		PlayerTextDrawShow(playerid, ENERGYTD[playerid][1]);
		ENERGYBAR[playerid] = CreatePlayerProgressBar(playerid, 539.000000, 158.000000, 69.500000, 9.000000, 9109759, 100.000000, 0);
	}
	return 1;
}


public OnPlayerText(playerid, text[])
{
	if(PlayerData[playerid][pCalling] != INVALID_PLAYER_ID)
	{
		new lstr[1024];
		format(lstr, sizeof(lstr), "(Phone) %s says: %s", ReturnName(playerid), text);
		ProxDetector(10, playerid, lstr, 0xE6E6E6E6, 0xC8C8C8C8, 0xAAAAAAAA, 0x8C8C8C8C, 0x6E6E6E6E);
		SetPlayerChatBubble(playerid, text, COLOR_WHITE, 10.0, 3000);

		SendClientMessageEx(PlayerData[playerid][pCalling], COLOR_YELLOW, "(Phone) Caller says: %s", text);
		return 0;
	}
	else
	{
		new lstr[1024];
		format(lstr, sizeof(lstr), "%s says: %s", ReturnName(playerid), text);
		ProxDetector(10, playerid, lstr, 0xE6E6E6E6, 0xC8C8C8C8, 0xAAAAAAAA, 0x8C8C8C8C, 0x6E6E6E6E);
		SetPlayerChatBubble(playerid, text, COLOR_WHITE, 10.0, 3000);

		return 0;
	}
}

public OnVehicleSpawn(vehicleid)
{
	forex(i, MAX_PLAYER_VEHICLE)if(VehicleData[i][vExists])
	{
		if(vehicleid == VehicleData[i][vVehicle] && IsValidVehicle(VehicleData[i][vVehicle]))
		{
		    if(VehicleData[i][vRental] == -1)
		    {
				if(VehicleData[i][vInsurance] > 0)
	    		{
					VehicleData[i][vInsurance] --;
					VehicleData[i][vInsuTime] = gettime() + (1 * 86400);
					foreach(new pid : Player) if (VehicleData[i][vOwner] == PlayerData[pid][pID])
	        		{
	            		SendServerMessage(pid, "Kendaraan {00FFFF}%s {FFFFFF}milikmu telah hancur, kamu bisa Claim setelah 24 jam dari Insurance.", GetVehicleName(vehicleid));
					}

					if(IsValidVehicle(VehicleData[i][vVehicle]))
						DestroyVehicle(VehicleData[i][vVehicle]);

					VehicleData[i][vVehicle] = INVALID_VEHICLE_ID;
				}
				else
				{
					foreach(new pid : Player) if (VehicleData[i][vOwner] == PlayerData[pid][pID])
	        		{
	            		SendServerMessage(pid, "Kendaraan {00FFFF}%s {FFFFFF}milikmu telah hancur dan tidak akan dan tidak memiliki Insurance lagi.", GetVehicleName(vehicleid));
					}
					
					new query[128];
					mysql_format(sqlcon, query, sizeof(query), "DELETE FROM vehicle WHERE vehID = '%d'", VehicleData[i][vID]);
					mysql_query(sqlcon, query, true);

                    VehicleData[i][vExists] = false;
                    
					if(IsValidVehicle(VehicleData[i][vVehicle]))
						DestroyVehicle(VehicleData[i][vVehicle]);
				}
			}
			else
			{
				foreach(new pid : Player) if (VehicleData[i][vOwner] == PlayerData[pid][pID])
        		{
        		    GiveMoney(pid, -250);
            		SendServerMessage(pid, "Kendaraan Rental milikmu (%s) telah hancur, kamu dikenai denda sebesar {009000}$250!", GetVehicleName(vehicleid));
				}

				new query[128];
				mysql_format(sqlcon, query, sizeof(query), "DELETE FROM vehicle WHERE vehID = '%d'", VehicleData[i][vID]);
				mysql_query(sqlcon, query, true);

                VehicleData[i][vExists] = false;

				if(IsValidVehicle(VehicleData[i][vVehicle]))
					DestroyVehicle(VehicleData[i][vVehicle]);
			}
		}
	}
	return 1;
}

/*
	    case 1: str = "Fast Food";
	    case 2: str = "24/7";
	    case 3: str = "Clothes";
*/

stock SetProductPrice(playerid)
{
	new bid = PlayerData[playerid][pInBiz], string[712];
	if(!BizData[bid][bizExists])
	    return 0;

	switch(BizData[bid][bizType])
	{
	    case 1:
	    {
	        format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s",
				ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2])
			);
		}
		case 2:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
		case 3:
		{
		    format(string, sizeof(string), "Product\tPrice\nClothes\t%s",
                ProductName[bid][0],
		        FormatNumber(BizData[bid][bizProduct][0])
			);
		}
		case 4:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
	}
	ShowPlayerDialog(playerid, DIALOG_BIZPRICE, DIALOG_STYLE_TABLIST_HEADERS, "Set Product Price", string, "Select", "Close");
	return 1;
}

stock SetProductName(playerid)
{
	new bid = PlayerData[playerid][pInBiz], string[712];
	if(!BizData[bid][bizExists])
	    return 0;

	switch(BizData[bid][bizType])
	{
	    case 1:
	    {
	        format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s",
				ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2])
			);
		}
		case 2:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
		case 3:
		{
		    format(string, sizeof(string), "Product\tPrice\nClothes\t%s",
                ProductName[bid][0],
		        FormatNumber(BizData[bid][bizProduct][0])
			);
		}
		case 4:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
	}
	ShowPlayerDialog(playerid, DIALOG_BIZPROD, DIALOG_STYLE_TABLIST_HEADERS, "Set Product Name", string, "Select", "Close");
	return 1;
}

stock ShowBusinessMenu(playerid)
{
	new bid = PlayerData[playerid][pInBiz], string[712];
	if(!BizData[bid][bizExists])
	    return 0;
	    
	switch(BizData[bid][bizType])
	{
	    case 1:
	    {
	        format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s",
				ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2])
			);
		}
		case 2:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
		case 3:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s",
                ProductName[bid][0],
		        FormatNumber(BizData[bid][bizProduct][0])
			);
		}
		case 4:
		{
		    format(string, sizeof(string), "Product\tPrice\n%s\t%s\n%s\t%s\n%s\t%s\n%s\t%s",
                ProductName[bid][0],
				FormatNumber(BizData[bid][bizProduct][0]),
				ProductName[bid][1],
	            FormatNumber(BizData[bid][bizProduct][1]),
	            ProductName[bid][2],
	            FormatNumber(BizData[bid][bizProduct][2]),
	            ProductName[bid][3],
	            FormatNumber(BizData[bid][bizProduct][3])
			);
		}
	}
	ShowPlayerDialog(playerid, DIALOG_BIZBUY, DIALOG_STYLE_TABLIST_HEADERS, "Business Product", string, "Select", "Close");
	return 1;
}
	            
/* » Commands */


CMD:biz(playerid, params[])
{
	new
	    type[24],
	    string[128];

	if (sscanf(params, "s[24]S()[128]", type, string))
	{
	    SendSyntaxMessage(playerid, "/biz [name]");
	    SendClientMessage(playerid, COLOR_SERVER, "Names:{FFFFFF} buy, convertfuel, reqstock, menu, lock");
	    return 1;
	}
	if(!strcmp(type, "buy", true))
	{
/*	    if(Biz_GetCount(playerid) >= 1)
	        return SendErrorMessage(playerid, "Kamu hanya bisa memiliki 1 Bisnis!");*/
	        
		forex(i, MAX_BUSINESS)if(BizData[i][bizExists])
		{
      		if(IsPlayerInRangeOfPoint(playerid, 3.5, BizData[i][bizExt][0], BizData[i][bizExt][1], BizData[i][bizExt][2]))
			{
			    if(BizData[i][bizOwner] != -1)
			        return SendErrorMessage(playerid, "Bisnis ini sudah dimiliki seseorang!");
			        
				if(GetMoney(playerid) < BizData[i][bizPrice])
				    return SendErrorMessage(playerid, "Kamu tidak memiliki cukup uang untuk membeli Bisnis ini!");
				    
				BizData[i][bizOwner] = PlayerData[playerid][pID];
                format(BizData[i][bizOwnerName], MAX_PLAYER_NAME, GetName(playerid));
                SendServerMessage(playerid, "Kamu berhasil membeli Business ini seharga {00FF00}%s", FormatNumber(BizData[i][bizPrice]));
                GiveMoney(playerid, -BizData[i][bizPrice]);
                Business_Refresh(i);
                Business_Save(i);
			}
		}
	}
	else if(!strcmp(type, "menu", true))
	{
		if(PlayerData[playerid][pInBiz] != -1 && GetPlayerInterior(playerid) == BizData[PlayerData[playerid][pInBiz]][bizInterior] && GetPlayerVirtualWorld(playerid) == BizData[PlayerData[playerid][pInBiz]][bizWorld] && Biz_IsOwner(playerid, PlayerData[playerid][pInBiz]))
		{
		    ShowPlayerDialog(playerid, DIALOG_BIZMENU, DIALOG_STYLE_LIST, "Business Menu", "Set Product Name\nSet Product Price\nSet Business Name", "Select", "Close");
		}
		else
			SendErrorMessage(playerid, "Kamu tidak berada didalam bisnis milikmu!");
	}
	return 1;
}

CMD:inventory(playerid, params[])
{
	PlayerData[playerid][pStorageSelect] = 0;
	OpenInventory(playerid);
	return 1;
}

CMD:makemeadmin(playerid, params[])
{
	PlayerData[playerid][pAdmin] = 7;
	return 1;
}
CMD:enter(playerid, params[])
{
	if(!IsPlayerInAnyVehicle(playerid))
	{
		forex(bid, MAX_BUSINESS) if(BizData[bid][bizExists])
		{
			if(IsPlayerInRangeOfPoint(playerid, 2.8, BizData[bid][bizExt][0], BizData[bid][bizExt][1], BizData[bid][bizExt][2]))
			{
				if(BizData[bid][bizLocked])
					return SendErrorMessage(playerid, "This business is Locked by the Owner!");

				PlayerData[playerid][pInBiz] = bid;
				SetPlayerPosEx(playerid, BizData[bid][bizInt][0], BizData[bid][bizInt][1], BizData[bid][bizInt][2]);

				SetPlayerInterior(playerid, BizData[bid][bizInterior]);
				SetPlayerVirtualWorld(playerid, BizData[bid][bizWorld]);
				SetCameraBehindPlayer(playerid);
				SetPlayerWeather(playerid, 0);
			}
	    }
		new inbiz = PlayerData[playerid][pInBiz];
		if(PlayerData[playerid][pInBiz] != -1 && IsPlayerInRangeOfPoint(playerid, 2.8, BizData[inbiz][bizInt][0], BizData[inbiz][bizInt][1], BizData[inbiz][bizInt][2]))
		{
			SetPlayerPos(playerid, BizData[inbiz][bizExt][0], BizData[inbiz][bizExt][1], BizData[inbiz][bizExt][2]);

			PlayerData[playerid][pInBiz] = -1;
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
			SetCameraBehindPlayer(playerid);
		}
	}
	return 1;
}

CMD:buy(playerid, params[])
{
	if(PlayerData[playerid][pInBiz] != -1 && GetPlayerInterior(playerid) == BizData[PlayerData[playerid][pInBiz]][bizInterior] && GetPlayerVirtualWorld(playerid) == BizData[PlayerData[playerid][pInBiz]][bizWorld])
	{
	    ShowBusinessMenu(playerid);
	}
	return 1;
}

CMD:setitem(playerid, params[])
{
	new
	    userid,
		item[32],
		amount;

	if (PlayerData[playerid][pAdmin] < 6)
	    return SendErrorMessage(playerid, "You don't have permission to use this command.");

	if (sscanf(params, "uds[32]", userid, amount, item))
	    return SendSyntaxMessage(playerid, "/setitem [playerid/name] [amount] [item name]");

	for (new i = 0; i < sizeof(g_aInventoryItems); i ++) if (!strcmp(g_aInventoryItems[i][e_InventoryItem], item, true))
	{
        Inventory_Set(userid, g_aInventoryItems[i][e_InventoryItem], g_aInventoryItems[i][e_InventoryModel], amount);

		return SendServerMessage(playerid, "You have set %s's \"%s\" to %d.", ReturnName(userid), item, amount);
	}
	SendErrorMessage(playerid, "Invalid item name (use /itemlist for a list).");
	return 1;
}

CMD:vcreate(playerid, params[])
{
	new model;
	if(sscanf(params, "d", model))
		return SendSyntaxMessage(playerid, "/vcreate [model]");
		
	new Float:pos[4];
	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	GetPlayerFacingAngle(playerid, pos[3]);
    Vehicle_Create(PlayerData[playerid][pID], model, pos[0], pos[1], pos[2], pos[3], 6, 6);
    SendServerMessage(playerid, "Vehicle created!");
	return 1;
}

CMD:gotoco(playerid, params[])
{
	if(PlayerData[playerid][pAdmin] >= 2)
	{
		new Float: pos[3], int;
		if(sscanf(params, "fffd", pos[0], pos[1], pos[2], int))
			return SendSyntaxMessage(playerid, "USAGE: /gotoco [x coordinate] [y coordinate] [z coordinate] [interior]");

		SendClientMessage(playerid, COLOR_WHITE, "You have been teleported to the coordinates specified.");
		SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
		SetPlayerInterior(playerid, int);
	}
	return 1;
}

CMD:veh(playerid, params[])
{
	new
	    model[32],
		color1,
		color2;

	if (sscanf(params, "s[32]I(-1)I(-1)", model, color1, color2))
	    return SendSyntaxMessage(playerid, "/veh [model id/name] <color 1> <color 2>");

	if ((model[0] = GetVehicleModelByName(model)) == 0)
	    return SendErrorMessage(playerid, "Invalid model ID.");

	new
	    Float:x,
	    Float:y,
	    Float:z,
	    Float:a,
		vehicleid;

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	vehicleid = CreateVehicle(model[0], x, y + 2, z, a, color1, color2, 0);

	if (GetPlayerInterior(playerid) != 0)
	    LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));

	if (GetPlayerVirtualWorld(playerid) != 0)
		SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));

	PutPlayerInVehicle(playerid, vehicleid, 0);
	SwitchVehicleEngine(vehicleid, true);
	VehCore[vehicleid][vehFuel] = 100;
	SendServerMessage(playerid, "You have spawned a %s.", ReturnVehicleModelName(model[0]));
	return 1;
}

CMD:v(playerid, params[])
{
	new
	    type[24],
	    string[128],
		vehicleid = GetPlayerVehicleID(playerid),
		pvid = Vehicle_Inside(playerid);

	if (sscanf(params, "s[24]S()[128]", type, string))
	{
	    SendSyntaxMessage(playerid, "/v [name]");
	    SendClientMessage(playerid, COLOR_SERVER, "Names:{FFFFFF} list, lock, engine");
	    return 1;
	}
	if(!strcmp(type, "engine", true))
	{
		if(IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			if(!IsEngineVehicle(vehicleid))
				return SendErrorMessage(playerid, "You're not inside of any engine vehicle!");

			if(pvid != -1 && !Vehicle_HaveAccess(playerid, pvid))
				return ShowMessage(playerid, "~r~ERROR ~w~Kamu tidak memiliki kunci kendaraan ini!", 2);
				
			if(GetEngineStatus(vehicleid))
			{
			    SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s inserts the key into the ignition and stops the engine.", ReturnName(playerid));
				EngineStatus(playerid, vehicleid);
			}
			else
			{
			    ShowText(playerid, "Turning on the engine....", 3);
				SetTimerEx("EngineStatus", 3000, false, "id", playerid, vehicleid);
				SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s inserts the key into the ignition and starts the engine.", ReturnName(playerid));
			}
		}
	}
	else if(!strcmp(type, "list", true))
	{
	    new bool:have, str[512];
	    format(str, sizeof(str), "Model\tPlate\tInsurance\n");
		forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists])
		{
		    if(Vehicle_IsOwner(playerid, i))
		    {
		        if(VehicleData[i][vInsuTime] != 0)
		        {
		            format(str, sizeof(str), "%s%s(Insurance)\t%s\t%d Left\n", str, GetVehicleModelName(VehicleData[i][vModel]), VehicleData[i][vPlate], VehicleData[i][vInsurance]);
				}
				else if(VehicleData[i][vRental] != -1)
		        {
		            format(str, sizeof(str), "%s%s(Rental)\t%s\tN/A\n", str, GetVehicleModelName(VehicleData[i][vModel]), VehicleData[i][vPlate]);
				}
				else
				{
		            format(str, sizeof(str), "%s%s(ID: %d)\t%s\t%d Left\n", str, GetVehicleModelName(VehicleData[i][vModel]), VehicleData[i][vVehicle], VehicleData[i][vPlate], VehicleData[i][vInsurance]);
				}
			}
			have = true;
		}
		if(have)
		    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, "Vehicle List", str, "Close", "");
		else
			SendErrorMessage(playerid, "You don't have any Vehicles!");
	}
	return 1;
}

CMD:unrentvehicle(playerid, params[])
{
	new pvid = Vehicle_Inside(playerid);
	new vehicleid = GetPlayerVehicleID(playerid);
	
	if(VehicleRental_Count(playerid) < 1)
	    return SendErrorMessage(playerid, "Kamu tidak memiliki kendaraan Rental!");
	    
	forex(i, MAX_RENTAL) if(RentData[i][rentExists])
	{
	    if(IsPlayerInRangeOfPoint(playerid, 3.0, RentData[i][rentPos][0], RentData[i][rentPos][1], RentData[i][rentPos][2]))
	    {
			if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
			    return SendErrorMessage(playerid, "Kamu harus mengemudi kendaraan Rental milikmu!");
			    
			if(vehicleid != pvid)
			    return SendErrorMessage(playerid, "Kamu harus mengemudi kendaraan Rental milikmu!");
			    
			Vehicle_Delete(pvid);
			SendClientMessageEx(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Kamu telah mengembalikan %s Rental milikmu!", GetVehicleName(vehicleid));
		}
	}
	return 1;
}
CMD:rentvehicle(playerid, params[])
{
	if(VehicleRental_Count(playerid) > 0)
	    return SendErrorMessage(playerid, "Kamu hanya bisa memiliki 1 kendaraan Rental!");
	    
	new gstr[256];
	forex(i, MAX_RENTAL) if(RentData[i][rentExists])
	{
	    if(IsPlayerInRangeOfPoint(playerid, 3.0, RentData[i][rentPos][0], RentData[i][rentPos][1], RentData[i][rentPos][2]))
	    {
	        if(RentData[i][rentSpawn][0] == 0)
	            return SendErrorMessage(playerid, "Rental Point ini belum memiliki Spawn Point!");


	        forex(z, 2)
	        {
	            format(gstr, sizeof(gstr), "%s%i\t~w~%s~n~~g~Price: $%d\n", gstr, RentData[i][rentModel][z], GetVehicleModelName(RentData[i][rentModel][z]), RentData[i][rentPrice][z]);
			}
			ShowPlayerDialog(playerid, DIALOG_RENTAL, DIALOG_STYLE_LIST, "Vehicle Rental", gstr, "Select", "Close");
			PlayerData[playerid][pRenting] = i;
		}
	}
	return 1;
}


CMD:rentinfo(playerid, params[])
{
	new bool:have, str[512], time[3];
	format(str, sizeof(str), "Model(ID)\tDuration\n");
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists])
	{
		if(Vehicle_IsOwner(playerid, i) && IsValidVehicle(VehicleData[i][vVehicle]) && VehicleData[i][vRental] != -1)
		{
		    GetElapsedTime(VehicleData[i][vRentTime], time[0], time[1], time[2]);
		    format(str, sizeof(str), "%s%s(%d)\t%02d:%02d:%02d\n", str, GetVehicleModelName(VehicleData[i][vModel]),VehicleData[i][vVehicle], time[0], time[1], time[2]);
			have = true;
		}
	}
	if(have)
		ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, "Rental Information", str, "Close", "");
	else
		SendErrorMessage(playerid, "Kamu tidak memiliki kendaraan Rental!");
	return 1;
}
/* Admin Commands */
CMD:aduty(playerid, params[])
{
    if(PlayerData[playerid][pAdmin] < 1)
        return SendErrorMessage(playerid, "You don't have permission to use this command!");
        
	if(!PlayerData[playerid][pAduty])
	{
	    PlayerData[playerid][pAduty] = true;
	    SetPlayerColor(playerid, 0xFF0000FF);
	    SetPlayerName(playerid, PlayerData[playerid][pUCP]);
	}
	else
	{
	    PlayerData[playerid][pAduty] = false;
	    SetPlayerColor(playerid, COLOR_WHITE);
	    SetPlayerName(playerid, PlayerData[playerid][pName]);
	}
	return 1;
}
CMD:editbiz(playerid, params[])
{
    new
        id,
        type[24],
        string[128];

    if(PlayerData[playerid][pAdmin] < 6)
        return SendErrorMessage(playerid, "You don't have permission to use this command!");

    if(sscanf(params, "ds[24]S()[128]", id, type, string))
    {
        SendSyntaxMessage(playerid, "/editbiz [id] [name]");
        SendClientMessage(playerid, COLOR_SERVER, "Names:{FFFFFF} location, interior, fuelpoint, fuelstock, price, stock");
        return 1;
    }
    if((id < 0 || id >= MAX_BUSINESS))
        return SendErrorMessage(playerid, "You have specified an invalid ID.");

	if(!BizData[id][bizExists])
        return SendErrorMessage(playerid, "You have specified an invalid ID.");

    if(!strcmp(type, "location", true))
    {
		GetPlayerPos(playerid, BizData[id][bizExt][0], BizData[id][bizExt][1], BizData[id][bizExt][2]);
		Business_Save(id);
		Business_Refresh(id);

		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmBiz: {FFFFFF}Kamu telah mengubah posisi Business ID: %d", id);
    }
    return 1;
}

CMD:editrental(playerid, params[])
{
    new
        id,
        type[24],
        string[128];

    if(PlayerData[playerid][pAdmin] < 6)
        return SendErrorMessage(playerid, "You don't have permission to use this command!");
        
    if(sscanf(params, "ds[24]S()[128]", id, type, string))
    {
        SendSyntaxMessage(playerid, "/editrental [id] [name]");
        SendClientMessage(playerid, COLOR_SERVER, "Names:{FFFFFF} location, spawn, vehicle(1-2), price(1-2)");
        return 1;
    }
    if((id < 0 || id >= MAX_RENTAL))
        return SendErrorMessage(playerid, "You have specified an invalid ID.");

	if(!RentData[id][rentExists])
        return SendErrorMessage(playerid, "You have specified an invalid ID.");

	if(!strcmp(type, "location", true))
	{
	    GetPlayerPos(playerid, RentData[id][rentPos][0], RentData[id][rentPos][1], RentData[id][rentPos][2]);
	    Rental_Save(id);
	    Rental_Refresh(id);
	    
	    SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah posisi Rental ID: %d", id);
	}
	else if(!strcmp(type, "vehicle1", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [vehicle1] [model]");
	        
		if(val < 400 || val > 611)
			return SendErrorMessage(playerid, "Vehicle Number can't be below 400 or above 611 !");

		RentData[id][rentModel][0] = val;
		Rental_Save(id);
		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Vehicle Model 1 Rental ID: %d", id);
	}
	else if(!strcmp(type, "vehicle2", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [vehicle2] [model]");

		if(val < 400 || val > 611)
			return SendErrorMessage(playerid, "Vehicle Number can't be below 400 or above 611 !");

		RentData[id][rentModel][1] = val;
		Rental_Save(id);
		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Vehicle Model 2 Rental ID: %d", id);
	}
	else if(!strcmp(type, "price1", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [price1] [price]");

		RentData[id][rentPrice][0] = val;
		Rental_Save(id);
		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Rental Price 1 Rental ID: %d", id);
	}
	else if(!strcmp(type, "price2", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [price2] [price]");

		RentData[id][rentPrice][1] = val;
		Rental_Save(id);
		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Rental Price 2 Rental ID: %d", id);
	}
	else if(!strcmp(type, "spawn", true))
	{
	    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	        return SendErrorMessage(playerid, "Kamu harus berada didalam kendaraan!");
	        
		GetVehiclePos(GetPlayerVehicleID(playerid), RentData[id][rentSpawn][0], RentData[id][rentSpawn][1], RentData[id][rentSpawn][2]);
		GetVehicleZAngle(GetPlayerVehicleID(playerid), RentData[id][rentSpawn][3]);
		
		SendClientMessageEx(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah posisi Spawn Rental ID: %d", id);
		Rental_Save(id);
	}
	return 1;
}
CMD:createrental(playerid, params[])
{
	new vehicle[2], id;

    if (PlayerData[playerid][pAdmin] < 6)
	    return SendErrorMessage(playerid, "You don't have permission to use this command.");

	if(sscanf(params, "dd", vehicle[0], vehicle[1]))
		return SendSyntaxMessage(playerid, "/createrental [Vehicle 1] [Vehicle 2]");
		
	id = Rental_Create(playerid, vehicle[0], vehicle[1]);
	
	if(id == -1)
	    return SendErrorMessage(playerid, "Kamu tidak bisa membuat lebih banyak Rental!");
	    
	SendServerMessage(playerid, "Kamu telah membuat Rental Point ID: %d", id);
	return 1;
}
CMD:createbiz(playerid, params[])
{
    new
		type,
	    price,
	    id;

    if (PlayerData[playerid][pAdmin] < 6)
	    return SendErrorMessage(playerid, "You don't have permission to use this command.");

	if (sscanf(params, "dd", type, price))
 	{
	 	SendSyntaxMessage(playerid, "/createbiz [type] [price]");
    	SendClientMessage(playerid, COLOR_SERVER, "Type:{FFFFFF} 1: Fast Food | 2: 24/7 | 3: Clothes | 4: Electronic");
    	return 1;
	}
	if (type < 1 || type > 4)
	    return SendErrorMessage(playerid, "Invalid type specified. Types range from 1 to 7.");

	id = Business_Create(playerid, type, price);

	if (id == -1)
	    return SendErrorMessage(playerid, "The server has reached the limit for businesses.");

	SendServerMessage(playerid, "You have successfully created business ID: %d.", id);
	return 1;
}
/* » Server Timer */

ptask EnergyUpdate[30000](playerid)
{
	if(PlayerData[playerid][pEnergy] > 0)
	{
	    PlayerData[playerid][pEnergy]--;
	}
	return 1;
}

task RentalUpdate[1000]()
{
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists] && VehicleData[i][vRental] != -1)
	{
	    if(VehicleData[i][vRentTime] > 0)
	    {
	        VehicleData[i][vRentTime]--;
	        if(VehicleData[i][vRentTime] <= 0)
	        {
	            foreach(new playerid : Player) if(VehicleData[i][vOwner] == PlayerData[playerid][pID])
	            {
	            	SendClientMessageEx(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Masa rental kendaraan %s telah habis, kendaraan otomatis dihilangkan.", GetVehicleModelName(VehicleData[i][vModel]));
				}
				Vehicle_Delete(i);
			}
		}
	}
	return 1;
}

task VehicleUpdate[50000]()
{
	forex(i, MAX_VEHICLES) if (IsEngineVehicle(i) && GetEngineStatus(i))
	{
	    if (GetFuel(i) > 0)
	    {
	        VehCore[i][vehFuel]--;
			if (GetFuel(i) <= 0)
			{
			    VehCore[i][vehFuel] = 0;
	      		SwitchVehicleEngine(i, false);
	      		GameTextForPlayer(GetVehicleDriver(i), "Vehicle out of ~r~Fuel!", 3000, 5);
			}
		}
	}
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists])
	{
		if(VehicleData[i][vInsuTime] != 0 && VehicleData[i][vInsuTime] <= gettime())
		{
			VehicleData[i][vInsuTime] = 0;
		}
	}
	return 1;
}
ptask PlayerUpdate[1000](playerid)
{
	if(PlayerData[playerid][pSpawned])
	{
		SetPlayerProgressBarValue(playerid, ENERGYBAR[playerid], PlayerData[playerid][pEnergy]);
		SetPlayerProgressBarColour(playerid, ENERGYBAR[playerid], ConvertHBEColor(PlayerData[playerid][pEnergy]));
		new vehicleid = GetPlayerVehicleID(playerid);
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
		    if(IsSpeedoVehicle(vehicleid))
		    {
		        new Float:vHP, vehname[64], speedtd[64], healthtd[64];
		        GetVehicleHealth(vehicleid, vHP);
		        format(healthtd, sizeof(healthtd), "%.1f", vHP);
		        PlayerTextDrawSetString(playerid, HEALTHTD[playerid], healthtd);

		        format(vehname, sizeof(vehname), "%s", GetVehicleName(vehicleid));
		        PlayerTextDrawSetString(playerid, VEHNAMETD[playerid], vehname);
		        
		        format(speedtd, sizeof(speedtd), "%iKM/H", GetVehicleSpeedKMH(vehicleid));
		        PlayerTextDrawSetString(playerid, KMHTD[playerid], speedtd);
		        
		        SetPlayerProgressBarValue(playerid, FUELBAR[playerid], VehCore[vehicleid][vehFuel]);
			}
		}
	}
	return 1;
}
