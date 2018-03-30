#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <conio.h>


using namespace std;

int flaga;		//flaga poprawnosci polozenia statku
int cheat = 0;	//flaga cheatu - odkrycia mapy 
int playRound = 1; //flaga rundy
int win = 0; //flaga końca gry
int hitFlag1 = 0; //flaga do algorytmu strzelania
int hitCounter1 = 0; //licznik trafien
int hitFlag2 = 0; //flaga do algorytmu strzelania
int hitCounter2 = 0; //licznik trafien

					 //kolor
HANDLE kolor = GetStdHandle(STD_OUTPUT_HANDLE);

//klasa planszy
class Map {
	friend class Ship;
	friend class Game;
	friend class Player;
public:
	int sizeY = 8; //rozmiar Y
	int sizeX = 7;	// Rozmiar X
	bool **isShip;	//czy na danym polu jest statek
	bool **isShoted;	//czy na dane pole oddano juz strzal
	string **ship;	//oznaczenie konkretnego statnku na danym polu
	char **matrix1;			//macierz pól mapy
	char **matrix2;			//macierz pól odkrytej mapy

	void showMap() {			//rysowanie mapy
		int i, j;
		int f = 4;			//liczba przerw - do rysowania
		int liczbaZ = 10;	//liczba do wykrywania numeru wiersza
		char A = 'A';		//A
		cout << endl << endl;
		cout << "	      ";	//przerwy by przesunac mapę do srodka konsoli
		for (i = 0; i < sizeX; i++) {
			cout << "|" << char(A + i);	//wypisanie nazw kolumn
		}
		cout << endl;
		cout << "	      ";
		for (i = 0; i < sizeX; i++) {
			cout << "--";	//wypisanie podkreslen
		}
		cout << endl;

		for (i = 0; i < sizeY; i++) {
			cout << "	";

			if (i + 1 >= liczbaZ) {			//sprawwdzanie numeru wiersza - jesli tak zmniejszenie liczby spacji
				f--;
				liczbaZ = liczbaZ * 10;
			}

			for (j = 0; j < f; j++) {
				cout << " ";
			}

			cout << i + 1 << "|";

			for (j = 0; j < sizeX; j++) {		//wypisanie pola mapy
				if (cheat == 0) {
					if (matrix1[i][j] == 'O') {
						SetConsoleTextAttribute(kolor, 11);
					}
					else if (matrix1[i][j] == 'X') {
						SetConsoleTextAttribute(kolor, 12);
					}
					cout << " " << matrix1[i][j];
					SetConsoleTextAttribute(kolor, 15);
				}
				else {
					if (matrix1[i][j] == 'O') {
						SetConsoleTextAttribute(kolor, 11);
					}
					else if (matrix1[i][j] == 'X') {
						SetConsoleTextAttribute(kolor, 12);
					}
					else if (matrix2[i][j] == '2' || matrix2[i][j] == '3' || matrix2[i][j] == '4' || matrix2[i][j] == '5') {
						SetConsoleTextAttribute(kolor, 10);
					}
					cout << " " << matrix2[i][j];
					SetConsoleTextAttribute(kolor, 15);
				}
			}
			cout << endl;
		}
	}

	//odswiezanie mapy
	void refreshMap() {
		system("CLS");
		showMap();
	}

	Map(int X, int Y) {		//konstruktor mapy
		int i, j;

		this->sizeY = Y;
		this->sizeX = X;

		matrix1 = new char*[sizeY];				//dynamiczna alokacja tablicy 2d
		matrix2 = new char*[sizeY];				//dynamiczna alokacja tablicy 2d
		isShip = new bool*[sizeY];				//dynamiczna alokacja tablicy 2d
		isShoted = new bool*[sizeY];			//dynamiczna alokacja tablicy 2d
		ship = new string*[sizeY];				//dynamiczna alokacja tablicy 2d
		for (i = 0; i < sizeY; i++) {
			matrix1[i] = new char[sizeX];
			matrix2[i] = new char[sizeX];
			isShip[i] = new bool[sizeX];
			isShoted[i] = new bool[sizeX];
			ship[i] = new string[sizeX];
		}

		for (i = 0; i < sizeY; i++) {		//wypelnianie tablic
			for (j = 0; j < sizeX; j++) {

				matrix1[i][j] = '*';
				matrix2[i][j] = '*';
				isShip[i][j] = false;
				isShoted[i][j] = false;
				ship[i][j] = "n";
			}
		}
	}
};

//klasa statku
class Ship {
	friend class Game;
	friend class Player;

public:
	int startPointX;	//punkt poczatkowy po kolumnach statku
	int startPointY;	//punkt poczatkowy po kolumnach statku
	string kierunek;	//kierunek rysowania
	string name;		//nazwa statku
	string shipId;		//id statku
	char type;			//typ statku
	int shipLen;		//dlugosc statku
	int hits = 0;			//liczba trafien
	bool isSunken = 0;	//czy zatopiony

	void checkShip() {
		if (hits == shipLen) {
			isSunken = true;
		}
	}
	void initiateShip(string kier, string start, Map *mapa, string id) {
		this->kierunek = kier;
		this->startPointX = toupper(start[0]) - 65;	//okreslenie punktu startowego Y z podanego ciagu znakow
		int tmp = start.length() - 1;
		string y = start.substr(1, tmp);
		this->startPointY = atoi(y.c_str()) - 1;		//okreslenie punktu startowego Y z podanego ciagu znakow
		int i, j;
		int chck = 0;		//zmienna do sprawwdzania poprawnosci polozenia
		shipId = id;
		flaga = 0;			//dopóki flaga pozostaje == 0 polozenie jest nieprawidlowe

		if (kierunek == "poziomo") {	//zaznacznie polozenia statku

										//wykrywanie kolizji krawędzi
			if (startPointX == 0 || startPointX > (mapa->sizeX - 1 - shipLen) || startPointY == 0 || startPointY == (mapa->sizeY - 1)) {
				cout << "Bledne polozenie statku!" << endl;
			}
			else {

				for (i = 0; i < 3; i++) {
					for (j = 0; j < shipLen + 2; j++) {
						if (mapa->isShip[startPointY - 1 + i][startPointX - 1 + j]) { //sprawdzanie kolizji statkow
							chck = 1;
						}
					}
				}

				if (chck == 1) {
					cout << "Statki nachodza na swoje pola!" << endl;
				}
				else {
					for (i = 0; i < shipLen; i++) {
						mapa->isShip[startPointY][startPointX + i] = true;		//ustawianie pola mapy is Ship na true na polach , w ktorych ma byc statek
						mapa->matrix2[startPointY][startPointX + i] = type;	//ustawienie pola mapy odkrytej
						mapa->ship[startPointY][startPointX + i] = shipId;	//ustawienie id statku na mapie
					}
					flaga = 1;
				}

			}
		}
		else if (kierunek == "pionowo") {

			//wykrywanie kolizji krawędzi
			if (startPointX == 0 || startPointX == (mapa->sizeX - 1) || startPointY == 0 || startPointY > (mapa->sizeY - 1 - shipLen)) {
				cout << "Bledne polozenie statku!" << endl;
			}
			else {

				for (i = 0; i < shipLen + 2; i++) {
					for (j = 0; j < 3; j++) {
						if (mapa->isShip[startPointY - 1 + i][startPointX - 1 + j]) { //sprawdzanie kolizji statkow
							chck = 1;
						}
					}
				}

				if (chck == 1) {
					cout << "Statki nachodza na swoje pola!" << endl;
				}
				else {
					for (i = 0; i < shipLen; i++) {
						mapa->isShip[startPointY + i][startPointX] = true;	//przypisanie miejca na planszy statkowi
						mapa->matrix2[startPointY + i][startPointX] = type;
						mapa->ship[startPointY + i][startPointX] = shipId;
					}
					flaga = 1;
				}
			}
		}
		else if (kierunek == "sl") {

			int x = startPointX;
			int y = startPointY;
			int k;

			//wykrywanie kolizji krawędzi
			if (startPointX < shipLen + 1 || startPointX == (mapa->sizeX - 1) || startPointY == 0 || startPointY >(mapa->sizeY - 1 - shipLen)) {
				cout << "Bledne polozenie statku!" << endl;
			}
			else {

				for (i = 0; i < shipLen; i++) {
					for (j = 0; j < 3; j++) {
						for (k = 0; k < 3; k++) {
							if (mapa->isShip[y - 1 + j][x - 1 + k]) { //sprawdzanie kolizji statkow
								chck = 1;
							}
						}
					}
					x = x - 1;
					y = y + 1;
				}

				if (chck == 1) {
					cout << "Statki nachodza na swoje pola!" << endl;
				}
				else {
					for (i = 0; i < shipLen; i++) {
						mapa->isShip[startPointY + i][startPointX - i] = true; //przypisanie miejca na planszy statkowi
						mapa->matrix2[startPointY + i][startPointX - i] = type;
						mapa->ship[startPointY + i][startPointX - i] = shipId;

					}
					flaga = 1;
				}
			}
		}
		else if (kierunek == "sp") {
			int x = startPointX;
			int y = startPointY;
			int k;

			//wykrywanie kolizji krawędzi
			if (startPointX == 0 || startPointX > (mapa->sizeX - 1 - shipLen) || startPointY == 0 || startPointY > (mapa->sizeY - 1 - shipLen)) {
				cout << "Bledne polozenie statku!" << endl;
			}
			else {

				for (i = 0; i < shipLen; i++) {
					for (j = 0; j < 3; j++) {
						for (k = 0; k < 3; k++) {
							if (mapa->isShip[y - 1 + j][x - 1 + k]) { //sprawdzanie kolizji statkow
								chck = 1;
							}
						}
					}
					x = x + 1;
					y = y + 1;
				}


				if (chck == 1) {
					cout << "Statki nachodza na swoje pola!" << endl;
				}
				else {
					for (i = 0; i < shipLen; i++) {
						mapa->isShip[startPointY + i][startPointX + i] = true;	//przypisanie miejca na planszy statkowi
						mapa->matrix2[startPointY + i][startPointX + i] = type;
						mapa->ship[startPointY + i][startPointX + i] = shipId;

					}
					flaga = 1;
				}
			}
		}
		else {
			cout << "ERROR!" << endl;
		}
	}

	Ship(int dlugosc) {	//konstruktor
		this->shipLen = dlugosc;

		switch (shipLen) {
		case 2:
			name = "2-masztowy";
			type = '2';
			break;
		case 3:
			name = "3-masztowy";
			type = '3';
			break;
		case 4:
			name = "4-masztowy";
			type = '4';
			break;
		case 5:
			name = "5-masztowy";
			type = '5';
			break;
		}
	}
};
//klasa gracza
class Player {
public:
	string type;
	int statki2;
	int statki3;
	int statki4;
	int statki5;

	//zmniejszenie liczby statkow
	void takeOff(int option) {
		switch (option) {
		case 2:
			statki2 -= 1;
			break;
		case 3:
			statki3 -= 1;
			break;
		case 4:
			statki4 -= 1;
			break;
		case 5:
			statki5 -= 1;
			break;
		}
	}

	Player(int s2, int s3, int s4, int s5, string type) {
		this->statki2 = s2;
		this->statki3 = s3;
		this->statki4 = s4;
		this->statki5 = s5;
		this->type = type;

	}
};
//klasa gry
class Game {
	friend class Player;
public:
	//wektory dla wielu obkiektow statkow
	vector<Ship*> s2;		//2-masztowe
	vector<Ship*> s3;		//3-masztowe
	vector<Ship*> s4;		//4-masztowe
	vector<Ship*> s5;		//5-masztowe

	int opt = 0;
	int menuOpt = 0;
	int sizeOpt = 0;
	int numOpt = 0;
	int i;
	int l2 = 0;
	int l3 = 0;
	int l4 = 0;
	int l5 = 0;
	int x = 0;					//wymiar X mapy
	int y = 0;					//wymiar Y mapy
	int strzalY1;		//skladowa y strzalu komputera 1
	int strzalX1;		//skladowa x strzalu komputera 1
	int strzalY2;		//skladowa y strzalu komputera 2
	int strzalX2;		//skladowa x strzalu komputera 2
	int tmpY1;		//skladowa tymczasowa y strzalu komputera 1
	int tmpX1;		//skladowa tymczasowa x strzalu komputera 1
	int tmpY2;		//skladowa tymczasowa y strzalu komputera 2
	int tmpX2;		//skladowa tymczasowa x strzalu komputera 2

	int pola;

	string direction;
	string place;

	Player *gracz1;
	Player *gracz2;
	Map *plansza1;
	Map *plansza2;

	//losowanie X do strzalu
	int losX() {
		int losx = (rand() % (x - 2)) + 66;
		return losx;
	}

	//losowanie Y do strzalu
	int losY() {
		int losy = (rand() % (y - 2)) + 2;
		return losy;
	}
	//funkcja losowego strzelania
	string makeShot(int losX, int losY) {
		char strX;
		string strY;
		string shotPoint;

		strX = losX;
		strY = to_string(losY);
		shotPoint = strX + strY;
		return shotPoint;
	}
	//funkcja zmieniajaca flage cheatu
	void switchCheat() {
		if (cheat == 0) {
			cheat = 1;
		}
		else {
			cheat = 0;
		}
	}
	//inicjowanie map
	void initiateMaps() {
		switch (sizeOpt)
		{
		case 1:

			while ((x < 10 || y < 10) || (x > 26)) {
				cout << "Podaj szerokosci (X) map: " << endl;
				cin >> x;

				while (cin.fail()) {
					cin.clear();
					cin.ignore();
					cout << "Wpisz poprawna opcje!" << endl;
					cin >> x;
				}

				cout << "Podaj wysokosci (Y) map: " << endl;
				cin >> y;

				while (cin.fail()) {
					cin.clear();
					cin.ignore();
					cout << "Wpisz poprawna opcje!" << endl;
					cin >> y;
				}

				if ((x < 10 || y < 10) || (x > 26)) {
					cout << "Bledne wymiary mapy!" << endl;
				}
			}
			break;

		case 2:
			int losX;
			int losY;

			while ((x < 10 || y < 10) || (x > 26)) {
				losX = (rand() % 17) + 10;
				losY = (rand() % 91) + 10;
				x = losX;
				y = losY;

				if ((x < 10 || y < 10) || (x > 26)) {
					cout << "Bledne wymiary mapy!" << endl;
				}
			}
			break;
		}
		//tworzenie plansz dla 2 graczy
		plansza1 = new Map(x, y);
		plansza2 = new Map(x, y);
	}
	//ustawienie liczby statkow
	void takeNumShips() {
		int all = 0;		//wielkosc pola zajmowana przez wszystkie statki
		int dziel = 1;		// do zmniejszania liczby statkow
		do {
			switch (numOpt) {
			case 1:

				cout << "Podaj liczbe 2-masztowcow na mape: " << endl;
				cin >> l2;
				for (i = 0; i < 2 * l2; i++) {						//inicjowanie statkow
					s2.push_back(new Ship(2));
				}
				//s2[0]->initiateShip("poziomo", "C5", plansza1);
				//cout << endl << plansza1->isShip[4][2];
				system("CLS");

				cout << "Podaj liczbe 3-masztowcow na mape: " << endl;
				cin >> l3;
				for (i = 0; i < 2 * l3; i++) {
					//inicjowanie statkow
					s3.push_back(new Ship(3));
				}
				system("CLS");

				cout << "Podaj liczbe 4-masztowcow na mape: " << endl;
				cin >> l4;
				for (i = 0; i < 2 * l4; i++) {						//inicjowanie statkow
					s4.push_back(new Ship(4));
				}
				system("CLS");

				cout << "Podaj liczbe 5-masztowcow na mape: " << endl;
				cin >> l5;
				for (i = 0; i < 2 * l5; i++) {						//inicjowanie statkow
					s5.push_back(new Ship(5));
				}
				break;


			case 2:
				l2 = (rand() % 9) + 1;
				l2 = l2 / dziel;
				if (l2 == 0) l2 = 1;
				for (i = 0; i < 2 * l2; i++) {						//inicjowanie statkow
					s2.push_back(new Ship(2));
				}
				//s2[0]->initiateShip("poziomo", "C5", plansza1);
				//cout << endl << plansza1->isShip[4][2];
				system("CLS");

				l3 = (rand() % 9) + 1;
				l3 = l3 / dziel;
				if (l3 == 0) l3 = 1;
				for (i = 0; i < 2 * l3; i++) {
					//inicjowanie statkow
					s3.push_back(new Ship(3));
				}
				system("CLS");

				l4 = (rand() % 9) + 1;
				l4 = l4 / dziel;
				for (i = 0; i < 2 * l4; i++) {						//inicjowanie statkow
					s4.push_back(new Ship(4));
				}
				system("CLS");

				l5 = (rand() % 9) + 1;
				l5 = l5 / dziel;
				for (i = 0; i < 2 * l5; i++) {						//inicjowanie statkow
					s5.push_back(new Ship(5));
				}
				break;
			}
			all = 2 * l2 + 3 * l3 + 4 * l4 + 5 * l5;
			//cout << l2 << " " << l3 << " " << l4 << " " << l5 << endl;
			//system("pause");
			if (all >((x - 1) * (y - 1) * 0.19)) {
				cout << "Liczba statkow jest za duza na ta mape! Wpisz jeszcze raz." << endl;
				dziel++;
			}
		} while (all >((x - 1) * (y - 1) * 0.19));
	}
	//ustawienie statkow
	void placeShips(Map *mapa, int gracz) {
		int move = 0; //przesuniecie i dla drugiego gracza

		switch (opt) {
		case 1:					//ustawianie statkow przez gracza

			if (gracz == 2) {
				move = l2;
			}

			for (i = 0; i < l2; i++) {
				flaga = 0;

				while (flaga != 1) {

					cout << "Wpisz punkt startowy " << i << "-ego 2-masztowca: " << endl;
					cin >> place;
					cout << "Wpisz kierunek:" << endl;
					cin >> direction;
					s2[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s2");
				}
			}

			if (gracz == 2) {
				move = l3;
			}

			for (i = 0; i < l3; i++) {
				flaga = 0;
				while (flaga != 1) {
					cout << "Wpisz punkt startowy " << i << "-ego 3-masztowca: " << endl;
					cin >> place;
					cout << "Wpisz kierunek:" << endl;
					cin >> direction;
					s3[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s3");
				}
			}

			if (gracz == 2) {
				move = l4;
			}

			for (i = 0; i < l4; i++) {
				flaga = 0;
				while (flaga != 1) {
					cout << "Wpisz punkt startowy " << i << "-ego 4-masztowca: " << endl;
					cin >> place;
					cout << "Wpisz kierunek:" << endl;
					cin >> direction;
					s4[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s4");
				}
			}

			if (gracz == 2) {
				move = l5;
			}

			for (i = 0; i < l5; i++) {
				flaga = 0;
				while (flaga != 1) {
					cout << "Wpisz punkt startowy " << i << "-ego 5-masztowca: " << endl;
					cin >> place;
					cout << "Wpisz kierunek:" << endl;
					cin >> direction;
					s5[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s5");
				}
			}
			system("CLS");
			break;

		case 2:							//losowe ustawienie statkow
			int losX;
			int losY;
			int losDirc;
			char strX;
			string strY;

			if (gracz == 2) {
				move = l2;
			}
			for (i = 0; i < l2; i++) {
				flaga = 0;
				while (flaga != 1) {
					losX = (rand() % (x - 1)) + 66;
					losY = (rand() % (y - 1)) + 2;
					losDirc = (rand()) % 4;
					strX = losX;
					strY = to_string(losY);
					place = strX + strY;

					switch (losDirc) {
					case 0:
						direction = "poziomo";
						break;
					case 1:
						direction = "pionowo";
						break;
					case 2:
						direction = "sl";
						break;
					case 3:
						direction = "sp";
						break;
					}
					s2[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s2");
					system("CLS");
					//cout << direction << " " << place <<" "<<flaga<< endl;
				}
			}
			if (gracz == 2) {
				move = l3;
			}
			for (i = 0; i < l3; i++) {
				flaga = 0;
				while (flaga != 1) {
					losX = (rand() % (x - 1)) + 66;
					losY = (rand() % (y - 1)) + 2;
					losDirc = (rand()) % 4;
					strX = losX;
					strY = to_string(losY);
					place = strX + strY;

					switch (losDirc) {
					case 0:
						direction = "poziomo";
						break;
					case 1:
						direction = "pionowo";
						break;
					case 2:
						direction = "sl";
						break;
					case 3:
						direction = "sp";
						break;
					}

					s3[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s3");
					system("CLS");
					//cout << direction << " " << place << " " << flaga << endl;
				}
			}

			if (gracz == 2) {
				move = l4;
			}

			for (i = 0; i < l4; i++) {
				flaga = 0;
				while (flaga != 1) {
					losX = (rand() % (x - 1)) + 66;
					losY = (rand() % (y - 1)) + 2;
					losDirc = (rand()) % 4;
					strX = losX;
					strY = to_string(losY);
					place = strX + strY;

					switch (losDirc) {
					case 0:
						direction = "poziomo";
						break;
					case 1:
						direction = "pionowo";
						break;
					case 2:
						direction = "sl";
						break;
					case 3:
						direction = "sp";
						break;
					}

					s4[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s4");
					system("CLS");
					//cout << direction << " " << place << " " << flaga << endl;
				}
			}

			if (gracz == 2) {
				move = l5;
			}

			for (i = 0; i < l5; i++) {
				flaga = 0;
				while (flaga != 1) {
					losX = (rand() % (x - 1)) + 66;
					losY = (rand() % (y - 1)) + 2;
					losDirc = (rand()) % 4;
					strX = losX;
					strY = to_string(losY);
					place = strX + strY;

					switch (losDirc) {
					case 0:
						direction = "poziomo";
						break;
					case 1:
						direction = "pionowo";
						break;
					case 2:
						direction = "sl";
						break;
					case 3:
						direction = "sp";
						break;
					}

					s5[i + move]->initiateShip(direction, place, mapa, to_string(i + move) + "s5");
					system("CLS");
					//cout << direction << " " << place << " " << flaga << endl;
				}
			}
			break;
		}
	}
	//funkcja zmiany rundy
	void switchRound() {
		if (playRound == 1) {
			playRound = 2;
		}
		else {
			playRound = 1;
		}
	}
	//pobierz strzal
	bool takeShotPoint(string bang) {
		bool isGood = false;
		int tmp = bang.length() - 1;
		string strY = bang.substr(1, tmp);
		string strX = bang.substr(0, 1);
		int bangY = atoi(strY.c_str()) - 1;
		int bangX = toupper(strX[0]) - 65;

		if (bangX >= 0 && bangX < x && bangY >= 0 && bangY < y) {
			isGood = true;
		}
		return isGood;
	}
	//funkcja strzelania 
	int shot(string shot, Map *mapa, Player *gracz) {
		int tmp = shot.length() - 1;
		string shotY = shot.substr(1, tmp);
		int shotPointX = toupper(shot[0]) - 65;
		int shotPointY = atoi(shotY.c_str()) - 1;
		int trafiony = 0;
		//cout << shotPointY << " " << shotPointX << endl;

		//Gdy trafi
		if (mapa->isShip[shotPointY][shotPointX] && mapa->isShoted[shotPointY][shotPointX] == false) {
			mapa->matrix1[shotPointY][shotPointX] = 'X';
			mapa->matrix2[shotPointY][shotPointX] = 'X';
			string ship = mapa->ship[shotPointY][shotPointX];
			int tmp = ship.length() - 2;
			string idS = ship.substr(0, tmp);
			string type = ship.substr(tmp + 1, 1);
			int id = atoi(idS.c_str());
			int typeInt = atoi(type.c_str());
			mapa->isShoted[shotPointY][shotPointX] = true;
			trafiony = 1;

			//dodanie obrażen konkretnemu statkowi
			switch (typeInt) {
			case 2:
				//cout << " s2s2" << s2[id]->hits << endl;
				s2[id]->hits += 1;				//zwiekszenie obrazen
				s2[id]->checkShip();			//sprawdzenie czy zatonal
				if (s2[id]->isSunken) {			//jesli zatonal to zmniejsz liczbe statkow danego gracza
					gracz->takeOff(2);
					trafiony = 3;
				}
				//cout << " s2s2" << s2[id]->hits << endl;

				break;
			case 3:
				s3[id]->hits += 1;
				s3[id]->checkShip();
				if (s3[id]->isSunken) {
					gracz->takeOff(3);
					trafiony = 3;
				}
				break;
			case 4:
				s4[id]->hits += 1;
				s4[id]->checkShip();
				if (s4[id]->isSunken) {
					gracz->takeOff(4);
					trafiony = 3;
				}
				break;
			case 5:
				s5[id]->hits += 1;
				s5[id]->checkShip();
				if (s5[id]->isSunken) {
					gracz->takeOff(5);
					trafiony = 3;
				}
				break;
			}
		}
		else if (mapa->isShoted[shotPointY][shotPointX]) {
			trafiony = 2;
		}
		else {
			//gdy nie trafi
			mapa->matrix1[shotPointY][shotPointX] = 'O';
			mapa->matrix2[shotPointY][shotPointX] = 'O';
			mapa->isShoted[shotPointY][shotPointX] = true;
			switchRound();
		}
		return trafiony;
	}
	//wyswietl instrukcje
	void manual() {		//wyswietlenie ekranu startowego
		cout << "===========================================GRA W STATKI=====================================================" << endl;
		cout << "-Podczas swojej rundy wpisz pole przeciwnika, w ktore chcesz strzelic:" << endl;
		cout << "Np. C12" << endl << endl;
		cout << "-Oznaczenia planszy: " << endl;
		cout << "* - pole zakryte; " << endl;
		cout << "O - pudlo; " << endl;
		cout << "X - trafiony; " << endl << endl;
		cout << "-Uklad statkow: " << endl;
		cout << "poziomo - od punktu startowego w prawo; " << endl;
		cout << "pionowo - od punktu startowego w dol; " << endl;
		cout << "sl - \"skosnie w lewo\" od punktu startowego w kierunku lewego-dolnego rogu; " << endl;
		cout << "sp - \"skosnie w prawo\" od punktu startowego w kierunku prawego-dolnego rogu; " << endl << endl;
		cout << "-Wpisanie '[' i zatwierdzenie ENTERem w turze gracza spowoduje wlaczenie / wylaczenie opcji 'cheat'; " << endl;
		cout << "-Wcisniecie klawisza '[' w turze komputera w czasie oczekiwania spowoduje wlaczenie / wylaczenie opcji 'cheat'; " << endl;
		cout << "-Losowanie ustawienia statkow moze czasami chwile potrwac;" << endl;
		cout << "============================================================================================================" << endl;

		system("pause");
		system("CLS");


	}
	//wybierz tryb gry
	void choseMenu() {
		while (menuOpt != 1 && menuOpt != 2 && menuOpt != 3) {
			system("CLS");
			cout << "====================================================" << endl;
			cout << "			 MENU						" << endl;
			cout << "====================================================" << endl;
			cout << "Wyierz Tryb:" << endl;
			cout << "1) PvP:" << endl;
			cout << "2) PvPC:" << endl;
			cout << "3) PCvPC:" << endl;

			cin >> menuOpt;

			while (cin.fail()) {
				cin.clear();
				cin.ignore();
				cout << "Wpisz poprawna opcje!" << endl;
				cin >> menuOpt;
			}


			if (menuOpt != 1 && menuOpt != 2 && menuOpt != 3) {
				cout << "Blad wpisywania opcji!" << endl;
			}
		}
	}
	//wybierz opcje ustawienia wielkosci mapy
	void choseSize() {
		while (sizeOpt != 1 && sizeOpt != 2) {
			system("CLS");
			cout << "Wybierz opcje: " << endl;
			cout << "1) Sam wpisz rozmiar mapy:" << endl;
			cout << "2) Losowy rozmiar mapy:" << endl;
			cin >> sizeOpt;

			while (cin.fail()) {
				cin.clear();
				cin.ignore();
				cout << "Wpisz poprawna opcje!" << endl;
				cin >> sizeOpt;
			}

			if (sizeOpt != 1 && sizeOpt != 2) {
				cout << "Blad wpisywania opcji!" << endl;
			}
		}
	}
	//wybor opcji ustawienia liczby statkow
	void choseNumber() {
		while (numOpt != 1 && numOpt != 2) {
			system("CLS");
			cout << "Wybierz opcje: " << endl;
			cout << "1) Sam wpisz liczbe statkow:" << endl;
			cout << "2) Losowa liczba statkow:" << endl;
			cin >> numOpt;

			while (cin.fail()) {
				cin.clear();
				cin.ignore();
				cout << "Wpisz poprawna opcje!" << endl;
				cin >> numOpt;
			}

			if (numOpt != 1 && numOpt != 2) {
				cout << "Blad wpisywania opcji!" << endl;
			}
		}
	}
	//wybor opcji ustawiania statkow
	void choseOpt() {
		while (opt != 1 && opt != 2) {
			cout << "Wybierz opcje: " << endl;
			cout << "1) Sam ustaw statki:" << endl;
			cout << "2) Losowa losowe ustawienie statkow:" << endl;

			cin >> opt;

			while (cin.fail()) {
				cin.clear();
				cin.ignore();
				cout << "Wpisz poprawna opcje!" << endl;
				cin >> opt;
			}

			if (opt != 1 && opt != 2) {
				cout << "Blad wpisywania opcji!" << endl;
			}
		}
	}
	//funkcja wyboru trybu gry
	void play() {
		switch (menuOpt) {
		case 1:
			gracz1 = new Player(l2, l3, l4, l5, "Gracz1");
			gracz2 = new Player(l2, l3, l4, l5, "Gracz2");

			playPvP();

			break;
		case 2:
			gracz1 = new Player(l2, l3, l4, l5, "Gracz1");
			gracz2 = new Player(l2, l3, l4, l5, "CPU1");

			playPvPC();

			break;
		case 3:
			gracz1 = new Player(l2, l3, l4, l5, "CPU1");
			gracz2 = new Player(l2, l3, l4, l5, "CPU2");

			playPCvPC();

			break;
		}
	}
	//sprawdzenie ilosci statkow
	void checkPlayer(Player *gracz) {
		if (gracz->statki2 == 0 && gracz->statki3 == 0 && gracz->statki4 == 0 && gracz->statki5 == 0) {
			win = 1;
		}
	}
	//wyswietlenie liczby statkow
	void showShipAmount(Player *gracz) {
		cout << "=========================================================" << endl;
		cout << "| 2-masztowe | 3-masztowe | 4-masztowe | 5-masztowe |" << endl;
		cout << "|     " << gracz->statki2 << "      |     " << gracz->statki3 << "      |     " << gracz->statki4 << "      |     " << gracz->statki5 << "      |" << endl;
		cout << "=========================================================" << endl;

	}
	//algorytm strzelania
	string shotAlgorithm(int &trafiony, int &licznik, int &strzalX, int &strzalY) {
		string shotA;

		if (trafiony > 16) {
			trafiony = 1;
		}

		switch (trafiony) {
		case 1:
			//	cout << "alg " << trafiony << endl;
			strzalY += 1;
			if (strzalY > y) {
				trafiony += 1;
			}
			break;
		case 2:
			//	cout << "alg " << trafiony << endl;

			strzalX -= 1;
			strzalY += 1;
			if ((strzalX - 66) < 0 || strzalY > y) {
				trafiony += 1;
			}
			break;
		case 3:
			//cout << "alg " << trafiony << endl;

			strzalX -= 1;
			if ((strzalX - 66) < 0) {
				trafiony += 1;
			}
			break;
		case 4:
			//cout << "alg " << trafiony << endl;

			strzalX -= 1;
			strzalY -= 1;
			if ((strzalX - 66) < 0 || strzalY < 0) {
				trafiony += 1;
			}
			break;
		case 5:
			//cout << "alg " << trafiony << endl;

			strzalY -= 1;
			if (strzalY < 0) {
				trafiony += 1;
			}
			break;
		case 6:
			//cout << "alg " << trafiony << endl;

			strzalX += 1;
			strzalY -= 1;
			//cout << strzalX << " "<<strzalY << " x:" << x << " y:" << y << endl;
			if ((strzalX - 66) > x || strzalY < 0) {
				//trafiony += 1;
			}
			break;
		case 7:
			//cout << "alg " << trafiony << endl;

			strzalX += 1;
			cout << strzalX << " " << strzalY << " x:" << x << " y:" << y << endl;
			if ((strzalX - 66) > x) {
				trafiony += 1;
			}
			break;
		case 8:
			//cout << "alg " << trafiony << endl;

			strzalX += 1;
			strzalY += 1;
			//cout << strzalX << " "<<strzalY << " x:" << x << " y:" << y << endl;
			if ((strzalX - 66)> x || strzalY > y) {
				trafiony += 1;
			}
			break;
		case 9:
			//cout << "alg " << trafiony << endl;

			strzalY -= (licznik + 1);
			trafiony = 5;

			break;
		case 10:
			//cout << "alg " << trafiony << endl;

			strzalX += (licznik + 1);
			strzalY -= (licznik + 1);
			trafiony = 6;
			break;
		case 11:
			//cout << "alg " << trafiony << endl;

			strzalX += (licznik + 1);
			trafiony = 7;
			break;
		case 12:
			//cout << "alg " << trafiony << endl;

			strzalX += (licznik + 1);
			strzalY += (licznik + 1);
			trafiony = 8;
			break;
		case 13:
			//cout << "alg " << trafiony << endl;

			strzalY += (licznik + 1);
			trafiony = 1;
			break;
		case 14:
			//cout << "alg " << trafiony << endl;

			strzalX -= (licznik + 1);
			strzalY += (licznik + 1);
			trafiony = 2;
			break;
		case 15:
			//cout << "alg " << trafiony << endl;

			strzalX -= (licznik + 1);
			trafiony = 3;
			break;
		case 16:
			//cout << "alg " << trafiony << endl;

			strzalX -= (licznik + 1);
			strzalY -= (licznik + 1);
			trafiony = 4;
			break;
		}

		shotA = makeShot(strzalX, strzalY);	//zamiana na punkt do strzalu
		return shotA;
	}
	/////////////////////////////////////////////////
	//funkcja rozgrywki PVP
	void playPvP() {
		string strzal;
		bool goOn = 0;
		int trafiony = 0;
		while (win == 0) {
			//////////////////////////////////runda 1 gracza///////////////////////////////////
			while (playRound == 1) {

				while (goOn == false) {
					system("CLS");
					cout << "GRACZ 1" << endl;
					showShipAmount(gracz2);
					plansza2->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
					}

					trafiony = 0;

					cout << "Wpisz pole strzalu: " << endl;
					//	
					cin >> strzal;

					if (strzal == "[") {
						switchCheat();
						continue;
					}

					goOn = takeShotPoint(strzal);	//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;
				trafiony = shot(strzal, plansza2, gracz2);

				checkPlayer(gracz2);

				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA GRACZ 1!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "GRACZ 1" << endl;
			showShipAmount(gracz2);
			plansza2->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;
			}
			trafiony = 0;
			cout << "Przejdz do rundy drugiego gracza: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
			//////////////////////////////////runda 2 gracza///////////////////////////////////
			while (playRound == 2) {
				while (goOn == false) {
					system("CLS");
					cout << "GRACZ 2" << endl;
					showShipAmount(gracz1);
					plansza1->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
					}

					trafiony = 0;

					cout << "Wpisz pole strzalu: " << endl;
					cin >> strzal;

					if (strzal == "[") {
						switchCheat();
						continue;
					}

					goOn = takeShotPoint(strzal);			//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;

				trafiony = shot(strzal, plansza1, gracz1);
				checkPlayer(gracz1);
				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA GRACZ 2!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "GRACZ 2" << endl;
			showShipAmount(gracz2);
			plansza1->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;
			}

			trafiony = 0;

			cout << "Przejdz do rundy pierwszego gracza: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
		}
	}
	//funkcja rozgrywki PVPC
	void playPvPC() {
		string strzal;
		char tmp;	//do wlaczania cheat
		bool goOn = 0;
		int trafiony = 0;


		while (win == 0) {
			//////////////////////////////////runda 1 gracza///////////////////////////////////
			while (playRound == 1) {

				while (goOn == false) {
					system("CLS");
					cout << "GRACZ 1" << endl;
					showShipAmount(gracz2);
					plansza2->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
					}

					trafiony = 0;

					cout << "Wpisz pole strzalu: " << endl;

					cin >> strzal;

					if (strzal == "[") {
						switchCheat();
						continue;
					}

					goOn = takeShotPoint(strzal);	//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;
				trafiony = shot(strzal, plansza2, gracz2);

				checkPlayer(gracz2);

				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA GRACZ 1!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "GRACZ 1" << endl;
			showShipAmount(gracz2);
			plansza2->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;
			}
			trafiony = 0;
			cout << "Przejdz do rundy komputera: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
			//////////////////////////////////runda 2 gracza///////////////////////////////////
			while (playRound == 2) {
				while (goOn == false) {
					system("CLS");
					cout << "CPU 1" << endl;
					showShipAmount(gracz1);
					plansza1->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitCounter1 += 1;
						if (hitCounter1 == 1) {
							hitFlag1 = 1;
						}
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitFlag1 = 0;
						hitCounter1 = 0;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
						if (hitCounter1 > 0) {
							hitFlag1 += 1;
							strzalX1 = tmpX1;
							strzalY1 = tmpY1;
						}
					}

					trafiony = 0;

					if (hitCounter1 == 0) {
						strzalX1 = losX();			//losowanie skladowej x
						strzalY1 = losY();			//losowanie skladowej y
						tmpX1 = strzalX1;
						tmpY1 = strzalY1;
						strzal = makeShot(strzalX1, strzalY1);
					}
					else {
						strzal = shotAlgorithm(hitFlag1, hitCounter1, strzalX1, strzalY1);
					}
					cout << "-------------------------------------" << endl;
					cout << strzal << "  strzal" << endl;
					//cout << hitFlag2 << " " << hitCounter2 << endl;
					cout << "-------------------------------------" << endl;
					tmp = _getch();
					if (tmp == '[') {
						switchCheat();
						tmp = 'p';
					}
					//cout << "Wpisz pole strzalu: " << endl;
					//cin >> strzal;

					//if (strzal == "[[") {
					//switchCheat();
					//continue;
					//}

					goOn = takeShotPoint(strzal);			//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;

				trafiony = shot(strzal, plansza1, gracz1);
				checkPlayer(gracz1);
				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA KOPMPUTER 1!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "CPU 1" << endl;
			showShipAmount(gracz1);
			plansza1->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;

				if (hitCounter1 == 1) {
					hitFlag1 += 1;
					strzalX1 = tmpX1;
					strzalY1 = tmpY1;
				}
				else if (hitCounter1 >= 2) {
					hitFlag1 += 8;
				}
			}

			trafiony = 0;

			cout << "Przejdz do rundy pierwszego komputera: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
		}
	}
	//funkcja rozgrywki PCVPC
	void playPCvPC() {
		string strzal;
		char tmp;  //do zmiany cheat
		bool goOn = 0;
		int trafiony = 0;


		while (win == 0) {
			//////////////////////////////////runda 1 gracza///////////////////////////////////
			while (playRound == 1) {
				while (goOn == false) {
					system("CLS");
					cout << "CPU 1" << endl;
					showShipAmount(gracz2);
					plansza2->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitCounter1 += 1;
						if (hitCounter1 == 1) {
							hitFlag1 = 1;
						}
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitFlag1 = 0;
						hitCounter1 = 0;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
						if (hitCounter1 > 0) {
							hitFlag1 += 1;
							strzalX1 = tmpX1;
							strzalY1 = tmpY1;
						}
					}

					trafiony = 0;

					if (hitCounter1 == 0) {
						strzalX1 = losX();			//losowanie skladowej x
						strzalY1 = losY();			//losowanie skladowej y
						tmpX1 = strzalX1;
						tmpY1 = strzalY1;
						strzal = makeShot(strzalX1, strzalY1);
					}
					else {
						strzal = shotAlgorithm(hitFlag1, hitCounter1, strzalX1, strzalY1);
					}
					cout << "-------------------------------------" << endl;
					cout << strzal << "  strzal" << endl;
					//cout << hitFlag1 << " " << hitCounter1 << endl;
					cout << "-------------------------------------" << endl;
					tmp = _getch();
					if (tmp == '[') {
						switchCheat();
						tmp = 'p';
					}
					//cout << "Wpisz pole strzalu: " << endl;
					//cin >> strzal;

					//if (strzal == "[[") {
					//switchCheat();
					//continue;
					//}

					goOn = takeShotPoint(strzal);			//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;

				trafiony = shot(strzal, plansza2, gracz2);
				checkPlayer(gracz2);
				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA KOPMPUTER 1!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "CPU 1" << endl;
			showShipAmount(gracz2);
			plansza2->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;

				if (hitCounter1 == 1) {
					hitFlag1 += 1;
					strzalX1 = tmpX1;
					strzalY1 = tmpY1;
				}
				else if (hitCounter1 >= 2) {
					hitFlag1 += 8;
				}
			}

			trafiony = 0;

			cout << "Przejdz do rundy drugiego komputera: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
			//////////////////////////////////runda 2 gracza///////////////////////////////////
			while (playRound == 2) {
				while (goOn == false) {
					system("CLS");
					cout << "CPU 2" << endl;
					showShipAmount(gracz1);
					plansza1->showMap();
					cout << endl;

					if (trafiony == 1) {
						cout << "TRAFIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitCounter2 += 1;
						if (hitCounter2 == 1) {
							hitFlag2 = 1;
						}
					}
					else if (trafiony == 3) {
						cout << "TRAFIONY - ZATOPIONY!" << endl;
						cout << "-------------------------------------" << endl;
						hitFlag2 = 0;
						hitCounter2 = 0;
					}
					else if (trafiony == 2) {
						cout << "Juz tu strzelales!" << endl;
						cout << "-------------------------------------" << endl;
						if (hitCounter2 > 0) {
							hitFlag2 += 1;
							strzalX2 = tmpX2;
							strzalY2 = tmpY2;
						}
					}

					trafiony = 0;

					if (hitCounter2 == 0) {
						strzalX2 = losX();			//losowanie skladowej x
						strzalY2 = losY();			//losowanie skladowej y
						tmpX2 = strzalX2;
						tmpY2 = strzalY2;
						strzal = makeShot(strzalX2, strzalY2);
					}
					else {
						strzal = shotAlgorithm(hitFlag2, hitCounter2, strzalX2, strzalY2);
					}
					cout << "-------------------------------------" << endl;
					cout << strzal << "  strzal" << endl;
					//cout << hitFlag2 << " " << hitCounter2 << endl;
					cout << "-------------------------------------" << endl;
					tmp = _getch();
					if (tmp == '[') {
						switchCheat();
						tmp = 'p';
					}
					//cout << "Wpisz pole strzalu: " << endl;
					//cin >> strzal;

					//if (strzal == "[[") {
					//switchCheat();
					//continue;
					//}

					goOn = takeShotPoint(strzal);			//sprawdza poprawnosc wpisanego pola
				}
				goOn = 0;

				trafiony = shot(strzal, plansza1, gracz1);
				checkPlayer(gracz1);
				if (win == 1) {
					system("CLS");
					cout << "=================================================================" << endl;
					cout << "			WYGRYWA KOPMPUTER 2!" << endl;
					cout << "=================================================================" << endl;
					system("pause");
					exit(0);
				}
			}
			system("CLS");
			cout << "CPU 2" << endl;
			showShipAmount(gracz1);
			plansza1->showMap();

			if (trafiony == 0) {
				cout << endl << "PUDLO!" << endl;
				cout << "-------------------------------------" << endl;

				if (hitCounter2 == 1) {
					hitFlag2 += 1;
					strzalX2 = tmpX2;
					strzalY2 = tmpY2;
				}
				else if (hitCounter2 >= 2) {
					hitFlag2 += 8;
				}
			}

			trafiony = 0;

			cout << "Przejdz do rundy pierwszego komputera: " << endl;
			cout << "-------------------------------------" << endl;
			system("pause");
		}
	}
};


int main() {
	srand(time(NULL));

	//	std::thread first(detectKey()); //sprawdzanie klawisza

	Game *gra = new Game();

	gra->manual();
	gra->choseMenu();
	gra->choseSize();
	gra->initiateMaps();
	//gra->plansza1->showMap();
	gra->choseNumber();
	gra->takeNumShips();
	//cout << gra->l2 << " " << gra->l3 << " " << gra->l4 << " " << gra->l5 << endl;

	cout << "Gracz 1:" << endl;
	gra->choseOpt();
	gra->placeShips(gra->plansza1, 1);
	gra->opt = 0;
	cout << "Gracz 2:" << endl;
	gra->choseOpt();
	gra->placeShips(gra->plansza2, 2);
	//////////////////////////
	gra->play();
	/*
	gra->plansza2->showMap();
	cout << gra->s2[1]->hits << " dwdw"<<endl;
	gra->shot("B2", gra->plansza2,gra->gracz2);
	cout << gra->s2[1]->hits << endl;*/
	/*while (1) {
	gra->plansza1->refreshMap();
	cheat = 1;
	system("pause");
	}

	gra->opt = 0;
	cout << "Gracz 2:" << endl;
	gra->choseOpt();
	gra->placeShips(gra->plansza2, 2);


	//gra->s2[0]->initiateShip("poziomo", "B2", gra->plansza1);
	//gra->s2[1]->initiateShip("pionowo", "B4", gra->plansza1);
	//cout << gra->plansza1->isShip[3][1];


	/*int i = 0;
	int move = 1;
	Map *plansza = new Map(26, 10);

	Player *gracz = new Player(2, 2, 2, 2, "gracz");
	plansza->showMap();

	Ship *ship1 = new Ship(2);
	Ship *ship2 = new Ship(2);

	cout << endl << ship1->name << endl;
	cout << endl << ship2->name << endl;

	ship1->initiateShip("poziomo", "C5", plansza, to_string(i + move) + "s2");
	ship2->initiateShip("sp", "E3", plansza, to_string(i + move) + "s2");
	gra->shot("E5", plansza);
	cheat = 1;

	cout << ship2->startPointY << endl;
	cout << ship2->startPointX << endl;
	cout << plansza->isShip[4][4] << endl;
	plansza->showMap();
	cout << "ship2 " << ship2->shipId << endl;*/

	//detectKey();

	system("pause");
	return 0;
}