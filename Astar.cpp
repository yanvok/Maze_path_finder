#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <stack>
#include <utility>
#include <limits>
#include <cmath>
using namespace std;

struct pos{
    int x;
    int y;
};

struct m{
    vector<string> grid;
    int rows = 0, cols = 0;
    int Sx = 0, Sy = 0, Ex = 0, Ey = 0;
};

using Path = vector<pos>;
void wait();
bool empty(const m &m, int r, int c);
bool loadGrid(const string &file, m &m);
double distHeu(int x1, int y1, int x2, int y2);
Path pathRec(const pos &endPos, const pos &startPos,const vector<vector<pair<int,int>>> &parent);
void print(const m & m, const vector<vector<bool>> & visited,bool pa = false, const Path & P = {});

void func(m &m) {
    
    //f = g + heuristic
    using it = pair<double, pair<int,int>>;

    const int dirH[4] = {-1,0, 0, 1};
    const int dirW[4] = {0, -1, 1, 0};
    int sx = m.Sx, sy = m.Sy, ex = m.Ex, ey = m.Ey;

    vector<vector<bool>> visited(m.rows,vector<bool>(m.cols,false));
    vector<vector<pair<int,int>>> parent(m.rows, vector<pair<int,int>>(m.cols, {-1, -1}));
    //cost from S to given cell
    vector<vector<double>> g(m.rows, vector<double>(m.cols, numeric_limits<double>::infinity()));
    g[sy][sx] = 0.0;
    priority_queue<it, vector<it>, greater<it>> opend;

    double stHeu = distHeu(sx, sy, ex, ey);
    opend.push({stHeu, {sx, sy}});

    bool foundEnd = false;
    int count = 0;

    while(!opend.empty()){

        auto [currentF, coord] = opend.top();
        opend.pop();
        int cx = coord.first;
        int cy = coord.second;

        if(visited[cy][cx]) continue; //skip if visited
        visited[cy][cx] = true;
        count++;
        
        if(cx == ex && cy == ey) {
            foundEnd = true;
            break;
        }

        //print(m, visited, false);
        //wait();

        for(int i=0; i<4; i++){
            
            int nx = cx + dirW[i];
            int ny = cy + dirH[i];
            
            if(!empty(m, ny, nx)) continue;

            double newC = g[cy][cx] + 1.0; //cost to move 1 step, if neighbor is not visited

            
            if(newC < g[ny][nx]) { //if best than update
                g[ny][nx] = newC;
                parent[ny][nx] = {cx, cy};

                double hVal = distHeu(nx, ny, ex, ey);
                double fVal = newC + hVal;
                
                opend.push({fVal, {nx, ny}}); //push
            }
        }
    }

    Path pa;
    if(foundEnd) {
        pos st = {sx, sy};
        pos end = {ex, ey};
        pa = pathRec(st, end, parent);
    }
    
    print(m, visited, true, pa); 
    cout << "Nodes expanded: " << count << "\n";
    if(!foundEnd) {
        cout << "No path\n";
    }
    else cout << "Path length: " << (pa.size() - 1) << "\n";

}

int main(int argc, char* argv[]) {
    
    string file;
    if(argc > 1) {
        file = argv[1];
    } 
    else return 1;
    
    m m;
    if(!loadGrid(file, m)) return 1;
    
    func(m);

    return 0;
}

Path pathRec(const pos &S, const pos &E, const vector<vector<pair<int,int>>> &parent) {
    Path P;
    pos curr = E;
    while (!(curr.x == S.x && curr.y == S.y)) {
        P.push_back(curr);
        auto [px, py] = parent[curr.y][curr.x];
        curr = {px, py};
    }
    P.push_back(S);
    reverse(P.begin(), P.end());
    return P;
}

bool loadGrid(const string &file, m &m){
    
    ifstream infile(file);
    if(!infile.is_open()) {
        cout << "Could not open the " << file << endl;
        return false;
    }
    
    m.grid.clear();
    string line;
    bool foundStart = false, foundEnd = false;

    while (getline(infile, line)) {
        if(line.rfind("start", 0) == 0) {
            istringstream iss(line.substr(5)); 
            char comma;
            int x, y;
            if(iss >> x >> comma >> y) {
                m.Sx = x;
                m.Sy = y;
                foundStart = true;
            }
        }
        else if(line.rfind("end", 0) == 0) {
            istringstream iss(line.substr(3));
            char comma;
            int x, y;
            if(iss >> x >> comma >> y) {
                m.Ex = x;
                m.Ey = y;
                foundEnd = true;
            }
        }
        else {
            m.grid.push_back(line);
        }
    }

    infile.close();

    if(!foundStart || !foundEnd) {
        cout << "m file missing \"start\" or 'end' line." <<endl;
        return false;
    }

    m.rows = (int)m.grid.size();
    if(m.rows > 0)
        m.cols = (int)m.grid[0].size();

    if(m.Sy < 0 || m.Sy >= m.rows || m.Sx < 0 || m.Sx >= m.cols){
        cout << "\"Start\" out of bounds" << endl;
        return false;
    }
    if(m.Ey < 0 || m.Ey >= m.rows || m.Ex < 0 || m.Ex >= m.cols){
        cout << "\"End\" out of bounds" << endl;
        return false;
    }

    return true;
}

double distHeu(int x1, int y1, int x2, int y2){
    return abs(x1 - x2) + abs(y1 - y2);
}

void print(const m & m, const vector<vector<bool>> & visited, bool pa, const Path & P) {
    vector<string> v = m.grid;

    //mark visited nodes
    if(!pa){
        for(int r = 0; r < m.rows; r++){
            for(int c = 0; c < m.cols; c++){
                if(visited[r][c] && v[r][c] == ' '){
                    v[r][c] = '#'; 
                }
            }
        }
    }

    if(pa){
        for(auto & p : P){
            int px = p.x, py = p.y;
            if(v[py][px] == ' ' || v[py][px] == '#'){
                v[py][px] = 'o';
            }
        }

    }

    v[m.Sy][m.Sx] = 'S';
    v[m.Ey][m.Ex] = 'E';

    //grid
    for(const auto & row: v) {
        cout << row << "\n";
    }
    cout << "--------------\n"
         << "S Start\n"
         << "E End\n"
         << "# Opened node\n"
         << "o Final P\n"
         << "X Wall\n"
         << "space Fresh node\n"
         << "--------------\n\n";
}

bool valid(const m &m, int r, int c) {
    return (r >= 0 && r < m.rows && c >= 0 && c < m.cols);
}

bool empty(const m &m, int r, int c) {
    if(!valid(m, r, c)) return false;
    return (m.grid[r][c] == ' ');
}

void wait(){
    cout << "Use \"Enter\" to go further";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
