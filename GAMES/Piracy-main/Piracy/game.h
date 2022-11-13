//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#include "common.h"
#include "fractiles.h"
#include "entity.h"
#include "actors.h"
#include "logic.h"
#include "media.h"
#include "system.h"
#include "menu.h"
#include "strings.h"

#define abs(x) ((x)<0 ? (-x) : (x))

class Game
{
    private:  
        enum class e_collision {
            none, ship, location, rock, land
        } _collision_type;     
          
        enum class e_mode { 
            view, land, town, ship, trading, tavern, recruit, plunder, battle, transaction, repairs, toolshop, 
            toolshop_deal, tools, fullmap, inventory, stats, menu, options, intro, pirate_run, game_over
        } _mode, _last_mode;
                           
        Entity _colliding_entity;
        uint8_t _colliding_ship_index;
        t_coords _colliding_entity_bucket;
        math2d::vector<t_size> _scroll;
        int8_t _pirates[4];
        uint16_t _generalTimer;
                            
    public:                   
        Game() {
            Serial.begin(115200);
            Serial.println();
            Serial.println("INIT TILES");
            object::tiles.init(map_width, map_height); 
            Serial.println("INIT SETBOUNDS");
            actors::setBounds(max_pos_x, max_pos_y);   
            Serial.println("INIT PLAYER");       
            object::player.init(math2d::t_vector(665, 1045), false);   
            store::loadGame();
            scrollMap();
            setMode(e_mode::intro);
        }
        
        void activateMenu(menu::set set) {
            menu::select(set); 
            setMode(e_mode::menu); 
            SOUND_PLAY(sound::collect, 1);               
        }

        void waitFrames(uint8_t duration) {
            IMP_swapBuffers();
            while (duration--) {
                SOUND_UPDATE();
                IMP_waitFrame();             
            }            
        }

        template <typename T> 
        void blankOut(uint8_t duration, uint8_t redraw, T call_func) {
            if (redraw > 0) IMP_clearScreen();
            if (redraw > 1) drawHeader();
            if (redraw > 2) drawIcon();
            call_func();
            waitFrames(duration);
        }
        
        void setPirateMode() {
            static int8_t xp[] = { -42, -74, -58, -106 };
            _last_mode = _mode;
            _mode = e_mode::pirate_run;   
            _generalTimer = 270;
            for (int8_t i=0; i<4; i++) {
                _pirates[i] = xp[(IMP_frameCount+i)%4];
            }  
            blankOut(10, 1, [this]() { } );
            SOUND_PLAY(sound::attack_b, 0); 
            SOUND_PLAY(sound::attack_a, 1);   
        }
        
        void setMode(e_mode newmode) {   
            _last_mode = _mode;                   
            _mode = newmode;   
            logic::misc.lightning = false; 
            
            if (_mode!=e_mode::menu && _mode!=e_mode::view) {
                SOUND_PLAY(sound::select, 1); 
            }
            
            switch (_mode) {                                                 
                case e_mode::plunder:                                     
                    if (_last_mode != e_mode::pirate_run) setPirateMode();
                    else logic::battleTown(_colliding_entity.data.lo == 0x01, _colliding_entity_bucket);                    
                    break; 
                 case e_mode::battle:                 
                    if (_last_mode != e_mode::pirate_run) setPirateMode();                  
                    else logic::battleShip(_colliding_ship_index);                   
                    break;                 
                case e_mode::tools:
                    menu::active.set(0, 0);
                    menu::active.set(1, logic::tools.has_worldmap);
                    menu::active.set(2, logic::tools.has_sextant);
                    menu::active.set(3, logic::tools.has_skysail);
                    menu::index = 1;
                    if (logic::tools.value && !logic::tools.has_secret) menu::next(); // adjust cursor
                    break;                                   
                case e_mode::fullmap:
                    blankOut(10, 1, [this]() { } );
                    drawSextantWait();
                    drawFullMap();
                    break; 
                case e_mode::menu:                    
                    drawView(); 
                    break;              
                case e_mode::intro:
                    SOUND_PLAY(sound::intro_b, 0);
                    SOUND_PLAY(sound::intro_a, 1);
                    break;              
                default:                      
                    break;
            }      
        }

        void scrollMap() {                    
            const math2d::t_vector& player_pos = object::player.position();
            uint16_t intposX = static_cast<uint16_t>(player_pos.x);
            uint16_t intposY = static_cast<uint16_t>(player_pos.y);
            intposX = mod(intposX-screen_center_x, max_pos_x);
            intposY = mod(intposY-screen_center_y, max_pos_y);          

            _scroll.x = mod(intposX, tile_size);
            _scroll.y = mod(intposY, tile_size);

            static uint16_t last_posX;
            static uint16_t last_posY;
            const uint16_t tposX = static_cast<uint16_t>(player_pos.x) / tile_size;
            const uint16_t tposY = static_cast<uint16_t>(player_pos.y) / tile_size;
            
            int16_t dx = tposX - last_posX; if (abs(dx) > max_pos_x/2) dx -= max_pos_x;
            int16_t dy = tposY - last_posY; if (abs(dy) > max_pos_y/2) dy -= max_pos_y;            
            if (dx !=0 || dy != 0) {
                object::tiles.scroll(static_cast<int8_t>(dx), static_cast<int8_t>(dy));
                last_posX = tposX;
                last_posY = tposY;
            }
        } 

        void update() {           
            // Update player
            static math2d::t_vector last_ppos;
            last_ppos = object::player.position();            
            object::player.update(logic::misc.skysail_deployed ? 1.5 : 1.0);
            const math2d::t_vector ppos = object::player.position();

            // .. enemy ships
            for (uint8_t i=0; i<object::num_ships; i++) {
                actors::Actor& ship = object::ships[i];
                if (ship.flags.state == actors::s_flags::e_state::ready) {
                    const math2d::t_unitvec dir = math2d::angle_to_vector(math2d::rnd()%360);
                    const float spawn_dist = screen_center_x * 2;
                    float vx = static_cast<float>(dir.x) * spawn_dist;
                    float vy = static_cast<float>(dir.y) * spawn_dist;                  
                    math2d::t_vector newpos(ppos.x+vx, ppos.y+vy);
                    
                    if (ship.valid_bucket(newpos)) {
                        ship.init(newpos);                       
                        // Un-spawn navy vessel if no more fleets 
                        if ((ship.info&3)==3 && logic::misc.navy_fleets==0) {
                            ship.flags.state = actors::s_flags::e_state::ready;
                        }
                    }
                }
                else {
                    ship.navigate();
                    if (ship.flags.timeout || ship.flags.state != actors::s_flags::e_state::active) continue;

                    // check out of reach / collision 
                    math2d::t_vector diff = ship.position() - ppos;
                    math2d::t_vector::t_type adiffx = abs(diff.x);
                    math2d::t_vector::t_type adiffy = abs(diff.y); 
                    if (adiffx > 256.0 || adiffy > 256.0) { 
                        ship.flags.state = actors::s_flags::e_state::ready;                        
                    }
                    else if (adiffx < 12.0 && adiffy < 12.0) {  
                        object::player.setVelocity(0.05);                        
                        _collision_type = e_collision::ship;
                        _colliding_ship_index = i;
                        ship.flags.timeout = 240;
                        activateMenu(menu::set::ship);
                        return;
                    }
                }
            }                             

            // Update map
            scrollMap();

            // Get current buckets and country
            const t_coords& pos = object::tiles.getPosition();
            t_size bucket_ox = pos.x/bucket_size_w;
            t_size bucket_oy = pos.y/bucket_size_h;
            uint8_t data = mapdata::buckets[bucket_oy][bucket_ox].item.hi;
            logic::misc.current_country =  data & 0b00000111;

            // lightning damage  
            if (_mode == e_mode::view && ((data & 0b00110000)>>4) == 3) {              
                if (IMP_frameCount%8 == 0) {
                    bool lightning = math2d::rnd()%12 == 0;
                    if (logic::misc.lightning && !lightning && math2d::rnd()%4 == 0) {
                        logic::takeDamage(4 + math2d::rnd()%15);
                    }
                    logic::misc.lightning = lightning;
                }
            }
            else logic::misc.lightning = false;  
                         
            // Get current tile
            t_size px = screen_center_x / tile_size;
            t_size py = screen_center_y / tile_size;
            t_tile current_tile = object::tiles.getLocalTile(px, py);
            t_size depth = object::tiles.getFractal().depth;

            // Update entities
            _colliding_entity.next = 0;
            object::entities.loop(*this, bucket_ox, bucket_oy, &Game::updateEntity);

            // Collisions
            _collision_type = e_collision::none; 
            math2d::t_unit velocity = object::player.velocity();            
            if (_colliding_entity.next != 0) { // collision index              
                object::player.bounceTo(last_ppos);

                // Crash if velocity too high
                if (velocity > 0.4) {
                    logic::takeDamage(5+math2d::rnd()%20);
                    return;
                }
                
                // Init shop when landing in a town
                t_size coll_type = _colliding_entity.data.lo;
                if (coll_type==1 || coll_type==2) 
                {
                    // ..or leave if not welcome
                    uint8_t bucket_x = _colliding_entity_bucket.x;
                    uint8_t bucket_y = _colliding_entity_bucket.y;
                    if (mapdata::buckets[bucket_y][bucket_x].item.hi & 0b10000000) {
                        blankOut(120, 0, [this]() { STRING(misc, strings::misc::denied); drawBoxedStr(true); } );
                        return;
                    }
                    s_nibbles<t_size> ent_pos = _colliding_entity.position;
                    s_nibbles<t_size> ent_data = _colliding_entity.data;    
                    logic::misc.current_population = 150-coll_type*50 + math2d::hash(ent_pos.value)%50;
                    logic::trading.initShop(ent_data);
                }
                // Collision actions
                switch (coll_type) {    
                    case 1: 
                        activateMenu(menu::set::town); 
                        _collision_type = e_collision::location; 
                        break;
                    case 2: 
                        activateMenu(menu::set::town); 
                        _collision_type = e_collision::location; 
                        break;
                    case 3: 
                        activateMenu(menu::set::land);
                        _collision_type = e_collision::land; 
                        break;
                    case 4: 
                        activateMenu(menu::set::port);
                        _collision_type = e_collision::location;
                        break;                    
                    default: break;
                }
            }
            else if (current_tile > depth-2) { 
                object::player.bounceTo(last_ppos); 
                              
                if (current_tile != depth) {
                    // hit the rocks //
                    if (velocity > 0.2) logic::takeDamage(13+math2d::rnd()%15);
                }
                else {
                    // hit the beach
                    if (velocity > 0.4) logic::takeDamage(5+math2d::rnd()%11);
                    else {
                        activateMenu(menu::set::land);  
                        _collision_type = e_collision::land;
                    }
                }               
            }
        }       

        void updateEntity(const Entity& entity, t_size bucketSumX, t_size bucketSumY) {   
            t_coords offs(screen_center_x / tile_size, screen_center_y / tile_size);
            t_coords pos = object::tiles.getPosition() + offs;               
            int16_t px = bucketSumX * bucket_size_w + entity.position.lo - pos.x;
            int16_t py = bucketSumY * bucket_size_h + entity.position.hi - pos.y;                
            if (!(px || py)) {
                uint8_t bucketX = mod(bucketSumX, buckets_w);
                uint8_t bucketY = mod(bucketSumY, buckets_h);
                _colliding_entity_bucket = t_coords(bucketX, bucketY);
                _colliding_entity = entity;
                _colliding_entity.next++;
            }
        }

        void nextFrame() { 
            input::update(_mode == e_mode::view);
            switch (_mode) {
                case e_mode::view: inputView(); break;
                case e_mode::land: inputLand(); break;
                case e_mode::town: inputTown(); break;
                case e_mode::ship: inputShip(); break;
                case e_mode::trading: inputTrading(); break;
                case e_mode::tavern: inputTavernOrRecruit(true); break;
                case e_mode::recruit: inputTavernOrRecruit(false); break;
                case e_mode::plunder:
                case e_mode::battle: inputPlunder(); break;
                case e_mode::transaction: inputTransaction(); break;
                case e_mode::repairs: inputRepairs(); break;
                case e_mode::toolshop: inputToolShop(); break;
                case e_mode::toolshop_deal: inputToolShopDeal(); break;
                case e_mode::tools: inputTools(); break;
                case e_mode::fullmap: inputFullmap(); break;              
                case e_mode::inventory:
                case e_mode::stats: inputGeneric(); break;
                case e_mode::menu: inputMenu(); break;
                case e_mode::options: inputOptions(); break;
                case e_mode::intro: inputIntro(); break;   
                case e_mode::pirate_run: inputPirateRun(); break;    
                case e_mode::game_over: inputGameOver(); break;               
                default: break;
            }   
            drawFrame();
            SOUND_UPDATE();
        }
        
        // Input schemes

        void menuUpDown() {
            if (input::keys.up) menu::previous(); 
            else if (input::keys.down) menu::next();           
        }
        
        void inputGeneric() {
            if (input::keys.a || input::keys.b) {               
                menu::select(menu::set::main);
                menu::index = menu::save_main_index;                
                setMode(e_mode::menu);    
            }
        }
        
        void inputView() {  
            if (object::player.flags.state == actors::s_flags::e_state::sinking) {
                if (--object::player.flags.timeout == 0) {
                    blankOut(30, 1, [this]() { } );
                    setMode(e_mode::game_over);
                }
                return;
            }
            
            if (input::keys.left) object::player.turn(2); 
            else if (input::keys.right) object::player.turn(-2);
            if (input::keys.up) object::player.addVelocity(0.02);
            else if (input::keys.down) object::player.addVelocity(-0.02); 
                       
            if (input::keys.b) {
                menu::select(menu::set::main);
                menu::index = 0;
                menu::save_main_index = 0;
                setMode(e_mode::menu);
            }
            update();
        }

        void inputLand() {
            if (input::keys.a || input::keys.b) setMode(e_mode::view);    
        }

        void inputTown() {            
            menuUpDown();
            if (input::keys.a) {
                switch (menu::index) {               
                    case 0:
                        logic::trading.setInventory(logic::stock);
                        menu::select(menu::set::cargo, reinterpret_cast<uint8_t*>(logic::trading.current_inventory));
                        setMode(e_mode::trading);
                        break;
                    case 1:                       
                        logic::transfer.value = logic::stats.crew * 15;
                        setMode(e_mode::tavern);
                        blankOut(10, 3, [this]() { } );
                        break;    
                    case 2:                        
                        logic::transfer.value = logic::stats.crew*35;
                        setMode(e_mode::recruit);
                        blankOut(10, 3, [this]() { } );
                        break;
                    case 3: 
                        setMode(e_mode::plunder); 
                        break; 
                    default: 
                        break;    
                } 
            }
            else if (input::keys.b) {
                setMode(e_mode::view);    
            }
        }

        void inputShip() {
            if (input::keys.a) setMode(e_mode::battle);
            else if (input::keys.b) setMode(e_mode::view);           
        }
        
        void inputRepairs() {
            if (input::keys.a || input::keys.b) {
                uint16_t cost = logic::getRepairCost();
                if (input::keys.a && cost!=0 && cost <= logic::stats.money) {
                    logic::stats.money -= cost;
                    logic::stats.ship_health = 100;
                    blankOut(10, 2, [this]() { } );
                    drawDealDone();
                    blankOut(10, 2, [this]() { } ); 
                }
                setMode(e_mode::menu);    
            }
        }

        void inputToolShop() {
            menuUpDown();
            if (input::keys.a) {
                bool has_item = menu::index>0 && !logic::tools.has(menu::index);
                bool can_upgrade_weapon = menu::index==0 && logic::weapons.level<3;
                if (has_item || can_upgrade_weapon) setMode(e_mode::toolshop_deal); 
                else {
                    blankOut(10, 1, [this]() { IMP_drawTextXY(2, 0, STRING(misc, strings::misc::toolshop)); } );
                    STRING(misc, strings::misc::already_have);
                    drawBoxedStr(true);
                    blankOut(100, 0, [this]() { } );
                }   
            }
            else if (input::keys.b) {
                menu::select(menu::set::port);
                setMode(e_mode::menu);                  
            }
        }
       
        void inputToolShopDeal() {
            if (input::keys.a) {              
                uint16_t cost = logic::getToolCost(menu::index);
                if (cost <= logic::stats.money) {
                    logic::stats.money -= cost;
                    
                    if (menu::index == 0) { logic::weapons.level++; } 
                    else { logic::tools.set(menu::index, 1); }
                    
                    blankOut(10, 1, [this]() { IMP_drawTextXY(2, 0, STRING(misc, strings::misc::toolshop)); } );
                    drawDealDone();
                }                 
            }
            else if (!input::keys.b) return;
            setMode(e_mode::toolshop);
        }
        
        void inputTrading() {
            menuUpDown();            
            if (input::keys.left || input::keys.right) {                 
                logic::trading.toggleSide(); 
                menu::select(menu::set::cargo, reinterpret_cast<uint8_t*>(logic::trading.current_inventory));
                SOUND_PLAY(sound::select, 1);
                blankOut(6, 1, [this]() { drawInventory(false); } );
            }
            if (input::keys.a) {
                uint8_t num_items = logic::trading.count(logic::trading.getInventory());
                uint8_t price = logic::trading.getPrice(menu::index);
                bool can_afford = !(logic::trading.isShop() && logic::stats.money < price);
                if (num_items && can_afford) {
                    logic::transaction.current_amount = 1;
                    setMode(e_mode::transaction);  
                }  
            } 
            else if (input::keys.b) {
                menu::select(menu::set::disembark);
                setMode(e_mode::town);
            }
        }

        void inputTavernOrRecruit(bool is_tavern) {
            if (input::keys.a || input::keys.b) {
                if (input::keys.a && 
                        logic::stats.money >= static_cast<uint16_t>(logic::transfer.value) &&
                        !(!is_tavern && logic::stats.crew >= logic::stats.max_crew)) 
                {
                    if (is_tavern) {
                        logic::stats.respect = min(logic::stats.respect + 8, 100);
                        logic::stats.morale = min(logic::stats.morale + 22, 100);
                    }
                    else {                       
                        logic::stats.crew += 1; 
                        logic::stats.morale = min(logic::stats.morale + 10, 100);   
                    }
                    logic::stats.money -= logic::transfer.value;
                    blankOut(10, 2, [this]() { } );
                    drawDealDone(is_tavern ? strings::misc::dealdone : strings::misc::crew_confirm); 
                    blankOut(10, 2, [this]() { } ); 
                }
                menu::select(menu::set::disembark);
                menu::index = is_tavern ? 1 : 2;
                setMode(e_mode::town); 
            }
        }
        
        void inputPlunder() {
            if (input::keys.a || input::keys.b) {
                blankOut(30, 1, [this]() { } ); 
                setMode(e_mode::view);    
            }
        }
        
        void inputTransaction() {                        
            uint8_t price = logic::trading.getPrice(menu::index);
            uint8_t budget = min(logic::stats.money/price, 99);
            uint8_t max_value = logic::trading.isShop() ? budget : min(logic::inventory[menu::index], 99); 
            
            if (input::keys.left) logic::transaction.addAmount(-1);
            else if (input::keys.down) logic::transaction.addAmount(-10);
            else if (input::keys.right) logic::transaction.addAmount(1, max_value);
            else if (input::keys.up) logic::transaction.addAmount(10, max_value);

            if (input::keys.a || input::keys.b) {               
                if (input::keys.a) {
                    uint16_t total_price = logic::transaction.current_amount * price;
                    logic::transaction.exchange(total_price, logic::transaction.current_amount, menu::index);
                    blankOut(10, 1, [this]() { drawTransactionHead(); } );
                    drawDealDone(); 
                    blankOut(10, 1, [this]() { } );                  
                }
                menu::select(menu::set::cargo, reinterpret_cast<uint8_t*>(logic::trading.current_inventory));
                setMode(e_mode::trading);    
            }   
        }

        void inputTools() {  
            if (logic::tools.value) {
                menuUpDown();
                if (input::keys.a) {
                    switch (menu::index) {
                        case 1: setMode(e_mode::fullmap); break; 
                        case 3: 
                            logic::misc.skysail_deployed = !logic::misc.skysail_deployed;
                            blankOut(10, 1, [this]() { } );
                            drawDealDone(logic::misc.skysail_deployed ? 
                                strings::misc::skysail_raised : 
                                strings::misc::skysail_lowered);
                            break;              
                        default: break;   
                    }
                    return;
                }
            }
            if (input::keys.a || input::keys.b) {
                menu::select(menu::set::main);
                setMode(e_mode::menu);
            }
        }
        
        void inputFullmap() {
            if (input::keys.a || input::keys.b) {
                blankOut(10, 1, [this]() { } );
                _mode = e_mode::tools; // setMode??
            }
        }
        
        void inputMenu() {
            menuUpDown();                        
            if (input::keys.b) setMode(e_mode::view);
            else if (input::keys.a) {               
                switch (menu::current) {
                    case menu::set::main: menuMainAction(); break;
                    case menu::set::land: menuLandAction(); break;   
                    case menu::set::town: menuTownAction(); break;   
                    case menu::set::port: menuPortAction(); break;   
                    case menu::set::ship: menuShipAction(); break;                
                    default: setMode(e_mode::view); break;
                }
            }
        }                        

        void inputOptions() {
            menuUpDown();
            if (input::keys.b) {
                menu::select(menu::set::main);
                menu::index = menu::save_main_index;                
                setMode(e_mode::menu);
            }
            else if (input::keys.a) {
                switch (menu::index) {
                    case 0: IMP_toggleSound(); break;
                    case 1:
                        blankOut(10, 1, [this]() { } ); 
                        setMode(e_mode::intro);
                        break;
                    case 2: 
                        blankOut(100, 1, [this]() { } );
                        store::clear();
                        if (!IMP_soundEnabled()) { IMP_toggleSound(); IMP_saveSoundState(); }
                        IMP_reboot();
                        break;
                    default: break;
                }
            }
        }

        void inputIntro() {
            if (input::keys.a || input::keys.b) {
                SOUND_STOP();
                blankOut(50, 1, [this]() { } );
                setMode(e_mode::view);
            }
        }

        void inputPirateRun() {
            if (!_generalTimer--) {
                blankOut(70, 1, [this]() { } );
                setMode(_last_mode); 
            }
        }

        void inputGameOver() {
            if (input::keys.a || input::keys.b) {
                IMP_reboot();
            }
        }


        // Menu actions

        void menuMainAction() {
            switch (menu::index) {
                case 0: setMode(e_mode::stats); break;  
                case 1: 
                    logic::trading.setInventory(logic::inventory);
                    menu::select(menu::set::cargo, reinterpret_cast<uint8_t*>(logic::trading.current_inventory));
                    setMode(e_mode::inventory); 
                    break;
                case 2: 
                    menu::select(menu::set::tools);
                    setMode(e_mode::tools); break;
                case 3:
                    store::saveGame();
                    IMP_saveSoundState();
                    blankOut(100, 1, [this]() { STRING(misc, strings::misc::game_saved); drawBoxedStr(true); } );
                    drawView();
                    break;
                case 4:
                    menu::select(menu::set::options);
                    setMode(e_mode::options); 
                    break;
                default: 
                    setMode(e_mode::view); 
                    break;
            }
        }

        void menuLandAction() {
            switch (menu::index) {
                case 0:                    
                    setMode(e_mode::land);
                    if (logic::misc.found_treasure==1) logic::misc.found_treasure=-1;
                    else if (logic::misc.found_treasure==0 && _colliding_entity.data.hi) {
                        logic::misc.found_treasure = 1;
                        logic::misc.treasure_bonus = 127 + math2d::rnd()%233;
                        logic::stats.money += logic::misc.treasure_bonus;
                        SOUND_PLAY(sound::attack_a, 1);
                    }
                    break;
                default: setMode(e_mode::view); break;
            }
        }
        
        void menuTownAction() {
            switch (menu::index) {
                case 0:                    
                    menu::select(menu::set::disembark); 
                    setMode(e_mode::town); 
                    break;
                default: setMode(e_mode::view); break;           
            }
        }
        
        void menuPortAction() {
            switch (menu::index) {    
                case 0:
                    menu::select(menu::set::tools); 
                    setMode(e_mode::toolshop); 
                    break;     
                case 1: 
                    setMode(e_mode::repairs);                    
                    break;                
                default: setMode(e_mode::view); break;            
            }            
        }
        
        void menuShipAction() {
            switch (menu::index) {
                case 0: 
                    setMode(e_mode::ship); 
                    break;
                default: setMode(e_mode::view); break;           
            }
        }
        

        // Drawing
            
        void drawFrame() {    
            if (_mode!=e_mode::view && _mode!=e_mode::menu && _mode!=e_mode::fullmap) IMP_clearScreen();  
            IMP_invertScreen(logic::misc.lightning);
            switch (_mode) { 
                case e_mode::view: drawView(); break;
                case e_mode::land: drawLand(); break;
                case e_mode::town: drawTown(); break;
                case e_mode::ship: drawShip(); break;
                case e_mode::trading: drawInventory(); break;
                case e_mode::tavern: drawTavernOrRecruit(strings::misc::tavern_cost); break;
                case e_mode::recruit: drawTavernOrRecruit(strings::misc::recruit_cost); break;
                case e_mode::plunder:
                case e_mode::battle: drawPlunder(); break;
                case e_mode::transaction: drawTransaction(); break;
                case e_mode::repairs: drawRepairs(); break;
                case e_mode::toolshop: drawToolShop(); break;
                case e_mode::toolshop_deal: drawToolShopDeal(); break;
                case e_mode::tools: drawTools(); break;
                case e_mode::fullmap: if (logic::tools.has_sextant) drawLocationDot(); break;
                case e_mode::inventory: drawInventory(); break;
                case e_mode::stats: drawStats(); break;
                case e_mode::menu: drawMenuGeneral();  break;
                case e_mode::options: drawOptions(); break;
                case e_mode::intro: drawIntro(); break;
                case e_mode::pirate_run: drawPirateRun(); break;
                case e_mode::game_over: drawGameOver(); break;
                default: break;
            }        
            IMP_swapBuffers();
        }
        
        void invertRegion(int8_t x, int8_t y, uint8_t w, uint8_t h) {
            uint8_t* buffer = IMP_getBuffer();
            for (int16_t line=y; line<y+h; line++) {
                if (line<0 || line>=IMP_screenH) continue;
                for (int16_t col=x; col<x+w; col++) { 
                    if (col<0 || col>=IMP_screenW) continue;
                    uint8_t* ptr = &buffer[(line/8)*IMP_screenW + col%IMP_screenW];
                    *ptr ^= 1 << (line % 8);
                }
            }
        }
        
        void drawBoxedStr(bool draw_border, t_coords pos = t_coords(0,0)) {
            strings::t_textsize text_size = strings::textSize();
            t_size len = text_size.line_width;
            t_size lines = text_size.line_count;
            t_size x = pos.x > 0 ? pos.x : IMP_screenW/2 - len/2;
            t_size y = pos.y > 0 ? pos.y : IMP_screenH/2 - (lines*11)/2;
            IMP_fillRect(x-8, y-5, len+16, 7+lines*11, IMP_bgcolor);
            if (draw_border) IMP_drawRect(x-8, y-5, len+16, 7+lines*11, IMP_fgcolor);
            IMP_drawTextXY(x, y, strings::_buffer);            
        }
        
        void drawDealButton(t_coords pos = t_coords(0,50)) {
            IMP_drawTextXY(pos.x+5, pos.y+2, STRING(misc, strings::misc::deal));
            invertRegion(pos.x, pos.y, 30, 12);
        }

        void drawDealDone(strings::misc txt = strings::misc::dealdone) {
            blankOut(100, 0, [this, txt]() { STRING(misc, txt); drawBoxedStr(true); } );
        }

        void drawCountry(strings::misc prefix = static_cast<strings::misc>(0)) {
            if (prefix) IMP_drawText(STRING(misc, prefix));
            IMP_drawText(STRING(countries, logic::misc.current_country));
        }

        void drawIcon() {
            IMP_fillRect(4, 15, 22, 22, IMP_fgcolor);
            IMP_fillRect(7, 18, 16, 16, IMP_bgcolor);
            IMP_drawImage(7, 18, image::entities, _colliding_entity.data.lo);
            if (_mode == e_mode::town || _mode==e_mode::recruit || _mode==e_mode::tavern) {
                IMP_drawTextXY(106, 0, STRING(misc, strings::misc::population));
                IMP_drawText(strings::cast(logic::misc.current_population));
            }  
        }

        void drawSextantWait() {
            if (logic::tools.has_sextant) {
                IMP_drawImage(56, 18, image::sextant, 0);
            }
            else { 
                STRING(misc, strings::misc::need_sextant); 
                drawBoxedStr(true, t_coords(0, 12)); 
            }
            IMP_drawTextXY(34, 42, STRING(misc, strings::misc::wait_fullmap));
            IMP_swapBuffers();    
        }
        
        void drawHeader() {
            IMP_fillRect(0,0,128,10, BLACK);
            switch (_collision_type) {
                case e_collision::ship:
                    IMP_drawTextXY(2,0, STRING(ship_types, (object::ships[_colliding_ship_index].info & 0b00000011)));  
                    break;                  
                case e_collision::land:
                    IMP_drawTextXY(2,0, STRING(misc, strings::misc::shore)); 
                    drawCountry(); 
                    break;
                case e_collision::location:  
                    switch (_colliding_entity.data.lo) { 
                        case 1: 
                            IMP_drawTextXY(2,0, STRING(cities, _colliding_entity.data.hi)); 
                            drawCountry(strings::misc::comma);
                            break;
                        case 2: 
                            IMP_drawTextXY(2,0, STRING(villages, _colliding_entity.data.hi)); 
                            drawCountry(strings::misc::comma);
                            break;
                        case 4: 
                            IMP_drawTextXY(2,0, STRING(misc, strings::misc::port)); 
                            drawCountry();
                        default: break;    
                    }                                                       
                    break;
                default: break;
            }
        }
               
        void drawView() {      
            // Draw tiles
            const t_coords& pos = object::tiles.getPosition();
            t_size bucket_ox = pos.x/bucket_size_w;
            t_size bucket_oy = pos.y/bucket_size_h;
            
            t_size odd = (pos.x + pos.y) % 2;
            for (t_size y=0; y<tiles_h; y++) {
                uint16_t hash_y = (pos.y+y) * tiles_w;
                for (t_size x=0; x<tiles_w; x++) {
                    image::t_bitmap* image_list = image::tiles16;
                    t_size tile = 0;
                    t_tile value = object::tiles.getLocalTile(x,y);
                    t_size depth = object::tiles.getFractal().depth;
                    t_size oddval = (x+y+odd)%2;              
                    if (value == depth) {
                        // Trees pattern
                        if ((math2d::hash(hash_y+(pos.x+x)) & 3) == 0) {   
                            image_list = image::entities;
                            tile = 3;
                        }
                        else tile = oddval; 
                    }
                    else if (value > depth-2) { tile = 2+oddval; }
                    else { tile = 4 + (mapdata::buckets[bucket_oy][bucket_ox].item.hi & 0b00110000)/32; }                                       
                    IMP_drawImage(x*tile_size-_scroll.x, y*tile_size-_scroll.y, image_list, tile);  
                }
            }
            
            // .. entities
            object::entities.loop(*this, bucket_ox, bucket_oy, &Game::drawEntity);

            // .. enemy ships
            bool flash_ship = IMP_frameCount%16 >= 8;
            for (uint8_t i=0; i<object::num_ships; i++) {
                const actors::Actor& ship = object::ships[i];
                if (ship.flags.state==actors::s_flags::e_state::active || 
                        (ship.flags.state==actors::s_flags::e_state::sinking && flash_ship)) {
                    drawActor(ship,
                        screen_center_x - ship_hotspot_x - static_cast<int16_t>(object::player.position().x - ship.position().x),
                        screen_center_y - ship_hotspot_y - static_cast<int16_t>(object::player.position().y - ship.position().y)
                    );
                }          
            }
            
            // .. player
            if ((object::player.flags.timeout==0 || object::player.flags.timeout>100) && 
                    (object::player.flags.state == actors::s_flags::e_state::active || 
                    (object::player.flags.state==actors::s_flags::e_state::sinking && flash_ship))) {
                drawActor(object::player, screen_center_x - ship_hotspot_x, screen_center_y - ship_hotspot_y);
            }

            // .. damage floater
            if (logic::damage_floater.update()) {
                uint8_t flx = screen_center_x + 20;
                uint8_t fly = screen_center_y - 18 + logic::damage_floater.counter/6;
                if (logic::damage_floater.amount > 0) strings::cast(logic::damage_floater.amount);
                else STRING(misc, strings::misc::skysail_broken);                          
                drawBoxedStr(true, t_coords(flx, fly));
            }
            // .. critical damage
            if (object::player.flags.state==actors::s_flags::e_state::active) {
                if (logic::stats.ship_health<20 && IMP_frameCount%100>40) {
                    STRING(misc, strings::misc::damage_warning);
                    drawBoxedStr(false, t_coords(2,1));
                }
            }
        }

        void drawActor(const actors::Actor& actor, int16_t posx, int16_t posy) {
            uint16_t actor_angle = actor.angle();
            t_size angle = actor_angle < 180 ? actor_angle+8 : 360-actor_angle+8;
            t_size ship_frame = static_cast<t_size>((angle/200.0)*7);  
            IMP_drawImageMasked(posx, posy + (IMP_frameCount/23)%2, image::ship_frames, image::ship_mask_frames, ship_frame);            
        }

        void drawEntity(const Entity& entity, t_size bucketSumX, t_size bucketSumY) {
            const t_coords& pos = object::tiles.getPosition();
            int16_t px = bucketSumX * bucket_size_w + entity.position.lo - pos.x;
            int16_t py = bucketSumY * bucket_size_h + entity.position.hi - pos.y; 
            int16_t x = px * tile_size - _scroll.x;
            int16_t y = py * tile_size - _scroll.y;
            IMP_fillRect(x, y, tile_size, tile_size, IMP_bgcolor);     
            IMP_drawImage(x, y, image::entities, entity.data.lo);    
        } 

        void drawEmptyPlace() {
            drawHeader();
            strings::get(strings::names_misc, strings::misc::nothing); 
            drawBoxedStr(false); 
        }
        
        void drawLand() {
            if (logic::misc.found_treasure>0) { 
                IMP_drawImage(56, 14, image::chest, 0);
                IMP_drawTextXY(24, 38, STRING(misc, strings::misc::treasure));
                IMP_drawText(strings::cast(logic::misc.treasure_bonus));
            }
            else drawEmptyPlace();
        }
        
        void drawTown() {
            drawHeader();
            drawIcon();
            drawMenu(t_coords(32,15), 4, 67, true); 
        }

        void drawShip() {
            drawHeader();
            IMP_drawImage(7, 18, image::ship_frames, 0);  
            const actors::Actor& ship = object::ships[_colliding_ship_index];
            
            IMP_drawTextXY(36, 17, STRING(stat, 0));
            IMP_drawTextXY(79, 17, strings::cast((ship.info & 0b01111100) >> 2));
            
            IMP_drawTextXY(36, 28, STRING(stat, 1));
            IMP_drawTextXY(79, 28, STRING(ship_defenses, (ship.info & 0b00000011)));
            
            IMP_drawTextXY(41, 46, STRING(misc, strings::misc::attack));
            invertRegion(36, 44, 40, 12);
        }

        void drawRepairs() {
            uint16_t cost = logic::getRepairCost();
            drawHeader();
            drawIcon();
            if (cost > 0) {
                IMP_drawTextXY(36, 17, STRING(misc, strings::misc::repair_cost));
                IMP_drawText(strings::cast(cost));
                if (logic::stats.money >= cost) drawDealButton(t_coords(36, 35));
                else IMP_drawTextXY(36, 33, STRING(misc, strings::misc::not_enough_money));
            }
            else {
                IMP_drawTextXY(36, 19, STRING(misc, strings::misc::no_damage));
            }
        }

        void drawToolShop() {
            drawTools(strings::misc::toolshop);
        }

        void drawToolShopDeal() {
            uint16_t cost = logic::getToolCost(menu::index); 
            IMP_drawTextXY(2, 0, STRING(misc, strings::misc::toolshop));
            drawIcon(); 
            IMP_drawTextXY(36, 17, STRING(misc, strings::misc::toolshop_deal));
            IMP_drawText(STRING(tool, menu::index));
            IMP_drawText(STRING(misc, strings::misc::toolshop_cost));
            IMP_drawText(strings::cast(cost)); 
            if (logic::stats.money >= cost) drawDealButton(t_coords(36, 45));
            else IMP_drawTextXY(36, 44, STRING(misc, strings::misc::not_enough_money));
        }

        void drawDotLine(uint8_t x, uint8_t y, bool vertical = true) {
            while (x < IMP_screenW-2 && y < IMP_screenH-2) {
                IMP_drawPixel(x, y, (x+y)%2);
                if (vertical) y++; else x++;
            }          
        }
        
        void drawFullMap() {   
            IMP_fillRect(0, 0, IMP_screenW, IMP_screenH, IMP_fgcolor);       
            uint8_t threshold = object::tiles.getFractal().depth-2;
            for (t_size y=1; y<IMP_screenH-1; y++) {               
                for (t_size x=1; x<IMP_screenW-1; x++) {
                    t_tile tile = object::tiles.getTile(static_cast<t_size>(x*2), static_cast<t_size>(y*2));
                    IMP_drawPixel(x, y, tile > threshold ? IMP_fgcolor : IMP_bgcolor);
                }
            }
            drawDotLine(32,1); drawDotLine(64,1); drawDotLine(96,1);
            drawDotLine(1,32, false);
            IMP_drawImage(6, 5, image::compass, 0);
        }

        void drawLocationDot() {  
            if (IMP_frameCount%20 == 0) { 
                const t_coords& pos = object::tiles.getPosition();
                invertRegion(pos.x/2-1, pos.y/2-1, 3, 3);
            }     
        }  

        void drawBuySellMenu() { 
            IMP_drawTextXY(6,1, STRING(misc, strings::misc::buy)); 
            IMP_drawTextXY(32,1, STRING(misc, strings::misc::sell));
            invertRegion(logic::trading.side*28, 0, 27, 10);         
        }

        uint8_t drawMoney(uint8_t x, uint8_t y) {
            char* money_str = strings::cast(logic::stats.money, 1);
            money_str[0] = '$';
            uint8_t money_len = strings::length();
            IMP_drawTextXY(x-money_len*5, y, money_str);
            return money_len;            
        }

        void drawTransactionHead() {                       
            // Draw buy/sell item
            uint8_t buysell = logic::trading.isShop() ? strings::misc::buy : strings::misc::sell;  
            IMP_drawTextXY(2,1, STRING(misc, buysell)); IMP_drawText(STRING(misc, strings::space));
            IMP_drawText(menu::string(menu::index));          

            // Draw money
            uint8_t money_len = drawMoney(126, 1);

            // Draw items left in inventory
            char* count = strings::cast(logic::inventory[menu::index], 2);
            count[0] = ' '; count[1] = '('; 
            uint8_t len = strings::length();
            count[len] = ')'; count[len+1] = '\0';
            IMP_drawTextXY(120-(len+money_len+1)*5, 1, count);
        }
            
        void drawTransaction() {            
            drawTransactionHead();
            
            // Draw amount selection
            IMP_drawRect(20, 19, 23, 16, IMP_fgcolor);
            IMP_drawTextXY(14, 22, STRING(misc, strings::misc::arrow_left)); 
            IMP_drawTextXY(45, 22, STRING(misc, strings::misc::arrow_right));
            uint8_t xoffs = logic::transaction.current_amount >= 10 ? 0 : 3; 
            IMP_drawTextXY(27+xoffs, 23, strings::cast(logic::transaction.current_amount)); 
            IMP_drawTextXY(58, 23, STRING(misc, strings::misc::units)); 

            // Draw total/offer
            IMP_drawTextXY(58, 37, STRING(misc, strings::misc::total));
            uint8_t price = logic::trading.getPrice(menu::index);
            IMP_drawText(strings::cast(logic::transaction.current_amount * price));

            drawDealButton(t_coords(17, 45));
        }

        void drawTools(strings::misc head = strings::misc::tools_head) {
            bool is_shop = head != strings::misc::tools_head; 
            if (is_shop) drawIcon();
            IMP_drawTextXY(2,0, STRING(misc, head));
            if (logic::tools.value > 0 || is_shop) {
                uint8_t menu_width = is_shop ? 84 : 56;
                drawMenu(t_coords(8+((int)is_shop)*26,15), 4, menu_width, true);     
            }
            else {
                STRING(misc, strings::misc::empty);
                drawBoxedStr(false);    
            }
        }
        
        void drawInventory(bool draw_items = true) {
            // Regular inventory
            if (_mode != e_mode::inventory) drawBuySellMenu();
            else IMP_drawTextXY(2,0, STRING(misc, strings::misc::inventory));

            // Draw money
            uint8_t money_len = _mode == e_mode::inventory ? -1 : drawMoney(126, 1);
            
            // Draw units
            char* txt_units = _mode == e_mode::inventory ? 
                STRING(misc, strings::misc::amount) : 
                STRING(misc, strings::misc::price);
            IMP_drawTextXY(126-(money_len+strings::length()+1)*5, 1, txt_units);
           
            // Draw items
            if (!draw_items) return;
            t_size item_count = logic::trading.count(*logic::trading.current_inventory);
            t_size stock_count = logic::trading.count(logic::stock);

            if (!stock_count && _mode != e_mode::inventory) {
                STRING(misc, strings::misc::empty);
                drawBoxedStr(false);
                return; 
            }
            else if (!item_count) {
                strings::get(strings::names_misc, strings::misc::cargoempty);
                drawBoxedStr(false);
                return;  
            }
            drawMenu(t_coords(0,18), 4, 63, _mode != e_mode::inventory); 
        }

        void drawTavernOrRecruit(strings::misc index) {
            uint16_t cost = logic::transfer.value;
            drawHeader(); drawIcon();
            IMP_drawTextXY(36, 17, STRING(misc, index));
            IMP_drawText(strings::cast(cost));
            
            if (logic::stats.money < cost) {
                IMP_drawTextXY(36, 44, STRING(misc, strings::misc::not_enough_money));
            }
            else if (_mode==e_mode::recruit && logic::stats.crew >= logic::stats.max_crew) {
                IMP_drawTextXY(36, 44, STRING(misc, strings::misc::ship_full));
            }
            else {
                drawDealButton(t_coords(36, 45));
            }
        }

        
        void drawPlunder() {     
            uint8_t lose_win = logic::transfer.value < 0 ? 0 : 1;
            IMP_drawImage(8, 5, image::lose_win[lose_win], 0);

            if (logic::transfer.value == logic::transfer.navy_sunk) { 
                if (logic::misc.navy_fleets) {
                    IMP_drawTextXY(12, 22, STRING(misc, strings::misc::navy_sunk));
                }
                else if (IMP_frameCount%90 < 70) {
                    IMP_drawTextXY(12, 22, STRING(misc, strings::misc::navy_destroyed)); 
                    IMP_drawText(STRING(misc, strings::misc::navy_destroyed2));    
                }
            }
            else {
                IMP_drawTextXY(12, 22, STRING(misc, strings::misc::plunder_lose + lose_win));
                IMP_drawText(strings::cast(abs(logic::transfer.value)));
                IMP_drawText(STRING(misc, strings::misc::newline));                
                if (logic::transfer.value >= 0) {             
                    bool first = true;
                    for (uint8_t i=0; i<8; i++) {
                        if (logic::stock[i]) {
                            if (first) { 
                                IMP_drawText(STRING(misc, strings::misc::dash)); 
                                first = false; 
                            }
                            else IMP_drawText(STRING(misc, strings::misc::comma));
                            IMP_drawText(STRING(cargo, i));                        
                        }
                    }
                }
                else if (logic::transfer.crew_change) {
                    IMP_drawText(STRING(misc, strings::misc::crew_lost));
                    IMP_drawText(strings::cast(abs(logic::transfer.crew_change)));
                }
            }
        }

        void drawStats() {
            // Money / crew
            drawMoney(124, 1); 
            IMP_drawTextXY(5, 6, STRING(stat, 0));
            IMP_drawTextXY(49, 6, strings::cast(logic::stats.crew));
            IMP_drawTextXY(5, 17, STRING(stat, 1));
            IMP_drawTextXY(49,17, STRING(ship_defenses, logic::weapons.level));
                                    
            // Scaled stats
            const uint8_t bar_length = 76;
            for (uint8_t i=0; i<3; i++) {
                uint8_t val = *(reinterpret_cast<uint8_t*>(&logic::stats.ship_health) + i);
                uint16_t length = (bar_length * val) / 100;
                uint8_t y = 28 + i*11;
                IMP_drawTextXY(5, y, STRING(stat, i+2));
                IMP_fillRect(49, y, length, 8, IMP_fgcolor);
            }
        }
        
        void drawMenu(t_coords offs, t_size lines, t_size default_width, bool show_cursor = true) {           
            const t_size line_height = 11;
            t_size menu_length = menu::length();

            // Box size
            t_size width = default_width;
            t_size height = menu::current==menu::set::main ? IMP_screenH : menu_length * line_height;           
            if (menu_length > lines) {
                width += 66;
                height -= lines * line_height;
            }
            if (_mode == e_mode::menu) IMP_fillRect(offs.x-4, offs.y-2, width+6, height+4, IMP_bgcolor);

            // Draw items    
            t_size n = 0;        
            for (t_size i=0; i<menu_length; i++) {
                if (!menu::active(i)) continue;

                // Check columns
                t_size xm = offs.x, ym = n;
                if (n >= lines) {
                    xm = offs.x + 64;
                    ym = n % lines;
                }
                // Draw item text                
                char* txt = menu::string(i);
                if (_mode == e_mode::inventory) {
                    uint16_t value = logic::trading.getInventory()[i];
                    strings::pad_number(value, 11, '.');
                }
                else if (_mode == e_mode::trading) {
                    uint16_t value = logic::trading.getPrice(i);
                    strings::pad_number(value, 11, '$');
                }
                else if (_mode == e_mode::toolshop) {
                    uint16_t value = logic::getToolCost(i);
                    strings::pad_number(value, 15, '$');
                }
                uint8_t yc = offs.y+ym*line_height;
                IMP_drawTextXY(xm+4, yc+1, txt);
                
                // Inverse if cursor here
                if (show_cursor && i == menu::index) {
                    invertRegion(xm, yc, default_width, line_height-1);    
                }
                n++;
            }          
        }

        void drawMenuGeneral() {
            if (menu::current == menu::set::main) drawMenu(t_coords(81,2), 8, 46);
            else { drawHeader(); drawMenu(t_coords(2,11), 5, 55); }            
        }
        
        void drawOptions() {
            IMP_drawTextXY(2, 0, STRING(misc, strings::misc::options_head));
            IMP_drawTextXY(41, 16, STRING(misc, IMP_soundEnabled() ? strings::misc::sound_on : strings::misc::sound_off));
            drawMenu(t_coords(8,15), 5, 62);
        }


        void drawIntro() { 
            static int8_t wave[] = { 0, 1, 2, 2, 1, 0, -1, -2, -2, -1 };
                                                                
            IMP_drawImage(18, 12, image::skull, 0); 
            IMP_drawTextXY(30, 39, STRING(misc, strings::misc::copyright)); 
                               
            for (uint16_t i=0; i<sizeof(image::title); i++) {
                for (uint8_t b=0; b<8; b++) {
                    if (image::title[i] & (1<<b)) {
                        int8_t wi = (IMP_frameCount+i)/9 % sizeof(wave);
                        float fac = (i+9) / 32.0f;
                        IMP_fillRect(38+i*2, 12+b*2 + static_cast<int16_t>(wave[wi]*fac), 2, 2, IMP_fgcolor);
                    }
                }
            }
        }

        void drawPirateRun() {
            IMP_fillRect(0, 52, IMP_screenW, 18, IMP_fgcolor);
            for (uint8_t x=0; x<IMP_screenW; x+=image::railing[0]) {
                IMP_drawImage(x, 6, image::railing, 1);
                IMP_drawImage(x, 54, image::railing, 0);
            }
            for (uint8_t i=0; i<4; i++) {
                if (i >= logic::stats.crew) break;
                _pirates[i]++; 
                IMP_drawImage(_pirates[i], 18+i*3, image::pirate, (IMP_frameCount+i)/5 % 3);
            }
        }

        void drawGameOver() {
            IMP_drawImage(56, 14, image::skull, 0);
            IMP_drawTextXY(22, 38, STRING(misc, strings::misc::ship_sunk));
        }
};
