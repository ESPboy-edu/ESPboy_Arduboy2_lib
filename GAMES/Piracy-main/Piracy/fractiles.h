//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#include "array2d.h"
#include "math2d.h"


typedef SFixed<6,9> t_fract;
typedef math2d::vector<t_fract> t_vector;
typedef math2d::vector<t_size> t_coords;
typedef t_size t_tile;

    
namespace fract
{
      
    struct s_fractal 
    {
        const t_size depth = 13;
        const float zoom = 180.0;
        const t_vector origin = t_vector(-1.31, -0.91);
        const t_vector seed = t_vector(1.2, 0.4);
    };
    
    
    template <t_size W, t_size H> 
    class Fractiles
    {
        private:
            t_coords _position;
            t_coords _mapsize;
            const s_fractal _fractal;
            array2D<t_tile, W, H> _localmap;
             
            t_tile tile_at(t_size x, t_size y) const {
                t_fract cx = _fractal.origin.x + x / _fractal.zoom;
                t_fract cy = _fractal.origin.y + y / _fractal.zoom;
                t_fract zx = _fractal.seed.x, zy = _fractal.seed.y;
                t_fract xt = 0;
                uint8_t i = 0;
                do {                   
                    xt = zx * zy;
                    zx = zx*zx - zy*zy + cx;
                    zy = 2*xt + cy;
                } 
                while (++i<_fractal.depth && (zx*zx + zy*zy) < 4);
                return i;
            }
              
            void refresh_local() {
                for (t_size y=0; y<_localmap.height; y++) {
                for (t_size x=0; x<_localmap.width; x++) {
                    t_size sx = mod(_position.x + x, _mapsize.x);
                    t_size sy = mod(_position.y + y, _mapsize.y);
                    Serial.println("REFESH LOCAL IN PROGRESS");
                    _localmap.set(x, y, tile_at(sx, sy));        
                }}
            }
        
        public:           
            void init(uint16_t w, uint16_t h) {
                _mapsize = t_coords(w, h);
                _position = t_coords(0, 0);
                Serial.println("BEFORE REFESH LOCAL");
                refresh_local();
                Serial.println("AFTER REFESH LOCAL");
            }
    
            const t_coords& getPosition() const {
                return _position;
            }
            
            const s_fractal& getFractal() const {
                return _fractal;
            }
    
            t_tile getTile(t_size x, t_size y) const {
                return tile_at(x,y); 
            }
            
            t_tile getLocalTile(t_size x, t_size y) const {
                return _localmap.get(x,y); 
            }
    
            void scroll(int8_t dx, int8_t dy) {
                _position.x = mod((_position.x + dx), _mapsize.x);
                _position.y = mod((_position.y + dy), _mapsize.y);
                
                if (abs(dx)>1 || abs(dy)>1) {
                    refresh_local();
                    return;
                }          
                _localmap.rotate(dx, dy);          
                if (dx != 0) {            
                    int8_t xpos = static_cast<int8_t>(dx<0 ? 0 : _localmap.width-1);
                    for (int8_t y=0; y<_localmap.height; y++) {
                        t_size sx = mod(_position.x + xpos, _mapsize.x);
                        t_size sy = mod(_position.y + y, _mapsize.y);
                        _localmap.set(xpos, y, tile_at(sx, sy));        
                    }
                }
                if (dy != 0) {              
                    int8_t ypos = (dy<0) ? 0 : _localmap.height-1;
                    for (int8_t x=0; x<_localmap.width; x++) { 
                        t_size sx = mod(_position.x + x, _mapsize.x);
                        t_size sy = mod(_position.y + ypos, _mapsize.y);
                        _localmap.set(x, ypos, tile_at(sx, sy));        
                    }
                }
            }
    };

};
