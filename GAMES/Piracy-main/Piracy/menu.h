//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#include "strings.h"


namespace menu
{

    const uint8_t max_size = 8;
    enum class set { main, land, town, port, ship, cargo, disembark, tools, options };

    // Bitfield for storing active item flags
    // LSB is first menu item
    
    struct s_active {
        uint8_t val;
        constexpr s_active() : val(255) {}
        
        bool operator () (uint8_t ind) { 
            return (val & (1<<ind)) > 0; 
        }
        
        void set(uint8_t index, bool st = true) { 
            uint8_t bitval = 1 << index;
            if (st) val |= bitval;
            else val &= 255-bitval;
        }
    }
    active;
    
    set current = set::main;
    set last = set::main;
    uint8_t index = 0;
    uint8_t save_main_index = index;

  
    void select(set s, uint8_t* values = nullptr) {
        if (s != current) last = current;
        current = s;
        index = 0;
        if (values == nullptr) {
            // activate all menu items
            active.val = 255;   
        }
        else {
            // activate non-zero items
            for (int8_t i=7; i>=0; i--) {
                bool nonzero = values[i] > 0;
                active.set(i, nonzero);
                if (nonzero) index = i;
            }
        }
        // Restore cursor position for main menu
        if (current == set::main) {
            index = save_main_index;
        }
    }

    // Number of items in pre-defined menu set
    t_size length() {
        switch(current) {
            case set::main: return strings::size_menu_main;
            case set::land: return strings::size_menu_land;
            case set::town: return strings::size_menu_town;
            case set::port: return strings::size_menu_port;
            case set::ship: return strings::size_menu_ship;
            case set::cargo: return max_size;
            case set::disembark: return strings::size_menu_disembark;
            case set::tools: return strings::size_menu_tools;
            case set::options: return strings::size_menu_options;
            default: return 0;
        }
    }

    // C-string of item N in pre-defined menu set
    char* string(uint8_t i = index) {
        switch(current) {
            case set::main: return strings::get(strings::names_menu_main, i);
            case set::land: return strings::get(strings::names_menu_land, i);
            case set::town: return strings::get(strings::names_menu_town, i);
            case set::port: return strings::get(strings::names_menu_port, i);
            case set::ship: return strings::get(strings::names_menu_ship, i);
            case set::cargo: return strings::get(strings::names_cargo, i);
            case set::disembark: return strings::get(strings::names_menu_disembark, i);
            case set::tools: return strings::get(strings::names_menu_tools, i);
            case set::options: return strings::get(strings::names_menu_options, i);
            default: return nullptr;
        }
    }

    // Change item
    void change(int8_t steps, uint8_t bound, uint8_t bound_inv) {
        if (active.val == 0) return;
        while (true) {
            if (index == bound) index = bound_inv;
            else index += steps;
            if (active(index)) {
                if (current == set::main) save_main_index = index;
                SOUND_PLAY(sound::click, 1);
                break;
            }
        }        
    }
    
    // Select previous menu item
    void previous() {
        change(-1, 0, length()-1);
    }

    // Select next menu item
    void next() {
        change(1, length()-1, 0);      
    }
    
};
