//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once


#ifdef _SOUND_
namespace sound
{
    typedef const uint8_t t_sound;

    constexpr uint8_t NOTE = 0b10000000;
    constexpr uint8_t WAIT = 0b11000000;
    constexpr uint8_t END = 0;

    constexpr uint8_t DOl  = 0; 
    constexpr uint8_t REl  = 1; 
    constexpr uint8_t MIl  = 2; 
    constexpr uint8_t FAl  = 3; 
    constexpr uint8_t SOLl = 4; 
    constexpr uint8_t LAl  = 5;
    constexpr uint8_t SIl  = 6;

    constexpr uint8_t DO  = 7;
    constexpr uint8_t RE  = 8;
    constexpr uint8_t MI  = 9;
    constexpr uint8_t FA  = 10;
    constexpr uint8_t SOL = 11;
    constexpr uint8_t LA  = 12;
    constexpr uint8_t SI  = 13;

    constexpr uint8_t DOh  = 14;
    constexpr uint8_t REh  = 15;
    constexpr uint8_t MIh  = 16;
    constexpr uint8_t FAh  = 17;
    constexpr uint8_t SOLh = 18;
    constexpr uint8_t LAh  = 19;
    constexpr uint8_t SIh  = 20;
    

    t_sound click[] = {
        NOTE|DO,1, WAIT|1,
        END    
    };
    
    t_sound select[] = {
        NOTE|FAl,1, NOTE|SOLl,1, NOTE|RE,1, WAIT|2,
        END    
    };
        
    t_sound collect[] = {
        NOTE|REl,2, NOTE|MIl,2, NOTE|FA,2, NOTE|SOL,2, WAIT|2,
        END    
    };

    t_sound not_available[] = {
        NOTE|DOl,2, WAIT|2, NOTE|DOl,2, WAIT|2,
        END    
    };
    
    t_sound intro_a[] = {  
               
        // What shall we do with the drunken sailor
        NOTE|REh,6, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,6, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,2, WAIT|2, 
        NOTE|REh,6, WAIT|2, NOTE|SOL,6, WAIT|2, NOTE|SI,6, WAIT|2, NOTE|REh,6, WAIT|2,
               
        // .. what shall we do with the drunken sailor       
        NOTE|DOh,6, WAIT|2, NOTE|DOh,2, WAIT|2, NOTE|DOh,2, WAIT|2, NOTE|DOh,6, WAIT|2, NOTE|DOh,2, WAIT|2, NOTE|DOh,2, WAIT|2,
        NOTE|DOh,6, WAIT|2, NOTE|FA,6, WAIT|2, NOTE|LA,6, WAIT|2, NOTE|DOh,6, WAIT|2,
        
        // .. what shall we do with the drunken sailor
        NOTE|REh,6, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,6, WAIT|2, NOTE|REh,2, WAIT|2, NOTE|REh,2, WAIT|2, 
        NOTE|REh,6, WAIT|2, NOTE|MIh,6, WAIT|2, NOTE|FAh,6, WAIT|2, NOTE|SOLh,6, WAIT|2,
        
        // .. early in the morning!
        NOTE|FAh,6, WAIT|2,  NOTE|REh,6, WAIT|2, NOTE|DOh,6, WAIT|2, NOTE|LA,6, WAIT|2,
        NOTE|SOL,12, WAIT|4, NOTE|SOL,8, WAIT|8,      
        END
    };
    
    t_sound intro_b[] = {     
        NOTE|REl,4, WAIT|4,  NOTE|LAl,4, WAIT|4,  NOTE|REl,4, WAIT|4,  NOTE|LAl,4, WAIT|4,
        NOTE|REl,4, WAIT|4,  NOTE|SOLl,4, WAIT|4, NOTE|LAl,4, WAIT|4, NOTE|FAl,6, WAIT|2,

        NOTE|DOl,4, WAIT|4,  NOTE|SOLl,4, WAIT|4,  NOTE|DOl,4, WAIT|4,  NOTE|SOLl,4, WAIT|4,
        NOTE|DOl,4, WAIT|4,  NOTE|SOLl,4, WAIT|4,  NOTE|MIl,4, WAIT|4,  NOTE|DOl,4, WAIT|4,

        NOTE|REl,4, WAIT|4,  NOTE|LAl,4, WAIT|4,  NOTE|REl,4, WAIT|4,  NOTE|LAl,4, WAIT|4,
        NOTE|REl,4, WAIT|4,  NOTE|LAl,4, WAIT|4, NOTE|FAl,4, WAIT|4, NOTE|SOLl,6, WAIT|2,

        NOTE|DOl,4, WAIT|4,  NOTE|SOLl,4, WAIT|4,  NOTE|LAl,4, WAIT|4,  NOTE|DOl,4, WAIT|4,
        NOTE|REl,12, WAIT|4, NOTE|RE,6, WAIT|10,       
        END
    };

    t_sound attack_a[] = {
        WAIT|16, NOTE|SOL,2, WAIT|2,  NOTE|LA,2, WAIT|2,  NOTE|SI,2, WAIT|2,  NOTE|REh,2, WAIT|6, 
        NOTE|DOh,2, WAIT|2,  NOTE|REh,8, WAIT|2, 
        END     
    };

    t_sound attack_b[] = {
        WAIT|16, NOTE|DOl,2, WAIT|2,  NOTE|REl,2, WAIT|2,  NOTE|MIl,2, WAIT|2,  NOTE|SOLl,2, WAIT|6, 
        NOTE|FAl,2, WAIT|2,  NOTE|SOLl,8, WAIT|2, 
        END     
    };    
    
};
#endif

namespace image
{

    typedef const uint8_t t_bitmap;  

    t_bitmap PROGMEM ship_frames[] = {
        16, 16,
        0x00, 0x00, 0x00, 0x00, 0x10, 0xf8, 0xf0, 0xe0, 0x02, 0x0a, 0xfc, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 
        0x00, 0x1c, 0x3c, 0x78, 0x50, 0x77, 0x73, 0x51, 0x70, 0x70, 0x57, 0x73, 0x79, 0x38, 0x1c, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf0, 0x02, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x3c, 0x7c, 0x5c, 0x77, 0x53, 0x70, 0x57, 0x73, 0x39, 0x1c, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xf8, 0xfa, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x3c, 0x7c, 0x5b, 0x53, 0x77, 0x73, 0x79, 0x3c, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x08, 0xf8, 0xf8, 0xfe, 0xf8, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x02, 0x3b, 0x73, 0x77, 0x73, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xfc, 0xfa, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x3c, 0x79, 0x73, 0x77, 0x53, 0x5b, 0x7c, 0x3c, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xfc, 0x02, 0xf0, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x1c, 0x39, 0x73, 0x57, 0x70, 0x53, 0x77, 0x5c, 0x7c, 0x3c, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xe0, 0xf0, 0xf8, 0xfc, 0x0a, 0x02, 0xe0, 0xf0, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x1c, 0x38, 0x79, 0x73, 0x57, 0x70, 0x70, 0x51, 0x73, 0x77, 0x50, 0x78, 0x3c, 0x1c, 0x00
    };

    t_bitmap PROGMEM ship_mask_frames[] = {
        0x00, 0x00, 0x00, 0x10, 0xf8, 0xfc, 0xf8, 0xf2, 0xef, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0x00, 
        0x1e, 0x3e, 0x7e, 0xfc, 0xff, 0xff, 0xff, 0xfb, 0xf9, 0xff, 0xff, 0xff, 0xff, 0x7d, 0x3e, 0x1e, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfc, 0xfa, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x3e, 0x7e, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x1e, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x08, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x3e, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3e, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x08, 0xfc, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x02, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x3e, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3e, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x3e, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7e, 0x3e, 0x00, 0x00, 
        0x00, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xef, 0xf2, 0xf8, 0xfc, 0xf8, 0x10, 0x00, 0x00, 0x00, 
        0x1e, 0x3e, 0x7d, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xfb, 0xff, 0xff, 0xff, 0xfc, 0x7e, 0x3e, 0x1e
    };
        
    t_bitmap PROGMEM tiles16[] = {
        16, 16,
        0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0x7f, 0xff, 0xfb, 0xf5, 0xfb, 
        0xff, 0xfd, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 
        0xdf, 0xff, 0xff, 0xfd, 0xbf, 0x5f, 0xbf, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 
        0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xdf, 0xff, 0xff, 0xf7, 0xff, 0xff, 
        0x66, 0x6f, 0x0f, 0x66, 0x60, 0x06, 0xe6, 0xe0, 0xc0, 0xc6, 0x06, 0x30, 0x30, 0x03, 0x1b, 0x18, 
        0x03, 0x63, 0x60, 0x0c, 0x0c, 0xc0, 0xd8, 0x18, 0x66, 0xf6, 0xf0, 0x66, 0x06, 0x60, 0x67, 0x07, 
        0x01, 0x00, 0x08, 0x9c, 0x9c, 0x08, 0x80, 0x8c, 0x0c, 0xe0, 0xe0, 0xe6, 0x06, 0x00, 0x32, 0x30, 
        0x03, 0x03, 0x60, 0x69, 0x01, 0x38, 0x39, 0x01, 0xc0, 0xc0, 0x00, 0x18, 0x18, 0xc1, 0xc1, 0x00, 
        0x00, 0x10, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
        0x08, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 
        0x00, 0x10, 0x00, 0x08, 0x00, 0x04, 0x02, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 
        0x00, 0x08, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x08, 0x00, 0x04, 0x02, 0x04
    };
    
    t_bitmap PROGMEM entities[] = {
        16, 16,
        0x20, 0x00, 0x20, 0x80, 0x70, 0xf8, 0xfc, 0xfa, 0xc4, 0x98, 0x20, 0x40, 0xa0, 0x00, 0x20, 0x00, 
        0x28, 0x0c, 0x1f, 0x1f, 0x1f, 0x1d, 0x5f, 0x1f, 0x1f, 0x1e, 0x5e, 0x18, 0x41, 0x02, 0x24, 0x08, 
        0x7f, 0xbf, 0x5d, 0x2f, 0x97, 0xcb, 0xeb, 0xcb, 0x9b, 0x3b, 0x7b, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 
        0xfe, 0xc0, 0xde, 0xdf, 0xd1, 0xd1, 0xd1, 0xdf, 0xc3, 0xc3, 0xde, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 
        0xe3, 0x09, 0x74, 0x7a, 0x1a, 0x76, 0x0e, 0x0e, 0x0e, 0x0c, 0x09, 0xe3, 0xff, 0xff, 0xff, 0xfb, 
        0xff, 0xef, 0xff, 0xff, 0xe3, 0x09, 0x74, 0x7a, 0x1a, 0x76, 0x0e, 0x0e, 0x0e, 0x0c, 0x09, 0xe3, 
        0xef, 0xf5, 0xf5, 0x13, 0xe7, 0xeb, 0xeb, 0xdf, 0x7f, 0xaf, 0xaf, 0x9f, 0x3f, 0x5f, 0x5d, 0xff, 
        0xf7, 0xff, 0xf0, 0xf5, 0xff, 0xf7, 0xff, 0xff, 0xbf, 0xff, 0x87, 0xa8, 0xff, 0xbf, 0xff, 0xfe, 
        0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xb3, 0x0d, 0x0d, 0xb3, 0xbf, 0xff, 0xff, 0xff, 0xdf, 0xff, 
        0xff, 0xfb, 0xf1, 0xe5, 0xef, 0xcf, 0xcf, 0x80, 0x80, 0xcf, 0xcf, 0xef, 0xe5, 0xf1, 0xfb, 0xff
    };

    t_bitmap PROGMEM pirate[] = {
        16, 16,
        0x00, 0x00, 0x00, 0x20, 0x20, 0x1c, 0xae, 0x6e, 0x66, 0x64, 0x10, 0x00, 0x40, 0xff, 0x4e, 0x00, 
        0x70, 0x30, 0x00, 0x14, 0x15, 0xd5, 0xc5, 0x85, 0x05, 0x00, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x40, 0x40, 0x38, 0x5c, 0xdc, 0xcc, 0xc8, 0x20, 0x00, 0x80, 0xfe, 0x9c, 0x00, 
        0x00, 0x00, 0x00, 0xd8, 0xda, 0x9a, 0x0b, 0x0a, 0x0a, 0x00, 0x06, 0x06, 0x02, 0x03, 0x00, 0x00, 
        0x00, 0x00, 0x20, 0x20, 0x1c, 0xae, 0x6e, 0x66, 0x64, 0x10, 0x00, 0x40, 0xff, 0x4e, 0x00, 0x00, 
        0xc0, 0xc0, 0x94, 0x75, 0x75, 0x45, 0x05, 0x05, 0x00, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 
    };

    t_bitmap PROGMEM defeat[] = {
        34, 8,
        0x81, 0xff, 0xff, 0x81, 0x81, 0xff, 0x7e, 0x00, 0x78, 0xfc, 0x94, 0xdc, 0x58, 0x00, 0xfe, 0xff, 0x09,
        0x01, 0x78, 0xfc, 0x94, 0xdc, 0x58, 0x00, 0x60, 0xf4, 0x94, 0xfc, 0xf8, 0x01, 0x7f, 0xff, 0x84, 0x44, 
    }; 
      
    t_bitmap PROGMEM victory[] = {
        46, 8,
        0x01, 0x1f, 0x3f, 0x60, 0xc0, 0x60, 0x3f, 0x1f, 0x00, 0x84, 0xfd, 0xfd, 0x00, 0x78, 0xfc, 0x84, 0xcc,
        0x48, 0x01, 0x7f, 0xff, 0x84, 0x44, 0x00, 0x78, 0xfc, 0x84, 0xfc, 0x78, 0x00, 0x04, 0xfc, 0xfc, 0x08, 
        0x04, 0x04, 0x00, 0x4c, 0x9c, 0x90, 0xfc, 0x7e, 0x02, 0xc0, 0xdf, 0x07, 
    };

    t_bitmap PROGMEM chest[] = {
        16, 16,
        0xb0, 0xbc, 0xbe, 0xbe, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0x4f, 0x43, 0x21, 0x21, 0x21, 0x12, 0xfc, 
        0x5f, 0x9f, 0x5c, 0x9d, 0x5d, 0x9c, 0x5f, 0x9f, 0x5f, 0x90, 0x50, 0x28, 0x48, 0x28, 0x14, 0x07 
    };    
        
    t_bitmap* lose_win[] = { defeat, victory };

    t_bitmap PROGMEM railing[] = {
        12, 8,
        0x80, 0x80, 0x80, 0x80, 0x42, 0x7e, 0x7e, 0x42, 0x80, 0x80, 0x80, 0x80, 
        0x82, 0x42, 0xaa, 0x42, 0x82, 0x42, 0x82, 0x42, 0xaa, 0x42, 0x82, 0x42 
    };

    t_bitmap PROGMEM sextant[] = {
        16, 16,
        0x00, 0x00, 0x00, 0x80, 0x60, 0x16, 0xf9, 0xf9, 0xd6, 0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
        0x18, 0x18, 0x36, 0x31, 0x62, 0x64, 0xff, 0xff, 0x64, 0x62, 0x31, 0x36, 0x1a, 0x18, 0x00, 0x00 
    };

    t_bitmap PROGMEM compass[] = {
        13, 14,
        0x80, 0x80, 0xe8, 0x10, 0x08, 0xce, 0x4f, 0xce, 0x08, 0x10, 0xe8, 0x80, 0x80, 
        0x00, 0x00, 0x0b, 0x04, 0x08, 0x09, 0x38, 0x09, 0x08, 0x04, 0x0b, 0x00, 0x00
    };

    t_bitmap PROGMEM marker_frames[] = {
        8, 8,
        0x00, 0x00, 0x00, 0x08, 0x1c, 0x08, 0x00, 0x00, 
        0x00, 0x00, 0x08, 0x14, 0x22, 0x14, 0x08, 0x00
    };

    t_bitmap PROGMEM skull[] = {
        16, 16,
        0x00, 0x00, 0x00, 0x00, 0x3e, 0x4f, 0xcf, 0xcf, 0xff, 0xcf, 0xcf, 0x4f, 0x3e, 0x00, 0x00, 0x00, 
        0x00, 0xcc, 0xcc, 0x44, 0x28, 0x28, 0x29, 0x10, 0x11, 0x10, 0x29, 0x28, 0x28, 0x44, 0x66, 0x66
    };

    t_bitmap PROGMEM generic_mask[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };
        
    // Create these in dynamic memory 
    
    uint8_t PROGMEM title[36] = {
        0x41, 0x7f, 0x7f, 0x49, 0x09, 0x0f, 0x06, 0x40, 0x7a, 0x7b, 0x41, 0x00, 0x44, 0x7c, 0x7c, 0x44, 0x0c, 0x08, 
        0x20, 0x74, 0x54, 0x7c, 0x78, 0x00, 0x38, 0x7c, 0x44, 0x6c, 0x28, 0x00, 0x9c, 0xbc, 0xa0, 0xfc, 0x7e, 0x02
    };
    
};
