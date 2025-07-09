stock UpdatePlayerSkin(playerid, skinid)
{
    SetPlayerSkin(playerid, skinid);
    pData[playerid][pSkin] = skinid;
}

PlayerHbeUpdate(playerid)
{
    if (pData[playerid][pSpawned])
    {
        new Float:hunger;
        hunger = pData[playerid][pHunger] * 30.0 / 100;
        PlayerTextDrawTextSize(playerid, HbeBarTXD[playerid][0], hunger, 6.0);
        PlayerTextDrawShow(playerid, HbeBarTXD[playerid][0]);
    }
    return 1;
}

stock GetEnergy(playerid)
return pData[playerid][pHunger];

GiveMoney(playerid, amount)
{
    pData[playerid][pMoney] += amount;
    GivePlayerMoney(playerid, amount);
    return 1;
}

stock GetMoney(playerid)
{
    return pData[playerid][pMoney];
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

ReturnName(playerid)
{
    static name[MAX_PLAYER_NAME + 1];

    GetPlayerName(playerid, name, sizeof(name));
    if (pData[playerid][pMaskOn])
    {
        format(name, sizeof(name), "Mask_#%d", pData[playerid][pMaskID]);
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
    GetPlayerName(playerid, name, sizeof(name));
    return name;
}

stock IsRoleplayName(const player[])
{
    forex(n, strlen(player))
    {
        if (player[n] == '_' && player[n + 1] >= 'A' && player[n + 1] <= 'Z') return 1;
        if (player[n] == ']' || player[n] == '[') return 0;
    }
    return 0;
}

stock IsPlayerNearPlayer(playerid, targetid, Float:radius)
{
    static Float:fX,
           Float:fY,
           Float:fZ;

    GetPlayerPos(targetid, fX, fY, fZ);

    return (GetPlayerInterior(playerid) == GetPlayerInterior(targetid) && GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(targetid)) && IsPlayerInRangeOfPoint(playerid, radius, fX, fY, fZ);
}

stock SendNearbyMessage(playerid, Float:radius, color, const str[], {Float, _} : ...)
{
    static args,
           start,
           end,
           string[144];
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
            if (IsPlayerNearPlayer(i, playerid, radius) && pData[i][pSpawned])
            {
                SendClientMessage(i, color, string);
            }
        }
        return 1;
    }
    foreach (new i : Player)
    {
        if (IsPlayerNearPlayer(i, playerid, radius) && pData[i][pSpawned])
        {
            SendClientMessage(i, color, str);
        }
    }
    return 1;
}

stock SaveData(playerid)
{
    new query[2512];
    if (pData[playerid][pSpawned])
    {
        GetPlayerHealth(playerid, pData[playerid][pHealth]);
        GetPlayerPos(playerid, pData[playerid][pPos][0], pData[playerid][pPos][1], pData[playerid][pPos][2]);

        mysql_format(sqlcon, query, sizeof(query), "UPDATE `characters` SET ");
        mysql_format(sqlcon, query, sizeof(query), "%s`PosX`='%f', ", query, pData[playerid][pPos][0]);
        mysql_format(sqlcon, query, sizeof(query), "%s`PosY`='%f', ", query, pData[playerid][pPos][1]);
        mysql_format(sqlcon, query, sizeof(query), "%s`PosZ`='%f', ", query, pData[playerid][pPos][2]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Health`='%f', ", query, pData[playerid][pHealth]);
        mysql_format(sqlcon, query, sizeof(query), "%s`World`='%d', ", query, GetPlayerVirtualWorld(playerid));
        mysql_format(sqlcon, query, sizeof(query), "%s`Interior`='%d', ", query, GetPlayerInterior(playerid));
        mysql_format(sqlcon, query, sizeof(query), "%s`Age`='%d', ", query, pData[playerid][pAge]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Origin`='%s', ", query, pData[playerid][pOrigin]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Gender`='%d', ", query, pData[playerid][pGender]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Skin`='%d', ", query, pData[playerid][pSkin]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Energy`='%d', ", query, pData[playerid][pHunger]);
        mysql_format(sqlcon, query, sizeof(query), "%s`AdminLevel`='%d', ", query, pData[playerid][pAdmin]);
        mysql_format(sqlcon, query, sizeof(query), "%s`InBiz`='%d', ", query, pData[playerid][pInBiz]);
        mysql_format(sqlcon, query, sizeof(query), "%s`Money`='%d', ", query, pData[playerid][pMoney]);
        mysql_format(sqlcon, query, sizeof(query), "%s`UCP`='%s' ", query, pData[playerid][pUCP]);
        mysql_format(sqlcon, query, sizeof(query), "%sWHERE `pID` = %d", query, pData[playerid][pID]);
        mysql_query(sqlcon, query, true);
    }
    return 1;
}