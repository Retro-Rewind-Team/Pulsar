#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <Config.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {

namespace Settings {

u8 Params::radioCount[Params::pageCount] ={
    3, 5, 4, 5, 2, 4, 5, 4 //menu, race, host, OTT, KO, RR1, RR2, RR3
    //Add user radio count here

};
u8 Params::scrollerCount[Params::pageCount] ={ 1, 1, 1, 0, 2, 0, 0, 2 }; //menu, race, host, OTT, KO, RR1, RR2, RR3

u8 Params::buttonsPerPagePerRow[Params::pageCount][Params::maxRadioCount] = //first row is PulsarSettingsType, 2nd is rowIdx of radio
{
    { 2, 2, 3, 0, 0, 0 }, //Menu 
    { 2, 2, 2, 2, 3, 0 }, //Race
    { 2, 4, 2, 3, 0, 0 }, //Host
    { 2, 2, 2, 2, 2, 0 }, //OTT
    { 2, 2, 0, 0, 0, 0 }, //KO
    { 4, 2, 2, 2, 0, 0 }, //RR1
    { 2, 2, 2, 2, 2, 0 }, //RR2
    { 3, 4, 2, 2, 0, 0 }, //RR3
};

u8 Params::optionsPerPagePerScroller[Params::pageCount][Params::maxScrollerCount] =
{
    { 5, 0, 0, 0, 0}, //Menu 
    { 4, 0, 0, 0, 0}, //Race
    { 7, 0, 0, 0, 0}, //Host
    { 0, 0, 0, 0, 0}, //OTT
    { 4, 4, 0, 0, 0}, //KO
    { 3, 0, 0, 0, 0}, //RR1
    { 0, 0, 0, 0, 0}, //RR2
    { 3, 4, 0, 0, 0}, //RR3
};

}//namespace Settings
}//namespace Pulsar



