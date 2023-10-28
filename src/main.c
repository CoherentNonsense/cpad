#include <ncurses.h>

typedef enum {
    Mode_Normal,
    Mode_Insert,
} Mode;

typedef struct {
    Mode mode;
    struct {
        int x; int y;
    } cursor;
    int input;
    int number_prefix;
} CPad;

void cpad_move(CPad* cpad, int x, int y) {
    int distance = cpad->number_prefix == 0 ? 1 : cpad->number_prefix;
    cpad->cursor.x += x * distance;
    cpad->cursor.y += y * distance;
    cpad->number_prefix = 0;

    move(cpad->cursor.y, cpad->cursor.x);
}

#define ESCAPE '\x1B'
#define ENTER  '\xA'
#define DELETE '\x7F'
void mode_normal(CPad* cpad) {
    if (cpad->input == 'q') { return; }

    switch (cpad->input) {
        case ESCAPE: cpad->number_prefix = 0; break;
        case 'h': cpad_move(cpad, -1,  0); break;
        case 'j': cpad_move(cpad,  0,  1); break;
        case 'k': cpad_move(cpad,  0, -1); break;
        case 'l': cpad_move(cpad,  1,  0); break;
        case 'i': curs_set(2); cpad->mode = Mode_Insert; break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            cpad->number_prefix *= 10;
            cpad->number_prefix += cpad->input - '0';
    }
}

void mode_insert(CPad* cpad) {
    switch (cpad->input) {
        case ESCAPE: curs_set(1); cpad->mode = Mode_Normal; break;
        case DELETE:
            move(cpad->cursor.y, --cpad->cursor.x);
            addch(' ');
            move(cpad->cursor.y, cpad->cursor.x);
            break;
        case ENTER: move(++cpad->cursor.y, cpad->cursor.x = 0); break;
        default: mvprintw(cpad->cursor.y, cpad->cursor.x++, "%c", cpad->input);
    }
}

int main(void) {
    initscr();
    cbreak();
    noecho();
    use_default_colors();
    start_color();

    CPad cpad = {0};
    cpad.mode = Mode_Normal;

    while (1) {
        cpad.input = getch();

        switch (cpad.mode) {
            case Mode_Normal: mode_normal(&cpad); break;
            case Mode_Insert: mode_insert(&cpad); break;
        }
    }

    endwin();
}
