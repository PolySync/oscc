/************************************************************************/
/* The MIT License (MIT)                                                */
/* =====================                                                */
/*                                                                      */
/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the “Software”), to deal in the Software without              */
/* restriction, including without limitation the rights to use,         */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell    */
/* copies of the Software, and to permit persons to whom the            */
/* Software is furnished to do so, subject to the following             */
/* conditions:                                                          */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        */
/* OTHER DEALINGS IN THE SOFTWARE.                                      */
/************************************************************************/




/**
 * @file terminal_print.c
 * @brief Debug printer functions.
 *
 */




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "terminal_print.h"




// *****************************************************
// static global veriables
// *****************************************************


static char lines[ MAX_LINES ][ LINE_SIZE ];


static int num_lines = 0;


static int last_lines = 0;




// *****************************************************
// static definitions
// *****************************************************


//
static void print_new_line( const char * name )
{
    printf( "%s\n", name );
}




// *****************************************************
// public definitions
// *****************************************************


//
void add_line( char * line )
{
    strcpy( lines[ num_lines ], line );

    num_lines++;
}


//
void print_lines()
{
    int i;

    for( i = 0; i < last_lines; i++ )
    {
        fputs( "\033[A\033[2K", stdout );

        rewind( stdout );

        ftruncate( 1, 0 );
    }

    last_lines = 0;

    for( i = 0; i < num_lines; i++ )
    {
        print_new_line( lines[ i ] );

        last_lines++;
    }

    num_lines = 0;
}
