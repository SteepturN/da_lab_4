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
    first_line = ""
    variants = [ "space", "digit", "digit", "digit", "digit", "newline" ]
    pattern_variants = [ "space", "digit", "digit", "digit", "newline", "?", "?" ]
    action = ""
    prev_action = ""
    number_size = 0
    number = 0
    abc = 0
    random.seed()
    with open( sys.argv[ 1 ], 'w' ) as output_file:
        while action != "newline":
            prev_action = action
            action = random.choice( pattern_variants )
            if action == "space":
                number_size = 0
                first_line += " "
            elif action == "digit":
                if prev_action == "?":
                    first_line += " "
                elif number_size == 9:
                    continue
                number = random.randint(0, 9)
                first_line += "{0}".format( number )
                if not( ( number_size == 0 ) and ( number == 0 ) ):
                    number_size += 1
            elif action == "newline":
                number_size = 0
                first_line += "\n"
            elif action == "?":
                if ( prev_action == "digit") or (prev_action == "?"):
                    first_line += " "
                number_size = 0
                first_line += "?"
            if action == "newline":
                break
        output_file.write( "{0}".format( first_line ) )
        for _ in range( random.randint( 0, count_of_tests ** 4 ) ):
            action = random.choice( variants )
            if action == "space":
                number_size = 0
                el = " "
            elif action == "digit":
                if number_size == 9:
                    continue
                number = random.randint(0, 9)
                el = "{0}".format( number )
                if not( ( number_size == 0 ) and ( number == 0 ) ):
                    number_size += 1
            elif action == "newline":
                number_size = 0
                el = "\n"
            output_file.write( "{0}".format( el ) )
