#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>

#include "Emergency.cpp"

WINDOW* init_map(RoadState &road_state)
{
    initscr();
    cbreak();
    curs_set(0);
    WINDOW* win = newwin(road_state.wall+2, road_state.wall+2, 0, 0);
    return win;
}

void read_input()
{
    while (true)
    {
        if(std::cin.get() == 27){
            endwin();
            break;
        }
    }
    exit(0);
}

void draw_map(WINDOW* win, RoadState& road_state)
{
    box(win,0,0);
    //horizontal
    for(int i=0;i<road_state.lanes;++i){
        for(int j=1;j<road_state.wall+1;++j){
            mvwprintw(win, road_state.slots+i+1,j , ".");
        }
    }
    //vertical
    for(int i = 0; i < road_state.lanes;++i){
        for(int j=1;j < road_state.wall+1;++j){
            mvwprintw(win, j, road_state.slots+i+1, ".");
        }
    }
    wrefresh(win);
}

void draw_E(WINDOW* win,Emergency* karetka, Movement_direction where)
{
    karetka->calculate_movement_to_do(where);
    wrefresh(win);
    while (!karetka->getHasArrived())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        karetka->move(where);
        wrefresh(win);
    }
    karetka->~Emergency();
    wrefresh(win);
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }
    WINDOW* win;
    RoadState road_state(atoi(argv[1]), atoi(argv[2]));

    win = init_map(road_state);
    draw_map(win, road_state);

    std::thread input(read_input);

    Emergency* karetka = new Emergency(win, road_state, RIGHT);
    std::thread moveER(draw_E, win, karetka, TURN_LEFT);
    moveER.join();
    input.join();

    return 0;
}