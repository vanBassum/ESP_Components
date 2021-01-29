#pragma once
#include <cstdint>




inline int nmemcpyswapendian(void* dst, const void* src, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[size - i - 1];
    }

    return i;
}


inline int nmemcpy(void* dst, const void* src, int size)
{
	memcpy(dst, src, size);
	return size;
}
