//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#pragma once

#ifndef IMP_HAS_MATH
#include <cmath>
#endif
#include "FixedPoints/FixedPoints.h"


namespace math2d 
{

    // Default float type    
    typedef double t_float;


    //
    // Free math helpers
    //

    template <typename T> constexpr T root2() {
        return static_cast<T>(1.4142135624);
    }
    
    template <typename T> constexpr T pi() {
        return static_cast<T>(3.1415926536);
    }
    
    template <typename T> constexpr T sqr(T n) {
        return n * n;
    }
    
    template <typename T> constexpr T sgn(T n) {
        return n<0 ? -1 : 1;
    }
    
    template <typename T> constexpr T rad(T n) {
        return (n / static_cast<T>(180.0)) * pi<T>();
    }
    
    template <typename T> constexpr T deg(T n) {
        return (n / pi<T>()) * static_cast<T>(180.0);
    }

    
    //
    // Random
    //
    
    uint16_t hash(uint16_t src) {
        src += src << 10;
        src ^= src >> 6;
        src += src << 3;
        src ^= src >> 11;
        return src;        
    }
    
    uint16_t rnd() {
        static uint16_t seed = 0x5a56;
        return seed = hash(seed);
    }    

    
    //
    // 2D Vector
    //
    
    template <typename T>
    struct vector
    {
        typedef T t_type;
    
        T x; 
        T y;
    
        vector() : x(0), y(0) {}
        
        template <typename U, typename V> vector(U pX, V pY)
            : x(static_cast<T>(pX)), y(static_cast<T>(pY)) {}
           
        template <typename U> vector(const vector<U>& v2)
            : x(static_cast<T>(v2.x)), y(static_cast<T>(v2.y)) {}
    
        template <typename U> bool operator == (const vector<U>& v2) const {
            T epsilon = 0.0001;
            bool xr = (abs(x - static_cast<T>(v2.x)) > epsilon);
            bool yr = (abs(y - static_cast<T>(v2.y)) > epsilon);
            return xr && yr;
        }
    
    
        // These return new vectors
     
        template <typename U> vector operator + (const vector<U>& v2) const {
            T xr = x + static_cast<T>(v2.x);
            T yr = y + static_cast<T>(v2.y);
            return vector(xr, yr);
        }
       
        template <typename U> vector operator - (const vector<U>& v2) const {
            T xr = x - static_cast<T>(v2.x);
            T yr = y - static_cast<T>(v2.y);
            return vector(xr, yr);
        }
       
        template <typename U> vector operator * (const U sc) const {
            T xr = x * static_cast<T>(sc);
            T yr = y * static_cast<T>(sc);
            return vector(xr, yr);
        }
       
        template <typename U> vector operator / (const U sc) const {
            T xr = x / static_cast<T>(sc);
            T yr = y / static_cast<T>(sc);
            return vector(xr, yr);
        }
    
        // These modify the vector
      
        template <typename U> vector& operator = (const vector<U>& v2) {
            x = static_cast<T>(v2.x);
            y = static_cast<T>(v2.y);
            return *this;
        }
       
        template <typename U> vector& operator += (const vector<U>& v2) {
            x += static_cast<T>(v2.x);
            y += static_cast<T>(v2.y);
            return *this;
        }
       
        template <typename U> vector& operator -= (const vector<U>& v2) {
            x -= static_cast<T>(v2.x);
            y -= static_cast<T>(v2.y);
            return *this;
        }
       
        template <typename U> vector& operator *= (const U sc) {
            x *= static_cast<T>(sc);
            y *= static_cast<T>(sc);
            return *this;
        }
       
        template <typename U> vector& operator /= (const U sc) {
            x /= static_cast<T>(sc);
            y /= static_cast<T>(sc);
            return *this;
        }
    
        // These return new vectors
       
        template <typename U> vector cross(const vector<U>& v2) const {
            T xr = x * static_cast<T>(v2.y);
            T yr = y * static_cast<T>(v2.x);
            return vector(xr, yr);
        }
       
        template <typename U> U dot(const vector<U>& v2) const {
            T xr = x * static_cast<T>(v2.x);
            T yr = y * static_cast<T>(v2.y);
            return xr + yr;
        }
        
        template <typename U> U distanceSqr(const vector<U>& v2) const {
            T xr = x - static_cast<T>(v2.x);
            T yr = y - static_cast<T>(v2.y);
            return sqr(xr) + sqr(yr);
        }
    };


    //
    // Angle vectors
    //
    
    typedef SFixed<16,15> t_fixed;
    typedef SFixed<1,6> t_unit;  
    typedef uint16_t t_angle; 
    
    typedef vector<t_unit> t_unitvec;
    typedef vector<t_fixed> t_vector;

    const t_unitvec angle_vectors[] = {
        {1.0000000, 0.0000000}, {0.9914452, 0.13052351}, {0.96592722, 0.2588138}, 
        {0.9238826, 0.3826760}, {0.8660308, 0.49999065}, {0.79336155, 0.6087507}, 
        {0.7071182, 0.7070953}, {0.6087764, 0.79334184}, {0.50001869, 0.8660146}, 
        {0.3827059, 0.9238702}, {0.2588451, 0.96591884}, {0.13055562, 0.9914410}
    };
    
    constexpr uint8_t angles_tablesize = sizeof(angle_vectors) / sizeof(t_unitvec); 
    constexpr t_fixed degs_per_segment = 90.0 / angles_tablesize;


    t_unitvec angle_to_vector(t_angle angle) {
        uint8_t quadrant = angle / 90;
        uint8_t segment = static_cast<uint8_t>((angle - quadrant*90) / degs_per_segment); 
        t_unitvec vec = angle_vectors[segment]; 
        if (quadrant < 2) vec.y = -vec.y;
        if (quadrant == 1 || quadrant == 2) vec.x = -vec.x;
        if (quadrant == 1 || quadrant == 3) vec = t_vector(vec.y, vec.x);
        return vec;                          
    }

};
