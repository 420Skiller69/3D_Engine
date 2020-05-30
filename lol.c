#include <stdio.h>
#include <SDL.h>
#undef main
#include <time.h>
#include <Windows.h>
#include <Math.h>

int PointInPoly(double** poly, double* point);
int drawFirst(double** pol1_2D, double** pol2_2D, double** pol1_3D, double** pol2_3D);
void drawPoly(int** points, SDL_Renderer* rend);

int WIN_W = 500;
int WIN_H = 500;
#define FPS  100
#define mouSens 5
#define speed 3				/100.0
#define POV 1
#define RES 20

SDL_Event event;

SDL_Rect rect;
SDL_Point p;

int main(int argc, char* args[])
{
	//----------------------------------------------------------------------------------------------------------------Init
	time_t t;
	srand((unsigned)time(&t));
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	WIN_W = DM.w;
	WIN_H = DM.h;
	SDL_Window* win = SDL_CreateWindow("3D_Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H,0/* SDL_WINDOW_FULLSCREEN */);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1,SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED);

	SDL_ShowCursor(SDL_DISABLE);

	//----------------------------------------------------------------------------------------------------------------Vars

	double  pl[3], alp = 1.9, B = 2.7, view[3];
	pl [0] = 7.7;
	pl [1] = -3.8;
	pl [2] = 3.8;

	FILE* file = fopen("cords.txt","r");

	int polysLen;
	fscanf(file,"%d \n",&polysLen);

	double*** polys;
	polys = (double***)malloc(sizeof(double**) * polysLen);
	for (int i = 0; i < polysLen; i++) polys[i] = (double**)malloc(sizeof(double*) * 4);
	for (int i = 0; i < polysLen; i++) for (int j = 0; j < 4; j++) polys[i][j] = (double*)malloc(sizeof(double) * 3);

	int*** polys2D;
	polys2D = (int***)malloc(sizeof(int**) * polysLen);
	for (int i = 0; i < polysLen; i++) polys2D[i] = (int**)malloc(sizeof(int*) * 4);
	for (int i = 0; i < polysLen; i++) for (int j = 0; j < 4; j++) polys2D[i][j] = (int*)malloc(sizeof(int) * 3);

	int* order = (int*)malloc(sizeof(int) * polysLen);

	for (int i = 0; i < polysLen; i++)
	{
		for (int j = 0; j < 4; j++) for (int y = 0; y < 3; y++) fscanf(file,"%lf ",&polys[i][j][y]);	
		fscanf(file,"\n");
	}
	fclose(file);

	//----------------------------------------------------------------------------------------------------------------GAme LoOp
	int wK = 0, aK = 0, sK = 0, dK = 0, spaceK = 0, LshiftK = 0;
	double t1 = 0, t2 = 0;
	int clsd = 0;
	int framecount = 0;
	double timecount = 0;
	SDL_WarpMouseInWindow(win, WIN_W / 2, WIN_H / 2);
	while (!clsd)
	{
		if (t2 - t1 > 1000.0 / FPS)
		{
			framecount++;
			timecount += t2 - t1;
			t1 = (double)clock() / (CLOCKS_PER_SEC / 1000);

			if ((framecount % 100) == 0)	printf("FPS: %.1f\n", framecount / (timecount / 1000));

			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{

				case(SDL_QUIT):

					clsd = 1;
					break;

				case(SDL_KEYDOWN):

					switch (event.key.keysym.sym)
					{
					case(SDLK_ESCAPE):
						clsd = 1;
						break;

					case(SDLK_SPACE):
						spaceK = 1;
						break;

					case(SDLK_LSHIFT):
						LshiftK = 1;
						break;

					case(SDLK_w):
						wK = 1;
						break;

					case(SDLK_a):
						aK = 1;
						break;

					case(SDLK_s):
						sK = 1;
						break;

					case(SDLK_d):
						dK = 1;
						break;

					default:
						break;
					}
					break;

				case(SDL_KEYUP):
					switch (event.key.keysym.sym)
					{

					case(SDLK_SPACE):
						spaceK = 0;
						break;

					case(SDLK_LSHIFT):
						LshiftK = 0;
						break;

					case(SDLK_a):
						aK = 0;
						break;

					case(SDLK_s):
						sK = 0;
						break;

					case(SDLK_d):
						dK = 0;
						break;

					case(SDLK_w):
						wK = 0;
						break;

					default:
						break;
					}
					break;

				default:
					break;
				}
			}


		//----------------------------------------------------------------------------------------------------------------Tick


			//---------------------------------------------------calculate view Vector
			int mx, my;
			SDL_GetMouseState(&mx, &my);

			int dmx, dmy;
			if (mx != WIN_W / 2 || my != WIN_H / 2)
			{
				dmx = -(mx - WIN_W / 2);
				dmy = -(my - WIN_H / 2);
				SDL_WarpMouseInWindow(win, WIN_W / 2, WIN_H / 2);
			}
			else
			{
				dmx = 0;
				dmy = 0;
			}

			B -= dmx * (double)mouSens / 10000.0;
			alp -= dmy * (double)mouSens / 10000.0;

			if (B >= 2 * M_PI) B -= 2 * M_PI;
			if (B <= 0) B += 2 * M_PI;

			if (alp > M_PI - 0.01) alp = M_PI - 0.01;
			if (alp < 0.01) alp = 0.01;

			view[0] = cos(B) * sin(alp);
			view[1] = sin(B) * sin(alp);
			view[2] = cos(alp);

			//---------------------------------------------------move pl

			if (wK)
			{
				pl[0] += cos(B) * speed;
				pl[1] += sin(B) * speed;
			}
			else if (sK)
			{
				pl[0] -= cos(B) * speed;
				pl[1] -= sin(B) * speed;
			}
			if (dK)
			{
				pl[0] += cos(B + M_PI / 2) * speed;
				pl[1] += sin(B + M_PI / 2) * speed;
			}
			else if (aK)
			{
				pl[0] -= cos(B + M_PI / 2) * speed;
				pl[1] -= sin(B + M_PI / 2) * speed;
			}
			if (spaceK)
			{
				pl[2] += speed;
			}
			else if (LshiftK)
			{
				pl[2] -= speed;
			}
			//---------------------------------------------------calculate 3D to 2D Image

			double p1[3];
			double p2[3];

			p1[0] = -view[0]*view[2];
			p1[1] = -view[1]*view[2];
			p1[2] = pow(view[1],2)+pow(view[0],2);

			p2[0] = -view[1];
			p2[1] = view[0];
			p2[2] = 0;
			
			double abso;
			abso = sqrt(pow(p1[0],2)+pow(p1[1],2)+pow(p1[2],2));
			for(int i = 0; i < 3; i++) p1[i] = p1[i]/abso;

			abso = sqrt(pow(p2[0],2)+pow(p2[1],2)+pow(p2[2],2));
			for(int i = 0; i < 3; i++) p2[i] = p2[i]/abso;		
			
			if ((framecount % 100) == 0)	printf("%.1f | %.1f \n",alp,B);

			for (int i = 0; i < polysLen; i++)
			{
				int outc = 0;
				for (int j = 0; j < 4; j++) {
					double c = polys[i][j][0] - pl[0];
					double f = polys[i][j][1] - pl[1];
					double o = polys[i][j][2] - pl[2];
					double u = polys[i][j][0] - pl[0] - view[0];
					double k = polys[i][j][1] - pl[1] - view[1];
					double l = polys[i][j][2] - pl[2] - view[2];

					double temp1 = p1[0] / p1[2]; // a/g
					double temp2 = p1[0] / p1[1]; // a/d
					double temp3 = (p2[1] * temp2 - p2[0]) / (p2[2] * temp1 - p2[0]);

					double zans = ((l * temp1 - u) * temp3 - (k * temp2 - u)) / ((o * temp1 - c) * temp3 - (f * temp2 - c)); // for lengt of point -> player vector
					if(zans  > 1 || zans < 0) { outc = 4; break; }
					double yans = ((k * temp2 - u - zans * (f * temp2 - c))) / (p2[1] * temp2 - p2[0]); // for second plane vector length
					double xans = (u - yans * p2[0] - zans * c) / p1[0]; // for first plane vector length

					polys2D[i][j][0] = round(POV * yans * WIN_W / 2 + WIN_W / 2); // make screen coordinates
					polys2D[i][j][1] = round(POV * -xans * WIN_W / 2 + WIN_H / 2);
					polys2D[i][j][2] = sqrt(pow(pl[0] - polys[i][j][0], 2) + pow(pl[1] - polys[i][j][1], 2) + pow(pl[2] - polys[i][j][2], 2)); //polys[*][*][2] = dist(point -> player)

					if (polys2D[i][j][0] > WIN_W || polys2D[i][j][0] < 0 || polys2D[i][j][1] > WIN_H || polys2D[i][j][1] < 0) outc++;
				}
				if (outc == 4) polys2D[i][0][2] = -1;
			}
			

			//for (ii = 0; ii < polysLen; ii++) // print polys2D
			//{
			//	printf("\n");
			//	for (jj = 0; jj < 4; jj++)
			//		if (polys2D[ii][jj][2] != -1)
			//		{
			//			printf("| %f , %f -- %f |", polys2D[ii][jj][0], polys2D[ii][jj][1], polys2D[ii][jj][2]);
			//		}
			//		else printf("| dc , dc -- %f |",polys2D[ii][jj][2]);
			//}


			/*printf("\n\n\n");
			for (ii = 0; ii < polysLen; ii++) order[ii] = -1;

			order[polysLen / 2] = 0;

			for (ii = 1, ind = polysLen / 2; ii < polysLen; ii++)
			{
				if (polys2D[ii][0][2] != -1)
				{
					while (order[ind] != -1)
					{
						printf("\n\n%d\n", ind);
						for (uu = 0; uu < 4; uu++)
						{
							for (uuc = 0; uuc < 4; uuc++) {
								if (polys2D[order[ind]][uu][2] > polys2D[ii][uuc][2])
									fr = 1;
								else if (polys2D[
									order[ind]][uu][2] < polys2D[ii][uuc][2])
									ba = 1;
							}
						}
					}
				}
			}*/

			//----------------------------------------------------------------------------------------------------------------Render
			 SDL_SetRenderDrawColor(rend, 46, 23, 27, 255);
			 SDL_RenderClear(rend);

			for (int i = 0; i < polysLen; i++)
			{
				if(polys2D[i][0][2] != -1)
				{
					SDL_SetRenderDrawColor(rend, rand() % 256, rand() % 256, rand() % 256, 0);
					drawPoly(polys2D[i], rend);
				}
			}
			SDL_RenderPresent(rend);
		}
		t2 = (double)clock() / ((int)CLOCKS_PER_SEC / 1000);
	}

	//----------------------------------------------------------------------------------------------------------------End & Close & Free

	printf("hello\n");
	for (int i = 0; i < polysLen; i++) for (int j = 0; j < 4; j++) free(polys[i][j]);
	for (int i = 0; i < polysLen; i++) free(polys[i]);
	free(polys);
	for (int i = 0; i < polysLen; i++) for (int j = 0; j < 4; j++) free(polys2D[i][j]);
	for (int i = 0; i < polysLen; i++) free(polys2D[i]);
	free(polys2D);
	free(order);

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

int PointInPoly(double** poly, double* point)
{
	int current, next, oc, on, out = 0;
	double xc, yc;
	for (current = 0; current < 4 && !out; current++)
	{
		if (current == 0) next = 1;
		else next = 0;


		do {
			oc = current;
			on = next;
			for (int i = 0; i < 4; i++)
			{
				xc = poly[next][0] - poly[current][0];
				yc = poly[next][1] - poly[current][1];
				if (i != current && i != next)
				{
					if (xc > 0)
					{
						if (yc / xc * (poly[i][0] - poly[current][0]) < poly[i][1] - poly[current][1])
						{
							next = i;
						}
					}
					else
					{
						if (yc / xc * (poly[i][0] - poly[current][0]) > poly[i][1] - poly[current][1])
						{
							next = i;
						}
					}
				}
			}
		} while (next != on);

		if (xc > 0)
		{
			if (yc / xc * (point[0] - poly[current][0]) < point[1] - poly[current][1]) out = 1;
			else out = 0;
		}
		else
		{
			if (yc / xc * (point[0] - poly[current][0]) > point[1] - poly[current][1]) out = 1;
			else out = 0;
		}
	}

	return !out;
}

int beh; // is one if point of pol2 is further away than a point of pol1 and later just a temporary variable
int drawFirst(double** pol1_2D, double** pol2_2D, double** pol1_3D, double** pol2_3D)
{

	for (int i = 0; i < 4; i++)
	{
		if (PointInPoly(pol1_2D, pol2_2D[i])) { beh = 1; break; }
		else beh = 0;
	}
	if (beh == 0) {
		for (int i = 0; i < 4; i++)
		{
			if (PointInPoly(pol2_2D, pol1_2D[i])) { beh = 2; break; }
			else beh = 0;
		}
	}
	if (beh == 0) return 0;

	beh = pol1_2D[0][2] < pol2_2D[0][2]; // this line belongs to the loop and checks if every point of one shape is closer or further than all the other points
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (pol1_2D[i][2] < pol2_2D[j][2] != beh) beh = 2;
		}
	}
	return beh;//---------------------
}

void drawPoly(int** points, SDL_Renderer* rend)
{
	int y = points[0][1], yl = points[0][1];
	for (int i = 1; i < 4; i++)
	{
		if (points[i][1] < y) y = points[i][1];
		if (points[i][1] > yl) yl = points[i][1];
	}
	if (y < 0) y = 0;
	if (yl > WIN_H) yl = WIN_H;


	int vektors[4][2] = { 1 };
	for (int current = 0; current < 4; current++)
	{
		int next = 0;
		for (int o = 0; o < 4; o++) if (o != current && points[current][0] != points[o][0] && points[current][1] != points[o][1]) next = o;

		for (int j = 0; j < 4; j++)
		{
			if (j != current && j != next)
			{
				int a = (points[next][1] - points[current][1]) * (points[j][0] - points[next][0]) + (points[current][0] - points[next][0]) * (points[j][1] - points[next][1]);
				if (0 < a) next = j;
			}
		}
		vektors[current][0] = points[next][0] - points[current][0];
		vektors[current][1] = points[next][1] - points[current][1];
	}
	y += RES;
	for (; y < yl; y += RES)
	{
		int x1 = 0;
		int x2 = WIN_W;

		for (int i = 0; i < 4; i++)
		{

			if (vektors[i][1] != 0)
			{
				int cross = (y - points[i][1]) * vektors[i][0] / vektors[i][1] + points[i][0];
				double b = (y - points[i][1]) / (double)vektors[i][1];

				if (0 <= b && b <= 1)
				{
					if (x1 < cross) x1 = cross;
					if (x2 > cross) x2 = cross;
				}
			}
			else if (vektors[i][1] == 0 && y == points[i][1])
			{
				x2 = points[i][0];
				x1 = points[i][0] + vektors[i][0];
				i = 4;
			}
		}


		if (x1 > WIN_W) x1 = WIN_W;
		if (x2 < 0) x2 = 0;

		SDL_Rect rect;
		rect.x = x2;
		rect.w = x1 - x2;
		rect.y = y;
		rect.h = RES;
		SDL_RenderFillRect(rend, &rect);
	}
}