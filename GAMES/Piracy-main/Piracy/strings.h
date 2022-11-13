//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

//#define _T(name,txt) t_txt name_##name [] PROGMEM = (txt)
#define _T(name,txt) t_txt name_##name [] = (txt)


namespace strings
{
   
    typedef const char t_txt;
    typedef t_txt* const t_pointers[];
    
    typedef struct {
        uint8_t line_count;
        uint8_t line_width;
    } t_textsize;


    
    // Countries
    
    _T(cntr1,"SIMERA"); _T(cntr2,"SEELAND"); _T(cntr3,"FERNICIA"); _T(cntr4,"NORTH ISLES"); 
    _T(cntr5,"SUDWEGEN"); _T(cntr6,"MONTENAS"); _T(cntr7,"HELVANIA"); _T(cntr8,"MORGENLAND");        
    t_pointers names_countries PROGMEM = { 
        name_cntr1, name_cntr2, name_cntr3, name_cntr4, name_cntr5, name_cntr6, name_cntr7, name_cntr8 
    };

       
    // Cities
    
    _T(city1,"AMTSTAD"); _T(city2,"EERHAVEN"); _T(city3,"BAKERUM"); _T(city4,"ZALTVAST"); 
    _T(city5,"SPARGO"); _T(city6,"WELTHON"); _T(city7,"CIVA"); _T(city8,"KRISTEL");
    _T(city9,"SEERBICK"); _T(city10,"ARLECH"); _T(city11,"KAMPIIR"); _T(city12,"TELZEVEN");
    _T(city13,"BRADEN"); _T(city14,"RUNDI"); _T(city15,"PETROHOF"); _T(city16,"KALASI"); 
      
    t_pointers names_cities PROGMEM = { 
        name_city1, name_city2, name_city3, name_city4, name_city5, name_city6, name_city7, name_city8,
        name_city9, name_city10, name_city11, name_city12, name_city13, name_city14, name_city15, name_city16
    };    

    
    // Villages
    
    _T(vilg1,"BERTON"); _T(vilg2,"BESEVILLE"); _T(vilg3,"TRAMNEGEN"); _T(vilg4,"BRIM"); 
    _T(vilg5,"DEN WELL"); _T(vilg6,"BALAKEN"); _T(vilg7,"SURFAX"); _T(vilg8,"VENSTUM");
    _T(vilg9,"MARIKU"); _T(vilg10,"BELHAUS"); _T(vilg11,"POLTA"); _T(vilg12,"DUNTER");
    _T(vilg13,"SEVERIN"); _T(vilg14,"ALANO"); _T(vilg15,"DORP"); _T(vilg16,"KLINVELT");   
     
    t_pointers names_villages PROGMEM = { 
        name_vilg1, name_vilg2, name_vilg3, name_vilg4, name_vilg5, name_vilg6, name_vilg7, name_vilg8,
        name_vilg9, name_vilg10, name_vilg11, name_vilg12, name_vilg13, name_vilg14, name_vilg15, name_vilg16
    };


    // Ship types
    
    _T(ship_type1,"FISHING BOAT"); _T(ship_type2,"MERCHANT SHIP"); _T(ship_type3,"PIRATE SHIP"); _T(ship_type4,"NAVY VESSEL"); 
        
    t_pointers names_ship_types PROGMEM = { 
        name_ship_type1, name_ship_type2, name_ship_type3, name_ship_type4
    };

    
    // Ship defenses
    
    _T(ship_defense1,"KNIVES"); _T(ship_defense2,"PISTOLS"); _T(ship_defense3,"RIFLES"); _T(ship_defense4,"CANNONS"); 
        
    t_pointers names_ship_defenses PROGMEM = { 
        name_ship_defense1, name_ship_defense2, name_ship_defense3, name_ship_defense4
    };

        
    // Cargo
    
    _T(carg1,"GOLD"); _T(carg2,"SILVER"); _T(carg3,"GUNS"); _T(carg4,"LIQUOR"); 
    _T(carg5,"TIMBER"); _T(carg6,"OIL"); _T(carg7,"POTTERY"); _T(carg8,"ROPE");         
    t_pointers names_cargo PROGMEM = { 
        name_carg1, name_carg2, name_carg3, name_carg4, name_carg5, name_carg6, name_carg7, name_carg8
    };


    // Tools
    
    _T(tool1,"WEAPON_UPG"); _T(tool2,"WORLD_MAP"); _T(tool3,"SEXTANT"); _T(tool4,"SKYSAIL");
    t_pointers names_tool PROGMEM = { 
        name_tool1, name_tool2, name_tool3, name_tool4
    };

    
    // Stats
    
    _T(stat1,"CREW"); _T(stat2,"WEAPONS"); _T(stat3,"SHIP"); _T(stat4,"RESPECT"); _T(stat5,"MORALE");    
    t_pointers names_stat PROGMEM = {
        name_stat1, name_stat2, name_stat3, name_stat4, name_stat5
    };

   
    // Misc
    
    _T(misc1,"PORT "); _T(misc2,"SHORES OF "); _T(misc3,"INVENTORY"); _T(misc4,"BUY"); _T(misc5,"SELL");
    _T(misc6,"(QTY)"); _T(misc7,"(PPU)"); _T(misc8,"NOTHING HERE"); _T(misc9,"CARGO BAY EMPTY"); _T(misc10,"SHIP IS FULL");
    _T(misc11,"UNITS"); _T(misc12,"THANK YOU"); _T(misc13,"DEAL"); _T(misc14,"<"); _T(misc15,">"); _T(misc16,"TOTAL $"); 
    _T(misc17,"- CREW LOST "); _T(misc18,"YOU LOST THE BATTLE\n- LOSS $"); _T(misc19,"YOU GOT THE BOOTY\n- MONEY $");
    _T(misc20,"YOU WERE BANNED"); _T(misc21,"REPAIR COST $"); _T(misc22,"NOT ENOUGH FUNDS"); _T(misc23,"NO DAMAGE");
    _T(misc24,"RECRUIT OFFER\nWAGES $"); _T(misc25,"LAVISH THE CREW\nPRICE $"); _T(misc26,"TOOLKIT"); _T(misc27,"TOOL SHOP"); 
    _T(misc28,"BUY A "); _T(misc29,"\nTOTAL COST $"); _T(misc30,"YOU ALREADY\n_HAVE THAT"); _T(misc31,"RECRUIT JOINED");
    _T(misc32,"NEED SEXTANT\nFOR POSITION"); _T(misc33,"PLEASE WAIT..."); _T(misc34,"@ "); _T(misc35,"WIND\n"); _T(misc36,"SHIP");  
    _T(misc37,"SAIL RAISED"); _T(misc38,"SAIL LOWERED"); _T(misc39,"ATTACK"); _T(misc40,"SKYSAIL");
    _T(misc41,"EMPTY"); _T(misc42,"GAME SAVED"); _T(misc43,"OPTIONS"); _T(misc44,"(ON)"); _T(misc45,"(OFF)");
    _T(misc46,"COPYRIGHT 2022\n_BERT VT VEER"); _T(misc47,"YOUR SHIP HAS SUNK.\n"); _T(misc48,"FOUND A TREASURE!\n___BONUS $");
    _T(misc49,"NAVY SHIP DESTROYED!"); _T(misc50,"NAVY DESTROYED!\nYOU ARE THE"); _T(misc51," RULER\nOF THE SEAS!");
    _T(misc90,"_"); _T(misc91,", "); _T(misc92,"- "); _T(misc93,"\n"); _T(misc94,":");
    
    t_pointers names_misc PROGMEM = {
        name_misc1, name_misc2, name_misc3, name_misc4, name_misc5, name_misc6, name_misc7, name_misc8, name_misc9, 
        name_misc10, name_misc11, name_misc12, name_misc13, name_misc14, name_misc15, name_misc16, name_misc17, name_misc18, 
        name_misc19, name_misc20, name_misc21, name_misc22, name_misc23, name_misc24, name_misc25, name_misc26, name_misc27,
        name_misc28, name_misc29, name_misc30, name_misc31, name_misc32, name_misc33, name_misc34, name_misc35, name_misc36,
        name_misc37, name_misc38, name_misc39, name_misc40, name_misc41, name_misc42, name_misc43, name_misc44, name_misc45, 
        name_misc46, name_misc47, name_misc48, name_misc49, name_misc50, name_misc51,
        name_misc90, name_misc91, name_misc92, name_misc93, name_misc94
    };
    
    enum misc { 
        port, shore, inventory, buy, sell, amount, price, nothing, cargoempty, ship_full, 
        units, dealdone, deal, arrow_left, arrow_right, total,
        crew_lost, plunder_lose, plunder_win, denied, repair_cost, not_enough_money, no_damage, 
        recruit_cost, tavern_cost, tools_head, toolshop, toolshop_deal, toolshop_cost, already_have, crew_confirm, 
        need_sextant, wait_fullmap, population, wind, damage_warning, skysail_raised, skysail_lowered,
        attack, skysail_broken, empty, game_saved, options_head, sound_on, sound_off, copyright, ship_sunk, treasure,
        navy_sunk, navy_destroyed, navy_destroyed2,
        space, comma, dash, newline, colon
    };


    // Menu items
    
    _T(menu1,"DISEMBARK"); _T(menu2,"INSPECT"); _T(menu3,"PLUNDER"); _T(menu4,"LEAVE"); _T(menu5,"BUY TOOLS"); _T(menu6,"TOOLS");
    _T(menu7,"CARGO"); _T(menu8,"STATS"); _T(menu10,"SAVE"); _T(menu11, "OPTIONS"); _T(menu12,"REPAIRS"); _T(menu13,"MARKETPLACE");
    _T(menu14,"VISIT TAVERN"); _T(menu15,"RECRUIT"); _T(menu16,"PLUNDER"); _T(menu17,"SOUND"); _T(menu18,"CREDITS"); _T(menu19,"RESET");

    t_pointers names_menu_main PROGMEM = { name_menu8, name_menu7, name_menu6, name_menu10, name_menu11 };
    t_pointers names_menu_land PROGMEM = { name_menu1, name_menu4 };
    t_pointers names_menu_town PROGMEM = { name_menu1, name_menu4 };
    t_pointers names_menu_port PROGMEM = { name_menu5, name_menu12, name_menu4 };
    t_pointers names_menu_ship PROGMEM = { name_menu2, name_menu4 };
    t_pointers names_menu_misc PROGMEM = { name_menu4 };
    t_pointers names_menu_disembark PROGMEM = { name_menu13, name_menu14, name_menu15, name_menu16 };
    t_pointers names_menu_tools PROGMEM = { name_tool1, name_tool2, name_tool3, name_tool4 };
    t_pointers names_menu_options PROGMEM = { name_menu17, name_menu18, name_menu19 };
        
    constexpr uint8_t size_menu_main = sizeof(names_menu_main) / sizeof(void*);
    constexpr uint8_t size_menu_land = sizeof(names_menu_land) / sizeof(void*);   
    constexpr uint8_t size_menu_town = sizeof(names_menu_town) / sizeof(void*); 
    constexpr uint8_t size_menu_port = sizeof(names_menu_port) / sizeof(void*);   
    constexpr uint8_t size_menu_ship = sizeof(names_menu_ship) / sizeof(void*); 
    constexpr uint8_t size_menu_disembark = sizeof(names_menu_disembark) / sizeof(void*);
    constexpr uint8_t size_menu_tools = sizeof(names_menu_tools) / sizeof(void*);  
    constexpr uint8_t size_menu_options = sizeof(names_menu_options) / sizeof(void*);   


    // Current string buffer
    char _buffer[32];


    char* get(t_pointers list, uint16_t index) {     
        IMP_strcpy(_buffer, list[index]);
        return &_buffer[0];        
    }

    uint8_t length() {
        return strlen(_buffer);
    }
    
    t_textsize textSize() {
        t_textsize size = { 0, 0 };
        uint8_t count = 0;
        for (uint8_t i=0; i<=length(); i++) {
            if (_buffer[i] == '\n' || _buffer[i] == '\0') {
                if (count > size.line_width) {
                    size.line_width = count;
                    count = 0;
                }
                size.line_count++;
            }
            else if (_buffer[i] == ' ') count+=3;
            else count+=5;
        }
        return size;
    }
    
    template <typename T> char* cast(T num, uint8_t index = 0) {
        itoa(static_cast<int>(num), &_buffer[index], 10);  
        return &_buffer[0];  
    }

    char* pad_number(uint16_t num, uint8_t offset, signed char prefix = 0) {
        uint8_t i, len = length();
        uint8_t digits = 1 + (int)(num>=10) + (int)(num>=100) + (int)(num>=1000);
        for (i=len; i<offset; i++) _buffer[i] = (prefix == '.' ? '.' : '_');
        cast(num, offset-digits);
        if (prefix) _buffer[offset-1-digits] = prefix; 
        return &_buffer[0]; 
    }
        
};
