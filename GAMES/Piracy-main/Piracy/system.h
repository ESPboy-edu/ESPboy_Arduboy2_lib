//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once


namespace input
{
    struct s_keys {
        uint8_t up : 1;
        uint8_t down : 1;
        uint8_t left : 1;
        uint8_t right : 1;
        uint8_t a : 1;
        uint8_t b : 1;
        uint8_t unused : 2;
    }; 
    
    s_keys keys;               
    uint8_t keys_last = 0;


    void update(bool repeat) {
        uint8_t current_key_bits = IMP_buttonStates();
        uint8_t key_bits = current_key_bits;
        uint8_t key_bits_mask = (current_key_bits ^ keys_last) & current_key_bits;            
        if (!repeat) key_bits = key_bits_mask;
        
        keys.up = (key_bits & IMP_keyup) > 0;
        keys.down = (key_bits & IMP_keydown) > 0;
        keys.left = (key_bits & IMP_keyleft) > 0;
        keys.right = (key_bits & IMP_keyright) > 0;                       
        keys.a = (key_bits_mask & IMP_keyA) > 0;
        keys.b = (key_bits_mask & IMP_keyB) > 0;        
        keys_last = current_key_bits;          
    }
};



namespace store
{
    typedef void (*t_copyfunc)(uint16_t, void*, uint16_t);
    typedef uint16_t t_header;
    
    constexpr uint16_t offs_header = 0;
    constexpr uint16_t offs_player = sizeof(t_header);
    constexpr uint16_t offs_ships = offs_player + sizeof(object::player);
    constexpr uint16_t offs_misc = offs_ships + sizeof(object::ships);
    constexpr uint16_t offs_stats = offs_misc + sizeof(logic::misc);
    constexpr uint16_t offs_tools = offs_stats + sizeof(logic::stats);
    constexpr uint16_t offs_inventory = offs_tools + sizeof(logic::tools);
    constexpr uint16_t offs_buckets = offs_inventory + sizeof(logic::inventory);

    t_header header_template = 0xBEEF;
        

    void read(uint16_t offs, void* target, uint16_t size) {
        for (uint16_t i=0; i<size; i++) {
            *(reinterpret_cast<uint8_t*>(target)+i) = IMP_storeRead(offs+i); 
        }   
    }

    void write(uint16_t offs, void* src, uint16_t size) {
        for (uint16_t i=0; i<size; i++) {           
            IMP_storeWrite(offs+i, *(reinterpret_cast<uint8_t*>(src)+i));  
        }  
        EEPROM.commit();
    }

    void setVariables(t_copyfunc func) {
        func(offs_player, &object::player, sizeof(object::player));
        func(offs_ships, &object::ships, sizeof(object::ships));
        func(offs_misc, &logic::misc, sizeof(logic::misc));
        func(offs_stats, &logic::stats, sizeof(logic::stats));       
        func(offs_tools, &logic::tools, sizeof(logic::tools)); 
        func(offs_inventory, &logic::inventory, sizeof(logic::inventory)); 
        func(offs_buckets, &mapdata::buckets, sizeof(mapdata::buckets)); 
    }

    
    void loadGame() {
        t_header header;
        read(offs_header, &header, sizeof(t_header)); 
        if (header == header_template) setVariables(read);
    }
    
    void saveGame() {              
        write(offs_header, &header_template, sizeof(t_header));
        setVariables(write);
    }   

    void clear() {
        IMP_storeWrite(offs_header, static_cast<t_header>(0));  
        EEPROM.commit();
    }

};


#ifdef _SOUND_
namespace soundman
{ 

    // Note frequencies
    
    constexpr uint16_t NOTE_C2  = 65;
    constexpr uint16_t NOTE_CS2 = 69;
    constexpr uint16_t NOTE_D2  = 73;
    constexpr uint16_t NOTE_DS2 = 78;
    constexpr uint16_t NOTE_E2  = 82;
    constexpr uint16_t NOTE_F2  = 87;
    constexpr uint16_t NOTE_FS2 = 93;
    constexpr uint16_t NOTE_G2  = 98;
    constexpr uint16_t NOTE_GS2 = 104;
    constexpr uint16_t NOTE_A2  = 110;
    constexpr uint16_t NOTE_AS2 = 117;
    constexpr uint16_t NOTE_B2  = 123;
    constexpr uint16_t NOTE_C3  = 131;
    constexpr uint16_t NOTE_CS3 = 139;
    constexpr uint16_t NOTE_D3  = 147;
    constexpr uint16_t NOTE_DS3 = 156;
    constexpr uint16_t NOTE_E3  = 165;
    constexpr uint16_t NOTE_F3  = 175;
    constexpr uint16_t NOTE_FS3 = 185;
    constexpr uint16_t NOTE_G3  = 196;
    constexpr uint16_t NOTE_GS3 = 208;
    constexpr uint16_t NOTE_A3  = 220;
    constexpr uint16_t NOTE_AS3 = 233;
    constexpr uint16_t NOTE_B3  = 247;
    constexpr uint16_t NOTE_C4  = 262;
    constexpr uint16_t NOTE_CS4 = 277;
    constexpr uint16_t NOTE_D4  = 294;
    constexpr uint16_t NOTE_DS4 = 311;
    constexpr uint16_t NOTE_E4  = 330;
    constexpr uint16_t NOTE_F4  = 349;
    constexpr uint16_t NOTE_FS4 = 370;
    constexpr uint16_t NOTE_G4  = 392;
    constexpr uint16_t NOTE_GS4 = 415;
    constexpr uint16_t NOTE_A4  = 440;
    constexpr uint16_t NOTE_AS4 = 466;
    constexpr uint16_t NOTE_B4  = 494;
    constexpr uint16_t NOTE_C5  = 523;
    constexpr uint16_t NOTE_CS5 = 554;
    constexpr uint16_t NOTE_D5  = 587;
    constexpr uint16_t NOTE_DS5 = 622;
    constexpr uint16_t NOTE_E5  = 659;
    constexpr uint16_t NOTE_F5  = 698;
    constexpr uint16_t NOTE_FS5 = 740;
    constexpr uint16_t NOTE_G5  = 784;
    constexpr uint16_t NOTE_GS5 = 831;
    constexpr uint16_t NOTE_A5  = 880;
    constexpr uint16_t NOTE_AS5 = 932;
    constexpr uint16_t NOTE_B5  = 988;
    constexpr uint16_t NOTE_C6  = 1047;
    constexpr uint16_t NOTE_CS6 = 1109;
    constexpr uint16_t NOTE_D6  = 1175;
    constexpr uint16_t NOTE_DS6 = 1245;
    constexpr uint16_t NOTE_E6  = 1319;
    constexpr uint16_t NOTE_F6  = 1397;
    constexpr uint16_t NOTE_FS6 = 1480;
    constexpr uint16_t NOTE_G6  = 1568;
    constexpr uint16_t NOTE_GS6 = 1661;
    constexpr uint16_t NOTE_A6  = 1760;
    constexpr uint16_t NOTE_AS6 = 1865;
    constexpr uint16_t NOTE_B6  = 1976;


    // Scale (D major)
    constexpr uint16_t scale[] = {
        NOTE_D3, NOTE_E3, NOTE_FS3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_CS4,
        NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_CS5,
        NOTE_D5, NOTE_E5, NOTE_FS5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_CS6,
    };

    
    constexpr uint8_t frames_per_beat = 2;  
    sound::t_sound const* current_stream[2] = { 0, 0 };
    uint16_t timers[2] = { 0, 0 };

    
    void play(sound::t_sound const* stream, uint8_t channel = 0) {
        current_stream[channel] = stream; 
        timers[channel] = 1;  
    }

    void stop() {
        IMP_noSound(); 
        timers[0] = 0;
        timers[1] = 0;   
    }
    
    void update() {    
        for (uint8_t i=0; i<2; i++) {  
            if (timers[i] && --timers[i] == 0) {
                uint8_t nextval = IMP_readbyte(current_stream[i]++);
                uint8_t comm = (nextval & 0b11000000) >> 6;
                uint8_t parm = nextval & 0b00111111;

                if (comm == 2) {
                    uint8_t nextnextval = IMP_readbyte(current_stream[i]++);
                    timers[i] = (nextnextval & 0b00111111) * frames_per_beat;
                    IMP_playSound(i, scale[parm]);
                } 
                else if (comm == 3) {
                    timers[i] = parm * frames_per_beat;
                    IMP_stopSound(i);
                }
            }
        }
    }
};
#endif
  
