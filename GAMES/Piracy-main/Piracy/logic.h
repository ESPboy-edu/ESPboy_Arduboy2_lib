//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once


namespace object
{
    const uint8_t num_ships = 2;
    
    actors::Actor player;  
    actors::Actor ships[num_ships];
    fract::Fractiles<tiles_w, tiles_h> tiles;
    Entities entities;
};


namespace logic
{

    struct Misc
    {
        uint8_t current_country = 0;
        uint16_t current_population = 0;
        uint8_t navy_fleets = 4;
        bool skysail_deployed = false;
        int8_t found_treasure = 0;
        int16_t treasure_bonus = 0;
        bool lightning = false;       
    }
    misc;
   
    struct Stats
    {
        typedef int8_t t_value;
        const t_value max_crew = 24;
        
        uint16_t money = 50;
        int8_t crew = 4;
        t_value ship_health = 100;
        t_value respect = 20;
        t_value morale = 40;       
    }
    stats;


    struct Weapons
    {
        uint8_t level = 0;
    }
    weapons;


    struct Tools
    {
        union {
            struct {
                uint8_t has_secret : 1;
                uint8_t has_worldmap : 1;
                uint8_t has_sextant : 1;
                uint8_t has_skysail : 1;
                uint8_t unused : 4;
            };
            uint8_t value;
        }; 
        
        Tools() : value(0) {}

        bool has(uint8_t index) {
            return (value & (1 << index)) > 0; 
        }

        void set(uint8_t index, uint8_t pval) {
            uint8_t bitmask = 1 << index;
            value = pval ? value | bitmask : value & (255-bitmask);
        }
    }
    tools;

    
    struct Inventory 
    {
        t_size gold = 0;
        t_size silver = 0;
        t_size guns = 0;
        t_size liquor = 0;
        t_size timber = 0;
        t_size oil = 0;
        t_size pottery = 0;
        t_size rope = 0;
        t_size& operator [] (uint8_t index) { 
            return *(&gold + index*sizeof(t_size)); 
        }
    };
    Inventory inventory;
    Inventory stock;
    
    
    struct Trading 
    {
        enum e_side { shop, player } side = e_side::shop;
        Inventory* current_inventory = &inventory; 
        uint16_t current_seed;
        
        
        Inventory& getInventory() { 
            return *current_inventory; 
        }
        
        void setInventory(Inventory& inv) {
            side = &inv == &stock ? e_side::shop : e_side::player;
            current_inventory = &inv;
        }
        
        void toggleSide() { 
            side = (e_side)(1-side);
            current_inventory = side ? &inventory : &stock;
         }   
    
        uint8_t count(Inventory& inv) { 
            uint8_t num = 0;
            for (uint8_t n=0; n<8; n++) if (inv[n]) num++;
            return num;
        }

        bool isShop() {
            return side == e_side::shop;
        }
    
        uint8_t getPrice(uint8_t index) {
            uint8_t factor = 1 << (7-index);  
            uint8_t price = 5 + logic::misc.current_population%4 + factor*4/5; // base price
            if (logic::misc.current_country == index) price += price/10 + 5;
            if (side == e_side::player) price -= 1 + price/50; // house advantage
            return price;    
        }
        
        void initShop(s_nibbles<t_size> data) {
            for (uint8_t i=0; i<8; i++) {                
                bool has_item = i==logic::misc.current_country || (math2d::hash(data.value+i)%2)==0;
                if (data.lo!=1 && i<3) has_item = false;
                stock[i] = has_item ? getPrice(i) : 0;
            }             
        }
    }
    trading;

    
    struct Transaction
    {
        const uint16_t max_money = 50000;
        const uint8_t max_inventory_items = 200;
        
        uint8_t current_amount;
    
        void addAmount(int8_t amount, uint8_t max_value = 99) { 
            if (current_amount==1 && amount==10) amount = 9; // jump neatly from 1 to 10 
            current_amount = max(min(current_amount + amount, max_value), 1);
        }
    
        void exchange(uint16_t total_price, uint8_t amount, uint8_t index) {
            if (trading.side == trading.e_side::shop) {
                stats.money = max(stats.money - total_price, 0);
                inventory[index] = min(inventory[index] + amount, max_inventory_items);
            }
            else {
                stats.money = min(stats.money + total_price, max_money);
                inventory[index] = max(inventory[index] - amount, 0);
            }
        }
    }
    transaction;
    
    
    struct Transfer 
    {
        const int16_t navy_sunk = 9999;
        int16_t value = 0;
        int8_t crew_change = 0;
    }
    transfer;
    
    
    struct DamageFloater
    {
        int8_t amount = 0;
        uint8_t counter = 0;
        
        void init(int8_t pAmount, uint8_t pCounter) { 
            amount = pAmount; 
            counter = pCounter; 
        }
        
        bool update() {
            return counter ? --counter > 0 : false;
        }
    }
    damage_floater;
    

    // Free functions
    
    void takeDamage(int8_t amount) { 
        // Damage skysail first
        if (logic::misc.skysail_deployed) { 
            damage_floater.init(-1, 90);
            logic::misc.skysail_deployed = false;
            logic::tools.set(3, 0);
        }  
        else {           
            damage_floater.init(amount, 90);  
            if (stats.ship_health > amount) stats.ship_health -= amount;
            else {
                object::player.flags.state = actors::s_flags::e_state::sinking;
                object::player.flags.timeout = 240;
            }
        }
    }
    
    void punish(uint8_t amount) {
        transfer.value = -amount;
        stats.money = max(static_cast<int16_t>(stats.money)-amount, 0);
        transfer.crew_change = -(math2d::rnd()%4);
        if (stats.crew+transfer.crew_change < 1) transfer.crew_change = -stats.crew+1;
        stats.crew += transfer.crew_change;
    }
    
    void getLoot(uint8_t crew, uint8_t weapon) {
        uint16_t money = crew * (3 + weapon*5); 
        transfer.value = money;
        stats.money = min(stats.money+money, transaction.max_money);              
        uint8_t count = 0;       
        for (uint8_t i=0; i<8; i++) {
            stock[i] = 0;
            if (count++ < 3) {
                uint8_t val = math2d::rnd()%((weapon+1)+i);               
                inventory[i] += val; 
                stock[i] = val;  
            }                              
        }      
    }


    // See Actor::init    
    //
    // WEAPON  PTS  CREW   PTS RANGE
    // cannon  10  (10-29)  100-290
    // rifle    5   (8-27)   40-135
    // pistol   3   (6-25)   18- 75
    // knife    2   (4-23)    8- 46

    bool battle_win(uint8_t crew, uint8_t weapon) {
        static uint8_t weapon_score[] = { 2, 3, 5, 10 };
        uint16_t player_score = stats.crew * weapon_score[weapons.level] + stats.morale/16;
        return player_score > crew * weapon_score[weapon];     
    }
    
    void battleShip(uint8_t ship_index) {
        uint8_t ship_info = object::ships[ship_index].info;
        uint8_t ship_weapon = ship_info & 0b00000011;
        uint8_t ship_crew = (ship_info & 0b01111100) >> 2; 
        
        if (!battle_win(ship_crew, ship_weapon)) {
            punish(25);
        }
        else {
            actors::s_flags& flags = object::ships[ship_index].flags;
            flags.state = actors::s_flags::e_state::sinking;
            flags.timeout = 100;

            if (ship_weapon == 3) {
                if (misc.navy_fleets) misc.navy_fleets--; 
                transfer.value = transfer.navy_sunk;
            }
            else getLoot(ship_crew, ship_weapon);   
        }
    }

    void battleTown(bool is_city, const t_coords& colliding_entity_bucket) {
        if (is_city) {
            if (!battle_win(misc.current_population, 0)) punish(50); // todo: also lose crew
            else getLoot(misc.current_population, 0); 
        }
        else {
            if (!battle_win(misc.current_population, 0)) punish(25);
            else getLoot(misc.current_population, 0);
        }
        // No longer welcome in this region
        uint8_t bucket_x = colliding_entity_bucket.x;
        uint8_t bucket_y = colliding_entity_bucket.y;
        mapdata::buckets[bucket_y][bucket_x].item.value |= 0x8000;
    }

    uint16_t getToolCost(uint8_t index) {
        return 275 - index*65;    
    }
    
    uint16_t getRepairCost() {
        return (100-stats.ship_health) * 3;
    }

};
