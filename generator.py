#! /usr/bin/env python3

# -*- coding: utf-8 -*-

import sys
import random
import string

if __name__ == "__main__":
    # Проверяем, что передали 2 аргумент.
    if len(sys.argv) != 3:
        print( "Usage: {0} <path to test file> <count of tests>".format( sys.argv[0] ) )
        sys.exit(1)

    count_of_tests = int( sys.argv[2] )
    random.seed()
    with open( sys.argv[ 1 ], 'w' ) as output_file:
        for _ in range( random.randrange( 0, count_of_tests, 1 ) ):
            word = "{0}".format( random.randint(0, 2**32 - 1) )\
                if not random.choice( [ True, False ] ) else "?"
            output_file.write( "{0} ".format( word ) )
        output_file.write( "\n" )
        for _ in range( random.randrange( 0, count_of_tests ** 2, 1 ) ):
            word = "{0}".format( random.randint(0, 2**32 - 1) )\
                if not random.choice( [ True, False ] ) else "\n"
            output_file.write( "{0} ".format( word ) )
