//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#include "math2d.h"


namespace actors
{
    constexpr math2d::t_unit max_velocity = 0.4; 
        
    uint16_t max_x;
    uint16_t max_y;

    
    void setBounds(uint16_t bx, uint16_t by) {
        max_x = bx;
        max_y = by; 
    }

                       
    struct s_flags 
    {
        enum e_state { ready, active, sinking, dead };

        e_state state;
        bool delayed180;
        uint8_t timeout;
        s_flags() : state(e_state::ready), delayed180(false), timeout(0) {} 
    };

    
    class Actor
    {
        private:
            math2d::t_vector _position;
            math2d::t_angle _angle;
            math2d::t_unit _velocity;
            math2d::t_unitvec _direction;
            int8_t _turn_dir;

        public:
            s_flags flags;
            uint8_t info;
                    
            Actor() : _angle(0), _velocity(0.0), _turn_dir(0), info(0) {}
    
            math2d::t_vector position() const { return _position; }        
            math2d::t_unit velocity() const { return _velocity; }
            math2d::t_angle angle() const { return _angle; }
            
            const math2d::t_unitvec& direction() {
                _direction = math2d::angle_to_vector(_angle); 
                return _direction;
            }
            
            bool valid_bucket(const math2d::t_vector& pos) { 
                int8_t bx = (static_cast<uint16_t>(pos.x) + screen_center_x) / bucket_res_x; 
                int8_t by = (static_cast<uint16_t>(pos.y) + screen_center_y) / bucket_res_y;
                bool inside = bx>=0 && bx<buckets_w && by>=0 && by<buckets_h;          
                return inside && (mapdata::buckets[by][bx].item.hi & 0b01000000) > 0;  
            }

            void init(const math2d::t_vector& ppos, bool isEnemy = true) {                 
                _position = ppos;
                if (isEnemy) {
                    _angle = math2d::rnd()%360;
                    _velocity = 0.3; 
                }                                                           
                flags.state = s_flags::e_state::active;
                flags.timeout = 0;        

                uint8_t rnd = math2d::rnd();
                uint8_t type = rnd & 0b00000011;
                uint8_t crew = 4 + type*2 + ((rnd>>2) % 20);
                info = type | (crew<<2);                 
            }
           
            void update(math2d::t_fixed velocity_multiplier = 1.0) { 
                if (flags.delayed180) { 
                    flags.delayed180=false; 
                    turn(180); 
                }                 
                _position += direction() * _velocity * velocity_multiplier;
                if (_position.x < 0) _position.x += max_x; else if (_position.x >= max_x) _position.x -= max_x;               
                if (_position.y < 0) _position.y += max_y; else if (_position.y >= max_y) _position.y -= max_y;
            }
            
            void navigate() { 
                if (_turn_dir) {
                    _turn_dir += _turn_dir<0 ? 1 : -1;
                    turn(math2d::sgn(_turn_dir));                    
                }               
                if (flags.timeout) {
                    if (--flags.timeout == 0) {
                        if (flags.state==s_flags::e_state::sinking) {
                            flags.state = s_flags::e_state::ready;                 
                        }
                    }
                }     
                if (flags.state==s_flags::e_state::active) {                      
                    math2d::t_vector newpos = _position + direction() * _velocity; 
                    if (valid_bucket(newpos)) {
                        _position = newpos;
                        if (!_turn_dir) {
                            int16_t rnd = math2d::rnd(); 
                            if (!(rnd&0xff00)) _turn_dir = (rnd%240) - 120; 
                        }
                    }
                    else {
                        _turn_dir = 127;                         
                    } 
                }      
            }

            void setPosition(const math2d::t_vector& pos) {
                _position = pos;              
            }

            void addPosition(const math2d::t_vector& vec) {
                _position += vec;    
            }

            void setVelocity(math2d::t_unit val) {
                _velocity = static_cast<float>(val);
            }           
            
            void addVelocity(math2d::t_unit val) {
                _velocity += static_cast<float>(val);
                if (_velocity < 0.02) _velocity = 0.02;
                else if (_velocity > max_velocity) _velocity = max_velocity;
            }
    
            void turn(int16_t deg) {
                _angle = mod<uint16_t>(_angle + deg, 360);
            }

            void bounceTo(const math2d::t_vector& pos) {
                setVelocity(0.05);
                setPosition(pos);
                flags.delayed180 = true;
            }
    };

};
