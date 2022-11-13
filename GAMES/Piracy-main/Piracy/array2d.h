//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once


template <typename T, t_size W, t_size H> 
class array2D
{    
    private:  
        t_size _headX;
        t_size _headY;
        T _data[H][W];
    
    public:
        static constexpr t_size width = W;
        static constexpr t_size height = H;
            
        array2D() : _headX(0), _headY(0) {}
        
        T get(t_size x, t_size y) const { 
            return _data[(_headY + y) % height][(_headX + x) % width]; 
        }
        
        void set(t_size x, t_size y, T value) { 
            _data[(_headY + y) % height][(_headX + x) % width] = value; 
        }     
        
        void rotate(int8_t dx, int8_t dy) {
            if (dx) _headX = mod((_headX + dx), width);
            if (dy) _headY = mod((_headY + dy), height);
        }
};
