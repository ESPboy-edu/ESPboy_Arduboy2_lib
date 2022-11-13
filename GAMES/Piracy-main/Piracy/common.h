//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#define STRING(list,index) strings::get(strings::names_##list, (index))

#ifdef _SOUND_ 
#define SOUND_PLAY(stream,ch) soundman::play((stream),(ch))
#define SOUND_STOP() soundman::stop()
#define SOUND_UPDATE() soundman::update()
#else
#define SOUND_PLAY(stream,ch) 
#define SOUND_STOP()
#define SOUND_UPDATE()
#endif


typedef uint8_t t_size;

constexpr uint16_t map_width = 256;
constexpr uint16_t map_height = 128;

constexpr t_size tile_size = 16;
constexpr t_size tiles_w = (IMP_screenW / tile_size) + 1;
constexpr t_size tiles_h = (IMP_screenH / tile_size) + 1;

constexpr int8_t buckets_w = 16;
constexpr int8_t buckets_h = 8;
constexpr t_size bucket_size_w = map_width / buckets_w;
constexpr t_size bucket_size_h = map_height / buckets_h;

constexpr uint16_t max_pos_x = map_width * tile_size;
constexpr uint16_t max_pos_y = map_height * tile_size;
constexpr uint16_t bucket_res_x = bucket_size_w * tile_size;
constexpr uint16_t bucket_res_y = bucket_size_h * tile_size;

constexpr t_size screen_center_x = IMP_screenW / 2;
constexpr t_size screen_center_y = IMP_screenH / 2;
constexpr t_size ship_hotspot_x = 8;
constexpr t_size ship_hotspot_y = 12;


template <typename T = t_size, typename U> constexpr T mod(int a, U b) {              
    if (!b) return(a);
    else return (a%b + b) % b;
}
