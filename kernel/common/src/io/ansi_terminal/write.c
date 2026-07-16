#include <io/ansi_terminal.h>

/* parse decimal number until buffer is exhausted or non-numeric character is detected */
static size_t ansi_terminal_parse_dec(const char* buf, size_t len, size_t* out) {
    *out = 0;
    size_t i = 0;
    for (; i < len; i++) {
        if (buf[i] >= '0' && buf[i] <= '9')
            *out = *out * 10 + buf[i] - '0';
        else
            break;
    }
    return i; // return number of characters ingested
}

static void ansi_terminal_handle_move(ansi_terminal_t* term) {
    if (!term->set_cursor) return; // not implemented

    size_t x = 0, y = 0;
    size_t y_idx = ansi_terminal_parse_dec(term->param_buf, term->param_idx, &x); // try to parse X
    if (!y_idx) goto set;
    y_idx++; if (y_idx >= term->param_idx) return;
    if (!ansi_terminal_parse_dec(&term->param_buf[y_idx], term->param_idx - y_idx, &y)) return; // try to parse Y
set:
    term->set_cursor(term->user, x, y);
}

static void ansi_terminal_handle_move_up(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, 0, -delta);
}

static void ansi_terminal_handle_move_down(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, 0, delta);
}

static void ansi_terminal_handle_move_left(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, -delta, 0);
}

static void ansi_terminal_handle_move_right(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, delta, 0);
}

static void ansi_terminal_handle_move_up_begin(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, INT_MIN, -delta);
}

static void ansi_terminal_handle_move_down_begin(ansi_terminal_t* term) {
    if (!term->move_cursor || !term->param_idx) return;

    size_t delta = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &delta) != term->param_idx) return; // malformed
    if (!delta) return; // no need to move

    term->move_cursor(term->user, INT_MIN, delta);
}

static void ansi_terminal_handle_move_x(ansi_terminal_t* term) {
    if (!term->set_cursor || !term->param_idx) return;

    size_t x = 0;
    if (ansi_terminal_parse_dec(term->param_buf, term->param_idx, &x) != term->param_idx) return; // malformed

    term->set_cursor(term->user, x, -1);
}

static void ansi_terminal_handle_erase_display(ansi_terminal_t* term) {
    int mode = -1;
    switch (term->param_idx) {
        case 0: mode = 0; break;
        case 1: mode = term->param_buf[0] - '0'; break;
        default: break;
    }
    
    switch (mode) {
        case 0: // erase from cursor until end of screen
            if (term->clear_line_right && term->clear_lines_after) {
                term->clear_line_right(term->user);
                term->clear_lines_after(term->user);
            }
            break;
        case 1: // erase from cursor to beginning of screen
            if (term->clear_line_left && term->clear_lines_before) {
                term->clear_lines_before(term->user);
                term->clear_line_left(term->user);
            }
            break;
        case 2: // erase entire screen
            if (term->clear_screen) term->clear_screen(term->user);
            else if (term->clear_lines_before && term->clear_lines_after) {
                if (term->clear_line) {
                    term->clear_lines_before(term->user);
                    term->clear_line(term->user);
                    term->clear_lines_after(term->user);
                } else if (term->clear_line_left && term->clear_line_right) {
                    term->clear_lines_before(term->user);
                    term->clear_line_left(term->user);
                    term->clear_line_right(term->user);
                    term->clear_lines_after(term->user);
                }
            }
            break;
    }
}

static void ansi_terminal_handle_erase_line(ansi_terminal_t* term) {
    int mode = -1;
    switch (term->param_idx) {
        case 0: mode = 0; break;
        case 1: mode = term->param_buf[0] - '0'; break;
        default: break;
    }
    
    switch (mode) {
        case 0: // erase from cursor until end of line
            if (term->clear_line_right) term->clear_line_right(term->user);
            break;
        case 1: // erase from cursor to beginning of line
            if (term->clear_line_left) term->clear_line_left(term->user);
            break;
        case 2: // erase entire line
            if (term->clear_line) term->clear_line(term->user);
            else if (term->clear_line_left && term->clear_line_right) {
                term->clear_line_left(term->user);
                term->clear_line_right(term->user);
            }
            break;
    }
}

#ifndef ANSI_TERMINAL_DEFAULT_FOREGROUND_8
#define ANSI_TERMINAL_DEFAULT_FOREGROUND_8 ANSI_TERMINAL_COLOUR_WHITE
#endif /* ANSI_TERMINAL_DEFAULT_FOREGROUND_8 */
#ifndef ANSI_TERMINAL_DEFAULT_FOREGROUND_8_MODIFIER
#define ANSI_TERMINAL_DEFAULT_FOREGROUND_8_MODIFIER ANSI_TERMINAL_COLOUR_NORMAL
#endif /* ANSI_TERMINAL_DEFAULT_FOREGROUND_8_MODIFIER */

#ifndef ANSI_TERMINAL_DEFAULT_BACKGROUND_8
#define ANSI_TERMINAL_DEFAULT_BACKGROUND_8 ANSI_TERMINAL_COLOUR_BLACK
#endif /* ANSI_TERMINAL_DEFAULT_BACKGROUND_8 */
#ifndef ANSI_TERMINAL_DEFAULT_BACKGROUND_8_MODIFIER
#define ANSI_TERMINAL_DEFAULT_BACKGROUND_8_MODIFIER ANSI_TERMINAL_COLOUR_NORMAL
#endif /* ANSI_TERMINAL_DEFAULT_BACKGROUND_8_MODIFIER */

#ifndef ANSI_TERMINAL_DEFAULT_FOREGROUND_256
#define ANSI_TERMINAL_DEFAULT_FOREGROUND_256 7
#endif /* ANSI_TERMINAL_DEFAULT_FOREGROUND_256 */

#ifndef ANSI_TERMINAL_DEFAULT_BACKGROUND_256
#define ANSI_TERMINAL_DEFAULT_BACKGROUND_256 0
#endif /* ANSI_TERMINAL_DEFAULT_BACKGROUND_256 */

#define RGB(r, g, b)                    (((r) << 0) | ((g) << 8) | ((b) << 16))

#ifndef ANSI_TERMINAL_PALETTE_BRIGHTNESS_100
#define ANSI_TERMINAL_PALETTE_BRIGHTNESS_100 255
#endif /* ANSI_TERMINAL_PALETTE_BRIGHTNESS_100*/

#ifndef ANSI_TERMINAL_PALETTE_BRIGHTNESS_50
#define ANSI_TERMINAL_PALETTE_BRIGHTNESS_50 170 // may also be 128?
#endif /* ANSI_TERMINAL_PALETTE_BRIGHTNESS_50 */

#define ANSI_TERMINAL_PALETTE_BRIGHTNESS_25 (ANSI_TERMINAL_PALETTE_BRIGHTNESS_50 / 2)

static const uint32_t ansi_terminal_palette_normal[8] = {
    RGB(0, 0, 0),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, 0, 0), 
    RGB(0, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, 0),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, 0),
    RGB(0, 0, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, 0, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50),
    RGB(0, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50, ANSI_TERMINAL_PALETTE_BRIGHTNESS_50)
};
static const uint32_t ansi_terminal_palette_light[8] = {
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_25, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100),
    RGB(ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100, ANSI_TERMINAL_PALETTE_BRIGHTNESS_100)
};

#ifndef ANSI_TERMINAL_DEFAULT_FOREGROUND_RGB
#define ANSI_TERMINAL_DEFAULT_FOREGROUND_RGB ansi_terminal_palette_normal[7]
#endif

#ifndef ANSI_TERMINAL_DEFAULT_BACKGROUND_RGB
#define ANSI_TERMINAL_DEFAULT_BACKGROUND_RGB ansi_terminal_palette_normal[0]
#endif

static void ansi_terminal_reset_foreground(ansi_terminal_t* term) {
    if (term->reset_foreground)
        term->reset_foreground(term->user);
    else if (term->set_foreground_8)
        term->set_foreground_8(term->user, ANSI_TERMINAL_DEFAULT_FOREGROUND_8, ANSI_TERMINAL_DEFAULT_FOREGROUND_8_MODIFIER);
    else if (term->set_foreground_256)
        term->set_foreground_256(term->user, ANSI_TERMINAL_DEFAULT_FOREGROUND_256);
    else if (term->set_foreground_rgb)
        term->set_foreground_rgb(term->user, ANSI_TERMINAL_DEFAULT_FOREGROUND_RGB);
}

static void ansi_terminal_reset_background(ansi_terminal_t* term) {
    if (term->reset_background)
        term->reset_background(term->user);
    else if (term->set_background_8)
        term->set_background_8(term->user, ANSI_TERMINAL_DEFAULT_BACKGROUND_8, ANSI_TERMINAL_DEFAULT_BACKGROUND_8_MODIFIER);
    else if (term->set_background_256)
        term->set_background_256(term->user, ANSI_TERMINAL_DEFAULT_BACKGROUND_256);
    else if (term->set_background_rgb)
        term->set_background_rgb(term->user, ANSI_TERMINAL_DEFAULT_BACKGROUND_RGB);
}

static void ansi_terminal_set_foreground_8(ansi_terminal_t* term, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier) {
    if (term->set_foreground_8)
        term->set_foreground_8(term->user, colour, modifier);
    else if (term->set_foreground_256) {
        switch (modifier) {
            case ANSI_TERMINAL_COLOUR_NORMAL:
                term->set_foreground_256(term->user, colour);
                break;
            case ANSI_TERMINAL_COLOUR_BRIGHT:
                term->set_foreground_256(term->user, 8 + colour);
                break;
            default:
                break;
        }
    } else if (term->set_foreground_rgb) {
        switch (modifier) {
            case ANSI_TERMINAL_COLOUR_NORMAL:
                term->set_foreground_rgb(term->user, ansi_terminal_palette_normal[colour]);
                break;
            case ANSI_TERMINAL_COLOUR_BRIGHT:
                term->set_foreground_256(term->user, ansi_terminal_palette_light[colour]);
                break;
            default:
                break;
        }
    }
}

static void ansi_terminal_set_background_8(ansi_terminal_t* term, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier) {
    if (term->set_background_8)
        term->set_background_8(term->user, colour, modifier);
    else if (term->set_background_256) {
        switch (modifier) {
            case ANSI_TERMINAL_COLOUR_NORMAL:
                term->set_background_256(term->user, colour);
                break;
            case ANSI_TERMINAL_COLOUR_BRIGHT:
                term->set_background_256(term->user, 8 + colour);
                break;
            default:
                break;
        }
    } else if (term->set_background_rgb) {
        switch (modifier) {
            case ANSI_TERMINAL_COLOUR_NORMAL:
                term->set_background_rgb(term->user, ansi_terminal_palette_normal[colour]);
                break;
            case ANSI_TERMINAL_COLOUR_BRIGHT:
                term->set_background_256(term->user, ansi_terminal_palette_light[colour]);
                break;
            default:
                break;
        }
    }
}

static void ansi_terminal_set_foreground_256(ansi_terminal_t* term, uint8_t colour) {
    if (term->set_foreground_256)
        term->set_foreground_256(term->user, colour);
    else if (term->set_foreground_rgb) {
        if (colour < 8)
            term->set_foreground_rgb(term->user, ansi_terminal_palette_normal[colour]);
        else if (colour < 16)
            term->set_foreground_rgb(term->user, ansi_terminal_palette_light[colour - 8]);
        else if (colour < 232) {
            uint8_t colour_idx = colour - 16;
            size_t b = colour_idx % 6; b = b * 256 / 24; if (b > 255) b = 255;
            size_t g = (colour_idx / 6) % 6; g = g * 256 / 24; if (g > 255) g = 255;
            size_t r = (colour_idx / 36) % 6; r = r * 256 / 24; if (r > 255) r = 255;
            term->set_foreground_rgb(term->user, RGB(r, g, b));
        } else {
            size_t x = (colour - 232) * 256 / 24; if (x > 255) x = 255;
            term->set_foreground_rgb(term->user, RGB(x, x, x));
        }
    } else if (term->set_foreground_8 && colour < 16) {
        if (colour < 8) term->set_foreground_8(term->user, colour, ANSI_TERMINAL_COLOUR_NORMAL);
        else term->set_foreground_8(term->user, colour - 8, ANSI_TERMINAL_COLOUR_BRIGHT);
    } 
}

static void ansi_terminal_set_background_256(ansi_terminal_t* term, uint8_t colour) {
    if (term->set_background_256)
        term->set_background_256(term->user, colour);
    else if (term->set_background_rgb) {
        if (colour < 8)
            term->set_background_rgb(term->user, ansi_terminal_palette_normal[colour]);
        else if (colour < 16)
            term->set_background_rgb(term->user, ansi_terminal_palette_light[colour - 8]);
        else if (colour < 232) {
            uint8_t colour_idx = colour - 16;
            size_t b = colour_idx % 6; b = b * 256 / 24; if (b > 255) b = 255;
            size_t g = (colour_idx / 6) % 6; g = g * 256 / 24; if (g > 255) g = 255;
            size_t r = (colour_idx / 36) % 6; r = r * 256 / 24; if (r > 255) r = 255;
            term->set_background_rgb(term->user, RGB(r, g, b));
        } else {
            size_t x = (colour - 232) * 256 / 24; if (x > 255) x = 255;
            term->set_background_rgb(term->user, RGB(x, x, x));
        }
    } else if (term->set_background_8 && colour < 16) {
        if (colour < 8) term->set_background_8(term->user, colour, ANSI_TERMINAL_COLOUR_NORMAL);
        else term->set_background_8(term->user, colour - 8, ANSI_TERMINAL_COLOUR_BRIGHT);
    } 
}

static void ansi_terminal_handle_set_colour(ansi_terminal_t* term) {
    size_t idx = 0;
    while (idx < term->param_idx) {
        size_t num;
        size_t idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &num);
        if (!idx_delta) return;
        idx += idx_delta + 1; // skip past semicolon
        if (!num) { // either regular colour, or reset all modes
            if (idx >= term->param_idx) {
                ansi_terminal_reset_background(term);
                ansi_terminal_reset_foreground(term);
                break; // no more to read
            } else continue;
        } else if (num >= 30 && num <= 37) { // normal foreground (8 colour mode)
            ansi_terminal_set_foreground_8(term, num - 30, ANSI_TERMINAL_COLOUR_NORMAL);
        } else if (num >= 40 && num <= 47) { // normal background (8 colour mode)
            ansi_terminal_set_background_8(term, num - 40, ANSI_TERMINAL_COLOUR_NORMAL);
        } else if (num >= 90 && num <= 97) { // bright foreground (8 colour mode)
            ansi_terminal_set_foreground_8(term, num - 90, ANSI_TERMINAL_COLOUR_BRIGHT);
        } else if (num >= 100 && num <= 107) { // bright background (8 colour mode)
            ansi_terminal_set_background_8(term, num - 100, ANSI_TERMINAL_COLOUR_BRIGHT);
        } else if (num == 39) { // set default foreground
            ansi_terminal_reset_foreground(term);
        } else if (num == 49) { // set default background
            ansi_terminal_reset_background(term);            
        } else if (num == 38) { // set 256/RGB foreground
            idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &num);
            if (!idx_delta) return;
            idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
            if (num == 2) { // RGB
                size_t r, g, b;
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &r);
                if (!idx_delta) return;
                idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &g);
                if (!idx_delta) return;
                idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &b);
                if (!idx_delta) return;
                idx += idx_delta + 1;
                if (term->set_foreground_rgb) term->set_foreground_rgb(term->user, RGB(r, g, b));
            } else if (num == 5) { // 256 colour
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &num);
                if (!idx_delta) return;
                idx += idx_delta + 1; 
                ansi_terminal_set_foreground_256(term, num);
            } else return;
        } else if (num == 48) { // set 256/RGB background
            idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &num);
            if (!idx_delta) return;
            idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
            if (num == 2) { // RGB
                size_t r, g, b;
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &r);
                if (!idx_delta) return;
                idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &g);
                if (!idx_delta) return;
                idx += idx_delta + 1; if (idx >= term->param_idx) return; // skip past semicolon
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &b);
                if (!idx_delta) return;
                idx += idx_delta + 1;
                if (term->set_background_rgb) term->set_background_rgb(term->user, RGB(r, g, b));
            } else if (num == 5) { // 256 colour
                idx_delta = ansi_terminal_parse_dec(&term->param_buf[idx], term->param_idx - idx, &num);
                if (!idx_delta) return;
                idx += idx_delta + 1; 
                ansi_terminal_set_background_256(term, num);
            } else return;
        }
    }
}

void ansi_terminal_write(void* user, char c) {
    ansi_terminal_t* term = (ansi_terminal_t*) user;
    if (!term->write) return; // nothing to do here

    switch (term->state) {
        case ANSI_TERMINAL_STATE_IDLE:
            if (c == 0x1B) term->state = ANSI_TERMINAL_STATE_ESC;
            else term->write(term->user, c);
            break;
        case ANSI_TERMINAL_STATE_ESC:
            if (c == '[') {
                term->state = ANSI_TERMINAL_STATE_BRACKET;
                term->param_idx = 0;
            } else {
                term->write(term->user, 0x1B);
                if (term->write_available(term->user)) term->write(term->user, c);
            }
            break;
        case ANSI_TERMINAL_STATE_BRACKET:
            switch (c) {
                case 'H':
                    ansi_terminal_handle_move(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'f':
                    ansi_terminal_handle_move(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'A':
                    ansi_terminal_handle_move_up(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'B':
                    ansi_terminal_handle_move_down(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'C':
                    ansi_terminal_handle_move_right(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'D':
                    ansi_terminal_handle_move_left(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'E':
                    ansi_terminal_handle_move_down_begin(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'F':
                    ansi_terminal_handle_move_up_begin(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'G':
                    ansi_terminal_handle_move_x(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'J':
                    ansi_terminal_handle_erase_display(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'K':
                    ansi_terminal_handle_erase_line(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                case 'm':
                    ansi_terminal_handle_set_colour(term);
                    term->state = ANSI_TERMINAL_STATE_IDLE;
                    break;
                default:
                    if (term->param_idx >= sizeof(term->param_buf)) term->state = ANSI_TERMINAL_STATE_IDLE;
                    else term->param_buf[term->param_idx++] = c;
                    break;
            }
            break;
    }
}