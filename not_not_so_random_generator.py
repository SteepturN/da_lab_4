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
    variants = [ "exist", "random" ]
    random.seed()
    with open( sys.argv[ 1 ], 'w' ) as output_file:
        for _ in range( random.randint( 0, 10 ) ):
            if not random.choice( [ True, False ] ):
                el = ""
                for _ in range( random.randint( 0, 3 ) ):
                    el += "{0}".format( random.randint(0, 9) )
                random_num = random.randint( 0, 2**5 )
                elements.append( el.zfill( random_num ) )
            else:
                el = "? "
            for _ in range( random.randint( 0, 2**6 - 1 ) ):
                el += " "
            output_file.write( "{0}".format( el ) )
        output_file.write( "\n" )
        for _ in range( random.randint( 0, count_of_tests ** 2 ) ):
            action = random.choice( variants )
            if action == "exist":
                word = "{0}".format( random.choice( elements ) \
                                     if not len( elements ) == 0 \
                                     else random.randint( 0, 10 ) )
                random_num = random.randint( 0, 2**5 )
                word = word.zfill( random_num )
            elif action == "random":
                word = ""
                for _ in range( random.randint( 0, 3 ) ):
                    word += "{0}".format( random.randint(0, 9) )
                random_num = random.randint( 0, 2**5 )
                word = word.zfill( random_num )
            for _ in range( random.randint( 0, 2**5 ) ):
                word += " "
            for _ in range( random.randint( 0, 2**5 ) ):
                word += "\n"
            output_file.write( "{0}".format( word ) )
