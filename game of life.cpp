#include <iostream>
#include <fstream>
#include <cstdlib>
#include <allegro.h>

const int height = 80, width = 120;
int resolution, border;
bool world[width][height];
bool next[width][height];
BITMAP *buffer;
bool quit;

using namespace std;

int neighbours(int x, int y);
void step(int &age);
void display(int age);
void debug(int age);
void close();
void edit();
void redraw();

int main() {
    int delay=5, desktoph, desktopw;
    
    allegro_init();

	// Set the window title when in a GUI environment

	install_keyboard();
	install_mouse();
    show_mouse(screen);
	// Set graphics mode
	set_color_depth(desktop_color_depth());
	get_desktop_resolution(&desktopw, &desktoph);

	if (desktoph*height < desktopw * width) resolution = desktoph/height -4;
	else resolution = desktopw/width -4;
	border = resolution/10;
    if (border == 0) border = 1;
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, width*resolution, height*resolution, 0, 0);
    set_close_button_callback(close);
    buffer = create_bitmap(width*resolution, height*resolution);   

    int age = 0;
    char input[width+1];
    
    ifstream in("world.txt");
    
    if (!in) return 1;
    
    for (int y = 0; y < height; y++) {
        in.getline(input, width+1);
        for (int x = 0; x < width; x++) 
            world[x][y] = (input[x]=='1');
    }
    
    while (!quit) {
        display(age);
        if (keypressed()) {
            int key = readkey();
            switch ((key >> 8) & 0xFF) {
                case KEY_E:
                    edit();
                    break;
                case KEY_1:
                    delay = 9;
                    break;
                case KEY_2:
                    delay = 8;
                    break;
                case KEY_3:
                    delay = 7;
                    break;
                case KEY_4:
                    delay = 6;
                    break;
                case KEY_5:
                    delay = 5;
                    break;
                case KEY_6:
                    delay = 4;
                    break;
                case KEY_7:
                    delay = 3;
                    break;
                case KEY_8:
                    delay = 2;
                    break;
                case KEY_9:
                    delay = 1;
                    break;
                case KEY_0:
                    delay = 0;
                    break;
            }
        }
        
        step(age);
        rest(delay*20);
    }    
    
    in.close();

    return 0;
}   
END_OF_MAIN()


int neighbours(int x, int y) {
    int sum = 0;
    for (int dy = -1; dy < 2; dy++) {
        for (int dx = -1; dx < 2; dx++) {
            if (dx == 0 && dy == 0) continue;
            if ((x+dx >= 0) && 
                (x+dx < width) && 
                (y+dy >= 0) &&
                (y+dy < height)) if(world[x+dx][y+dy]) sum++;
        }
    }
    return sum;
}

void step(int &age) {
    age++;
    for (int y = 0; y < height; y++) 
        for (int x = 0; x < width; x++) {
            int myNeighbours = neighbours(x, y);
            switch(world[x][y]) {
                case 1:
                    if (myNeighbours == 2 || myNeighbours == 3) next[x][y] = 1;
                    else next[x][y] = 0;
                    break;
                case 0:
                    if (myNeighbours == 3) next[x][y] = 1;
                    else next[x][y] = 0;
                    break;
            }
        }
    for (int y = 0; y < height; y++) 
        for (int x = 0; x < width; x++) 
            world[x][y] = next[x][y];
}


void display(int age) {
    char title[32];
    sprintf(title, "Life - Time: %d", age);
    set_window_title(title);
    clear_to_color(buffer, makecol(255, 255, 255));
    for (int i = 0; i < width; i++) {
        rectfill(buffer, (i+1)*resolution-border, 0, (i+1)*resolution-1, buffer->h, makecol(0,0,0));
        rectfill(buffer, i*resolution, 0, i*resolution+border-1, buffer->h, makecol(0,0,0));
    }
    for (int i = 0; i < height; i++) {
        rectfill(buffer, 0, (i+1)*resolution-border, buffer->w, (i+1)*resolution-1, makecol(0,0,0));
        rectfill(buffer, 0, i*resolution, buffer->w, i*resolution+border-1, makecol(0,0,0));
    }
    for (int i = 0; i< height; i++) {
        for (int j = 0; j < width; j++) {
            if (world[j][i] == 1) rectfill(buffer, j*resolution+border, i*resolution+border, (j+1)*resolution-border-1, (i+1)*resolution-border-1, makecol(255, 0, 0));
        }
    }
    
    
    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
}

void redraw(int x, int y) {
    rectfill(buffer, x*resolution, y*resolution, (x+1)*resolution-1, (y+1)*resolution-1, makecol(0,0,0));
    rectfill(buffer, x*resolution+border, y*resolution+border, (x+1)*resolution-border-1, (y+1)*resolution-border-1, world[x][y] ? makecol(255,0,0) : makecol(255,255,255));
}

void edit() {
    int mdx, mdy, x=0, y=0, drawx, drawy, oldz=0, filter=0, released=1;
    bool brush=true, draw=false, end=false;
    BITMAP *selection;
    selection = create_bitmap(resolution,resolution);
    clear_to_color(selection, makecol(20,200,10));
    set_window_title("Life - Editing");
    while (!key[KEY_ESC]&&!quit&&!end) {
        get_mouse_mickeys(&mdx, &mdy);
        if ((mdx!=0 || mdy!=0) && (mouse_x > 0 && mouse_y > 0 && mouse_x < width*resolution && mouse_y < height*resolution)) {
            redraw(x, y);
            x = mouse_x/resolution;
            y = mouse_y/resolution;
        }
        if (mouse_b & 1) {
            world[x][y] = brush;
        }
        if (mouse_b & 2) {
            if (released) cout << "keydown";
            
            if (released&&!draw) {
                drawx = x;
                drawy = y;
                draw = !draw;
            }
                       
            
            
            
            /*
            
            
            
            
            if (drawx!=x || drawy!=y) {        
                if(!draw){
                    //set initial coordinates
                    drawx = x;
                    drawy = y;
                }
                else 
                    //draw everything from initial coordinates all the way to current coordinates
                    for (int i = drawy; drawy<y ? i<=y: i>=y; drawy<y ? i++ : i--) {
                        for (int j = drawx; drawx<x ? j<=x: j>=x; drawx<x ? j++ : j--) {
                            world[j][i] = brush;
                            redraw(j, i);
                        }
                    }
                //toggle drawing
                draw=!draw;
                drawx = x;
                drawy = y;
                */
            
            released = false;
        }
        else {
            if (!released&&(drawx!=x || drawy!=y)) {
                for (int i = drawy; drawy<y ? i<=y: i>=y; drawy<y ? i++ : i--) {
                    for (int j = drawx; drawx<x ? j<=x: j>=x; drawx<x ? j++ : j--) {
                        world[j][i] = brush;
                        redraw(j, i);
                    }
                }
            }
                
            released=true;
        }
        if (mouse_z!=oldz) {
            brush = !brush;
            oldz = mouse_z;
        }
    	if (keypressed()) {
    		int key = readkey();
    		switch ((key >> 8) & 0xFF) {
                //navigation
    			case KEY_DOWN:
                case KEY_S:
                    redraw(x, y);
                    if (y+1<height) y++;
    				break;
    			case KEY_UP:
                case KEY_W:
                    redraw(x, y);
                    if (y-1>=0) y--;
    				break;
    			case KEY_RIGHT:
                case KEY_D:
                    redraw(x, y);
                    if (x+1<width) x++;
    				break;
    			case KEY_LEFT:
                case KEY_A:
                    redraw(x, y);
    				if (x-1>=0) x--;
    				break;
    			//change brush
    			case KEY_Z:
                    brush = !brush;
                    break;
                //draw
                case KEY_SPACE:
                    world[x][y] = brush;
                    break;
                //draw large blocks
                case KEY_X:
                    if(!draw){
                        //set initial coordinates
                        drawx = x;
                        drawy = y;
                    }
                    else 
                        //draw everything from initial coordinates all the way to current coordinates
                        for (int i = drawy; drawy<y ? i<=y: i>=y; drawy<y ? i++ : i--) {
                            for (int j = drawx; drawx<x ? j<=x: j>=x; drawx<x ? j++ : j--) {
                                    world[j][i] = brush;
                                    redraw(j, i);
                            }
                        }
                    //toggle drawing
                    draw=!draw;
                    break;
                case KEY_E:

                    if (filter > 20) end = true;
                    else filter = 0;
                    break;
                }
            }
            filter++;
	        clear_to_color(selection, !draw ? makecol(200, 10, 200) : makecol(20, 200, 10));

            //change selection colour
            rectfill(selection, border, border, resolution-border-1, resolution-border-1, brush ? makecol(255,0,0) : makecol(255,255,255));
            
            //draw initial coordinate location
            if (draw) {
                draw_sprite(buffer, selection, drawx*resolution, drawy*resolution);
            }
            
            //change selection outline color once more
            clear_to_color(selection, draw ? makecol(200, 10, 200) : makecol(20, 200, 10));
            rectfill(selection, border, border, resolution - border-1, resolution - border-1, brush ? makecol(255,0,0) : makecol(255,255,255));
            
            draw_sprite(buffer, selection, x*resolution, y*resolution);
            
    
    		blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
        }
    
}

void close() {
    quit = true;
}
