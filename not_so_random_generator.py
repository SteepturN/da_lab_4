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
    elements = []
    variants = [ "newline", "exist" ]
    random.seed()
    with open( sys.argv[ 1 ], 'w' ) as output_file:
        for _ in range( random.randint( 0, 10 ) ):
            el = random.randint(0, 2**32 - 1)
            elements.append( el )
            word = "{0}".format( el )\
                if not random.choice( [ True, False ] ) else "?"
            output_file.write( "{0} ".format( word ) )
        output_file.write( "\n" )
        for _ in range( random.randint( 0, count_of_tests ** 2 ) ):
            action = random.choice( variants )
            if action == "newline":
                word = "\n"
            elif action == "exist":
                word = "{0}".format( random.choice( elements ) \
                                     if not len( elements ) == 0 \
                                     else random.randint( 0, 10 ) )
            output_file.write( "{0} ".format( word ) )
