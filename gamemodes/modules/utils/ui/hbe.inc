CreateHbeTD(playerid)
{
    HbeTXD[0] = TextDrawCreate(598.000, 354.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[0], 40.000, 23.000);
    TextDrawAlignment(HbeTXD[0], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[0], 255);
    TextDrawSetShadow(HbeTXD[0], 0);
    TextDrawSetOutline(HbeTXD[0], 0);
    TextDrawBackgroundColour(HbeTXD[0], 255);
    TextDrawFont(HbeTXD[0], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[0], true);

    HbeTXD[1] = TextDrawCreate(582.000, 348.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[1], 28.000, 35.000);
    TextDrawAlignment(HbeTXD[1], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[1], 255);
    TextDrawSetShadow(HbeTXD[1], 0);
    TextDrawSetOutline(HbeTXD[1], 0);
    TextDrawBackgroundColour(HbeTXD[1], 255);
    TextDrawFont(HbeTXD[1], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[1], true);

    HbeTXD[2] = TextDrawCreate(584.000, 350.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[2], 24.000, 31.000);
    TextDrawAlignment(HbeTXD[2], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[2], -1);
    TextDrawSetShadow(HbeTXD[2], 0);
    TextDrawSetOutline(HbeTXD[2], 0);
    TextDrawBackgroundColour(HbeTXD[2], 255);
    TextDrawFont(HbeTXD[2], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[2], true);

    HbeTXD[3] = TextDrawCreate(585.000, 351.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[3], 22.000, 29.000);
    TextDrawAlignment(HbeTXD[3], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[3], 255);
    TextDrawSetShadow(HbeTXD[3], 0);
    TextDrawSetOutline(HbeTXD[3], 0);
    TextDrawBackgroundColour(HbeTXD[3], 255);
    TextDrawFont(HbeTXD[3], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[3], true);

    HbeTXD[4] = TextDrawCreate(591.000, 358.000, "HUD:radar_burgerShot");
    TextDrawTextSize(HbeTXD[4], 10.000, 14.000);
    TextDrawAlignment(HbeTXD[4], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[4], -1);
    TextDrawSetShadow(HbeTXD[4], 0);
    TextDrawSetOutline(HbeTXD[4], 0);
    TextDrawBackgroundColour(HbeTXD[4], 255);
    TextDrawFont(HbeTXD[4], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[4], true);

    HbeTXD[5] = TextDrawCreate(606.000, 353.000, "HUNGER");
    TextDrawLetterSize(HbeTXD[5], 0.230, 1.099);
    TextDrawAlignment(HbeTXD[5], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[5], -1);
    TextDrawSetShadow(HbeTXD[5], 0);
    TextDrawSetOutline(HbeTXD[5], 0);
    TextDrawBackgroundColour(HbeTXD[5], 150);
    TextDrawFont(HbeTXD[5], TEXT_DRAW_FONT_1);
    TextDrawSetProportional(HbeTXD[5], true);

    HbeTXD[6] = TextDrawCreate(606.000, 369.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[6], 30.000, 6.000);
    TextDrawAlignment(HbeTXD[6], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[6], 1768516095);
    TextDrawSetShadow(HbeTXD[6], 0);
    TextDrawSetOutline(HbeTXD[6], 0);
    TextDrawBackgroundColour(HbeTXD[6], 1768516095);
    TextDrawFont(HbeTXD[6], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[6], true);

    HbeTXD[7] = TextDrawCreate(606.000, 363.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[7], 30.000, 1.000);
    TextDrawAlignment(HbeTXD[7], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[7], -7601921);
    TextDrawSetShadow(HbeTXD[7], 0);
    TextDrawSetOutline(HbeTXD[7], 0);
    TextDrawBackgroundColour(HbeTXD[7], 1768516095);
    TextDrawFont(HbeTXD[7], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[7], true);

    HbeTXD[8] = TextDrawCreate(598.000, 380.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[8], 40.000, 23.000);
    TextDrawAlignment(HbeTXD[8], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[8], 255);
    TextDrawSetShadow(HbeTXD[8], 0);
    TextDrawSetOutline(HbeTXD[8], 0);
    TextDrawBackgroundColour(HbeTXD[8], 255);
    TextDrawFont(HbeTXD[8], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[8], true);

    HbeTXD[9] = TextDrawCreate(582.000, 374.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[9], 28.000, 35.000);
    TextDrawAlignment(HbeTXD[9], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[9], 255);
    TextDrawSetShadow(HbeTXD[9], 0);
    TextDrawSetOutline(HbeTXD[9], 0);
    TextDrawBackgroundColour(HbeTXD[9], 255);
    TextDrawFont(HbeTXD[9], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[9], true);

    HbeTXD[10] = TextDrawCreate(584.000, 376.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[10], 24.000, 31.000);
    TextDrawAlignment(HbeTXD[10], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[10], -1);
    TextDrawSetShadow(HbeTXD[10], 0);
    TextDrawSetOutline(HbeTXD[10], 0);
    TextDrawBackgroundColour(HbeTXD[10], 255);
    TextDrawFont(HbeTXD[10], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[10], true);

    HbeTXD[11] = TextDrawCreate(585.000, 377.000, "LD_BEAT:chit");
    TextDrawTextSize(HbeTXD[11], 22.000, 29.000);
    TextDrawAlignment(HbeTXD[11], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[11], 255);
    TextDrawSetShadow(HbeTXD[11], 0);
    TextDrawSetOutline(HbeTXD[11], 0);
    TextDrawBackgroundColour(HbeTXD[11], 255);
    TextDrawFont(HbeTXD[11], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[11], true);

    HbeTXD[12] = TextDrawCreate(591.000, 384.000, "HUD:radar_diner");
    TextDrawTextSize(HbeTXD[12], 11.000, 15.000);
    TextDrawAlignment(HbeTXD[12], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[12], -1);
    TextDrawSetShadow(HbeTXD[12], 0);
    TextDrawSetOutline(HbeTXD[12], 0);
    TextDrawBackgroundColour(HbeTXD[12], 255);
    TextDrawFont(HbeTXD[12], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[12], true);

    HbeTXD[13] = TextDrawCreate(606.000, 379.000, "THIRST");
    TextDrawLetterSize(HbeTXD[13], 0.230, 1.099);
    TextDrawAlignment(HbeTXD[13], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[13], -1);
    TextDrawSetShadow(HbeTXD[13], 0);
    TextDrawSetOutline(HbeTXD[13], 0);
    TextDrawBackgroundColour(HbeTXD[13], 150);
    TextDrawFont(HbeTXD[13], TEXT_DRAW_FONT_1);
    TextDrawSetProportional(HbeTXD[13], true);

    HbeTXD[14] = TextDrawCreate(606.000, 395.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[14], 30.000, 6.000);
    TextDrawAlignment(HbeTXD[14], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[14], 1768516095);
    TextDrawSetShadow(HbeTXD[14], 0);
    TextDrawSetOutline(HbeTXD[14], 0);
    TextDrawBackgroundColour(HbeTXD[14], 1768516095);
    TextDrawFont(HbeTXD[14], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[14], true);

    HbeTXD[15] = TextDrawCreate(606.000, 389.000, "LD_BUM:blkdot");
    TextDrawTextSize(HbeTXD[15], 30.000, 1.000);
    TextDrawAlignment(HbeTXD[15], TEXT_DRAW_ALIGN_LEFT);
    TextDrawColour(HbeTXD[15], 13554175);
    TextDrawSetShadow(HbeTXD[15], 0);
    TextDrawSetOutline(HbeTXD[15], 0);
    TextDrawBackgroundColour(HbeTXD[15], 1768516095);
    TextDrawFont(HbeTXD[15], TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawSetProportional(HbeTXD[15], true);

    ///////////////////////////////////////////////////////

    HbeBarTXD[playerid][0] = CreatePlayerTextDraw(playerid, 606.000, 369.000, "LD_BUM:blkdot");
    PlayerTextDrawTextSize(playerid, HbeBarTXD[playerid][0], 30.000, 6.000);
    PlayerTextDrawAlignment(playerid, HbeBarTXD[playerid][0], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, HbeBarTXD[playerid][0], -5963521);
    PlayerTextDrawSetShadow(playerid, HbeBarTXD[playerid][0], 0);
    PlayerTextDrawSetOutline(playerid, HbeBarTXD[playerid][0], 0);
    PlayerTextDrawBackgroundColour(playerid, HbeBarTXD[playerid][0], 1768516095);
    PlayerTextDrawFont(playerid, HbeBarTXD[playerid][0], TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawSetProportional(playerid, HbeBarTXD[playerid][0], true);

    HbeBarTXD[playerid][1] = CreatePlayerTextDraw(playerid, 606.000, 395.000, "LD_BUM:blkdot");
    PlayerTextDrawTextSize(playerid, HbeBarTXD[playerid][1], 15.000, 6.000);
    PlayerTextDrawAlignment(playerid, HbeBarTXD[playerid][1], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColour(playerid, HbeBarTXD[playerid][1], 548580095);
    PlayerTextDrawSetShadow(playerid, HbeBarTXD[playerid][1], 0);
    PlayerTextDrawSetOutline(playerid, HbeBarTXD[playerid][1], 0);
    PlayerTextDrawBackgroundColour(playerid, HbeBarTXD[playerid][1], 1768516095);
    PlayerTextDrawFont(playerid, HbeBarTXD[playerid][1], TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawSetProportional(playerid, HbeBarTXD[playerid][1], true);
}