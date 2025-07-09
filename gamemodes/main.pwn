#if defined DONT_REMOVE

    » Gamemode Xyronite by Luminouz
    
    > Credits: - LuminouZ (For the main Gamemode Scripter)
               - Katzu (For porting the Gamemode from SA:MP to Open.MP)
    
    > NOTE: Please don't remove the credits!
    
    ===================== » Changelog ========================
    
    > Migrated entire gamemode from SA:MP to Open.MP (by Katzu)
    
#endif

/* »Includes */
#include <open.mp>
#undef MAX_PLAYERS
#define MAX_PLAYERS	100
#include <a_mysql>
#include <sscanf2>
#include <crashdetect>
#include <YSI_Data\y_foreach>
#include <Pawn.CMD>
#include <Pawn.RakNet>
#include <streamer>
#include <samp_bcrypt>
// #include <progress2>
#include <YSI_Coding\y_timers>
#include <easyDialog>

/* »Modulars */
#include "modules\core\define"
#include "modules\core\color"
#include "modules\core\macro"
#include "modules\database\core"
#include "modules\utils\ui\core"
#include "modules\core\data"
#include "modules\core\function"
#include "modules\players\core"
#include "modules\vehicles\core"
#include "modules\utils\timer"

/* »Enums */
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

stock Biz_GetCount(playerid)
{
	new count = 0;
	forex(i, MAX_BUSINESS) if(BizData[i][bizExists] && BizData[i][bizOwner] == pData[playerid][pID])
	{
	    count++;
	}
	return count;
}

stock StreamerConfig()
{
    Streamer_MaxItems(STREAMER_TYPE_OBJECT, 990000);
    Streamer_MaxItems(STREAMER_TYPE_MAP_ICON, 2000);
    Streamer_MaxItems(STREAMER_TYPE_PICKUP, 2000);

    SetTimer("DestroyStreamerItems", 100, false);

    Streamer_VisibleItems(STREAMER_TYPE_OBJECT, 1000);
    return 1;
}

forward DestroyStreamerItems();
public DestroyStreamerItems()
{
    for (new playerid = 0; playerid < GetMaxPlayers(); playerid++) 
    {
        Streamer_DestroyAllVisibleItems(playerid, 0);
    }
}


FUNC::OnPlayerUseItem(playerid, itemid, const name[])
{
	if(!strcmp(name, "Snack"))
	{
        if (pData[playerid][pHunger] > 90)
            return SendErrorMessage(playerid, "Energy milikmu sudah penuh.");

        pData[playerid][pHunger] += 10;
		Inventory_Remove(playerid, "Snack", 1);
		ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, SYNC_ALL);
        SendNearbyMessage(playerid, 30.0, COLOR_PURPLE, "* %s takes a snack and eats it.", ReturnName(playerid));
	}
	else if(!strcmp(name, "Water"))
	{
        if (pData[playerid][pHunger] > 90)
            return SendErrorMessage(playerid, "Energy milikmu sudah penuh.");

        pData[playerid][pHunger] += 10;
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
	format(string, sizeof(string), "DELETE FROM `inventory` WHERE `ID` = '%d'", pData[playerid][pID]);
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
	    format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, pData[playerid][pID], InventoryData[playerid][itemid][invID]);
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

		    format(string, sizeof(string), "DELETE FROM `inventory` WHERE `ID` = '%d' AND `invID` = '%d'", pData[playerid][pID], InventoryData[playerid][itemid][invID]);
	        mysql_tquery(sqlcon, string);
		}
		else if (quantity != -1 && InventoryData[playerid][itemid][invQuantity] > 0)
		{
			format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = `invQuantity` - %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, pData[playerid][pID], InventoryData[playerid][itemid][invID]);
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

			format(string, sizeof(string), "INSERT INTO `inventory` (`ID`, `invItem`, `invModel`, `invQuantity`) VALUES('%d', '%s', '%d', '%d')", pData[playerid][pID], item, model, quantity);
			mysql_tquery(sqlcon, string, "OnInventoryAdd", "dd", playerid, itemid);
	        return itemid;
		}
		return -1;
	}
	else
	{
	    format(string, sizeof(string), "UPDATE `inventory` SET `invQuantity` = `invQuantity` + %d WHERE `ID` = '%d' AND `invID` = '%d'", quantity, pData[playerid][pID], InventoryData[playerid][itemid][invID]);
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

	ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 4.1, false, false, false, false, 0, SYNC_ALL);
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
	    
	if(BizData[id][bizOwner] == pData[playerid][pID])
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

stock VehicleRental_Count(playerid)
{
	new count = 0;
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists] && VehicleData[i][vRental] != -1 && VehicleData[i][vOwner] == pData[playerid][pID])
	{
	    count++;
	}
	return count;
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
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), pData[playerid][pAttempt]);
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to LevelUP", str, "Login", "Exit");
	}
	else
	{
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Create Password: {FF00FF}(Input Below)", GetName(playerid), pData[playerid][pAttempt]);
		ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to LevelUP", str, "Register", "Exit");
	}
	return 1;
}

stock SetupPlayerData(playerid)
{
    SetSpawnInfo(playerid, NO_TEAM, pData[playerid][pSkin], 1642.1681, -2333.3689, 13.5469, 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
    SpawnPlayer(playerid);
    GiveMoney(playerid, 150);
    return 1;
}

FUNC::LoadCharacterData(playerid)
{
	cache_get_value_name_int(0, "pID", pData[playerid][pID]);
	cache_get_value_name(0, "Name", pData[playerid][pName]);
	cache_get_value_name_float(0, "PosX", pData[playerid][pPos][0]);
	cache_get_value_name_float(0, "PosY", pData[playerid][pPos][1]);
	cache_get_value_name_float(0, "PosZ", pData[playerid][pPos][2]);
	cache_get_value_name_float(0, "Health", pData[playerid][pHealth]);
	cache_get_value_name_int(0, "Interior", pData[playerid][pInterior]);
	cache_get_value_name_int(0, "World", pData[playerid][pWorld]);
	cache_get_value_name_int(0, "Age", pData[playerid][pAge]);
	cache_get_value_name(0, "Origin", pData[playerid][pOrigin]);
	cache_get_value_name_int(0, "Gender", pData[playerid][pGender]);
	cache_get_value_name_int(0, "Skin", pData[playerid][pSkin]);
	cache_get_value_name(0, "UCP", pData[playerid][pUCP]);
	cache_get_value_name_int(0, "Energy", pData[playerid][pHunger]);
	cache_get_value_name_int(0, "AdminLevel", pData[playerid][pAdmin]);
	cache_get_value_name_int(0, "InBiz", pData[playerid][pInBiz]);
	cache_get_value_name_int(0, "Money", pData[playerid][pMoney]);
	
	new invQuery[256];
    format(invQuery, sizeof(invQuery), "SELECT * FROM `inventory` WHERE `ID` = '%d'", pData[playerid][pID]);
	mysql_tquery(sqlcon, invQuery, "LoadPlayerItems", "d", playerid);
	
    SetSpawnInfo(playerid, NO_TEAM, pData[playerid][pSkin], pData[playerid][pPos][0], pData[playerid][pPos][1], pData[playerid][pPos][2], 0.0, WEAPON_FIST, 0, WEAPON_FIST, 0, WEAPON_FIST, 0);
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
    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), pData[playerid][pAttempt]);
    
	if(!success)
	{
	    if(pData[playerid][pAttempt] < 5)
	    {
		    pData[playerid][pAttempt]++;
	        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to LevelUP", str, "Login", "Exit");
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
		pData[playerid][pID] = cache_insert_id();
	 	cache_delete(execute);
	 	SetPlayerName(playerid, name);
		format(pData[playerid][pName], MAX_PLAYER_NAME, name);
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
	pData[playerid][pHunger] = 100;
	pData[playerid][pMoney] = 0;
	pData[playerid][pInBiz] = -1;
	pData[playerid][pListitem] = -1;
	pData[playerid][pAttempt] = 0;
	pData[playerid][pCalling] = INVALID_PLAYER_ID;
	pData[playerid][pSpawned] = false;
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
	print("[ LevelUP Gamemode Loaded ]");
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
	Database_GLoad();
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
			// FUELBAR[playerid] = CreatePlayerProgressBar(playerid, 520.000000, 433.000000, 110.000000, 7.000000, 9109759, 100.000000, BAR_DIRECTION_RIGHT);
		}
		if(pvid != -1 && VehicleData[pvid][vRental] != -1)
		{
		    GetElapsedTime(VehicleData[pvid][vRentTime], time[0], time[1], time[2]);
		    SendClientMessage(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Sisa rental {00FFFF}%s {FFFFFF}milikmu adalah {FFFF00}%02d jam %02d menit %02d detik", GetVehicleName(vehicleid), time[0], time[1], time[2]);
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
		// DestroyPlayerProgressBar(playerid, FUELBAR[playerid]);
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
	        pData[playerid][pListitem] = listitem;
	        format(str, sizeof(str), "{FFFFFF}Current Product Price: %s\n{FFFFFF}Silahkan masukan harga baru untuk product {00FFFF}%s", FormatNumber(BizData[pData[playerid][pInBiz]][bizProduct][listitem]), ProductName[pData[playerid][pInBiz]][listitem]);
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
	        pData[playerid][pListitem] = listitem;
	        format(str, sizeof(str), "{FFFFFF}Current Product Name: %s\n{FFFFFF}Silahkan masukan nama baru untuk product {00FFFF}%s", ProductName[pData[playerid][pInBiz]][listitem], ProductName[pData[playerid][pInBiz]][listitem]);
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

			new id = pData[playerid][pInBiz];
			new slot = pData[playerid][pListitem];
			SendClientMessage(playerid, COLOR_SERVER, "BIZ: {FFFFFF}Kamu telah mengubah nama product dari {00FFFF}%s {FFFFFF}menjadi {00FFFF}%s", ProductName[id][slot], inputtext);
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
	            
			new id = pData[playerid][pInBiz];
			new slot = pData[playerid][pListitem];
			SendClientMessage(playerid, COLOR_SERVER, "BIZ: {FFFFFF}Kamu telah mengubah harga product dari {009000}%s {FFFFFF}menjadi {009000}%s", FormatNumber(BizData[id][bizProduct][slot]), FormatNumber(strval(inputtext)));
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
				format(str, sizeof(str), "{FFFFFF}Current Biz Name: %s\n{FFFFFF}Silahkan masukan nama Business mu yang baru:\n\n{FFFFFF}Note: Max 24 Huruf!", BizData[pData[playerid][pInBiz]][bizName]);
				ShowPlayerDialog(playerid, DIALOG_BIZNAME, DIALOG_STYLE_INPUT, "Business Name", str, "Set", "Close");
			}
		}
	}
	if(dialogid == DIALOG_RENTAL)
	{
	    if(response)
	    {
	        new rentid = pData[playerid][pRenting];
	        if(GetMoney(playerid) < RentData[rentid][rentPrice][listitem])
	            return SendErrorMessage(playerid, "Kamu tidak memiliki cukup uang!");
	            
			new str[256];
			format(str, sizeof(str), "{FFFFFF}Berapa jam kamu ingin menggunakan kendaraan Rental ini ?\n{FFFFFF}Maksimal adalah {FFFF00}4 jam\n\n{FFFFFF}Harga per Jam: {009000}$%d", RentData[rentid][rentPrice][listitem]);
			ShowPlayerDialog(playerid, DIALOG_RENTTIME, DIALOG_STYLE_INPUT, "{FFFFFF}Rental Time", str, "Rental", "Close");
			pData[playerid][pListitem] = listitem;
		}
	}
	if(dialogid == DIALOG_RENTTIME)
	{
	    if(response)
	    {
	        new id = pData[playerid][pRenting];
	        new slot = pData[playerid][pListitem];
			new time = strval(inputtext);
			if(time < 1 || time > 4)
			{
				new str[256];
				format(str, sizeof(str), "{FFFFFF}Berapa jam kamu ingin menggunakan kendaraan Rental ini ?\n{FFFFFF}Maksimal adalah {FFFF00}4 jam\n\n{FFFFFF}Harga per Jam: {009000}$%d", RentData[id][rentPrice][listitem]);
				ShowPlayerDialog(playerid, DIALOG_RENTTIME, DIALOG_STYLE_INPUT, "{FFFFFF}Rental Time", str, "Rental", "Close");
				return 1;
			}
			GiveMoney(playerid, -RentData[id][rentPrice][slot] * time);
			SendClientMessage(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Kamu telah menyewa {00FFFF}%s {FFFFFF}untuk %d Jam seharga {009000}$%d", GetVehicleModelName(RentData[id][rentModel][slot]), time, RentData[id][rentPrice][slot] * time);
            VehicleRental_Create(pData[playerid][pID], RentData[id][rentModel][slot], RentData[id][rentSpawn][0], RentData[id][rentSpawn][1], RentData[id][rentSpawn][2], RentData[id][rentSpawn][3], time*3600, pData[playerid][pRenting]);
		}
	}
	if(dialogid == DIALOG_BUYSKINS)
	{
	    if(response)
	    {
	        GiveMoney(playerid, -pData[playerid][pSkinPrice]);
			SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(pData[playerid][pSkinPrice]), ProductName[pData[playerid][pInBiz]][0]);
			BizData[pData[playerid][pInBiz]][bizStock]--;
			if(pData[playerid][pGender] == 1)
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
			    itemid = pData[playerid][pListitem],
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

			itemid = pData[playerid][pListitem];

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
			    //Log_Write("logs/give_log.txt", "[%s] %s (%s) has given a %s to %s (%s).", ReturnDate(), ReturnName(playerid), pData[playerid][pIP], string, ReturnName(userid, 0), pData[userid][pIP]);
	  		}
			else
			{
				new str[152];
				format(str, sizeof(str), "Item: %s (Amount: %d)\n\nPlease enter the amount of this item you wish to give %s:", string, InventoryData[playerid][itemid][invQuantity], ReturnName(userid));
			    ShowPlayerDialog(playerid, DIALOG_GIVEAMOUNT, DIALOG_STYLE_INPUT, "Give Item", str, "Give", "Cancel");
			    pData[playerid][pTarget] = userid;
			}
		}
	}
	if(dialogid == DIALOG_GIVEAMOUNT)
	{
		if (response && pData[playerid][pTarget] != INVALID_PLAYER_ID)
		{
		    new
		        userid = pData[playerid][pTarget],
		        itemid = pData[playerid][pListitem],
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
		  //  Log_Write("logs/give_log.txt", "[%s] %s (%s) has given %d %s to %s (%s).", ReturnDate(), ReturnName(playerid), pData[playerid][pIP], strval(inputtext), string, ReturnName(userid, 0), pData[userid][pIP]);
		}
	}
	if(dialogid == DIALOG_INVACTION)
	{
	    if(response)
	    {
		    new
				itemid = pData[playerid][pListitem],
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
				        
					pData[playerid][pListitem] = itemid;
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
            pData[playerid][pListitem] = listitem;

			switch (pData[playerid][pStorageSelect])
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
	        new bid = pData[playerid][pInBiz], price, prodname[34];
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

							pData[playerid][pHunger] += 20;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 1)
						{
						    if(GetEnergy(playerid) >= 100)
						        return SendErrorMessage(playerid, "Your energy is already full!");

							pData[playerid][pHunger] += 40;
							SendNearbyMessage(playerid, 20.0, COLOR_PURPLE, "* %s has paid %s and purchased a %s.", ReturnName(playerid), FormatNumber(price), prodname);
							GiveMoney(playerid, -price);
							BizData[bid][bizStock]--;
						}
						if(listitem == 2)
						{
						    if(GetEnergy(playerid) >= 100)
						        return SendErrorMessage(playerid, "Your energy is already full!");

							pData[playerid][pHunger] += 15;
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
					    if(pData[playerid][pGender] == 1)
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
						        
							pData[playerid][pPhoneNumber] = pData[playerid][pID]+RandomEx(13158, 98942);
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
							pData[playerid][pCredit] += 50;
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
	    format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Create Password: {FF00FF}(Input Below)", GetName(playerid), pData[playerid][pAttempt]);

        if(strlen(inputtext) < 7)
			return ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to LevelUP", str, "Register", "Exit");

        if(strlen(inputtext) > 32)
			return ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register to LevelUP", str, "Register", "Exit");

        bcrypt_hash(playerid, "HashPlayerPassword", inputtext, BCRYPT_COST);
	}
	if(dialogid == DIALOG_LOGIN)
	{
	    if(!response)
	        return Kick(playerid);
	        
        if(strlen(inputtext) < 1)
        {
			new str[256];
            format(str, sizeof(str), "{FFFFFF}UCP Account: {00FFFF}%s\n{FFFFFF}Attempts: {00FFFF}%d/5\n{FFFFFF}Password: {FF00FF}(Input Below)", GetName(playerid), pData[playerid][pAttempt]);
            ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login to LevelUP", str, "Login", "Exit");
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

			pData[playerid][pChar] = listitem;
			SetPlayerName(playerid, PlayerChar[playerid][listitem]);

			new cQuery[256];
			mysql_format(sqlcon, cQuery, sizeof(cQuery), "SELECT * FROM `characters` WHERE `Name` = '%s' LIMIT 1;", PlayerChar[playerid][pData[playerid][pChar]]);
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

		    format(pData[playerid][pUCP], 22, GetName(playerid));
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

			pData[playerid][pAge] = strval(inputtext);
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

        format(pData[playerid][pOrigin], 32, inputtext);
        ShowPlayerDialog(playerid, DIALOG_GENDER, DIALOG_STYLE_LIST, "Character Gender", "Male\nFemale", "Continue", "Cancel");
	}
	if(dialogid == DIALOG_GENDER)
	{
	    if(!response)
	        return ShowPlayerDialog(playerid, DIALOG_GENDER, DIALOG_STYLE_LIST, "Character Gender", "Male\nFemale", "Continue", "Cancel");

		if(listitem == 0)
		{
			pData[playerid][pGender] = 1;
			pData[playerid][pSkin] = 240;
			pData[playerid][pHealth] = 100.0;
			SetupPlayerData(playerid);
		}
		if(listitem == 1)
		{
			pData[playerid][pGender] = 2;
			pData[playerid][pSkin] = 172;
			pData[playerid][pHealth] = 100.0;
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
	if(!pData[playerid][pSpawned])
	{
	    pData[playerid][pSpawned] = true;
	    GivePlayerMoney(playerid, pData[playerid][pMoney]);
	    SetPlayerHealth(playerid, pData[playerid][pHealth]);
	    SetPlayerSkin(playerid, pData[playerid][pSkin]);
	    SetPlayerVirtualWorld(playerid, pData[playerid][pWorld]);
		SetPlayerInterior(playerid, pData[playerid][pInterior]);
		
		// Hbe Textdraws
		forex(txd, 16)
		{
			TextDrawShowForPlayer(playerid, HbeTXD[txd]);
			PlayerTextDrawShow(playerid, HbeBarTXD[playerid][0]);
			PlayerTextDrawShow(playerid, HbeBarTXD[playerid][1]);
		}
	}
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if (result == -1)
    {
        SendErrorMessage(playerid, "Unknow Command! /help for more info.");
        return 0;
    }
	printf("[CMD]: %s(%d) has used the command '%s' (%s)", pData[playerid][pName], playerid, cmd, params);
    return 1;
}

public OnPlayerText(playerid, text[])
{
	if(pData[playerid][pCalling] != INVALID_PLAYER_ID)
	{
		new lstr[1024];
		format(lstr, sizeof(lstr), "(Phone) %s says: %s", ReturnName(playerid), text);
		ProxDetector(10, playerid, lstr, 0xE6E6E6E6, 0xC8C8C8C8, 0xAAAAAAAA, 0x8C8C8C8C, 0x6E6E6E6E);
		SetPlayerChatBubble(playerid, text, COLOR_WHITE, 10.0, 3000);

		SendClientMessage(pData[playerid][pCalling], COLOR_YELLOW, "(Phone) Caller says: %s", text);
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
					foreach(new pid : Player) if (VehicleData[i][vOwner] == pData[pid][pID])
	        		{
	            		SendServerMessage(pid, "Kendaraan {00FFFF}%s {FFFFFF}milikmu telah hancur, kamu bisa Claim setelah 24 jam dari Insurance.", GetVehicleName(vehicleid));
					}

					if(IsValidVehicle(VehicleData[i][vVehicle]))
						DestroyVehicle(VehicleData[i][vVehicle]);

					VehicleData[i][vVehicle] = INVALID_VEHICLE_ID;
				}
				else
				{
					foreach(new pid : Player) if (VehicleData[i][vOwner] == pData[pid][pID])
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
				foreach(new pid : Player) if (VehicleData[i][vOwner] == pData[pid][pID])
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
	new bid = pData[playerid][pInBiz], string[712];
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
	new bid = pData[playerid][pInBiz], string[712];
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
	new bid = pData[playerid][pInBiz], string[712];
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
				    
				BizData[i][bizOwner] = pData[playerid][pID];
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
		if(pData[playerid][pInBiz] != -1 && GetPlayerInterior(playerid) == BizData[pData[playerid][pInBiz]][bizInterior] && GetPlayerVirtualWorld(playerid) == BizData[pData[playerid][pInBiz]][bizWorld] && Biz_IsOwner(playerid, pData[playerid][pInBiz]))
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
	pData[playerid][pStorageSelect] = 0;
	OpenInventory(playerid);
	return 1;
}

CMD:makemeadmin(playerid, params[])
{
	pData[playerid][pAdmin] = 7;
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

				pData[playerid][pInBiz] = bid;
				SetPlayerPosEx(playerid, BizData[bid][bizInt][0], BizData[bid][bizInt][1], BizData[bid][bizInt][2]);

				SetPlayerInterior(playerid, BizData[bid][bizInterior]);
				SetPlayerVirtualWorld(playerid, BizData[bid][bizWorld]);
				SetCameraBehindPlayer(playerid);
				SetPlayerWeather(playerid, 0);
			}
	    }
		new inbiz = pData[playerid][pInBiz];
		if(pData[playerid][pInBiz] != -1 && IsPlayerInRangeOfPoint(playerid, 2.8, BizData[inbiz][bizInt][0], BizData[inbiz][bizInt][1], BizData[inbiz][bizInt][2]))
		{
			SetPlayerPos(playerid, BizData[inbiz][bizExt][0], BizData[inbiz][bizExt][1], BizData[inbiz][bizExt][2]);

			pData[playerid][pInBiz] = -1;
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
			SetCameraBehindPlayer(playerid);
		}
	}
	return 1;
}

CMD:buy(playerid, params[])
{
	if(pData[playerid][pInBiz] != -1 && GetPlayerInterior(playerid) == BizData[pData[playerid][pInBiz]][bizInterior] && GetPlayerVirtualWorld(playerid) == BizData[pData[playerid][pInBiz]][bizWorld])
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

	if (pData[playerid][pAdmin] < 6)
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
    
    if (model < 400 || model > 611)
        return SendServerMessage(playerid, "Error: Invalid vehicle model.");

    new Float:pos[4];
    GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
    GetPlayerFacingAngle(playerid, pos[3]);

    Vehicle_Create(pData[playerid][pID], model, pos[0], pos[1], pos[2], pos[3], 6, 6);
    SendServerMessage(playerid, "Vehicle created!");
    return 1;
}

CMD:gotoco(playerid, params[])
{
	if(pData[playerid][pAdmin] >= 2)
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

	vehicleid = CreateVehicle(model[0], x, y + 2, z, a, color1, color2, 0, false);

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
			SendClientMessage(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Kamu telah mengembalikan %s Rental milikmu!", GetVehicleName(vehicleid));
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
			pData[playerid][pRenting] = i;
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
    if(pData[playerid][pAdmin] < 1)
        return SendErrorMessage(playerid, "You don't have permission to use this command!");
        
	if(!pData[playerid][pAduty])
	{
	    pData[playerid][pAduty] = true;
	    SetPlayerColor(playerid, 0xFF0000FF);
	    SetPlayerName(playerid, pData[playerid][pUCP]);
		SendServerMessage(playerid, "You are now onduty as %s", pData[playerid][pUCP]);
	}
	else
	{
	    pData[playerid][pAduty] = false;
	    SetPlayerColor(playerid, COLOR_WHITE);
	    SetPlayerName(playerid, pData[playerid][pName]);
		SendServerMessage(playerid, "You are now off duty and your name has been changed to %s", pData[playerid][pName]);
	}
	return 1;
}
CMD:editbiz(playerid, params[])
{
    new
        id,
        type[24],
        string[128];

    if(pData[playerid][pAdmin] < 6)
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

		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmBiz: {FFFFFF}Kamu telah mengubah posisi Business ID: %d", id);
    }
    return 1;
}

CMD:editrental(playerid, params[])
{
    new
        id,
        type[24],
        string[128];

    if(pData[playerid][pAdmin] < 6)
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
	    
	    SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah posisi Rental ID: %d", id);
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
		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Vehicle Model 1 Rental ID: %d", id);
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
		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Vehicle Model 2 Rental ID: %d", id);
	}
	else if(!strcmp(type, "price1", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [price1] [price]");

		RentData[id][rentPrice][0] = val;
		Rental_Save(id);
		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Rental Price 1 Rental ID: %d", id);
	}
	else if(!strcmp(type, "price2", true))
	{
	    new val;
	    if(sscanf(string, "d", val))
	        return SendSyntaxMessage(playerid, "/editrental [price2] [price]");

		RentData[id][rentPrice][1] = val;
		Rental_Save(id);
		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah Rental Price 2 Rental ID: %d", id);
	}
	else if(!strcmp(type, "spawn", true))
	{
	    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	        return SendErrorMessage(playerid, "Kamu harus berada didalam kendaraan!");
	        
		GetVehiclePos(GetPlayerVehicleID(playerid), RentData[id][rentSpawn][0], RentData[id][rentSpawn][1], RentData[id][rentSpawn][2]);
		GetVehicleZAngle(GetPlayerVehicleID(playerid), RentData[id][rentSpawn][3]);
		
		SendClientMessage(playerid, COLOR_LIGHTRED, "AdmRental: {FFFFFF}Kamu telah mengubah posisi Spawn Rental ID: %d", id);
		Rental_Save(id);
	}
	return 1;
}
CMD:createrental(playerid, params[])
{
    new vehicle[2], id;

    if (pData[playerid][pAdmin] < 6)
        return SendErrorMessage(playerid, "You don't have permission to use this command.");

    if (sscanf(params, "dd", vehicle[0], vehicle[1]))
        return SendSyntaxMessage(playerid, "/createrental [Vehicle 1] [Vehicle 2]");

    id = Rental_Create(playerid, vehicle[0], vehicle[1]);

    if (id == -1)
        return SendErrorMessage(playerid, "Kamu tidak bisa membuat lebih banyak Rental!");

    SendServerMessage(playerid, "Kamu telah membuat Rental Point ID: %d", id);
    return 1;
}
CMD:createbiz(playerid, params[])
{
    new type,
        price,
        id;

    if (pData[playerid][pAdmin] < 6)
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