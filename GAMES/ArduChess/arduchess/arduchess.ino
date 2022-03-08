#include <SPI.h>
//#include <EEPROM.h>
#include <Arduboy2.h>
#include <Arduino.h>

#include "vars.hpp"
#include "title.hpp"
#include "font.hpp"
#include "graphics.hpp"
#include "save.hpp"

#define STACK_CANARY_VAL 0x77
extern char* __bss_end;
extern uint8_t __stack;



void ISRfunc()
{  
    switch(state)
    {
    case STATE_TITLE_START:
        render_title_part(TITLE_IMG + 0);
        paint_left_half(false);
        ta = 0;
        state = STATE_TITLE;
    case STATE_TITLE:
        render_title_part(TITLE_IMG + 64);
        render_title_text();
        paint_right_half(true);
        break;
    //case STATE_HELP_START:
    //    render_help_left();
    //    paint_left_half(true);
    //    render_help_right();
    //    paint_right_half(true);
    //    state = STATE_HELP;
    //    break;
    case STATE_NEW_GAME_START:
        render_board();
        paint_left_half(true);
        ta = 0;
        ailevel[0] = 0;
        ailevel[1] = 1;
        state = STATE_NEW_GAME;
    case STATE_NEW_GAME:
        render_new_game_menu();
        paint_right_half(true);
        break;
    case STATE_AI_START:
        render_board();
        paint_left_half(true);
        render_game_info();
        paint_right_half(false);
        state = STATE_AI;
        break;
    case STATE_AI:
        poll_buttons_during_rendering();
        if(just_pressed(B_BUTTON))
            g.stop_ = true;
        render_ai_progress();
        paint_right_half(g.stop_);
        break;
    case STATE_HUMAN_START:
        render_game_info();
        paint_right_half(true);
        state = STATE_HUMAN;
        // fallthrough
    case STATE_HUMAN:
    case STATE_HUMAN_MOVE:
        render_board();
        if(state == STATE_HUMAN_MOVE)
        {
            uint8_t r = (ta >> 4) * 8;
            uint8_t c = (ta & 7) * 8;
            if(rotated) r = 56 - r, c = 56 - c;
            render_solid_cursor(r, c);
        }
        render_cursor(rcy, rcx);
        rcx = rcx == cx + 1 ? cx : (cx + rcx + 1) / 2;
        rcy = rcy == cy + 1 ? cy : (cy + rcy + 1) / 2;
        paint_left_half(true);
        break;
    case STATE_HUMAN_PROMOTION:
        render_board();
        render_promotion_menu();
        {
            uint8_t x = tc * 8 + 16;
            rcx = rcx == x + 1 ? x : (x + rcx + 1) / 2;
            render_cursor(24, rcx);
        }
        paint_left_half(true);
        break;
    case STATE_ANIM:
    case STATE_ANIM_UNDO:
    case STATE_ANIM_UNDO2:
        if(nframe == 0)
        {
            clear_buf();
            render_game_info();
            paint_right_half(true);
        }
        if(nframe == 15)
            update_board_cache();
        render_board();
        if(nframe < 15)
            render_anim();
        paint_left_half(true);
        if(nframe == 15)
        {
            if(state == STATE_ANIM)
                update_game_state_after_move();
            else if(state == STATE_ANIM_UNDO2)
            {
                undo_single_ply();
                state = STATE_ANIM_UNDO;
            }
            else
                update_game_state();
        }
        break;
    case STATE_GAME_PAUSED_START:
        render_board();
        paint_left_half(true);
        ta = 0;
        state = STATE_GAME_PAUSED;
    case STATE_GAME_PAUSED:
        render_game_paused();
        paint_right_half(true);
        break;
    case STATE_SAVE_START:
        render_save_game_board();
        paint_left_half(true);
        state = STATE_SAVE;
    case STATE_SAVE:
        render_save_game();
        paint_right_half(true);
        break;
    case STATE_SAVE_OVERWRITE:
        render_save_game_overwrite();
        paint_right_half(true);
        break;
    case STATE_EXIT_CONFIRM:
        render_exit_confirm();
        paint_right_half(true);
        break;
    case STATE_GAME_OVER:
        render_game_info();
        paint_right_half(true);
        break;
    default: break;
    }
    
    ++nframe;
}


static void update_board_cache()
{
    for(uint8_t r = 0; r < 8; ++r)
        for(uint8_t c = 0; c < 8; ++c)
        {
            ch2k::piece_index i = g.square_to_index(
                ch2k::square::from_rowcol(r, c));
            b[r][c] = i.is_nothing() ? ch2k::piece::NOTHING : g.index_to_piece(i);
        }
}

static void update_game_info()
{
    turn = g.gd.c_.binary_index();
    game_status = g.check_status();
}

static void update_game_state()
{
    //static uint8_t const CONTEMPT_VALS[NUM_MSGS_BLUNDER] PROGMEM =
    //{ 0, 16, 32, 48 };
    update_game_info();
    if(game_status > ch2k::game::STATUS_CHECK)
        state = STATE_GAME_OVER;
    else if(ailevel[turn] != 0)
    {
        if(ailevel[!turn] == 0)
            rotated = !turn;
        g.max_depth_ = 8;
        g.max_nodes_ = uint16_t(1) << (ailevel[turn]*2 + 7);
        //g.contempt_ = pgm_read_byte(&CONTEMPT_VALS[aiblunder[turn]]);
        state = STATE_AI_START;
    }
    else
    {
        rotated = !!turn;
        state = STATE_HUMAN_START;
    }
}

static void update_game_state_after_move()
{
  if(ailevel[turn] != 0)
  {
    u8 happy = 2;
    if(g.score_ < -850) happy = 0;
    else if(g.score_ < -250) happy = 1;
    else if(g.score_ > +850) happy = 4;
    else if(g.score_ > +250) happy = 3;
    aihappy[turn] = happy;
  }
  update_game_state();
}

static void poll_buttons_debounce(uint8_t debounce_num)
{
    uint8_t b = Arduboy2Core::buttonsState();
    uint8_t chg = 0;
    uint8_t diff = b ^ buttons;
    
    for(uint8_t n = 0, m = 1; m != 0; m <<= 1, ++n)
    {
        if(diff & m)
        {
            if(++button_debounce[n] >= debounce_num)
            {
                button_debounce[n] = 0;
                chg |= m;
                button_rep[n] = REP_INIT_NUM;
            }
        }
        else
            button_debounce[n] = 0;
    }
    
    buttons_prev = buttons;
    buttons ^= chg;
    
    static constexpr uint8_t const DIR_BUTTONS =
        UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON;
    for(uint8_t n = 0, m = 1; m != 0; m <<= 1, ++n)
    {
        if((buttons & m) && --button_rep[n] == 0)
        {
            button_rep[n] = REP_NUM;
            buttons_prev &= (~m | (~DIR_BUTTONS));
        }
    }
}


static void poll_buttons()
{
    poll_buttons_debounce(DEBOUNCE_NUM);
}

static void poll_buttons_during_rendering()
{
    poll_buttons_debounce(0);
}

static uint8_t just_pressed(uint8_t b)
{
    return (buttons & ~buttons_prev & b) ? 1 : 0;
}

static void setup_for_game()
{
    cx = rcx = 4 * 8;
    cy = rcy = 6 * 8;
    aihappy[0] = aihappy[1] = 2;
    undohist_num = 0;
    rotated = false;
    game_saved = true;
}

void setup() {
    Arduboy2Core::boot();

    //uint8_t *p = (uint8_t *)&__bss_end;  
    //while(p <= SP)
    //  *p++ = STACK_CANARY_VAL;
  
    init_saves();

    state = STATE_TITLE_START;
}

ch2k::move valid_move(uint8_t a, uint8_t b)
{
  uint8_t n = g.gen_moves(&g.mvs_[0]);
  while(n-- > 0)
  {
    if(g.mvs_[n].fr().x == a && g.mvs_[n].to().x == b)
      return g.mvs_[n];
  }
  return ch2k::move::NO_MOVE;
}

static void send_move(ch2k::move mv)
{
    uint8_t i = 0;
    ch2k::piece& p = b[mv.fr().row()][mv.fr().col()];
    uint8_t n = g.gen_moves(&g.mvs_[0]);
    bool need_rank = false, need_file = false;
    for(uint8_t j = 0; j < n; ++j)
    {
        ch2k::move m = g.mvs_[j];
        if(m.to() != mv.to()) continue;
        if(m.fr() == mv.fr()) continue;
        if(g.square_to_piece(m.fr()) != p) continue;
        if(m.fr().row() == mv.fr().row()) need_file = true;
        if(m.fr().col() == mv.fr().col()) need_rank = true;
    }
    bool cap = !g.square_to_index(mv.to()).is_nothing() ||
        (!mv.is_promotion() && mv.is_en_passant());
    if(p.type().is_pawn() && cap)
        need_file = true;
    
    for(uint8_t i = UNDOHIST_SIZE-1; i > 0; --i)
        undohist[i] = undohist[i - 1];
    for(uint8_t i = SANHIST_SIZE-1; i > 0; --i)
        sanhist[i] = sanhist[i - 1];
    
    {
        auto ci = g.square_to_index(mv.to());
        undohist[0].cap = ci.is_nothing() ? 0 : g.index_to_piece(ci).x;
        undohist[0].flags = g.stack_base()->flags;
        undohist[0].half_move = g.stack_base()->half_move;
        sanhist[0] = SF_NULL;
        if(!p.type().is_pawn())
            sanhist[0] = pgm_read_byte(&SAN_PIECE_CHARS[p.type().x - 3]);
        if(cap) sanhist[0] |= SANFLAG_CAP;
        if(need_rank) sanhist[0] |= SANFLAG_RANK;
        if(need_file) sanhist[0] |= SANFLAG_FILE;
    }
    if(++undohist_num > UNDOHIST_SIZE)
        undohist_num = UNDOHIST_SIZE;
    
    g.execute_move(mv);
    
    n = g.check_status();
    if(n == ch2k::game::STATUS_MATED)
        sanhist[0] |= SANFLAG_MATE;
    else if(n == ch2k::game::STATUS_CHECK || g.in_check)
        sanhist[0] |= SANFLAG_CHECK;
    
    game_saved = false;
    
    ta = mv.fr().x;
    tb = mv.to().x;
    tc = p.x;
    p = ch2k::piece::NOTHING;
    nframe = 0;
    state = STATE_ANIM;
}

static uint8_t undo_single_ply()
{
    if(g.gd.ply_ == 0 || undohist_num == 0) return 0;

    ch2k::move m = g.get_rep_move(0);
    ch2k::piece_index icap;
    ch2k::piece pcap = ch2k::piece::NOTHING;
    ch2k::square fr = m.fr();
    ch2k::square to = m.to();
    bool ep = !m.is_promotion() && m.is_en_passant();
    if(m.is_castle())
    {
        // set captured piece to rook
        if(to.col() < 4) // queenside
            icap = g.square_to_index(to + ch2k::square_delta::EAST);
        else            // kingside
            icap = g.square_to_index(to + ch2k::square_delta::WEST);
    }
    else
    {
        // add captured piece to piece lists
        pcap = ch2k::piece{ undohist[0].cap };
        ch2k::square to2 = to;
        if(ep)
        {
            to2 = ch2k::square::from_rowcol(fr.row(), to.col());
            pcap = ch2k::piece::pawn(g.gd.c_);
        }
        ch2k::piece_index t = g.square_to_index(to2);
        icap = g.add_piece(pcap, to2);
        g.index_to_square(icap) = ch2k::square::NOWHERE;
        g.square_to_index(to2) = t;
    }
    g.unexecute_move(m, icap, undohist[0].flags, undohist[0].half_move);
        
    --undohist_num;
    for(uint8_t j = 0; j < UNDOHIST_SIZE - 1; ++j)
        undohist[j] = undohist[j + 1];
    for(uint8_t j = 0; j < SANHIST_SIZE - 1; ++j)
        sanhist[j] = sanhist[j + 1];
    
    game_saved = false;
    
    // anim info
    ta = to.x;
    tb = fr.x;
    tc = g.square_to_piece(fr).x;
    if(ep)
    {
        b[fr.row()][to.col()] = pcap;
        pcap = ch2k::piece::NOTHING;
    }
    b[to.row()][to.col()] = pcap;
    nframe = 0;
    return 1;
}

static void undo_move()
{
    if(!undo_single_ply())
        return;
    else if(ailevel[turn] == 0 && ailevel[!turn] > 0)
        state = STATE_ANIM_UNDO2;
    else
        state = STATE_ANIM_UNDO;
}

static void new_game_cycle_option(bool reverse)
{
    if(ta < 2) return;
    uint8_t n;
    uint8_t* t;
    uint8_t i = (ta >> 1) - 1;
    if(ta & 1)
        n = NUM_MSGS_BLUNDER, t = &aiblunder[i];
    else
        n = NUM_MSGS_PLAYER, t = &ailevel[i];
    if( reverse && --*t >= n) *t = n - 1;
    if(!reverse && ++*t >= n) *t = 0;
}

void loop() {
    
    poll_buttons();
    
    switch(state)
    {
    case STATE_TITLE:
        if(just_pressed(LEFT_BUTTON))
        {
            if(--ta >= NUM_MSGS_TITLE) ta = NUM_MSGS_TITLE - 1;
        }
        else if(just_pressed(RIGHT_BUTTON))
        {
            if(++ta >= NUM_MSGS_TITLE) ta = 0;
        }
        else if(just_pressed(A_BUTTON))
        {
            // seed random number generator
            {
                uint32_t t = random(65535);//Arduboy2Core::generateRandomSeed();
                rand_state0 = uint16_t(t);
                rand_state1 = uint16_t(t >> 16);
            }
            if(ta == 0)
            {
                g.new_game();
                //g.load_fen("8/2P5/8/8/8/8/k6K/8 w - -"); // test promotion
                //g.load_fen("2k5/8/8/8/4p3/8/5P2/2K5 w - -"); // test ep
                //g.load_fen("8/6p1/8/7P/8/8/8/K1k5 b - -");
                update_board_cache();
                state = STATE_NEW_GAME_START;
            }
            else if(ta == 1)
            {
                ta = 0, tc = 1;
                state = STATE_SAVE_START;
            }
            //else if(ta == 2)
            //    state = STATE_HELP_START;
        }
        break;
    //case STATE_HELP:
    //    if(just_pressed(A_BUTTON) || just_pressed(B_BUTTON))
    //        state = STATE_TITLE_START;
    //    break;
    case STATE_NEW_GAME:
        if(just_pressed(LEFT_BUTTON))
        {
            new_game_cycle_option(true);
        }
        else if(just_pressed(RIGHT_BUTTON) && ta > 1)
        {
            new_game_cycle_option(false);
        }
        else if(just_pressed(UP_BUTTON))
        {
            if(--ta >= 6) ta = 5;
            if(ta == 3 && ailevel[0] == 0) ta = 2;
            if(ta == 5 && ailevel[1] == 0) ta = 4;
        }
        else if(just_pressed(DOWN_BUTTON))
        {
            if(++ta >= 6) ta = 0;
            if(ta == 3 && ailevel[0] == 0) ta = 4;
            if(ta == 5 && ailevel[1] == 0) ta = 0;
        }
        else if(just_pressed(A_BUTTON))
        {
            if(ta == 0)
                state = STATE_TITLE_START;
            else if(ta == 1)
            {
                setup_for_game();
                update_game_state();
            }
            else
                new_game_cycle_option(false);
        }
        else if(just_pressed(B_BUTTON))
            state = STATE_TITLE_START;
        break;
    case STATE_HUMAN_PROMOTION:
        if(just_pressed(LEFT_BUTTON )) --tc;
        if(just_pressed(RIGHT_BUTTON)) ++tc;
        tc &= 3;
        if(just_pressed(A_BUTTON))
        {
            tc += 3;
            ch2k::move mv;
            mv.clear_set_fr(ch2k::square{ta});
            mv.clear_set_to(ch2k::square{tb});
            mv.set_promotion();
            mv.set_promotion_piece_type(ch2k::piece_type{tc});
            rcx = cx;
            rcy = cy;
            send_move(mv);
        }
        else if(just_pressed(B_BUTTON))
        {
            state = STATE_HUMAN_MOVE;
        }
        break;
    case STATE_HUMAN:
    case STATE_HUMAN_MOVE:
        if(just_pressed(B_BUTTON))
        {
            state = STATE_GAME_PAUSED_START;
            break;
        }
        if(just_pressed(UP_BUTTON   )) cy -= 8;
        if(just_pressed(DOWN_BUTTON )) cy += 8;
        if(just_pressed(LEFT_BUTTON )) cx -= 8;
        if(just_pressed(RIGHT_BUTTON)) cx += 8;
        cy &= 56;
        cx &= 56;
        if(just_pressed(A_BUTTON))
        {
            uint8_t cyi = cy >> 3, cxi = cx >> 3;
            if(rotated)
            {
                cyi = 7 - cyi;
                cxi = 7 - cxi;
            }
            ch2k::square s = ch2k::square::from_rowcol(cyi, cxi);
            if(state == STATE_HUMAN)
            {
                if(b[cyi][cxi].color() == g.gd.c_)
                {
                    state = STATE_HUMAN_MOVE;
                    ta = s.x;
                }
            }
            else
            {
                if(b[cyi][cxi].color() == g.gd.c_)
                {
                    ta = s.x;
                }
                else
                {
                    tb = s.x;
                    ch2k::move mv = valid_move(ta, tb);
                    if(mv.is_promotion())
                        tc = 3, rcx = 40, state = STATE_HUMAN_PROMOTION;
                    else if(mv != ch2k::move::NO_MOVE)
                        send_move(mv);
                }
            }
        }
        break;
    case STATE_AI:
        g.ai_move();
        //g.iterative_deepening();
        if(g.stop_)
            state = STATE_GAME_PAUSED_START;
        else if(g.best_ != ch2k::move::NO_MOVE)
        {
          // check for blunder
          static uint8_t const BLUNDER_VALS[NUM_MSGS_BLUNDER] PROGMEM =
          { 0, 26, 64, 128, 192 };
          uint8_t bt = CH2K_RAND();
          ch2k::move aimove = g.best_;
          if(bt < pgm_read_byte(&BLUNDER_VALS[aiblunder[turn]]))
          {
            uint8_t n = g.gen_moves(&g.mvs_[0]);
            bt = CH2K_RAND();
            while(bt >= n) bt -= n;
            aimove = g.mvs_[bt];
          }
          send_move(aimove);
        }
        else
            state = STATE_GAME_OVER;
        break;
    case STATE_ANIM:
    case STATE_ANIM_UNDO:
    case STATE_ANIM_UNDO2:
        if(just_pressed(B_BUTTON))
        {
            if(state == STATE_ANIM_UNDO2)
                undo_single_ply();
            update_board_cache();
            update_game_info();
            state = STATE_GAME_PAUSED_START;
        }
        break;
    case STATE_GAME_OVER:
        if(just_pressed(B_BUTTON))
            state = STATE_GAME_PAUSED_START;
        break;
    case STATE_GAME_PAUSED:
        if(just_pressed(B_BUTTON))
            update_game_state();
        else if(just_pressed(UP_BUTTON))
        {
            if(--ta >= NUM_MSGS_PAUSE) ta = NUM_MSGS_PAUSE - 1;
        }
        else if(just_pressed(DOWN_BUTTON))
        {
            if(++ta >= NUM_MSGS_PAUSE) ta = 0;
        }
        else if(just_pressed(A_BUTTON))
        {
            switch(ta)
            {
                case 0: update_game_state(); break;
                case 1: undo_move(); break;
                case 2: ta = tc = 0; state = STATE_SAVE_START; break;
                case 3:
                    ta = 0;
                    state = game_saved ? STATE_TITLE_START : STATE_EXIT_CONFIRM;
                    break;
                default: break;
            }
        }
        break;
    case STATE_SAVE:
        if(just_pressed(B_BUTTON))
        {
            update_board_cache();
            state = tc ? STATE_TITLE_START : STATE_GAME_PAUSED_START;
        }
        else if(just_pressed(UP_BUTTON))
        {
            if(--ta > NUM_SAVE_FILES) ta = NUM_SAVE_FILES;
            state = STATE_SAVE_START;
        }
        else if(just_pressed(DOWN_BUTTON))
        {
            if(++ta > NUM_SAVE_FILES) ta = 0;
            state = STATE_SAVE_START;
        }
        else if(just_pressed(A_BUTTON))
        {
            bool valid = save_is_valid(ta - 1);
            if(ta == 0)
            {
                update_board_cache();
                state = tc ? STATE_TITLE_START : STATE_GAME_PAUSED_START;
            }
            else if(tc)
            {
                // this is load game
                if(valid)
                {
                    setup_for_game();
                    load_game(ta - 1);
                    update_board_cache();
                    update_game_state();
                }
            }
            else if(valid)
            {
                tb = 1;
                state = STATE_SAVE_OVERWRITE;
            }
            else
            {
                save_game(ta - 1);
                update_board_cache();
                update_game_state();
            }
        }
        break;
    case STATE_SAVE_OVERWRITE:
        if(just_pressed(UP_BUTTON) || just_pressed(DOWN_BUTTON))
            tb = !tb;
        else if(just_pressed(B_BUTTON))
            state = STATE_SAVE;
        else if(just_pressed(A_BUTTON))
        {
            if(tb == 0)
            {
                save_game(ta - 1);
                update_board_cache();
                update_game_state();
            }
            else
                state = STATE_SAVE;
        }
        break;
    case STATE_EXIT_CONFIRM:
        if(just_pressed(UP_BUTTON) || just_pressed(DOWN_BUTTON))
            ta = !ta;
        else if(just_pressed(B_BUTTON))
            state = STATE_GAME_PAUSED_START;
        else if(just_pressed(A_BUTTON))
            state = ta ? STATE_TITLE_START : STATE_GAME_PAUSED_START;
    default: break;
    }
    
    delay(50);
    ISRfunc();
}
