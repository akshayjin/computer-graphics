#ifndef VECTOR4_H
#define VECTOR4_H

/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#pragma once

class Vector4	{
public:
    Vector4(void) {
        x = y = z = w = 1.0f;
    }
    Vector4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    ~Vector4(void){}

    float x;
    float y;
    float z;
    float w;
};

#endif // VECTOR4_H