#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
typedef unsigned char col;
typedef long long ll;

struct Color{
	col r,g,b;
};

int rectMode = 0;

void RectMode(string mode){
	if(mode == "ALIGN") rectMode = 0;
	else if(mode == "CENTER") rectMode = 1;
}

class Canvas{
	public:
		int width, height;
		
		Canvas(int w, int h){
			width = w;
			height = h;
			
			board.resize(w,vector<Color>(h));
			
			for(int i=0;i<w;i++){
				for(int j=0;j<h;j++){
					board[i][j] = {0,0,0};
				}
			}
		}
		
		void rect(int xpos, int ypos, int w, int h, Color c){
			int x1, y1, x2, y2;
			if(rectMode == 0){
				x1 = xpos; y1 = ypos;
				x2 = xpos + w - 1; y2 = ypos + h - 1;
			}
			else{
				x1 = xpos - (w-1)/2; y1 = ypos - (h-1)/2;
				x2 = x1 + w - 1; y2 = y1 + h - 1;
			}
			
			x1 = max(min(x1,width-1),0); y1 = max(min(y1,height-1),0);
			x2 = max(min(x2,width-1),0); y2 = max(min(y2,height-1),0);
			
			//cout << x1 << " " << x2 << " " << y1 << " " << y2 << "\n";
			
			for(int i=x1;i<=x2;i++){
				for(int j=y1;j<=y2;j++){
					board[i][j] = c;
				}
			}
		}
		
		void Gradient(int xpos, int ypos, int w, int h, Color c1, Color c2){
			int x1, y1, x2, y2;
			if(rectMode == 0){
				x1 = xpos; y1 = ypos;
				x2 = xpos + w - 1; y2 = ypos + h - 1;
			}
			else{
				x1 = xpos - (w-1)/2; y1 = ypos - (h-1)/2;
				x2 = x1 + w - 1; y2 = y1 + h - 1;
			}
			
			x1 = max(min(x1,width-1),0); y1 = max(min(y1,height-1),0);
			x2 = max(min(x2,width-1),0); y2 = max(min(y2,height-1),0);
			int r1 = (int)c1.r, g1 = (int)c1.g, b1 = (int)c1.b, cr = rectMode;
			int r2 = (int)c2.r, g2 = (int)c2.g, b2 = (int)c2.b;
			rectMode = 1;
			
			for(int i=y1;i<=y2;i++){
				int nr = r1 + (int)(1.0*(i-y1)/h*(r2-r1));
				int ng = g1 + (int)(1.0*(i-y1)/h*(g2-g1));
				int nb = b1 + (int)(1.0*(i-y1)/h*(b2-b1));
				rect((x1+x2)/2,i,w,1,{(uni)nr,(uni)ng,(uni)nb});
			}
			
			rectMode = cr;
		}
		
		void Export(string name, string mode = "name"){
			if(mode=="name") name += ".bmp";
			file = fopen(name.c_str(),"wb");
			SetHeader();
			for(int i=0;i<height;i++){
				for(int j=0;j<width;j++){
					putf(board[j][i].b);
					putf(board[j][i].g);
					putf(board[j][i].r);
				}
			}
			fclose(file);
		}
		
	private:
		FILE *file;
		vector<vector<Color>> board;
		
		void SetHeader(){
			putf(66); putf(77);
			ConvertUint8(width*height*30+54);
			for(int j=0;j<4;j++) putf(0);
			putf(54); for(int j=0;j<3;j++) putf(0);
			putf(40); for(int j=0;j<3;j++) putf(0);
			ConvertUint8(width); ConvertUint8(height);
			putf(1); putf(0);
			putf(24); for(int j=0;j<5;j++) putf(0);
			ConvertUint8(width*height*30);
			for(int j=0;j<16;j++) putf(0);
		}
		
		void ConvertUint8(long long num){
			int tmp1 = num/(256*256*256);
			num -= (ll)tmp1*(256*256*256);
			int tmp2 = num/(256*256);
			num -= (ll)tmp2*(256*256);
			int tmp3 = num/256;
			num -= (ll)tmp3*256;
			putc((col)num,file); putc((col)tmp3,file);
			putc((col)tmp2,file); putc((col)tmp1,file);
		}
		
		void putf(int x){putc((col)x,file);}
};
