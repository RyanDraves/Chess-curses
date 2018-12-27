#include <cppurses/cppurses.hpp>

#include "chess_ui.hpp"

int main() {
    using namespace cppurses;
    System sys;

    Chess_UI w;

    Tree::set_head(&w);
    return sys.run();
}
