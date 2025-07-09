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
	forex(i, MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists] && VehicleData[i][vOwner] == pData[playerid][pID])
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

	temp_speed = floatsqroot(((speed_x*speed_x) + (speed_y*speed_y)) + (speed_z*speed_z)) * 136.666667;

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
	if(pData[playerid][pID] == -1)
		return 0;

	if(VehicleData[carid][vExists] && VehicleData[carid][vOwner] == pData[playerid][pID])
		return 1;

	return 0;
}

stock Vehicle_HaveAccess(playerid, carid)
{
	if(pData[playerid][pID] == -1)
		return 0;

	if(VehicleData[carid][vExists] && VehicleData[carid][vOwner] == pData[playerid][pID] || pData[playerid][pVehKey] == VehicleData[carid][vID])
		return 1;

	return 0;
}
FUNC::UnloadPlayerVehicle(playerid)
{
 	forex(i,MAX_PLAYER_VEHICLE) if(VehicleData[i][vExists])
	{
		if(VehicleData[i][vOwner] == pData[playerid][pID])
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
	mysql_format(sqlcon, query, sizeof(query), "SELECT * FROM `vehicle` WHERE `vehOwner` = %d", pData[playerid][pID]);
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

stock GetVehicleModelName(modelid)
{
	new vehname[20];

	if(modelid >= 400 && modelid <= 611) format(vehname, 20,  g_arrVehicleNames[modelid - 400]);

	return vehname;
}

stock GetVehicleName(vehicleid)
{
    return GetVehicleModelName(GetVehicleModel(vehicleid));
}

stock SwitchVehicleLight(vehicleid,bool:light_status){
	new engine,lights,alarm,doors,bonnet,boot,objective;
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	SetVehicleParamsEx(vehicleid,engine,_:light_status,alarm,doors,bonnet,boot,objective);
}

stock SwitchVehicleEngine(vehicleid,bool:engine_status){
	new engine,lights,alarm,doors,bonnet,boot,objective;
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	SetVehicleParamsEx(vehicleid,_:engine_status,lights,alarm,doors,bonnet,boot,objective);
}

stock SwitchVehicleDoors(vehicleid,bool:doors_status){
	new engine,lights,alarm,doors,bonnet,boot,objective;
	GetVehicleParamsEx(vehicleid,engine,lights,alarm,doors,bonnet,boot,objective);
	SetVehicleParamsEx(vehicleid,engine,lights,alarm,_:doors_status,bonnet,boot,objective);
}