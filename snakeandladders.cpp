//CS 757 
//Author: Surender Singh Lamba, 140050075
//Compile command: g++ -std=c++11 snakeandladders.cpp -lfltk -lfltk_images -o snakeandladders

#include <iostream>
#include <string>
#include <random>//For random number generation in dice
#include <vector>//For holding players
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Box.H>

using namespace std;
int SQ_size = 10;int num_players=3;//Number of players MAX=4(requires more images)
//Postions of snakes and ladders are encoded as mappings[startPos]=destinationPos
int mappings[]={0,0,79,0,0,75,0,73,0,0,
				 0,0,0,0,0,0,36,0,0,0,
				 99,0,0,0,0,0,0,0,91,0,
				 0,19,0,60,0,0,0,0,0,0,
				 0,0,0,0,0,0,34,0,0,67,
				 0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,
				 42,0,0,0,0,0,0,84,0,0,
				 0,0,0,7,0,0,0,0,0,0,
				 38,0,0,14,0,0,0,0,31,0};
Fl_Window* window;
 //Generates char* from given int
char *int2charstar (int v) {
 char *s = new char[sizeof(int)];
 sprintf(s,"%d",v);
 return s;
};

class Board;//Entity that holds the board and button showing turns
class Button;//This is used for the dice
class Player;//This holds the state and entity of player

class Player: public Fl_Box{
	Fl_Box *box;
	int pos;int player;int final_pos;//pos and final_pos are start and end points in a move of the player
	Board* board;
public:
	Player(int p,Board* b);
	void setPos(int x);
	int getPos();
	void setfinalPos(int x);
	int getfinalPos();
	Fl_Box* getbox();
	Board* getboard();
};

//For randomization button and its handling
class Button: public Fl_Button{
	Fl_Button *button;
	Board* b;//For updating turns and moving players
public:
	Button(int x, int y, int pw, int ph,Board *board);
	int handle(int);
};

class Board {
 Button *result_button;//To show the turn of the player
 vector<Player*> player;
 int turn;
 public:
	Board(int, int, int, int);
	int move(int,int);//Move current player
	int randomize();//Gives dice value
	void setTurn(int t);
	int getTurn();
	Button* getButton();
};

Player::Player(int p,Board* b):
	Fl_Box(0,0,40,40){
		box=this;
		board=b;
		//Loads image for player
		string s=""+to_string(p+1)+".png";
		char *buf = new char[s.size()];
		strcpy(buf,s.c_str());
		Fl_PNG_Image* pngImg = new Fl_PNG_Image(buf);
		delete []buf;
		this->image(pngImg);
		this->show();
		pos=0;final_pos=0;
}

void Player::setPos(int x){
	pos=x;
}

int Player::getPos(){
	return pos;
}
void Player::setfinalPos(int x){
	final_pos=x;
}

int Player::getfinalPos(){
	return final_pos;
}

Fl_Box* Player::getbox(){
	return box;
}

Board* Player::getboard(){
	return board;
}

Button::Button(int x, int y, int pw, int ph,Board *board):
	Fl_Button(x,y,pw,ph,"Throw Dice"){;
		b=board;
		button=this;
	}
//Special funtion to animate movement of player based on initial and final positions
void move_player(void* data)
{
	//Static counter to do 100 iterations per move
    static int counter=0;
    counter++;
    
    Player* player= (Player*) data;
    //Get initial and final position number and calculate actual coordinates for both p1x,p1y and p2x,p2y
    int p1=player->getPos();
    int p2=player->getfinalPos();
    int p1x,p2x;
    if(p1%10==0){
    	p1x=((p1/10)%2!=1)?50:410;
    }else{
    	p1x=((p1/10)%2!=1)?50+(p1-1-(p1/10)*10)*40:360+50-(p1-1-(p1/10)*10)*40;
    }
    int p1y=(p1%10!=0)?150+360-(p1/10)*40:150+360-(p1/10-1)*40;
    if(p2%10==0){
    	p2x=((p2/10)%2!=1)?50:410;
    }else{
    	p2x=((p2/10)%2!=1)?50+(p2-1-(p2/10)*10)*40:360+50-(p2-1-(p2/10)*10)*40;
    }
    int p2y=(p2%10!=0)?150+360-(p2/10)*40:150+360-(p2/10-1)*40;
    //Move the box step by step
    player->getbox()->position(p1x+(p2x-p1x)*counter*1.0/100,p1y+(p2y-p1y)*counter*1.0/100);
    //redraw window to clear past frames
    window->redraw();
    //Repeat for counter below 100
    if(counter<100){
    	Fl::add_timeout(.02, move_player, player);
	}else{
		//For counter above 100, set current postion and check if a snake or ladder is present and move
		player->setPos(p2);
		player->getbox()->position(p2x,p2y);
    	window->redraw();
    	counter=0;//reset counter
    	int a=mappings[(p2x-50)/40 + (p2y-150)/4];
    	if(a!=0){
    		player->getboard()->move(a-p2,1);
    		
    	}
	}
}
//Special funtion to activate dice after 3 seconds
void display_number(void* data)
{
    Fl_Button* button= (Fl_Button*) data;
    button->label("Throw Dice");
    button->activate();  
}

int Button::handle(int event){
	if (event==FL_PUSH) {
		//This part throws dice and blocks te button for 3 seconds
		b->setTurn((b->getTurn()+1)%num_players);
		button->label(int2charstar(b->randomize()));
		Fl::add_timeout(3, display_number, button);
		button->deactivate();
		//This part updates turn display button
		string s="Turn: "+to_string(b->getTurn()+1);
		char *buf = new char[s.size()];
		strcpy(buf,s.c_str());
		b->getButton()->label(buf);
	}
		return 1;
}

Board::Board(int x0, int y0, int pw, int ph){
	//Initialize board with turn button and players
	turn=0;
	result_button=new Button(x0/2, y0/2, 100, 50,this);
	result_button->color(FL_BLACK);
	result_button->labelcolor(FL_WHITE);
	result_button->label("Turn: 1");
	result_button->deactivate();
	player.resize(num_players);
	for (int i = 0; i < num_players; ++i)
	{
		player[i]=new Player(i,this);
	}

}

void Board::setTurn(int t){
	turn=t;
}

int Board::getTurn(){
	return turn;
}

Button* Board::getButton(){
	return result_button;
}
//sets final position and invokes special function to animate movement
int Board:: move(int x,int special) {
	int k;
	if(special!=1){
		k=(turn-1<0)?(num_players-1):turn-1;
	}else{
		k=(turn-1<0)?(num_players-1):turn-1;
	}
	if(player[k]->getPos()+x>100){return 0;}
	player[k]->setfinalPos(player[k]->getPos()+x);
	if(player[k]->getPos()==0){player[k]->setPos(1);}
	Fl::add_timeout(.5,move_player,player[k]);
}
//gives back dice value and also invokes move
int Board::randomize(){
	//use a random number generator to form moves
	mt19937 generator{random_device{}()};
    uniform_int_distribution<int> distribution{'1', '6'};

    auto generate_len = 1;
    string rand_str(generate_len, '\0');
    for(auto& dis: rand_str)
        dis = distribution(generator);
    //execute moves according to generated string ensuring boundary conditions
    move(stoi(rand_str),0);
    return stoi(rand_str);
}

int main(int argc, char *argv[]) {

   int X=400, Y=400;  // height width of actual puzzle
   int bx=150, by=50; // border margins from x and y axis
   int pw=X/SQ_size, ph=X/SQ_size; // height width
   Fl_Window* w = new Fl_Window(X+2*by, Y+2*bx); // board is 25% larger
   window=w;//Global so that we can redraw it in animations
   w->label("CS 757 15 Puzzle Lecture 2");
   Fl_JPEG_Image* j1pgImg = new Fl_JPEG_Image("snakesboard.jpg");
   Fl_Box* box = new Fl_Box(by,bx,400,400);//for example
   box->image(j1pgImg);		
   Board b(by, bx, pw, ph);
   Button* rand_button=new Button(bx/2+100, 3*by/2, 100, 50,&b);
   w->end();
   w->show(argc, argv);
   return Fl::run();
}