#include <iostream>
#include <inttypes.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>

#define RELEASE_VERSION

struct entry {
    unsigned line;
    unsigned position;
};

class AKTry {
    public:
        unsigned patterns_count;
        AKTry();
        void find_all_entries( std::vector< std::vector< entry > >& );
    private:
        AKTNode* first_node;
        std::vector< unsigned > add_pos;
        unsigned pattern_size;
        struct AKTNode;
};
// 0 ... 2 ^ 32 - 1
struct AKTry::AKTNode {
    std::map< unsigned, AKTry::AKTNode* > next;
    AKTNode* link;
    int pattern_number;
    unsigned length;
    AKTNode();
};
AKTry::AKTNode::AKTNode()
    : next( { nullptr } ), link( nullptr ) {}
AKTry::AKTry( std::string& _pattern )
    : patterns_count( 0 ) {
    std::istringstream pattern( std::move( _pattern ) );
    std::string word;
    unsigned uword;
    AKTNode* cur_node = first_node;
    for( unsigned i = 0, prev = 0; i < pattern.size(); ++i ) {
        pattern >> word;
        if( word == "?" ) {
            joker_pos.push_back( i );
            if( cur_node != first_node ) {
                pattern_found( cur_node ); // add pattern_number here
                add_pos.push_back( cur_pos - cur_node.size - prev ); // add this new pattern
            }
            cur_node = first_node;
        } else {
            uword = std::stoi( str );
            add( cur_node, uword );
        }
    }
}
void AKTry::add( AKTNode* cur_node, char ch ) {
    AKTNode *&cur_ref = cur_node->next[ ch - 'a' ];
    if( cur_ref == nullptr ) {
        cur_ref = new AKTNode;
    }
    cur_node = cur_ref;
}

int main() {
    char ch = EOF;
    unsigned size_of_pattern;
    std::string pattern_str;
    std::getline( std::cin, pattern_str, '\n' );
    AKTry tree( pattern_str );
    std::vector< std::vector< entry > > entries( tree.patterns_count );
    // can be multiple words in line
    std::vector< entry > line_positions;
    // tree.find_all_entries( entries );
    AKTry::state cur_state = tree.start();
    AKTry::pattern pattern;
    for( unsigned cur_pos = 0, line_pos = 0, cur_line = 0, cur_word;; ) {
        ch = std::getc( stdin );
        if( ch == EOF ) {
            break;
        } else if( ch == '\n' ) {
            ++cur_line;
            if( cur_state != tree.start() ) {
                line_pos = cur_pos + 1;
                line_positions.push_back( entry( cur_line, line_pos ) );
            }
        } else {
            ++cur_pos;
            std::ungetc( stdin, ch );
            std::cin >> cur_word;
            tree.forward( cur_state, cur_word, pattern );
            while( pattern.found ) {
                unsigned position = cur_pos - pattern.length;
                shift( line_positions, position );
                unsigned line = line_positions.front().line;
                entries[ pattern.number ].push_back( entry( cur_line, cur_pos ) );
                tree.other_solution( cur_state, pattern );
            }
            if( pattern.swithched ) {
                shift( line_positions, cur_pos - pattern.length );
                size_of_pattern = tree.size_of_pattern( cur_state );
            }
        }
    }
    // generate and write answers
    return 0;
}

void shift(  ) {
    for( auto entry = line_positions.begin();; ) {
        if( entry->position == position ) {
            line = entry->line;
            line_positions.erase( line_positions.begin(), entry );
        } else if( entry->position > position ) {
#ifndef RELEASE_VERSION
            if( entry == line_positions.begin() ) exit( 1 );
#endif
            --entry;
            line = entry->line;
            line_positions.erase( line_positions.begin(), entry );
        } else if( !( ++entry < line_positions.end() ) ) {
            line = ( line_positions.end() - 1 )->line;
            line_positions.erase( line_positions.begin(), line_positions.end() - 1 );
        }
    }
}
