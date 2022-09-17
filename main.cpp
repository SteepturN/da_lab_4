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

// можно хранить энтри линий, находя нужную линию просто взяв первый энтри,
// оттуда линию и потом прибавить нужное количество
//
// может там где add_pos ставить  длину паттерна + длину следующих джокеров
//
// бляяя а если паттерн повторится в строке исходной, что тогда
//        ну длина будет такой же, только номер вот...
class AKTry {
    public:
        struct Pattern;
        struct AKTNode;
        struct Entry;


        unsigned patterns_count;
        unsigned first_jokers;
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
    std::vector< unsigned > number;
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
    : link( nullptr ), exit_link( nullptr ), number(), length( 0 ) {}
AKTry::AKTry( std::string& _pattern )
    : patterns_count( 0 ), first_jokers( 0 ), first_node( new AKTNode ) {
    std::istringstream pattern( std::move( _pattern ) );
    std::string word;
    AKTNode* cur_node = first_node;
    unsigned _add_pos = 0, cur_pattern_number = 0, length = 0;

    while( ( pattern >> word ) && ( word == "?" ) ) {
        ++first_jokers;
    }
    
    _add_pos = 0;


    if( pattern ) {
        add( cur_node, static_cast< unsigned >( std::stoul( word ) ), ++length );
        while( pattern >> word ) {
            if( word == "?" ) {
                // joker_pos.push_back( i );
                if( cur_node != first_node ) {
                    pattern_found( cur_node, cur_pattern_number++ ); // add pattern_number here
                    cur_node = first_node;
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
        // last element has been prepared
        if( word == "?" ) {
            if( cur_node != first_node ) {
                pattern_found( cur_node, cur_pattern_number++ ); // add pattern_number here
                cur_node = first_node;
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




        add_pos.push_back( _add_pos );
        create_links();
    }

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
                if( cur_child->link->number.size() != 0 ) {
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
    p->number.push_back( number );
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
            if( cur_state->number.size() != 0 ) {
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
int result_f( const std::vector< std::vector< unsigned > >& entries,
            unsigned pattern_num, std::vector< unsigned >& cur_positions,
            AKTry& tree );

AKTry::Entry get_entry( unsigned left_boundary,
                        const std::vector< unsigned >& line_pos,
                        unsigned position );
int main() {
    std::string pattern_str;
    std::getline( std::cin, pattern_str, '\n' );
    if( pattern_str.empty() ) return 0;
    AKTry tree( pattern_str );


    char ch = EOF;
    unsigned first_jokers = tree.first_jokers;
    std::vector< std::vector< unsigned > > entries( tree.patterns_count );
    std::vector< AKTry::Entry > pattern_pos( 1, AKTry::Entry( 0, 0 ) );
                                             // just for easy algorithm
    std::vector< unsigned > line_positions( 1, 0 );
        // last_jokers;
    AKTry::AKTNode* cur_state = tree.start();
    AKTry::Pattern pattern_found;
    unsigned cur_pos = 0, cur_word;
   

    while( cur_pos < first_jokers ){
        while( std::cin.get( ch ) && ( ch == ' ' ) );
        if( std::cin.eof() ) {
            return 0;

        } else if( ch == '\n' ) {
            line_positions.push_back( cur_pos );
        } else { //word
            std::cin.unget();
            std::cin >> cur_word;
            ++cur_pos;
        }
    }

    while( true ) {
        while( std::cin.get( ch ) && ( ch == ' ' ) );
        if( std::cin.eof() ) {
            break;
        } else if( ch == '\n' ) {
            line_positions.push_back( cur_pos );
        } else { //word
            std::cin.unget();
            std::cin >> cur_word;
            
            tree.forward( cur_state, cur_word, pattern_found );
// 1 2 3
// 1 0 0 0 1 2 3 4 5
// 0 1 2 3 4 5 6 7 8
            while( pattern_found.found ) {
                for( auto i : pattern_found.cur_solution->number ) {
                    entries[ i ].push_back(
                        cur_pos - pattern_found.cur_solution->length + 1 );
                }
                if( pattern_found.cur_solution->number.front() == 0 ) {
                    pattern_pos.push_back(
                        get_entry( pattern_pos.back().line, line_positions,
                                   cur_pos - first_jokers
                                   - pattern_found.cur_solution->length + 1 )
                    );

// ? ? ? 1 2 3
// 1 0 0 0 1 2 3 4 5
// 0 1 2 3 4 5 6 7 8
                }
                tree.other_solution( pattern_found );
            }
            ++cur_pos;
        }
    }
    line_positions.push_back( cur_pos );

    // 0 в pattern_pos не использовать
    if( tree.only_jokers() ) {
        unsigned left = 0,
            right = line_positions.back() - first_jokers + 1;
// ? ? ?
// 0 1 2 3 4 5
        unsigned cur_on_line = 0;
        unsigned right_on_line = *( line_positions.begin() + 1 ) -
            *line_positions.begin();
        unsigned cur_line = 0;
        while( left <= right ) {
            if( cur_on_line < right_on_line ) {
                std::cout << cur_line
                          << ',' << cur_on_line << '\n';
                ++cur_on_line;
                ++left;
            } else {
                ++cur_line;
                right_on_line = line_positions[ cur_line + 1 ] -
                    line_positions[ cur_line ];
// 1 2 3 \n 0 1 2 3 4 \n 0 1
                cur_on_line = 0;
            }
        }
    } else {
        pattern_pos.erase( pattern_pos.begin() ); //for algorithm
        std::vector< unsigned > cur_positions( tree.patterns_count, 0 );
        bool out_of_patterns = false, success = false;
        for( auto& entry : entries ) {
            if( entry.empty() ) {
                out_of_patterns = true;
                break;
            }
        }
        while( !out_of_patterns ) {
            if( entries[ tree.patterns_count - 1 ].size() == 0 ) {
                out_of_patterns = true;
            } else if( entries[ tree.patterns_count - 1 ].back() +
                       tree.add_pos[ tree.patterns_count - 1 ] - 1
                       >= line_positions.back() ) {
                entries[ tree.patterns_count - 1 ].pop_back();
            } else {
                break;
            }
        }
        // 1 2 ? ? ?
        // 1 2 3 4
        // 0 1 2 3 4
        if( tree.patterns_count == 1 ) {
            // в конце не удалены позиции, которые не являются правильными из-за
            // последних джокеров
            for( unsigned i = 0; i < entries[ 0 ].size(); ++i ) {
                 std::cout << pattern_pos[ i ].line
                           << ',' << pattern_pos[ i ].position << '\n';
            }
            out_of_patterns = true;
        }
        while( !out_of_patterns ) {
            success = true;
            for( unsigned pattern_num = 0; pattern_num < tree.patterns_count - 1; ) {
                int result = result_f( entries, pattern_num, cur_positions, tree );
                if( result > 0 ) { // right is fewer than it should be, I should shift right
                    while( ( ++cur_positions[ pattern_num + 1 ] < entries[ pattern_num + 1 ].size() ) &&
                           ( result_f( entries, pattern_num, cur_positions, tree ) > 0 ) );
                    if( cur_positions[ pattern_num + 1 ] >= entries[ pattern_num + 1 ].size() ) {
                        out_of_patterns = true;
                        success = false;
                        break;
                    } else {
                        continue;
                    }
                } else if( result < 0 ) { // right is bigger than it should be, I should shift left
                    while( ( ++cur_positions[ pattern_num ] < entries[ pattern_num ].size() ) &&
                           ( result_f( entries, pattern_num, cur_positions, tree ) < 0 ) );
                    if( cur_positions[ pattern_num ] >= entries[ pattern_num ].size() ) {
                        out_of_patterns = true;
                    }
                    success = false;
                    break;
                }
                ++pattern_num;
            }
            if( success ) {
                std::cout << pattern_pos[ cur_positions[ 0 ] ].line
                          << ',' << pattern_pos[ cur_positions[ 0 ] ].position << '\n';
                ++cur_positions[ 0 ];
                if( cur_positions[ 0 ] >= entries[ 0 ].size() ) {
                    out_of_patterns = true;
                }
            }
        }
    }
    return 0;
}
int result_f( const std::vector< std::vector< unsigned > >& entries,
              unsigned pattern_num, std::vector< unsigned >& cur_positions,
              AKTry& tree ){
    return entries[ pattern_num ][ cur_positions[ pattern_num ] ] +
           tree.add_pos[ pattern_num ]
        -  entries[ pattern_num + 1 ][ cur_positions[ pattern_num + 1 ] ]
            ;
}
// 1 2 3 ? ? ?
// 0 0 0 1 2 3 4 5 6
AKTry::Entry get_entry( unsigned left_edge,
                        const std::vector< unsigned >& line_pos,
                        unsigned position ) {
    unsigned right_edge = line_pos.size() - 1, middle;
    int comparison;
    AKTry::Entry find_this( 0, 0 );
    while( true ) {
        middle = ( right_edge + left_edge ) / 2;
        comparison = position - line_pos[ middle ];
        if( comparison != 0 ) {
            if( comparison > 0 ) {
                if( ( middle == line_pos.size() - 1 )

                    || ( position < line_pos[ middle + 1 ] ) ) {

                    find_this.line = middle;

                    break;

                }
                left_edge = middle + 1;
            } else if( comparison < 0 ) {
                if( ( middle == 0 )
                    || ( position > line_pos[ --middle ] ) ) {
                    find_this.line = middle;
                    break;
                }
                right_edge = middle;
            }
        } else {
            find_this.line = middle;
            break;
        }
    }
    find_this.position = position - line_pos[ find_this.line ];
    // ? 5
    // 1 2 3 4 \n 2 3 4 \n 3 \n
    //              $
    // 5 - 4 = 1
    return find_this;
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
