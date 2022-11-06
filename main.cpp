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
int fov = 60, mode = 1, noClip = 0, run = 0;
Color flCol = {(uni)170,(uni)170,(uni)170};
Color sbCol = {(uni)66,(uni)225,(uni)255}, stCol = {(uni)255,(uni)255,(uni)255};
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
	
	canvas.rect(0,0,outW,outH/2,flCol);
	canvas.Gradient(0,outH-outH/2,outW,outH-outH/2,sbCol,stCol);
	
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
		
		double wid = 1.0*outW/sz;
		double xpos = 1.0*wid*(2*i+1)/2;
		
		//cout << (int)(outH/dist[i]) << "\n";
		canvas.rect((int)xpos,outH/2,(int)wid+1,(int)(outH/dist),tmp);
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

int CheckIn(double a, double b){
	if(a<0||b<0||a>=w||b>=h) return 0;
	else return 1;
}

int ColVal(int r, int g, int b){
	return !(r>255||g>255||b>255||r<0||b<0||g<0);
}

void Command(string cmd){
	if(cmd=="camloc"){
		string mmode; cin >> mmode;
		double xp,yp,nx,ny; cin >> xp >> yp;
		if(mmode=="add"){nx=camX+xp;ny=camY+yp;}
		else if(mmode=="set"){nx=xp;ny=yp;}
		else{
			cout << "Invalid mode inputed!\n";
			return;
		}
		
		if(!CheckIn(nx,ny)) cout << "Inputed Location is out of bounds!\n";
		else if(!noClip&&!Check(nx,ny)){
			cout << "Inputed Location is inside a wall!\n";
			cout << "Turn on noclip(c) to move inside wall\n";
		}
		else{
			camX = nx, camY = ny;
			cout << "Changed camera location to: " << nx << " " << ny << "\n";
		}
	}
	else if(cmd=="camrot"){
		string rmode; cin >> rmode;
		double rt,nr; cin >> rt;
		if(rmode=="add") nr = rt+camR;
		else if(rmode=="set") nr = rt;
		else{
			cout << "Invalid mode inputed!\n";
			return;
		}
		
		camR = nr;
		cout << "Changed camera rotation to: " << nr << "\n";
	}
	else if(cmd=="floorcl"){
		int r,g,b; cin >> r >> g >> b;
		if(!ColVal(r,g,b)) cout << "Invalid RGB values!\n";
		else{
			flCol = {(uni)r,(uni)g,(uni)b};
			cout << "Changed floor color to: {" << r << "," << g << "," << b << "}\n";
		}
	}
	else if(cmd=="skycl"){
		string smode; cin >> smode;
		if(smode=="solid"){
			int r,g,b; cin >> r >> g >> b;
			if(!ColVal(r,g,b)) cout << "Invalid RGB values!\n";
			else{
				sbCol = {(uni)r,(uni)g,(uni)b};
				stCol = sbCol;
				cout << "Changed sky color to: {" << r << "," << g << "," << b << "}\n";
			}
		}
		else if(smode=="grad"){
			int r1,g1,b1; cin >> r1 >> g1 >> b1;
			int r2,g2,b2; cin >> r2 >> g2 >> b2;
			if(!ColVal(r1,g1,b1)||!ColVal(r2,g2,b2)) cout << "Invalid RGB values!\n";
			else{
				sbCol = {(uni)r1,(uni)g1,(uni)b1};
				stCol = {(uni)r2,(uni)g2,(uni)b2};
				cout << "Changed sky color to: ";
				cout << "{" << r1 << "," << g1 << "," << b1 << "} to ";
				cout << "{" << r2 << "," << g2 << "," << b2 << "}\n";
			}
		}
		else{
			cout << "Invalid mode inputed!\n";
			return;
		}
	}
	else if(cmd=="fov"){
		string fmode; cin >> fmode;
		int fd,nf; cin >> fd;
		if(fmode=="add") nf = (fov+fd)%360;
		else if(fmode=="set") nf = fd%360;
		else{
			cout << "Invalid mode inputed!\n";
			return;
		}
		
		fov = nf;
		cout << "Changed camera FOV to: " << nf << "\n";
	}
	else if(cmd=="res"){
		int x,y; cin >> x >> y;
		if(x<200||y<200) cout << "Too small! Width and height must be at least 200 pixels\n";
		else{
			outW = x, outH = y;
			cout << "Changed output resolution to: " << x << " pixels * " << y << " pixels\n";
		}
	}
}

void Move(char x){
	string name;
	double r = camR*PI/180;
	double cs = cos(r), sn = sin(r);
	if(x=='w'){
		if(!noClip&&!Check(camX+speed*cs,camY+speed*sn)) return;
		if(!CheckIn(camX+speed*cs,camY+speed*sn)) return;
		camX += speed*cs;
		camY += speed*sn;
	}
	else if(x=='s'){
		if(!noClip&&!Check(camX-speed*cs,camY-speed*sn)) return;
		if(!CheckIn(camX-speed*cs,camY-speed*sn)) return;
		camX -= speed*cs;
		camY -= speed*sn;
	}
	else if(x=='d'){
		if(!noClip&&!Check(camX-speed*sn,camY+speed*cs)) return;
		if(!CheckIn(camX-speed*sn,camY+speed*cs)) return;
		camX -= speed*sn;
		camY += speed*cs;
	}
	else if(x=='a'){
		if(!noClip&&!Check(camX+speed*sn,camY-speed*cs)) return;
		if(!CheckIn(camX+speed*sn,camY-speed*cs)) return;
		camX += speed*sn;
		camY -= speed*cs;
	}
	else if(x=='q') camR -= rs;
	else if(x=='e') camR += rs;
	else if(x=='Q') camR -= 3*rs;
	else if(x=='E') camR += 3*rs;
	else if(x=='1') mode = 1;
	else if(x=='2') mode = 2;
	else if(x=='3') mode = 3;
	else if(x=='c'){
		if(noClip&&!Check(camX,camY)) return;
		noClip = !noClip;
	}
	else if(x=='r') run = !run;
	else if(x=='f'){
		name = screenStr;
		name += " " + currentTime();
		name += ".bmp";
		canvas.Export(name,"path");
	}
	else if(x==' ') camR += 180;
	else if(x=='/'){
		cout << "\n";
		cout << ">> ";
		string cmd; cin >> cmd;
		Command(cmd);
	}
	else if(x=='x'){
		exit(0);
	}
}

int main(){
	inStr = "Input\\input6.bmp";
	outStr = "Output\\output.bmp";
	screenStr = "Output\\Screenshots\\screenshot";
	
	cout<<" *******       **     **    **   ******      **      ******** ********** ******** *******                        \n";
	cout<<"/**////**     ****   //**  **   **////**    ****    **////// /////**/// /**///// /**////**       *          *    \n";
	cout<<"/**   /**    **//**   //****   **    //    **//**  /**           /**    /**      /**   /**      /*         /*    \n";
	cout<<"/*******    **  //**   //**   /**         **  //** /*********    /**    /******* /*******    *********  *********\n";
	cout<<"/**///**   **********   /**   /**        **********////////**    /**    /**////  /**///**   /////*///  /////*/// \n";
	cout<<"/**  //** /**//////**   /**   //**    **/**//////**       /**    /**    /**      /**  //**      /*         /*    \n";
	cout<<"/**   //**/**     /**   /**    //****** /**     /** ********     /**    /********/**   //**     /          /     \n";
	cout<<"//     // //      //    //      //////  //      // ////////      //     //////// //     //                       \n\n";
	
	cout<<"A C++ bitmap raycaster project by gs22123\n\n";
	cout<<"Type /help for a list of controls and commands\n\n\n\n";
	
	Setup(inStr);
	fov %= 360;
	mode = 1;
	camR = 130.0;
	
	cout << "X: " << camX << " Y: " << camY << " Rot: " << camR << " FOV: " << fov << "\r";
	for(ll t=0;;t++){
		Draw();
		char x = getch();
		Move(x);
		
		cout.precision(3); cout << fixed;
		cout << "X: " << camX << " Y: " << camY << " Rot: " << camR << " FOV: " << fov << "\r";
	}
}
