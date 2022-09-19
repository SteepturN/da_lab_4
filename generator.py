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
    el = ""
    elements = []
    variants = [ "space", "digit", "digit", "digit", "digit", "newline" ]
    action = ""
    random.seed()
    with open( sys.argv[ 1 ], 'w' ) as output_file:
        for _ in range( random.randint( 0, 10000 ) ):
            if random.choice( [ True, False, False, False, False ] ):
                action = random.choice( variants )
                if action == "space":
                    el = " "
                elif action == "digit":
                    el = "{0}".format( random.randint(0, 9) )
                elif action == "newline":
                    el = "\n"
            else:
                el = " ? "
            output_file.write( "{0}".format( el ) )
            if action == "newline":
                break
        for _ in range( random.randint( 0, count_of_tests ** 4 ) ):
            action = random.choice( variants )
            if action == "space":
                el = " "
            elif action == "digit":
                el = "{0}".format( random.randint(0, 9) )
            elif action == "newline":
                el = "\n"
            output_file.write( "{0}".format( el ) )
