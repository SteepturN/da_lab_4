#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <map>
#include <string>
#include <memory>
#include <sstream>

#include <cctype>
#include <algorithm>

#define RELEASE_VERSION
using WordType = std::string;
int main() {

    std::cin.tie( 0 );
    std::ios_base::sync_with_stdio( false );
    std::cout.tie( 0 );
    struct PatternWord {
        WordType word;
        bool joker;
        PatternWord( WordType w, bool j )
            : word( w ), joker( j ) {}
    };
    struct Word {
        unsigned position;
        unsigned line;
        WordType word;
        Word( unsigned p, unsigned l, WordType w )
            : position( p ), line( l ), word( w ) {}
    };

    std::string pattern_str;
    std::getline( std::cin, pattern_str, '\n' );
    if( pattern_str.empty() ) return 0;
    std::stringstream pttrn( std::move( pattern_str ) );
    std::string str_word;
    std::vector< PatternWord > pattern;
    while( pttrn >> str_word ) {
        if( str_word == "?" ) {
            pattern.push_back( PatternWord( 0, true ) );
        } else {
            std::transform( str_word.begin(), str_word.end(), str_word.begin(),
                            []( unsigned char c ){ return std::tolower(c); });
            pattern.push_back(
                PatternWord( str_word, false )
            );
        }
    }
    std::deque< Word > line;
    unsigned cur_line = 0, cur_pos = 0;
    WordType cur_word;
    char ch;
    while( true ) {
        while( std::cin.get( ch ) && ( ch == ' ' ) );
        if( std::cin.eof() ) {
            break;
        } else if( ch == '\n' ) {
            ++cur_line;
            cur_pos = 0;
        } else { //word
            std::cin.unget();
            std::cin >> cur_word;
            std::transform( cur_word.begin(), cur_word.end(), cur_word.begin(),
                            []( unsigned char c ){ return std::tolower(c); });
            line.push_back( Word( cur_pos, cur_line, cur_word ) );
            ++cur_pos;
            if( line.size() == pattern.size() ) {
                bool success = true;
                for( unsigned i = 0; i < line.size(); ++i ) {
                    if( !pattern[ i ].joker && ( pattern[ i ].word != line[ i ].word ) ) {
                        success = false;
                        break;
                    }
                }
                if( success ) {
                    std::cout << line.front().line + 1 << ", "
                              << line.front().position + 1 << '\n';
                }
                line.pop_front();
            }
        }
    }
    return 0;
}
