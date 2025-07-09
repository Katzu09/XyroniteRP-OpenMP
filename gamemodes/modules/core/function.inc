stock GetElapsedTime(time, &hours, &minutes, &seconds)
{
    hours = 0;
    minutes = 0;
    // seconds = 0;

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
    new validtime = time * 1000;

    PlayerTextDrawSetString(playerid, MSGTD[playerid], string);
    PlayerTextDrawShow(playerid, MSGTD[playerid]);
    SetTimerEx("HideMessage", validtime, false, "d", playerid);
    return 1;
}

FUNC::HideMessage(playerid)
{
    return PlayerTextDrawHide(playerid, MSGTD[playerid]);
}

stock RandomEx(min, max)
{
    new rand = random(max - min) + min;
    return rand;
}

stock ConvertHBEColor(value)
{
    new color;
    if (value >= 90 && value <= 100)
        color = 0x15a014FF;
    else if (value >= 80 && value < 90)
        color = 0x1b9913FF;
    else if (value >= 70 && value < 80)
        color = 0x1a7f08FF;
    else if (value >= 60 && value < 70)
        color = 0x326305FF;
    else if (value >= 50 && value < 60)
        color = 0x375d04FF;
    else if (value >= 40 && value < 50)
        color = 0x603304FF;
    else if (value >= 30 && value < 40)
        color = 0xd72800FF;
    else if (value >= 10 && value < 30)
        color = 0xfb3508FF;
    else if (value >= 0 && value < 10)
        color = 0xFF0000FF;
    else
        color = 0x15a014FF;

    return color;
}

stock CreatePlayerHUD(playerid)
{
    /* Speedometer */
    SPEEDOTD[playerid][0] = CreatePlayerTextDraw(playerid, 572.000000, 372.000000, "_");
    PlayerTextDrawFont(playerid, SPEEDOTD[playerid][0], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][0], 0.600000, 8.300003);
    PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][0], 298.500000, 135.000000);
    PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][0], 1);
    PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][0], 0);
    PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][0], TEXT_DRAW_ALIGN_CENTRE);
    PlayerTextDrawColour(playerid, SPEEDOTD[playerid][0], -1);
    PlayerTextDrawBackgroundColour(playerid, SPEEDOTD[playerid][0], 255);
    PlayerTextDrawBoxColour(playerid, SPEEDOTD[playerid][0], 135);
    PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][0], false);
    PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][0], true);
    PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][0], false);

    SPEEDOTD[playerid][1] = CreatePlayerTextDraw(playerid, 519.000000, 412.000000, "FUEL:");
    PlayerTextDrawFont(playerid, SPEEDOTD[playerid][1], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][1], 0.287500, 1.350000);
    PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][1], 400.000000, 17.000000);
    PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][1], 0);
    PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][1], 0);
    PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][1], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, SPEEDOTD[playerid][1], -1061109505);
    PlayerTextDrawBackgroundColour(playerid, SPEEDOTD[playerid][1], 255);
    PlayerTextDrawBoxColour(playerid, SPEEDOTD[playerid][1], 50);
    PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][1], false);
    PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][1], true);
    PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][1], false);

    SPEEDOTD[playerid][2] = CreatePlayerTextDraw(playerid, 519.000000, 396.000000, "HEALTH:");
    PlayerTextDrawFont(playerid, SPEEDOTD[playerid][2], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, SPEEDOTD[playerid][2], 0.287500, 1.350000);
    PlayerTextDrawTextSize(playerid, SPEEDOTD[playerid][2], 400.000000, 17.000000);
    PlayerTextDrawSetOutline(playerid, SPEEDOTD[playerid][2], 0);
    PlayerTextDrawSetShadow(playerid, SPEEDOTD[playerid][2], 0);
    PlayerTextDrawAlignment(playerid, SPEEDOTD[playerid][2], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, SPEEDOTD[playerid][2], -1061109505);
    PlayerTextDrawBackgroundColour(playerid, SPEEDOTD[playerid][2], 255);
    PlayerTextDrawBoxColour(playerid, SPEEDOTD[playerid][2], 50);
    PlayerTextDrawUseBox(playerid, SPEEDOTD[playerid][2], false);
    PlayerTextDrawSetProportional(playerid, SPEEDOTD[playerid][2], true);
    PlayerTextDrawSetSelectable(playerid, SPEEDOTD[playerid][2], false);

    HEALTHTD[playerid] = CreatePlayerTextDraw(playerid, 572.000000, 396.000000, "--");
    PlayerTextDrawFont(playerid, HEALTHTD[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, HEALTHTD[playerid], 0.287500, 1.350000);
    PlayerTextDrawTextSize(playerid, HEALTHTD[playerid], 400.000000, 17.000000);
    PlayerTextDrawSetOutline(playerid, HEALTHTD[playerid], 0);
    PlayerTextDrawSetShadow(playerid, HEALTHTD[playerid], 0);
    PlayerTextDrawAlignment(playerid, HEALTHTD[playerid], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, HEALTHTD[playerid], -1061109505);
    PlayerTextDrawBackgroundColour(playerid, HEALTHTD[playerid], 255);
    PlayerTextDrawBoxColour(playerid, HEALTHTD[playerid], 50);
    PlayerTextDrawUseBox(playerid, HEALTHTD[playerid], false);
    PlayerTextDrawSetProportional(playerid, HEALTHTD[playerid], true);
    PlayerTextDrawSetSelectable(playerid, HEALTHTD[playerid], false);

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
    static value[32],
           length;

    format(value, sizeof(value), "%d", (number < 0) ? (-number) : (number));

    if ((length = strlen(value)) > 3)
    {
        for (new i = length, l = 0; --i >= 0; l ++)
        {
            if ((l > 0) && (l % 3 == 0)) strins(value, ",", i + 1);
        }
    }
    if (prefix[0] != 0)
        strins(value, prefix, 0);

    if (number < 0)
        strins(value, "-", 0);

    return value;
}

stock ShowText(playerid, const text[], time)
{
    new total = time * 1000;
    new str[256];
    format(str, sizeof(str), "%s", text);
    GameTextForPlayer(playerid, str, total, 5);
    return 1;
}