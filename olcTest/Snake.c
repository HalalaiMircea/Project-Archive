#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

typedef struct {
	int x, y;
}cart;
cart *coada;
typedef struct {
	int score, Highscore;
}scoruri;

enum direction { stop = 0, st, dr, sus, jos };
enum direction direct;

FILE *out;

void start(int *sfjoc, cart *harta, cart *cap, cart *fruct, scoruri *sc)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO kek;
	kek.dwSize = 100;
	kek.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &kek);

	srand(time(NULL));

	*sfjoc = 1;
	cap->x = harta->x / 2;
	cap->y = harta->y / 2;
	fruct->x = rand() % (harta->x - 2) + 2;
	fruct->y = rand() % (harta->y - 2) + 2;
	//citeste Highscor-ul din fisier
	out = fopen("HighScore.txt", "r");
	fscanf(out, "%d", &sc->Highscore);
}
//functia de desenare
void render(cart *harta, cart *cap, cart *fruct, int *ncoada, scoruri *sc)
{
	int i, j, k;
	for (i = 1; i <= harta->x; i++)
	{
		for (j = 1; j <= harta->y; j++)
		{
			if (i == 1 || i == harta->x || j == 1 || j == harta->y)
				printf("#");
			else if (i == cap->x && j == cap->y)
				printf("O");
			else if (i == fruct->x && j == fruct->y)
				printf("F");
			else
			{
				int ok = 0;
				for (k = 1; k <= *ncoada; k++)
				{
					if (i == coada[k].x && j == coada[k].y)
					{
						printf("o"); ok = 1;
					}
				}
				if (ok == 0)
					printf(" ");
			}
		}
		printf("\n");
	}
	printf("Score: %d\nHighscore: %d", sc->score, sc->Highscore);
	//refresh ecran
	Sleep(100);
	system("cls");
}
void logica(cart *cap, cart *dir, cart *harta, cart *fruct, int *ncoada, int *sfjoc, scoruri *sc)
{
	//schimba directia si interzice mersul inapoi
	switch (direct)
	{
	case sus:
		if (cap->x - 1 != coada[1].x && cap->y != coada[1].y)
		{
			dir->x = -1; dir->y = 0;
		} break;
	case st:
		if (cap->x != coada[1].x && cap->y - 1 != coada[1].y)
		{
			dir->x = 0; dir->y = -1;
		} break;
	case jos:
		if (cap->x + 1 != coada[1].x && cap->y != coada[1].y)
		{
			dir->x = 1; dir->y = 0;
		} break;
	case dr:
		if (cap->x != coada[1].x && cap->y + 1 != coada[1].y)
		{
			dir->x = 0; dir->y = 1;
		} break;
	default: break;
	}
	//update coordonate sarpe
	int k;
	coada[0].x = cap->x; coada[0].y = cap->y;
	for (k = *ncoada; k >= 1; k--)
	{
		coada[k].x = coada[k - 1].x;
		coada[k].y = coada[k - 1].y;
	}
	cap->x = cap->x + dir->x;
	cap->y = cap->y + dir->y;
	//teleportatri la coliziuni cu pereti
	if (cap->x == harta->x)
		cap->x = 2;
	if (cap->x == 1)
		cap->x = harta->x - 1;
	if (cap->y == 1)
		cap->y = harta->y - 1;
	if (cap->y == harta->y)
		cap->y = 2;
	//marire coada, scor si coord noi pt fruct
	if (cap->x == fruct->x && cap->y == fruct->y)
	{
		(*ncoada)++;
		(sc->score)++;
		if (sc->Highscore < sc->score)
			sc->Highscore = sc->score;
		fruct->x = rand() % (harta->x - 2) + 2;
		fruct->y = rand() % (harta->y - 2) + 2;
	}
	//verific daca capul a lovit coada
	for (k = 1; k <= *ncoada; k++)
		if (cap->x == coada[k].x && cap->y == coada[k].y)
			*sfjoc = 0;
}
void input(int *sfjoc)
{
	if (_kbhit())
		switch (_getch())
		{
		case 'w':
			direct = sus;
			break;
		case 'a':
			direct = st;
			break;
		case 's':
			direct = jos;
			break;
		case 'd':
			direct = dr;
			break;
		case 'p':
			*sfjoc = 0;
			break;
		}
}
void End(scoruri *sc)
{
	out = fopen("HighScore.txt", "r");
	int x;
	fscanf(out, "%d", &x);
	if (x < sc->score)
	{
		out = fopen("HighScore.txt", "w");
		fprintf(out, "%d", sc->score);
	}
	system("cls");
	printf("Game Over!\n");
	printf("Your score is: %d\nHighscore: %d\nPress any key to restart\n", sc->score, sc->Highscore);
	Sleep(1000);
	_getch();
	system("cls");
	fclose(out);
}
int main()
{
	int ok = 1;
	while (ok)
	{
		cart harta = { 20,20 }, fruct = { 0,0 }, cap = { 0,0 }, dir = { 0,0 };
		direct = stop;
		int sfjoc = 0, ncoada = 0;
		scoruri sc = { 0,0 };
		coada = (cart*)malloc(harta.x* harta.y * sizeof(int));
		start(&sfjoc, &harta, &cap, &fruct, &sc);
		while (sfjoc)
		{
			input(&sfjoc);
			logica(&cap, &dir, &harta, &fruct, &ncoada, &sfjoc, &sc);
			render(&harta, &cap, &fruct, &ncoada, &sc);
		}
		End(&sc);
	}
	return 0;
}