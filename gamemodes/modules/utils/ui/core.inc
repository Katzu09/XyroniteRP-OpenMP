#include <YSI_Coding\y_hooks>

// Variable
new Text: HbeTXD[16];
new PlayerText: HbeBarTXD[MAX_PLAYERS][2];

new PlayerText:SPEEDOTD[MAX_PLAYERS][4];
// new PlayerBar:FUELBAR[MAX_PLAYERS];
new PlayerText:HEALTHTD[MAX_PLAYERS];
new PlayerText:KMHTD[MAX_PLAYERS];
new PlayerText:VEHNAMETD[MAX_PLAYERS];
new PlayerText:MSGTD[MAX_PLAYERS];

//imports
#include "hbe"

//hooks OnPlayerConnect
hook OnPlayerConnect(playerid)
{
    CreateHbeTD(playerid);
    return 1;
}