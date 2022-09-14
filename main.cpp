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

#include <vector>
#include <queue>
#include <deque>
#include <map>
#include <string>
#include <memory>
#include <sstream>
#define RELEASE_VERSION


class AKTry {
    public:
        struct Pattern;
        struct AKTNode;
        struct Entry;

        unsigned patterns_count;
        std::vector< unsigned > add_pos;

        AKTry( std::string& );
        void find_all_entries( std::vector< std::vector< AKTry::Entry > >& );
        void forward( AKTNode*& cur_state, unsigned cur_word, Pattern& pattern );
        AKTNode* start();
        void other_solution( Pattern& );
        bool only_jokers();
    private:
        void add( AKTNode* &cur_node, unsigned word, unsigned length );
        void pattern_found( AKTNode* p, unsigned number );
        void create_links();
        AKTNode* first_node;
        // unsigned pattern_size;
};
// 0 ... 2 ^ 32 - 1
struct AKTry::AKTNode {
    std::map< unsigned, AKTry::AKTNode* > next;
    AKTNode* link;
    AKTNode* exit_link;
    unsigned number; // 2^32 - 1 - maximum of pattern;
    unsigned length;
    AKTNode();
};
struct AKTry::Entry {
    Entry( unsigned, unsigned );
    unsigned line;
    unsigned position;
};
AKTry::Entry::Entry( unsigned line, unsigned position )
    : line( line ), position( position ) {}
// struct AKTry::State {
//     State( AKTNode* s )
//         : cur( s ) {}
//     bool operator==( State rhs ) {
//         return cur == rhs.cur;
//     }
//     AKTNode* cur;
// };
struct AKTry::Pattern {
    bool found;
    // bool switched;
    // unsigned length;
    // unsigned number;
    AKTNode* cur_solution;
};

AKTry::AKTNode::AKTNode()
    : link( nullptr ), exit_link( nullptr ), number( -1 ), length( 0 ) {}
AKTry::AKTry( std::string& _pattern )
    : patterns_count( 0 ), first_node( new AKTNode ) {
    std::istringstream pattern( std::move( _pattern ) );
    std::string word;
    AKTNode* cur_node = first_node;
    unsigned _add_pos = 0, cur_pattern_number = 0, length = 0;

    while( ( pattern >> word ) && ( word == "?" ) ) {
        ++_add_pos;
    }
    add_pos.push_back( _add_pos );
    _add_pos = 0;


    if( pattern ) {
        add( cur_node, static_cast< unsigned >( std::stoul( word ) ), ++length );
        while( pattern >> word ) {
            if( word == "?" ) {
                // joker_pos.push_back( i );
                if( cur_node != first_node ) {
                    pattern_found( cur_node, cur_pattern_number++ ); // add pattern_number here
                    cur_node = first_node;
                    _add_pos = 0;
                    length = 0;
                }
                ++_add_pos;
            } else {
                if( cur_node == first_node ) {
                    add_pos.push_back( _add_pos ); // add this new pattern
                    _add_pos = 0;
                }
                add( cur_node, static_cast< unsigned >( std::stoul( word ) ), ++length );
            }
        }

        add_pos.push_back( _add_pos );
    }

    create_links();
}
void AKTry::create_links() {
    struct Links {
        AKTNode* parent;
        AKTNode* child;
        unsigned word;
        Links( AKTNode* p, AKTNode* c, unsigned w )
            : parent( p ), child( c ), word( w ) {}
    };
    std::queue< Links > queue;
    for( auto& node : first_node->next ) {
        node.second->link = first_node;
        for( auto child_node = node.second->next.begin();
             child_node != node.second->next.end(); ++child_node ) {
            queue.push( Links( node.second, child_node->second, child_node->first ) );
        }
    }
    // Links cur_child;
    while( !queue.empty() ) {
        auto [ cur_node, cur_child, word ] = queue.front();
        queue.pop();
        cur_node = cur_node->link;
        while( true ) {
            if( cur_node->next.count( word ) ) {
                cur_child->link = cur_node->next[ word ];
                if( cur_child->link->number != -1 ) {
                    cur_child->exit_link = cur_child->link;
                } else {
                    cur_child->exit_link = cur_child->link->exit_link;
                }
                break;
            } else if( cur_node == first_node ) {
                cur_child->link = first_node;
                break;
            } else {
                cur_node = cur_node->link;
            }
        }
        for( auto child_node = cur_child->next.begin();
             child_node != cur_child->next.end(); ++child_node ) {
            queue.push( Links( cur_child, child_node->second, child_node->first ) );
        }
    }
    // мне так-то надо переходить по ссылкам, пока я не дойду до ссылки выхода
    // но даже она одна не гарантирует, что она будет только одна, хотя,
    // уже должна быть ссылка выхода на предыдущем шаге, так как там где-то есть суффикс от
    // текущего паттерна, а значит, все возможные паттерны, входящие в этот суффикс до этого уже
    // были заклеймены
    //
    // при поиске я буду должен просто идти по ссылкам выхода и забирать все готовые паттерны,
    // пока не дойду до ссылки выхода, равной нуллпоинтеру
}
void AKTry::pattern_found( AKTNode* p, unsigned number ) {
    p->number = number;
}
void AKTry::add( AKTNode *&cur_node, unsigned word, unsigned _length ) {
    if( /* cur_node->next.empty() || */ !cur_node->next.contains( word ) ) {
        cur_node = cur_node->next[ word ] = new AKTNode;
        cur_node->length = _length;
    } else {
        cur_node = cur_node->next[ word ];
    }
}
AKTry::AKTNode* AKTry::start() {
    return first_node;
}
void AKTry::forward( AKTNode*& cur_state, unsigned cur_word, Pattern& pattern ) {
    while( true ) {
        if( cur_state->next.contains( cur_word ) ) {
            cur_state = cur_state->next[ cur_word ];
            if( cur_state->number != -1 ) {
                pattern.found = true;
                pattern.cur_solution = cur_state;
            } else {
                pattern.found = false;
            }
            break;
        } else if( cur_state == first_node ) {
            pattern.found = false;
            break;
        } else {
            cur_state = cur_state->link;
        }
    }
}
void AKTry::other_solution( Pattern& pattern ) {
    if( pattern.cur_solution->exit_link != nullptr ) {
        pattern.cur_solution = pattern.cur_solution->exit_link;
        pattern.found = true;
    } else {
        pattern.found = false;
    }
}
bool AKTry::only_jokers() {
    return add_pos.size() == 1;
}
void shift( std::vector< AKTry::Entry >& line_positions, unsigned position );
int main() {
    char ch = EOF;
    // unsigned size_of_pattern;
    std::string pattern_str;
    std::getline( std::cin, pattern_str, '\n' );
    AKTry tree( pattern_str );
    std::vector< std::deque< AKTry::Entry >/* std::vector< AKTry::Entry > */ > entries( tree.patterns_count );
    // can be multiple words in line
    std::deque< AKTry::Entry > line_positions( 1, AKTry::Entry( 0, 0 ) ),
    last_jokers;
    while( cur_pos < tree.add_pos.begin() ){
        while( std::cin.get( ch ) && ( ch == ' ' ) );

        if( std::cin.eof() ) {

            // std::cout << "eof\n";

            break;

        } else if( ch == '\n' ) {

            // std::cout << "newline\n";

            // if( cur_state == tree.start() ) {

            //     line_positions.pop_back();

            // }

            ++cur_line;

            line_positions.push_back( AKTry::Entry( cur_line, cur_pos ) );

        } else { //word

            // std::cout << "newword\n";

            std::cin.unget();

            std::cin >> cur_word;

            

            tree.forward( cur_state, cur_word, pattern_found );

            if( pattern_found.found ) {

                shift( line_positions, cur_pos - pattern_found.cur_solution->length + 1 );

            }

            while( pattern_found.found ) {

                unsigned position = cur_pos - pattern_found.cur_solution->length + 1;

                

                // unsigned line = line_positions.front().line;

                entries[ pattern_found.cur_solution->number ].push_back(

                    AKTry::Entry( line_positions.front().line, position - line_positions.front().position )

                );

                tree.other_solution( pattern_found );

            }

            // if( pattern_found.switched ) {

            //     shift( line_positions, cur_pos - pattern_found.length );

            //     // size_of_pattern = tree.size_of_pattern( cur_state );

            // }

            ++cur_pos;

        }
    }
        

    // tree.find_all_entries( entries );
    AKTry::AKTNode* cur_state = tree.start();
    AKTry::Pattern pattern_found;
    unsigned cur_pos = 0, cur_line = 0;
    for( unsigned cur_word;; ) {
        while( std::cin.get( ch ) && ( ch == ' ' ) );
        if( std::cin.eof() ) {
            // std::cout << "eof\n";
            break;
        } else if( ch == '\n' ) {
            // std::cout << "newline\n";
            // if( cur_state == tree.start() ) {
            //     line_positions.pop_back();
            // }
            ++cur_line;
            line_positions.push_back( AKTry::Entry( cur_line, cur_pos ) );
        } else { //word
            // std::cout << "newword\n";
            std::cin.unget();
            std::cin >> cur_word;
            
            tree.forward( cur_state, cur_word, pattern_found );
            if( pattern_found.found ) {
                shift( line_positions, cur_pos - pattern_found.cur_solution->length + 1 );
            }
            while( pattern_found.found ) {
                unsigned position = cur_pos - pattern_found.cur_solution->length + 1;
                
                // unsigned line = line_positions.front().line;
                entries[ pattern_found.cur_solution->number ].push_back(
                    AKTry::Entry( line_positions.front().line, position - line_positions.front().position )
                );
                tree.other_solution( pattern_found );
            }
            // if( pattern_found.switched ) {
            //     shift( line_positions, cur_pos - pattern_found.length );
            //     // size_of_pattern = tree.size_of_pattern( cur_state );
            // }
            ++cur_pos;
        }
    }
    line_positions.push_back( AKTry::Entry( cur_line, cur_pos ) );


    
    if( tree.only_jokers() ) {
        unsigned left = 0,
            right = line_positions.back().position - tree.add_pos.front() + 1;
        unsigned cur_on_line = 0;
        unsigned right_on_line = ( line_positions.begin() + 1 )->position - 
            line_positions.begin()->position;
        unsigned cur_line = 0;
        /*if( line_positions.size() == 1 )
            right_on_line = line_positions.front().position;
       
        else
            right_on_line*/
        while( left < right ) {
            if( cur_on_line < right_on_line ) {
                std::cout << cur_line
                          << ',' << cur_on_line << '\n';
                ++cur_on_line;
                ++left;
            } else {
                ++cur_line;
                right_on_line = line_positions[ cur_line + 1 ].position - 

            line_positions[ cur_line ].position
                cur_on_line = 0;
            }
        }
    } else {
        std::vector< unsigned > cur_positions( tree.patterns_count, 0 );
        bool out_of_patterns = false, success = false;
        for( auto& entry : entries ) {
            if( entry.empty() ) {
                out_of_patterns = true;
                break;
            }
        }
        while( !out_of_patterns ) {
            if( tree.add_pos.front() > entries.front().front().position error) {
                entries.front().pop_front();
            } else {
                break;
            }
            if( entries.front().size() == 0 ) {
                out_of_patterns = true;
            }
        }
        while( !out_of_patterns ) {
            if( tree.add_pos.back() > line_positions.back().position
                - entries.back().back().position ) {
                entries.back().pop_back();
            } else {
                break;
            }
            if( entries.back().size() == 0 ) {
                out_of_patterns = true;
            }
        }
        while( !out_of_patterns ) {
            success = true;
            for( int pattern_num = 0; pattern_num < tree.patterns_count - 1; ++pattern_num ) {
                // unsigned cur_pattern = ;
                // unsigned next_pattern = ;
                int result = entries[ pattern_num ][ cur_positions[ pattern_num ] ].position
                    - ( entries[ pattern_num + 1 ][ cur_positions[ pattern_num + 1 ] ].position
                        + tree.add_pos[ pattern_num + 1 ] );
                // 0 - first jokers,
                // i + 1 - add position between i and i + 1 pattern
                if( result > 0 ) {
                    if( cur_positions[ pattern_num + 1 ] < entries[ pattern_num + 1 ].size() ) {
                        ++cur_positions[ pattern_num + 1 ];
                    } else {
                        out_of_patterns = true;
                        success = false;
                    }
                    break;
                } else if( result < 0 ) {
                    if( cur_positions[ pattern_num ] < entries[ pattern_num ].size() ) {
                        ++cur_positions[ pattern_num ];
                    } else {
                        out_of_patterns = true;
                        success = false;
                    }
                    break;
                }
            }
            if( success ) {
                std::cout << entries.front().front().line
                          << ',' << entries.front().front().position << '\n';
            }
        }
    }
    // generate and write answers
    // if first and last elements are jokers like "? ? ? 123 ? ? ?"
    // I just add this like counts of first jokers and last jokers as elements of
    // vector: first and last accordingly
    return 0;
}

void shift( std::deque< AKTry::Entry >& line_positions, unsigned position ) {
    
    for( auto cur_iter = line_positions.begin();; ) {
        if( line_positions.size() < 2 ) return;
        int result = ( ++cur_iter )->position - position;
        if( result < 0 ) {
            line_positions.pop_front();
        } else if( result == 0 ) {
            line_positions.pop_front();

            return;
        } else {
            return;
        }

    }
}
