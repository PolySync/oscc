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
