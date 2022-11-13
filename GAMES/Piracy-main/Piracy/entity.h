//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once


template <typename T> 
struct s_nibbles
{
    union {
        struct {
            T lo : sizeof(T)*4;
            T hi : sizeof(T)*4;
        };
        T value;
    }; 
    constexpr s_nibbles() : value(0) {}  
    constexpr s_nibbles(T num) : value(num) {}  
    s_nibbles(T numlo, T numhi) { lo = numlo; hi = numhi; }
    s_nibbles& operator = (const s_nibbles& b) { value = b.value; return *this; }
    s_nibbles& operator = (T b) { value = b; return *this; }
};


struct Entity
{
    s_nibbles<t_size> position; // x, y
    s_nibbles<t_size> data;     // type, parm
    t_size next;                // next index in bucket, or collision index alternatively

    Entity& operator = (const Entity& ent) {
        position = ent.position;
        data = ent.data;
        next = ent.next;
        return *this;
    }
};

struct Bucket 
{
    s_nibbles<uint16_t> item; // country, first_entity
};

typedef const uint8_t t_entities;
typedef Bucket t_buckets[buckets_h][buckets_w];


// Include map data here
#include "mapdata.h"


class Entities
{
    private:
        const t_size _size;
        Entity _localcopy;
        t_size _current_entity;

    public:  
        Entities() : _size(sizeof(mapdata::entities) / sizeof(Entity)) {}

        t_size size() const { return _size; }
        
        void getHead(t_size x, t_size y) {
            Bucket bucket = mapdata::buckets[y][x];
            _current_entity = bucket.item.lo;
        }

        bool more() const {
            return _current_entity != 0;
        }

        const Entity& collect() { 
            get(_current_entity-1);
            _current_entity = _localcopy.next;
            return _localcopy; 
        }

        const Entity& get(t_size index) {
            uint16_t addr = index * sizeof(Entity);
            _localcopy.position.value = IMP_readbyte(&mapdata::entities[addr]); 
            _localcopy.data.value = IMP_readbyte(&mapdata::entities[addr]+1); 
            _localcopy.next = IMP_readbyte(&mapdata::entities[addr]+2); 
            _current_entity = _localcopy.next;
            return _localcopy;
        }

        template <class T> void loop(T& obj, t_size bucket_ox, t_size bucket_oy, 
            void (T::*func)(const Entity&, t_size, t_size), t_size buckets = 2) {  
        
            for (int8_t y=0; y<buckets; y++) {
            for (int8_t x=0; x<buckets; x++) {  
                t_size bucketSumX = bucket_ox + x;
                t_size bucketSumY = bucket_oy + y;                
                t_size bucketX = mod(bucketSumX, buckets_w);
                t_size bucketY = mod(bucketSumY, buckets_h);                                     
                getHead(bucketX, bucketY);     
                                                                             
                while (more()) {
                    const Entity& entity = collect();
                    (obj.*func)(entity, bucketSumX, bucketSumY);
                }
            }}
        }
};
