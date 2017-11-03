#ifndef CHESSBOARD_WIDGET_HPP
#define CHESSBOARD_WIDGET_HPP
#include "chess_engine.hpp"
#include "move.hpp"
#include "player.hpp"
#include "player_human.hpp"
#include "player_random_ai.hpp"
#include "position.hpp"

#include <cppurses/cppurses.hpp>
#include <optional/optional.hpp>
#include <signals/signals.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

using namespace cppurses;

class Chessboard_widget : public Widget {
   public:
    Chessboard_widget();

    void toggle_show_moves();
    void reset_game();
    void trigger_next_move();
    void make_move(const Move& move);
    Side current_side() const;

    template <typename T, typename... Args>
    void set_player(Side side, Args... args) {
        if (side == Side::Black) {
            player_black_ =
                std::make_unique<T>(engine_, std::forward<Args>(args)...);
        } else if (side == Side::White) {
            player_white_ =
                std::make_unique<T>(engine_, std::forward<Args>(args)...);
        }
    }

    template <typename Rule_t, typename... Args>
    void set_ruleset(Args&&... args);

    // Signals
    sig::Signal<void(Move)> move_made;
    sig::Signal<void(Piece)> capture;
    sig::Signal<void(const Move&)> invalid_move;
    sig::Signal<void(Side)> checkmate;
    sig::Signal<void(Side)> check;
    sig::Signal<void()> board_reset;

   protected:
    bool paint_event() override;
    bool mouse_press_event(Mouse_button button,
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;

   private:
    Chess_engine engine_;
    opt::Optional<Position> first_position_;
    opt::Optional<Position> selected_position_;
    bool show_moves_{false};

    std::unique_ptr<Player> player_black_{
        std::make_unique<Player_human>(engine_)};
    std::unique_ptr<Player> player_white_{
        std::make_unique<Player_human>(engine_)};

    cppurses::Color get_tile_color(Position p);
};

// Template Implementations - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename Rule_t, typename... Args>
void Chessboard_widget::set_ruleset(Args&&... args) {
    engine_.set_ruleset<Rule_t>(std::forward<Args>(args)...);
}

namespace slot {

sig::Slot<void()> toggle_show_moves(Chessboard_widget& cbw);
sig::Slot<void()> reset_game(Chessboard_widget& cbw);
sig::Slot<void(const Move&)> trigger_next_move(Chessboard_widget& cbw);
sig::Slot<void(Move)> make_move(Chessboard_widget& cbw);

template <typename Player_t, typename... Args>
sig::Slot<void()> set_player(Chessboard_widget& cbw,
                             Side side,
                             Args&&... args) {
    sig::Slot<void()> slot{[&cbw, side, args...] {
        cbw.set_player<Player_t>(side, std::forward<Args>(args)...);
    }};
    slot.track(cbw.destroyed);
    return slot;
}

template <typename Rule_t, typename... Args>
sig::Slot<void()> set_ruleset(Chessboard_widget& cbw, Args&&... args) {
    sig::Slot<void()> slot{[&cbw, args...] {
        cbw.set_ruleset<Rule_t>(std::forward<Args>(args)...);
    }};
    slot.track(cbw.destroyed);
    return slot;
}

}  // namespace slot

#endif  // CHESSBOARD_WIDGET_HPP
