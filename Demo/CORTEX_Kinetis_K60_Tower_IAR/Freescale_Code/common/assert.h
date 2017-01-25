/*
 * File:        assert.h
 * Purpose:     Provide macro for software assertions
 *
 * Notes:       assert_failed() defined in assert.c
 */

#ifndef _ASSERT_H_
#define _ASSERT_H_

/********************************************************************/

void assert_failed(char *, int);

#ifdef DEBUG_PRINT
#define ASSERT(expr) \
    if (!(expr)) \
        assert_failed(__FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif

/********************************************************************/
#endif /* _ASSERT_H_ */

