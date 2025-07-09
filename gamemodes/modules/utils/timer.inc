ptask EnergyUpdate[30000](playerid)
{
	if(pData[playerid][pHunger] > 0)
	{  
		pData[playerid][pHunger]--;
	}
	return 1;
}

task RentalUpdate[1000]()
{
    forex(i, MAX_PLAYER_VEHICLE) if (VehicleData[i][vExists] && VehicleData[i][vRental] != -1)
    {
        if (VehicleData[i][vRentTime] > 0)
        {
            VehicleData[i][vRentTime]--;
            if (VehicleData[i][vRentTime] <= 0)
            {
                foreach(new playerid : Player) if (VehicleData[i][vOwner] == pData[playerid][pID])
                {
                    SendClientMessage(playerid, COLOR_SERVER, "RENTAL: {FFFFFF}Masa rental kendaraan %s telah habis, kendaraan otomatis dihilangkan.", GetVehicleModelName(VehicleData[i][vModel]));
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
    forex(i, MAX_PLAYER_VEHICLE) if (VehicleData[i][vExists])
    {
        if (VehicleData[i][vInsuTime] != 0 && VehicleData[i][vInsuTime] <= gettime())
        {
            VehicleData[i][vInsuTime] = 0;
        }
    }
    return 1;
}
ptask PlayerUpdate[1000](playerid)
{
    if (pData[playerid][pSpawned])
    {
        PlayerHbeUpdate(playerid);
        new vehicleid = GetPlayerVehicleID(playerid);
        if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
        {
            if (IsSpeedoVehicle(vehicleid))
            {
                new Float:vHP, vehname[64], speedtd[64], healthtd[64];
                GetVehicleHealth(vehicleid, vHP);
                format(healthtd, sizeof(healthtd), "%.1f", vHP);
                PlayerTextDrawSetString(playerid, HEALTHTD[playerid], healthtd);

                format(vehname, sizeof(vehname), "%s", GetVehicleName(vehicleid));
                PlayerTextDrawSetString(playerid, VEHNAMETD[playerid], vehname);

                format(speedtd, sizeof(speedtd), "%iKM/H", GetVehicleSpeedKMH(vehicleid));
                PlayerTextDrawSetString(playerid, KMHTD[playerid], speedtd);

                // SetPlayerProgressBarValue(playerid, FUELBAR[playerid], VehCore[vehicleid][vehFuel]);
            }
        }
    }
    return 1;
}