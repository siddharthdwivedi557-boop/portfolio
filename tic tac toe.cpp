// TicTacToe_Full.cpp
// Fully working 3x3 Tic Tac Toe (Win32 + SAPI Voice)
// Includes: Hover highlight, AI, move numbering, undo, score, voice commentary
// Build with Visual Studio. Link: Ole32.lib, sapi.lib

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <ctime>
#include <sapi.h>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "sapi.lib")

using std::string;
using std::vector;

// ------------------ CONFIG ------------------
const int CELL = 120;
const int LEFT = 100;
const int TOP = 100;

// ------------------ GAME STATE ------------------
char board[9];
int moveNum[9];
int moveCount = 0;

bool gameOver = false;
int currentPlayer = 1;   // 1 = X human, 2 = O AI
int hoverCell = -1;

int scoreX = 0, scoreO = 0, scoreDraw = 0;

struct Move { int pos; char mark; int num; };
vector<Move> history;

int winLine[3] = { -1,-1,-1 };

ISpVoice* voice = nullptr;

// ------------------ UTILITY ------------------
wstring toW(const string& s) {
    int len = MultiByteToWideChar(CP_UTF8,0,s.c_str(),-1,NULL,0);
    wstring w(len,L'\0');
    MultiByteToWideChar(CP_UTF8,0,s.c_str(),-1,&w[0],len);
    if(!w.empty()) w.pop_back();
    return w;
}

void speak(string t) {
    if (!voice) return;
    voice->Speak(toW(t).c_str(), SPF_ASYNC, NULL);
}

// ------------------ GAME LOGIC ------------------
void resetBoard(bool keepScore=false) {
    for (int i=0;i<9;i++){
        board[i]=' ';
        moveNum[i]=0;
    }
    moveCount = 0;
    history.clear();
    gameOver = false;
    currentPlayer = 1;
    hoverCell = -1;
    winLine[0]=winLine[1]=winLine[2]=-1;

    if(!keepScore) scoreX = scoreO = scoreDraw = 0;
}

bool checkWin(int& w) {
    int L[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };
    for(int i=0;i<8;i++){
        int a=L[i][0], b=L[i][1], c=L[i][2];
        if(board[a] != ' ' && board[a]==board[b] && board[b]==board[c]){
            w = (board[a]=='X'?1:2);
            winLine[0]=a; winLine[1]=b; winLine[2]=c;
            return true;
        }
    }
    for(int i=0;i<9;i++) if(board[i]==' ') return false;
    w = -1;
    return true; // draw
}

int AIWinningMove(char M) {
    int L[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };
    for(int i=0;i<8;i++){
        int a=L[i][0], b=L[i][1], c=L[i][2];
        int cnt=0, empty=-1;
        int arr[3]={a,b,c};
        for(int k=0;k<3;k++){
            if(board[arr[k]] == M) cnt++;
            else if(board[arr[k]]==' ') empty = arr[k];
        }
        if(cnt==2 && empty!=-1) return empty;
    }
    return -1;
}

int AIRandomMove() {
    vector<int> E;
    for(int i=0;i<9;i++) if(board[i]==' ') E.push_back(i);
    if(E.empty()) return -1;

    static std::mt19937 rng(time(NULL));
    return E[rng()%E.size()];
}

int AIChoose() {
    int w = AIWinningMove('O');
    if(w!=-1) return w;
    int b = AIWinningMove('X');
    if(b!=-1) return b;
    return AIRandomMove();
}

void applyMove(int pos, char m) {
    board[pos] = m;
    moveCount++;
    moveNum[pos] = moveCount;
    history.push_back({pos, m, moveCount});
}

void undo() {
    if(history.empty()) return;

    Move last = history.back();
    history.pop_back();
    board[last.pos] = ' ';
    moveNum[last.pos] = 0;
    moveCount--;

    if(currentPlayer == 2 && !history.empty()) {
        Move last2 = history.back();
        history.pop_back();
        board[last2.pos] = ' ';
        moveNum[last2.pos]=0;
        moveCount--;
    }

    currentPlayer = 1;
    gameOver = false;
    winLine[0]=winLine[1]=winLine[2]=-1;
    speak("Move undone.");
}

// ------------------ DRAWING ------------------
void drawBoard(HDC hdc) {
    HPEN gridPen = CreatePen(PS_SOLID, 3, RGB(0,0,0));
    SelectObject(hdc, gridPen);

    for(int r=0;r<3;r++){
        for(int c=0;c<3;c++){
            int idx = r*3+c;
            int x = LEFT + c*CELL;
            int y = TOP + r*CELL;

            RECT rc = {x, y, x+CELL, y+CELL};

            if(idx == hoverCell && !gameOver && board[idx]==' ') {
                HBRUSH h = CreateSolidBrush(RGB(220,220,255));
                FillRect(hdc, &rc, h);
                DeleteObject(h);
            }

            Rectangle(hdc, x, y, x+CELL, y+CELL);

            if(board[idx] != ' ') {
                HFONT f = CreateFont(
                    60,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,
                    0,0,0,0,"Arial");
                SelectObject(hdc, f);

                string s(1,board[idx]);
                TextOutA(hdc, x+45, y+30, s.c_str(), 1);

                DeleteObject(f);

                // Move number
                HFONT f2 = CreateFont(20,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
                    0,0,0,0,"Arial");
                SelectObject(hdc, f2);

                string num = std::to_string(moveNum[idx]);
                TextOutA(hdc, x+5, y+5, num.c_str(), num.size());
                DeleteObject(f2);
            }
        }
    }

    // Highlight winning line
    if(gameOver && winLine[0]!=-1){
        HPEN w = CreatePen(PS_SOLID, 5, RGB(0,255,0));
        SelectObject(hdc, w);
        int a=winLine[0], b=winLine[2];
        int ax = LEFT + (a%3)*CELL + CELL/2;
        int ay = TOP + (a/3)*CELL + CELL/2;
        int bx = LEFT + (b%3)*CELL + CELL/2;
        int by = TOP + (b/3)*CELL + CELL/2;
        MoveToEx(hdc,ax,ay,NULL);
        LineTo(hdc,bx,by);
        DeleteObject(w);
    }

    DeleteObject(gridPen);

    // Score
    string sc = "Score: X=" + std::to_string(scoreX)
              + "   O=" + std::to_string(scoreO)
              + "   Draw=" + std::to_string(scoreDraw);

    TextOutA(hdc, 50, 20, sc.c_str(), sc.size());
}

// ------------------ WINDOW CALLBACK ------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {

    switch(msg){
    case WM_MOUSEMOVE: {
        int mx = LOWORD(l);
        int my = HIWORD(l);

        if(gameOver) { hoverCell = -1; break; }

        for(int i=0;i<9;i++){
            int r=i/3,c=i%3;
            int x=LEFT+c*CELL, y=TOP+r*CELL;
            if(mx>=x && mx<x+CELL && my>=y && my<y+CELL){
                if(board[i]==' ') hoverCell = i;
                else hoverCell = -1;
                break;
            }
            hoverCell = -1;
        }
        InvalidateRect(hwnd,NULL,TRUE);
    } break;

    case WM_LBUTTONDOWN: {
        if(gameOver) { resetBoard(true); InvalidateRect(hwnd,NULL,true); return 0; }

        int mx = LOWORD(l);
        int my = HIWORD(l);

        for(int i=0;i<9;i++){
            int r=i/3,c=i%3;
            int x=LEFT+c*CELL, y=TOP+r*CELL;
            if(mx>=x && mx<x+CELL && my>=y && my<y+CELL){
                if(board[i]==' '){
                    applyMove(i,'X');
                    speak("Player X moves.");
                    int w;
                    if(checkWin(w)){
                        gameOver = true;
                        if(w==1){ speak("X wins!"); scoreX++; }
                        else if(w==2){ speak("O wins!"); scoreO++; }
                        else { speak("Draw match."); scoreDraw++; }
                        InvalidateRect(hwnd,NULL,true);
                        return 0;
                    }
                    currentPlayer = 2;

                    // AI turn
                    int mv = AIChoose();
                    if(mv!=-1){
                        applyMove(mv,'O');
                        speak("AI moves.");
                    }

                    if(checkWin(w)){
                        gameOver = true;
                        if(w==1){ speak("X wins!"); scoreX++; }
                        else if(w==2){ speak("O wins!"); scoreO++; }
                        else { speak("Match drawn."); scoreDraw++; }
                    }
                }
            }
        }

        InvalidateRect(hwnd,NULL,true);
    } break;

    case WM_KEYDOWN:
        if(w == 'U'){ undo(); InvalidateRect(hwnd,NULL,true); }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd,&ps);
        drawBoard(hdc);
        EndPaint(hwnd,&ps);
    } break;

    case WM_DESTROY:
        if(voice) voice->Release();
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd,msg,w,l);
}

// ------------------ WINMAIN ------------------
int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int n) {

    CoInitialize(NULL);
    CoCreateInstance(CLSID_SpVoice,NULL,CLSCTX_ALL,IID_ISpVoice,(void**)&voice);

    resetBoard();

    WNDCLASS wc = {0};
    wc.hInstance = h;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = "Tic3Class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("Tic3Class","Tic Tac Toe (Voice + AI)",
        WS_OVERLAPPEDWINDOW,200,100,600,600,
        NULL,NULL,h,NULL);

    ShowWindow(hwnd,n);

    MSG msg;
    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}
