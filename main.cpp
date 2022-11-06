#include <cstdio>
#include <iostream>
#include <conio.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;
typedef long long ll;
typedef unsigned char uni;
#define PI 3.14159265

#include "bmputil.h"

ll w, h, outW = 1000, outH = 1000;
double camX, camY, camR = 0.0, speed = 1, rspeed = 2, rs = 5, darkr = 0.7;
int fov = 60, mode = 0, noClip = 0, run = 0;
vector<vector<int> > board;
vector<vector<Color> > colData;
string inStr, outStr, screenStr;

Canvas canvas = Canvas(100,100);

struct Ray{
	double dist;
	int side;
	Color col;
	Ray(double d, int s, Color c): dist(d), side(s), col(c) {}
};

void Setup(string path){
	FILE *inp = fopen(path.c_str(),"rb");
	for(int i=0;i<18;i++) getc(inp);
	for(int i=0;i<4;i++){
		w += (ll)pow(256,i)*(int)getc(inp);
	}
	for(int i=0;i<4;i++){
		h += (ll)pow(256,i)*(int)getc(inp);
	}
	for(int i=0;i<28;i++) getc(inp);
	
	board.resize(w+2,vector<int>(h+2));
	colData.resize(w+2,vector<Color>(h+2));
	for(int i=1;i<=w;i++){
		for(int j=1;j<=h;j++){
			uni r,g,b;
			b = getc(inp);
			g = getc(inp);
			r = getc(inp);
			if(r==(uni)255&&b==g&&g==0){camX = i; camY = j;}
			else if(!(r==b&&b==g&&g==(uni)255)) board[i][j] = 1;
			if(r==b&&b==g&&g==0) colData[i][j] = {(uni)255,(uni)255,(uni)255};
			else colData[i][j] = {r,g,b};
		}
	}
	
	for(int i=0;i<w+2;i++){
		board[i][0] = 1;
		board[i][h+1] = 1;
		colData[i][0] = {(uni)255,(uni)255,(uni)255};
		colData[i][h+1] = {(uni)255,(uni)255,(uni)255};
	}
	for(int i=0;i<h+2;i++){
		board[0][i] = 1;
		board[w+1][i] = 1;
		colData[0][i] = {(uni)255,(uni)255,(uni)255};
		colData[w+1][i] = {(uni)255,(uni)255,(uni)255};
	}
}

vector<Ray> Raycast(){
	vector<Ray> rays;
	
	for(int t=-fov/2;t<=fov-fov/2;t++){
		double rot = camR+t;
		while(rot<0) rot += 360;
		while(rot>=360) rot -= 360;
		double dirX = cos(rot*PI/180), dirY = sin(rot*PI/180);
		
		int mapX = (int)camX, mapY = (int)camY;
		
		int nX = dirX>0?1:-1, nY = dirY>0?1:-1, hit = 0, side;
		
		double deltaX = dirX==0?1e30:abs(1/dirX), sideX;
		double deltaY = dirY==0?1e30:abs(1/dirY), sideY;
		if(dirX<0) sideX = (camX - mapX)*deltaX;
		else sideX = (mapX + 1.0 - camX)*deltaX;
		if(dirY<0) sideY = (camY - mapY)*deltaY;
		else sideY = (mapY + 1.0 - camY)*deltaY;
		
		Color bc;
		while(!hit){
			if(sideX < sideY){
				sideX += deltaX;
				mapX += nX;
				side = 0;
			}
			else{
				sideY += deltaY;
				mapY += nY;
				side = 1;
			}
			if(board[mapX][mapY]>0){
				hit = 1;
				bc = colData[mapX][mapY];
			}
		}
		double dist;
		
		if(side == 0) dist = sideX - deltaX;
		else dist = sideY - deltaY;
		dist *= cos(abs(t)*PI/180);
		
		//cout << (int)bc.r << (int)bc.g << (int)bc.b << "\n";
		
		rays.push_back(Ray(dist, side, bc));
	}
	
	return rays;
}

void Draw(){
	canvas = Canvas(outW, outH);
	RectMode("CENTER");
	
	vector<Ray> rays = Raycast();
	int sz = rays.size();
	
	for(int i=0;i<sz;i++){
		double dist = rays[i].dist;
		
		double val = 1.0*(dist-1)/(max(w,h)-1);
		val = 1-val;
		int col = (int)(255*val);
		Color tmp = {(uni)col,(uni)col,(uni)col};
		
		if(mode == 1){
			Color c = rays[i].col;
			
			if(rays[i].side == 0) tmp = c;
			else{
				Color c = rays[i].col;
				int r = (int)((int)c.r*darkr);
				int g = (int)((int)c.g*darkr);
				int b = (int)((int)c.b*darkr);
				tmp = {(uni)r,(uni)g,(uni)b};
			}
		}
		else if(mode == 2){
			Color c = rays[i].col;
			int r = (int)((int)c.r*val);
			int g = (int)((int)c.g*val);
			int b = (int)((int)c.b*val);
			tmp = {(uni)r,(uni)g,(uni)b};
		}
		
		//cout << (int)(outH/dist[i]) << "\n";
		canvas.rect(outW/sz+outW/sz*i,outH/2,outW/sz,(int)(outH/dist),tmp);
	}
	
	RectMode("ALIGN");
	if(noClip) canvas.rect(1,1,50,50,{0,(uni)100,(uni)255});
	if(run) canvas.rect(60,1,50,50,{0,(uni)255,0});
	
	canvas.Export(outStr,"path");
}

string currentTime(){
	time_t now = time(0);
    tm t;
    char buf[80];
    t = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H-%M-%S", &t);
    
    return buf;
}

int Check(double a, double b){
	int mx = (int)a, my = (int)b;
	return !board[mx][my];
}

void Move(char x){
	string name;
	double r = camR*PI/180;
	if(x=='w'){
		if(!noClip&&!Check(camX+speed*cos(r),camY+speed*sin(r))) return;
		camX += speed*cos(r);
		camY += speed*sin(r);
	}
	else if(x=='s'){
		if(!noClip&&!Check(camX-speed*cos(r),camY-speed*sin(r))) return;
		camX -= speed*cos(r);
		camY -= speed*sin(r);
	}
	else if(x=='d'){
		if(!noClip&&!Check(camX-speed*sin(r),camY+speed*cos(r))) return;
		camX -= speed*sin(r);
		camY += speed*cos(r);
	}
	else if(x=='a'){
		if(!noClip&&!Check(camX+speed*sin(r),camY-speed*cos(r))) return;
		camX += speed*sin(r);
		camY -= speed*cos(r);
	}
	else if(x=='q') camR -= rs;
	else if(x=='e') camR += rs;
	else if(x=='Q') camR -= 3*rs;
	else if(x=='E') camR += 3*rs;
	else if(x=='1') mode = 1;
	else if(x=='2') mode = 2;
	else if(x=='3') mode = 3;
	else if(x=='c') noClip = !noClip;
	else if(x=='r') run = !run;
	else if(x=='f'){
		name = screenStr;
		name += " " + currentTime();
		name += ".bmp";
		canvas.Export(name,"path");
	}
	else if(x==' ') camR += 180;
	else if(x=='x'){
		cout << camX << " " << camY << " " << camR;
		exit(0);
	}
}

int main(){
	inStr = "Input\\input6.bmp";
	outStr = "Output\\output.bmp";
	screenStr = "Output\\Screenshots\\screenshot";
	
	Setup(inStr);
	fov %= 360;
	mode = 1;
	camR = 130.0;
	
	for(ll t=0;;t++){
		Draw();
		char x = getch();
		Move(x);
		cout << "done " << t << "\r";
	}
}
