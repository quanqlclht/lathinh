#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
using namespace std;


const int CARD_WIDTH = 150;
const int CARD_HEIGHT = 150;

const int DEFAULT_NUM_COLS = 6;
const int DEFAULT_NUM_ROWS = 6;

const string GAME_PATH = "lathinh.png";
const string WINDOW_TITLE = "lat hinh";


bool opened[DEFAULT_NUM_ROWS*DEFAULT_NUM_COLS];

int click=0,selected=0;

vector<int> imagePos;

struct Graphic {
    SDL_Window *window;
    SDL_Texture *texture;
    vector<SDL_Rect> gameRects;
    SDL_Renderer *renderer;
};


bool initGraphic(Graphic &g,int nRows,int nCols); // khởi tạo graphic

void finalizeGraphic(Graphic &g); // huỷ những gì khởi tạo bởi SDL và SDL_image

SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path); // load ảnh từ đường dẫn, trả về dưới dạng SDL_Texture

void initImageRects(vector<SDL_Rect> &rects); // lưu vị trí các ảnh vào tham giá rects

void initGame(Graphic &g, int nRows,int nCols); // khởi tạo game

void update(Graphic &g,int click,int value,int tr); // update game khi có event

void clickMouse(SDL_Event &event, Graphic &g); // sự kiện click chuột

void err(const string &msg); // cửa sổ thông báo lỗi

void randomImages(int nRows,int nCols); // random ảnh

int main(int argc, char* argv[])
{
    srand(time(0));
    int nRows=DEFAULT_NUM_ROWS;
    int nCols=DEFAULT_NUM_COLS;
    Graphic g;
    if (!initGraphic(g, nRows, nCols))
    {
        finalizeGraphic(g);
        return EXIT_FAILURE;
    }
    randomImages(nRows, nCols);
    initGame(g, nRows, nCols);
    SDL_Event event;
    bool quit=false;
    while (!quit)
    {
        if (click==1) update(g,1,selected,0);
        else update(g,0,0,0);
        while (SDL_PollEvent(&event)!=0)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                clickMouse(event, g);
            }
            if (event.type == SDL_QUIT)
            {
                quit=true;
                break;
            }
        }

        bool win=true;
        for (int i=0;i<DEFAULT_NUM_COLS*DEFAULT_NUM_COLS;i++)
        {
            if (opened[i]==false) win=false;
        }
        if (win==true)
        {
            quit=true;
        }
    }
    finalizeGraphic(g);
    return EXIT_SUCCESS;
}


bool initGraphic(Graphic &g, int nRows, int nCols) {
    g.window = NULL;
    g.renderer = NULL;
    g.texture = NULL;

    int sdlFlags = SDL_INIT_VIDEO;  // Dùng SDL để hiển thị hình ảnh
    if (SDL_Init(sdlFlags) != 0) {
        err("SDL could not initialize!");
        return false;
    }

    int imgFlags = IMG_INIT_PNG;    // Dùng SDL_Image để load ảnh png
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        err("SDL_Image could not initialize!");
        return false;
    }

    g.window = SDL_CreateWindow(WINDOW_TITLE.c_str(), // tiêu đề
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                nCols * CARD_WIDTH, // chiều rộng window
                                nRows * CARD_HEIGHT, //chiều dài window
                                SDL_WINDOW_SHOWN); // hiện

    if (g.window == NULL) {
        err("Window could not be created!");
        return false;
    }

    g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);
    if (g.renderer == NULL) {
        err("Renderer could not be created!");
        return false;
    }

    g.texture = createTexture(g.renderer, GAME_PATH);
    if (g.texture == NULL) {
        err("Unable to create texture from " + GAME_PATH + "!");
        return false;
    }

    initImageRects(g.gameRects);
    return true;
}

void finalizeGraphic(Graphic &g) {
    SDL_DestroyTexture(g.texture);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyWindow(g.window);
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* createTexture(SDL_Renderer *renderer, const string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (surface == NULL)
    {
        err("Unable to load image " + path + "!");
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void initImageRects(vector<SDL_Rect> &rects)
{
    SDL_Rect rect;
    for (int i=0;i<2;i++)
        for (int j=0;j<5;j++)
        {
                rect = {j*150, //toạ độ x trong ảnh gốc
                            i*150,  // toạ độ y trong ảnh gốc
                            150,    // chiều rộng ảnh
                            150};    // chiều dài ảnh
            rects.push_back(rect);
        }
    for (int i=2;i<=7;++i)
    {
        for (int j=0;j<=5;++j)
        {
            rect={j*150,i*150,150,150};
            rects.push_back(rect);
        }
    }
}

void randomImages(int nRows,int nCols)
{
    int maxVal=nRows*nCols;

    vector<bool> chosenVals(maxVal,false);
    while (imagePos.size()<maxVal)
    {
    int random=rand()%maxVal;
        if (!chosenVals[random])
        {
            imagePos.push_back(random);
            chosenVals[random]= true;
        }
    }
}

void initGame(Graphic &g, int nRows,int nCols)
{
    SDL_RenderClear(g.renderer); // xoá những gì đã vẽ
    SDL_Rect rect;
    for (int i=0;i<nRows;i++)
    {
        for (int j=0;j<nCols;j++)
        {
            int val=i*nRows+j;
            rect={j*CARD_WIDTH, //toạ độ x
                i*CARD_HEIGHT, // toạ độ y
                CARD_WIDTH,  // chiều rộng
                CARD_HEIGHT}; //chiều cao
            SDL_RenderCopy(g.renderer,
                           g.texture,
                           &g.gameRects[imagePos[val]%9], // có 9 ảnh
                           &rect);
        }
    }
    SDL_RenderPresent(g.renderer); // hiện những gì vừa vẽ
}

void update(Graphic &g,int click,int value,int selected)
{
    SDL_RenderClear(g.renderer);
    SDL_Rect rect;
    int nRows=DEFAULT_NUM_ROWS;
    int nCols=DEFAULT_NUM_COLS;
    for (int i=0;i<nRows;i++)
    {
        for (int j=0;j<nCols;j++)
        {
            int val=nRows*i+j;
            int k=0,Pos=9;
            if ((click==1 && val==value) || (click==2 && val==value) || (click==2 && val==selected) ) k=1;
            if (click==2 && value==selected) k=0;
            //hiện những ô click lần 1 và lần 2
            if (opened[val]) k=2;
            if (k==2) Pos=val+10;
            if (k==1) Pos=imagePos[val]%9;
            rect={j*CARD_WIDTH,i*CARD_HEIGHT,CARD_WIDTH,CARD_HEIGHT};
            SDL_RenderCopy(g.renderer,
                           g.texture,
                           &g.gameRects[Pos],
                           &rect);
        }
    }
    SDL_RenderPresent(g.renderer);
    SDL_Delay(300);
}
void clickMouse(SDL_Event &event,Graphic &g)
{
    SDL_MouseButtonEvent mouse=event.button;
    int row=mouse.y/CARD_WIDTH;
    int col=mouse.x/CARD_HEIGHT;
    int value=row*DEFAULT_NUM_COLS+col;
    if (opened[value]) update(g, 0, 0, 0); //tránh click 2 lần vào 1 ô rồi mất hình
    else
        {
            click++;
            if (click==2)
            {
                if (selected==value) update(g,2,value,selected); //nếu click 2 lần vào 1 ô thì úp
                else
                {
                if ((imagePos[value]%9)==(imagePos[selected]%9))
                {
                    update(g,2,value,selected);
                    opened[value]=true;
                    opened[selected]=true;
                }
                else
                {
                    update(g,2,value,selected);
                }
                }
                click=0;
            }
            else
            {
                selected=value;
                update(g,1,value,0);
            }
        }
}

void err(const string &msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg.c_str(), NULL);
}
