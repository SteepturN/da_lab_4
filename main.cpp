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
#define RELEASE_VERSION

using WordType = unsigned;
using PositionType = unsigned long;
using CountType = unsigned;
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


        CountType patterns_count;
        CountType first_jokers;
        std::vector< PositionType > add_pos;

        AKTry( std::string& );
        void find_all_entries( std::vector< std::vector< AKTry::Entry > >& );
        void forward( AKTNode*& cur_state, WordType cur_word, Pattern& pattern );
        AKTNode* start();
        void other_solution( Pattern& );
        bool only_jokers();
        bool empty();

        void _print();
        void print( AKTNode*, int, WordType );
    private:
        void add( AKTNode* &cur_node, WordType word, PositionType length );
        void pattern_found( AKTNode* p, CountType number );
        void create_links();

        AKTNode* first_node;
        // unsigned pattern_size;
};
// 0 ... 2 ^ 32 - 1
struct AKTry::AKTNode {
    std::map< WordType, AKTry::AKTNode* > next;
    AKTNode* link;
    AKTNode* exit_link;
    std::vector< CountType > number;
    PositionType length;
    AKTNode();
};
struct AKTry::Entry {
    Entry( PositionType, PositionType );
    PositionType line;
    PositionType position;
};
AKTry::Entry::Entry( PositionType line, PositionType position )
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
    Pattern( bool f, AKTNode* c )
        : found( f ), cur_solution( c ) {}
    Pattern()
        : found( false ), cur_solution( nullptr ) {}
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
    PositionType _add_pos = 0, length = 0;
    CountType cur_pattern_number = 0;
    while( ( pattern >> word ) && ( word == "?" ) ) {
        ++first_jokers;
    }
// 1 ? ? ?

    if( pattern ) {
        ++_add_pos;
        add( cur_node, static_cast< WordType >( std::stoul( word ) ), ++length );
        while( pattern >> word ) {
            if( word == "?" ) {
                // joker_pos.push_back( i );
                if( cur_node != first_node ) {
                    pattern_found( cur_node, cur_pattern_number++ ); // add pattern_number here
                    cur_node = first_node;
                    length = 0;
                }
            } else {
                if( cur_node == first_node ) {
                    add_pos.push_back( _add_pos ); // add this new pattern
                    _add_pos = 0;
                }
                add( cur_node, static_cast< WordType >( std::stoul( word ) ), ++length );
            }
            ++_add_pos;
        }
        add_pos.push_back( _add_pos );
        if( word != "?" ) {
            pattern_found( cur_node, cur_pattern_number++ ); // add pattern_number here
        }
        create_links();
    }
    patterns_count = cur_pattern_number;
}
void AKTry::create_links() {
    struct Links {
        AKTNode* parent;
        AKTNode* child;
        WordType word;
        Links( AKTNode* p, AKTNode* c, WordType w )
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
void AKTry::pattern_found( AKTNode* p, CountType number ) {
    p->number.push_back( number );
}
void AKTry::add( AKTNode *&cur_node, WordType word, PositionType _length ) {
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
bool AKTry::empty() {
    return ( patterns_count == 0 ) && ( first_jokers == 0 );
}
void AKTry::forward( AKTNode*& cur_state, WordType cur_word, Pattern& pattern ) {
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
    return ( first_jokers != 0 ) && ( add_pos.size() == 0 ) && ( patterns_count == 0 );
}

void AKTry::_print() {
    if( first_node == nullptr ) return;
    for( auto next = first_node->next.begin(); next != first_node->next.end(); ++next ) {
        print( next->second, 0, next->first );
    }
}
void AKTry::print( AKTNode* node, int height, WordType word ) {
    if( node == nullptr ) return;
    int i = 0;
    while( i++ < height ) {
        std::cout << '-';
    }
    std::cout << word << '\t' << "[ ";
    for( auto word : node->number ){
        std::cout << word << ':' << "add_pos = " << add_pos[ word ] << ";\t";
    }
    std::cout << "]\tlength = " << node->length
              << "\tlink = " << node->link
              << "\texitlink = " << node->exit_link << std::endl;
    for( auto next = node->next.begin(); next != node->next.end(); ++next ) {
        print( next->second, height + 1, next->first );
    }
}

void shift( std::vector< AKTry::Entry >& line_positions, PositionType position );
int result_f( const std::vector< std::vector< PositionType > >& entries,
            CountType pattern_num, std::vector< PositionType >& cur_positions,
            AKTry& tree );

AKTry::Entry get_entry( PositionType left_boundary,
                        const std::vector< PositionType >& line_pos,
                        PositionType position );
int main() {

    std::cin.tie( 0 );
    std::ios_base::sync_with_stdio( false );
    std::cout.tie( 0 );


    std::string pattern_str;
    std::getline( std::cin, pattern_str, '\n' );
    if( pattern_str.empty() ) return 0;
    AKTry tree( pattern_str );
    // tree._print();
    if( tree.empty() ) return 0;

    char ch = EOF;
    CountType first_jokers = tree.first_jokers;
    std::vector< std::vector< PositionType > > entries( tree.patterns_count );
    std::vector< AKTry::Entry > pattern_pos( 1, AKTry::Entry( 0, 0 ) );
                                             // just for easy algorithm
    std::vector< PositionType > line_positions( 1, 0 );
        // last_jokers;
    AKTry::AKTNode* cur_state = tree.start();
    AKTry::Pattern pattern_found;
    PositionType cur_pos = 0;
    WordType cur_word;
   

    while( cur_pos < first_jokers ) {
        while( std::cin.get( ch ) && ( ch == ' ' ) );
        if( std::cin.eof() ) {
            return 0;
        } else if ( !std::cin ) {
            std::cerr << "error reading\n";
            exit( 1 );
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
        } else if ( !std::cin ) {
            std::cerr << "error reading\n";
            exit( 1 );
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
        if( first_jokers == 0 ) return 0;
        PositionType left = 0,
            right = line_positions.back() - first_jokers + 1;
// ? ? ?
// 0 1 2 3 4 5 _ ( последний элемент line_positions указывает на элемент за последним )
        PositionType cur_on_line = 0;
        PositionType right_on_line = *( line_positions.begin() + 1 ) -
            *line_positions.begin();
        PositionType cur_line = 0;
        while( left < right ) {
            if( cur_on_line < right_on_line ) {
                std::cout << cur_line + 1
                          << ", " << cur_on_line + 1 << '\n';
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
        if( tree.patterns_count == 0 ) return 0;
        pattern_pos.erase( pattern_pos.begin() ); //for algorithm
        std::vector< PositionType > cur_positions( tree.patterns_count, 0 );
        bool out_of_patterns = false, success = false;
        if( entries.empty() ) {
            out_of_patterns = true;
        } else {
            for( auto& entry : entries ) {
                if( entry.empty() ) {
                    out_of_patterns = true;
                    break;
                }
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
            for( PositionType i = 0; i < entries[ 0 ].size(); ++i ) {
                 std::cout << pattern_pos[ i ].line + 1
                           << ',' << ' ' << pattern_pos[ i ].position + 1 << '\n';
            }
            out_of_patterns = true;
        }
        while( !out_of_patterns ) { // error somewhere here
            success = true;
            for( CountType pattern_num = 0; pattern_num < tree.patterns_count - 1; ) {
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
                std::cout << pattern_pos[ cur_positions[ 0 ] ].line + 1
                          << ", " << pattern_pos[ cur_positions[ 0 ] ].position + 1 << '\n';
                ++cur_positions[ 0 ];
                if( cur_positions[ 0 ] >= entries[ 0 ].size() ) {
                    out_of_patterns = true;
                }
            }
        }
    }
    return 0;
}
int result_f( const std::vector< std::vector< PositionType > >& entries,
              CountType pattern_num, std::vector< PositionType >& cur_positions,
              AKTry& tree ){
    return entries[ pattern_num ][ cur_positions[ pattern_num ] ]
        + tree.add_pos[ pattern_num ]
        -  entries[ pattern_num + 1 ][ cur_positions[ pattern_num + 1 ] ];
}
// 1 2 3 ? ? ?
// 0 0 0 1 2 3 4 5 6
AKTry::Entry get_entry( PositionType first_left_edge,
                        const std::vector< PositionType >& line_pos,
                        PositionType position ) {
    PositionType left_edge = first_left_edge,
        right_edge = line_pos.size() - 1, middle;

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
                if( ( middle == first_left_edge )
                    || ( position > line_pos[ --middle ] ) ) {
                    find_this.line = middle;
                    break;
                }
                right_edge = middle; // I reduce by 1 in -4 lines
            }
        } else {
            find_this.line = middle;
            break;
        }
    }
    while( ( find_this.line < line_pos.size() - 1 ) &&
           ( line_pos[ find_this.line ] == line_pos[ find_this.line + 1 ] ) ) {
        ++find_this.line;
    }
    find_this.position = position - line_pos[ find_this.line ];
    // ? 3
    // 1 2 3 4 \n 2 3 4 \n 3 \n
    //              $
    // 5 - 4 = 1
    return find_this;
}
