#include <iostream>
#include <glut.h>
#include <Windows.h>
#include <mmsystem.h>

#define WINDOW_HEIGHT	400
#define WINDOW_WIDTH	900

#define MODEL_SCALE		5


typedef float position;
position model_a_y = 150;
bool no_head_1 = false;
bool no_head_2 = false;

float push_power = 0.0f;
float kick_power = 0.0f;
float R = 0;
float G = 1;
float power_color[3] = { R,G,0 };
bool isHoldingMouse = false;

bool player_1_turn = true;

float deltaGameBall = 0.0;
bool clicked = false; // wait after playing the ball

//change color method (variables)
double backRed = 0;
double backGreen = 0;
double backBlue = 0.5;

int redD = 1;
int greenD = 1;
int blueD = 1;



int* bezier(float t, int* p0, int* p1, int* p2, int* p3)
{
	int res[2];
	res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
	res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
	return res;
}
void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 

		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 

		glVertex2f(x + cx, y + cy);//output vertex 

	}
	glEnd();
}

void changeColor(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	backRed += 0.1*redD;							
	if (backRed < 0 || backRed>1)					
		redD *= -1;								
												
	backGreen += 0.1*greenD;						
	if (backGreen<0 || backGreen>0.5)					
		greenD *= -1;								
												
	backBlue += 0.1*blueD;						
	if (backBlue<0.5 || backBlue>0.75)				
		blueD *= -1;				
	glutPostRedisplay();					
	glutTimerFunc(100, changeColor, 0);
}
/*Player: Model A:*/
class model_a {

public:
	typedef int pos;	
	int health = 150;
	bool visible = true;
	struct head {
		pos pos_x, pos_y;
		void jump(float height) {
			draw(pos_x, model_a_y + height);
			model_a_y += height;
		}
		void draw(int x, int y) {
			pos_x = x;
			pos_y = y;
			glEnable(GL_POINT_SMOOTH);
			glBegin(GL_POLYGON); // These vertices form a closed polygon
			glColor3f(1.0f, 1.0f, 0.0f); // Yellow
			glVertex3d(x, y, 0);
			glVertex3d(x + (5 * MODEL_SCALE), y - (2* MODEL_SCALE), 0);
			glVertex3d(x + (5 * MODEL_SCALE), y - (5 * MODEL_SCALE), 0);
			glVertex3d(x, y - (7 * MODEL_SCALE), 0); //lowest part
			glVertex3d(x - (5 * MODEL_SCALE), y - (5 * MODEL_SCALE), 0);
			glVertex3d(x - (3 * MODEL_SCALE), y - (3 * MODEL_SCALE), 0);
			glVertex3d(x - (5 * MODEL_SCALE), y - (2 * MODEL_SCALE), 0);
			glEnd();

			glPointSize(5.0f);
			glBegin(GL_POINTS); {

				glColor3f(0.0f, 0.0f, 0.0f); //black
				glVertex2i(x, y - (2* MODEL_SCALE));
			}
			glEnd();

			glDisable(GL_POINT_SMOOTH);
			//glPointSize(25);

		}
	}h;
	struct body {
		void draw(int x, int y) {
			glBegin(GL_QUADS);
			glColor3f(backRed, backGreen, backBlue);
			glVertex3d(x, y - (6 * MODEL_SCALE), 0);
			glVertex3d(x + (7 * MODEL_SCALE), y - (14 * MODEL_SCALE), 0);
			glVertex3d(x, y - (21 * MODEL_SCALE), 0); // lowest point
			glVertex3d(x - (7 * MODEL_SCALE), y - (14 * MODEL_SCALE), 0);
			glEnd();
		}
	}b;
	struct leg {
		GLfloat angle = 0.0;
		GLfloat deltaAngle = 0.06;
		void draw(int x, int y) {
			glPushMatrix();
			glTranslated(x, y - (20 * MODEL_SCALE), 0);//tranlate by -p where p is the point you want to rotate about
			glRotated(angle, 0.0, 0, 0.1f);//rotate by some degrees
			glTranslated(-x, -y + (20 * MODEL_SCALE), 0);//tranlate back by p
			glLineWidth(7.0);
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3d(x, y - (20 * MODEL_SCALE), 0);
			glVertex3d(x, y - (25 * MODEL_SCALE), 0);
			glEnd();
			glPopMatrix();
			glutPostRedisplay();
			if (angle > 15 || angle < -15)
				deltaAngle = -deltaAngle;
			angle += deltaAngle;
		}
	}l;
	struct arm {
		GLfloat angle = 0.0;
		GLfloat deltaAngle = 0.1;

		void draw(int x, int y) {
			glPushMatrix();
			glTranslated(x, y - (7 * MODEL_SCALE), 0);//tranlate by -p where p is the point you want to rotate about
			glRotated(angle, 0.0, 0, 0.1f);//rotate by some degrees
			glTranslated(-x, -y + (7 * MODEL_SCALE), 0);//tranlate back by p
			glLineWidth(4.0);
			glBegin(GL_LINE_LOOP);
			glColor3f(0.45f, 0.41f, 0.14f);
			glVertex3d(x, y - (7 * MODEL_SCALE), 0);
			glVertex3d(x + (15 * MODEL_SCALE), y, 0);
			glEnd();
			glPopMatrix();
			glutPostRedisplay();
			if (angle > 90 || angle < 0)
				deltaAngle = -deltaAngle;
			angle += deltaAngle;
		}
	}a;
	struct arm2 {
		GLfloat angle = 45.0;
		GLfloat deltaAngle = -0.1;

		void draw(int x, int y) {
			glPushMatrix();
			glTranslated(x, y - (7 * MODEL_SCALE), 0);//tranlate by -p where p is the point you want to rotate about
			glRotated(angle, 0.0, 0, 0.1f);//rotate by some degrees
			glTranslated(-x, -y +( 7 * MODEL_SCALE), 0);//tranlate back by p
			glLineWidth(4.0);
			glBegin(GL_LINE_LOOP);
			glColor3f(0.45f, 0.41f, 0.14f);
			glVertex3d(x, y - (7 * MODEL_SCALE), 0);
			glVertex3d(x - (15 * MODEL_SCALE), y, 0);
			glEnd();
			glPopMatrix();
			glutPostRedisplay();
			if (angle > 45 || angle < -45)
				deltaAngle = -deltaAngle;
			angle += deltaAngle;
		}
	}a2;
	
	void jump(int height) {
		h.jump(height);
	}
	void draw(int x, int y) {
		if (visible) {
			a.draw(x, y);
			if (!no_head_1) {
				h.draw(x, y);
			}
			b.draw(x, y);
			l.draw(x, y);
			a2.draw(x, y);
		}
	}
};

model_a player_1;
/***********/

/*Player: Model B:*/
class model_b {

public:
	typedef int pos;
	bool visible = true;
	int health = 150;
	struct head {
		pos pos_x, pos_y;
		void jump(float height) {
			draw(pos_x, model_a_y + height);
			model_a_y += height;
		}
		void draw(int x, int y) {
			pos_x = x;
			pos_y = y;
			glEnable(GL_POINT_SMOOTH);
			glBegin(GL_POLYGON); // These vertices form a closed polygon
			glColor3f(0.0f, 1.0f, 0.5f); // Yellow
			glVertex3d(x, y, 0);
			glVertex3d(x + (5 * MODEL_SCALE), y - (2 * MODEL_SCALE), 0);
			glVertex3d(x - (3 * MODEL_SCALE), y - (3 * MODEL_SCALE), 0);
			glVertex3d(x + (5 * MODEL_SCALE), y - (5 * MODEL_SCALE), 0);			

			glVertex3d(x, y - (7 * MODEL_SCALE), 0); //lowest part
			glVertex3d(x - (5 * MODEL_SCALE), y - (5 * MODEL_SCALE), 0);			
			glVertex3d(x - (5 * MODEL_SCALE), y - (2 * MODEL_SCALE), 0);
			glEnd();

			glPointSize(5.0f);
			glBegin(GL_POINTS); {

				glColor3f(0.0f, 0.0f, 0.0f); //black
				glVertex2i(x, y - (2 * MODEL_SCALE));
			}
			glEnd();

			glDisable(GL_POINT_SMOOTH);
		}
	}h;

	struct body {
		void draw(int x, int y) {
			glBegin(GL_QUADS);
			glColor3f(backBlue, backRed, backRed);
			glVertex3d(x, y - (6 * MODEL_SCALE), 0);
			glVertex3d(x + (7 * MODEL_SCALE), y - (14 * MODEL_SCALE), 0);
			glVertex3d(x, y - (21 * MODEL_SCALE), 0); // lowest point
			glVertex3d(x - (7 * MODEL_SCALE), y - (14 * MODEL_SCALE), 0);
			glEnd();
		}
	}b;
	struct leg {
		GLfloat angle = 0.0;
		GLfloat deltaAngle = 0.06;
		void draw(int x, int y) {
			glPushMatrix();
			glTranslated(x, y - (20 * MODEL_SCALE), 0);//tranlate by -p where p is the point you want to rotate about
			glRotated(angle, 0.0, 0, 0.1f);//rotate by some degrees
			glTranslated(-x, -y + (20 * MODEL_SCALE), 0);//tranlate back by p
			glLineWidth(7.0);
			glBegin(GL_LINE_LOOP);
			glColor3f(0.4f, 0.5f, 0.3f);
			glVertex3d(x, y - (20 * MODEL_SCALE), 0);
			glVertex3d(x, y - (25 * MODEL_SCALE), 0);
			glEnd();
			glPopMatrix();
			glutPostRedisplay();
			if (angle > 15 || angle < -15)
				deltaAngle = -deltaAngle;
			angle += deltaAngle;
		}
	}l;
	struct arm {
		GLfloat x_axis = 0.0;
		GLfloat deltaX = 0.03;

		void draw(int x, int y) {
			glPushMatrix();
			glTranslated(x_axis, 0 , 0);
			glLineWidth(2.5);
			glBegin(GL_LINES);
			glColor3f(0.45f, 0.41f, 0.14f);

			glVertex3d(x - 60, y - (7 * MODEL_SCALE) - 20, 0);
			glVertex3d(x - 35, y - (7 * MODEL_SCALE) - 20, 0);

			glVertex3d(x - 35, y - (7 * MODEL_SCALE)-20, 0);
			glVertex3d(x - 35, y - (7 * MODEL_SCALE), 0);

			glVertex3d(x - 35, y - (7 * MODEL_SCALE), 0);
			glVertex3d(x + 35, y - (7 * MODEL_SCALE), 0);

			glVertex3d(x + 60, y - (7 * MODEL_SCALE) + 20, 0);
			glVertex3d(x + 35, y - (7 * MODEL_SCALE) + 20, 0);

			glVertex3d(x + 35, y - (7 * MODEL_SCALE) + 20, 0);
			glVertex3d(x + 35, y - (7 * MODEL_SCALE), 0);


			glEnd();
			glPopMatrix();
			glutPostRedisplay();
			if (x_axis > 5 || x_axis < -5)
				deltaX = -deltaX;
			x_axis += deltaX;
		}
	}a;

	void jump(int height) {
		h.jump(height);
	}
	void draw(int x, int y) {
		if (visible) {
			if (!no_head_2) {
				h.draw(x, y);

			}
			a.draw(x, y);
			b.draw(x, y);
			l.draw(x, y);
		}
	}
};

model_b player_2;
/***********/

/*Obstacles: */
class wall {
public:
	int pos_x, pos_y;
	void draw(int x, int y) {
		pos_x = x; pos_y = y;
		glPushMatrix();
		glBegin(GL_POLYGON); // These vertices form a closed polygon
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow
		glVertex3d(x, y, 0);
		glVertex3d(x + (5*MODEL_SCALE), y - 20, 0);
		glVertex3d(x + (5 * MODEL_SCALE), y - (WINDOW_HEIGHT/2), 0);
		glVertex3d(x - (5 * MODEL_SCALE), y - (WINDOW_HEIGHT/2), 0);
		glVertex3d(x - (5 * MODEL_SCALE), y - 20, 0);
		glEnd();
		glPopMatrix();
	}
};
wall obstacle;

class pendulum {
public:
	position ball[2] = { WINDOW_WIDTH/2 , WINDOW_HEIGHT/2 };
	void draw(int x, int y) {
		glPushMatrix();
		glLineWidth(1.5);
		glBegin(GL_LINES); {
			glColor3f(0.330f, 0.1453f, 0.785f); //color
			glVertex2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT);
			glVertex2f(ball[0], ball[1]);
		}
		glEnd();
		glColor3f(1.330f, 0.1453f, 0.785f); //color
		DrawCircle(ball[0], ball[1], 30, 50);		
		glPopMatrix();
		
	}
};

pendulum obstacle_2;
bool inc = true;

float deltaBall = 0.0;
void pendulumAnimation(int val) {
	int p0[2] = { 0, WINDOW_HEIGHT };
	int p1[2] = { 0, 180 };
	int p2[2] = { WINDOW_WIDTH, 180 };
	int p3[2] = { WINDOW_WIDTH , WINDOW_HEIGHT };
	

	int* p = bezier(deltaBall, p0, p1, p2, p3);
	obstacle_2.ball[0] = p[0];
	obstacle_2.ball[1] = p[1];
	glutPostRedisplay();
	if (deltaBall > 1) {
		deltaBall = 1;
		inc = false;
	}
	else if (deltaBall < 0) {
		deltaBall = 0;
		inc = true;
	}
	deltaBall = inc?deltaBall+ 0.04: deltaBall -0.04;
	glutTimerFunc(100, pendulumAnimation, 0);
}

/***********/

/*Health Bars*/
class payload_bar {
public:
	
	struct outline {		
		void draw(int x, int y) {
			glPushMatrix();
			glBegin(GL_LINE_LOOP); {
				glColor3f(1.0, 1.0, 1.0); //color white
				glVertex2f(x, y);
				glVertex2f(x+150, y);
				glVertex2f(x+150, y-20);
				glVertex2f(x, y-20);
			}
			glEnd();
			glPopMatrix();
		}
	}out;
	struct payload {		
		int max = 150;
		int min = 0;
		void draw(float x, float value, float y, float color[3]) {
			float ratio = 0.5;
			glPushMatrix();
			glBegin(GL_QUADS); {
				glColor3f(color[0], color[1], color[2]); //color 
				glVertex2f(x+ ratio, y - ratio);
				glVertex2f(x + value - ratio, y + ratio);
				glVertex2f(x + value - ratio, y - 20 + ratio);
				glVertex2f(x + ratio, y - 20 + ratio);
			}
			glEnd();
			glPopMatrix();
		}
		
	}bar;

	void draw(int x, int y, float value, float color[3]) {
		bar.draw(x, value, y, color);
		out.draw(x, y);
	}

};

payload_bar player_1_health;
payload_bar player_2_health;
payload_bar player_1_power;
payload_bar player_2_power;

/***********/

/*BALL*/
class ball {
public:
	bool visible = true;
	position ball_pos[2] = { WINDOW_WIDTH - 120,  50 };
	void draw() {
		if (visible) {
			glPushMatrix();
			glColor3f(backGreen, backRed, backBlue); //color
			DrawCircle(ball_pos[0], ball_pos[1], 15, 50);
			glPopMatrix();
		}
	}
};
ball gameBall;
/*******/


/*OnPlayerLose*/
int counter = 0;
void onPlayerLose(int player_id) 
{
	clicked = true;
	switch (player_id)
	{
	case 1://player 1
	{
		if (counter == 100) {
			gameBall.visible = false;
			player_1.visible = false;
		}
		else {
			player_1.visible = !player_1.visible;
			glutTimerFunc(500, onPlayerLose, 1);
			counter++;
		}
		break;
	}
	case 2://player 2
	{
		if (counter == 100) {
			gameBall.visible = false;
			player_2.visible = false;
		}
		else {
			player_2.visible = !player_2.visible;
			glutTimerFunc(500, onPlayerLose, 2);
			counter++;
		}
		break;
	}
	}	
	glutPostRedisplay();
}
/*************/

/*Game Logic*/
bool detectCollision() {
	if (player_1_turn) {//if player's 1 turn
		//if ball's position within range of player's 2 quads
		if (gameBall.ball_pos[0] <= player_2.h.pos_x + 30 && gameBall.ball_pos[0] >= player_2.h.pos_x - 30
			&& gameBall.ball_pos[1] <= player_2.h.pos_y && gameBall.ball_pos[1] >= 20) {
			return true;
		}
	}
	if (!player_1_turn) {//if player's 2 turn
		//if ball's position within range of player's 1 quads
		if (gameBall.ball_pos[0] <= player_1.h.pos_x + 30 && gameBall.ball_pos[0] >= player_1.h.pos_x - 30
			&& gameBall.ball_pos[1] <= player_1.h.pos_y && gameBall.ball_pos[1] >= 20)
			return true;
	}
	return false;
}
bool detectCollisionObject(){
	//detect collision with obstacles
	if (gameBall.ball_pos[0] <= obstacle.pos_x + 30 && gameBall.ball_pos[0] >= obstacle.pos_x - 30
		&& gameBall.ball_pos[1] <= obstacle.pos_y) {
		return true;
	}
	if (gameBall.ball_pos[0] <= obstacle_2.ball[0] + 50 && gameBall.ball_pos[0] >= obstacle_2.ball[0] - 50
		&& gameBall.ball_pos[1] <= obstacle_2.ball[1] + 50 && gameBall.ball_pos[1] >= obstacle_2.ball[1]-50) {
		return true;
	}
	return false;
}

void canClickAgain(int val) {
	clicked = false;
}
void switchTurns(int val) {
	player_1_turn = !player_1_turn;
	if (player_1_turn) {
		gameBall.ball_pos[0] = WINDOW_WIDTH - 120;
		gameBall.ball_pos[1] = 50;
	}
	else {
		gameBall.ball_pos[0] = 120;
		gameBall.ball_pos[1] = 50;
	}	
	canClickAgain(0);
}

void restoreHead_1(int val) {
	no_head_1 = false;
}

void restoreHead_2(int val) {
	no_head_2 = false;
}

bool switchMusic = false;
float RGB[3]{ 0, 1, 0.3 };
float RGB2[3]{ 0, 1, 0.3 };
void ballAnimation(int val) {
	if (player_1_turn) {
		int p0[2] = { WINDOW_WIDTH - 120 , 50 };
		int p1[2] = { WINDOW_WIDTH, WINDOW_HEIGHT };
		int p2[2] = { (WINDOW_WIDTH / 2) - kick_power * 4 , WINDOW_HEIGHT };
		int p3[2] = { (WINDOW_WIDTH / 2) - kick_power * 4 , 50 };

		int* p = bezier(deltaGameBall, p0, p1, p2, p3);

		gameBall.ball_pos[0] = p[0];
		gameBall.ball_pos[1] = p[1];
		glutPostRedisplay();


		if (deltaGameBall < 1 && player_1_turn) {
			deltaGameBall += 0.04;
			glutTimerFunc(100, ballAnimation, 0);
		}
		else if (deltaGameBall >= 1 && player_1_turn) {
			glutTimerFunc(200, switchTurns, 0);
			deltaGameBall = 0;
		}

		if (detectCollisionObject()) {
			deltaGameBall = 100;
		}

		if (detectCollision()) {
			RGB[0] += 0.1;
			RGB[1] -= 0.1;
			RGB[2] -= 0.05;
			player_2.health -= 5;
			player_2_health.draw(WINDOW_WIDTH - 200, 350, player_2.health, RGB);
			no_head_2 = true;
			glutTimerFunc(300, restoreHead_2, 0);
			
		}
	}
	else {
		int p0[2] = { 120 , 50 };
		int p1[2] = { 20, WINDOW_HEIGHT };

		int p2[2] = { (WINDOW_WIDTH / 2) + kick_power * 4, WINDOW_HEIGHT };
		int p3[2] = { (WINDOW_WIDTH / 2) + kick_power * 4 , 50 };

		int* p = bezier(deltaGameBall, p0, p1, p2, p3);

		gameBall.ball_pos[0] = p[0];
		gameBall.ball_pos[1] = p[1];
		glutPostRedisplay();

		if (deltaGameBall < 1 ) {
			deltaGameBall += 0.04;
			glutTimerFunc(100, ballAnimation, 0);
		}
		else if (deltaGameBall >= 1) {
			glutTimerFunc(200, switchTurns, 0);
			deltaGameBall = 0;
		}

		if (detectCollisionObject()) {
			deltaGameBall = 100;
		}
		if (detectCollision()) {
			RGB2[0] += 0.1;
			RGB2[1] -= 0.1;
			RGB2[2] -= 0.07;
			player_1.health -= 5;
			player_1_health.draw(WINDOW_WIDTH - 200, 350, player_1.health, RGB2);
			no_head_1 = true;
			glutTimerFunc(300, restoreHead_1, 0);
		}
	}
	if (!switchMusic) {
		if (player_1.health < 80 || player_2.health < 80) {
			switchMusic = true;
			PlaySound(TEXT("audio/lost.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
	}
	if (player_1.health <= 0){
		player_1.health = 0;
		onPlayerLose(1);
	}
	if (player_2.health <= 0) {
		player_2.health = 0;
		onPlayerLose(2);
	}
}
/**********/

/*Mouse Functions*/
int ctr = 0;
void mouseAction(int button, int state, int x, int y)//mouse function takes 4 parameters: button: which button has been clicked (GLUT_RIGHT_BUTTON or GLUT_LEFT_BUTTON),
													//state wether the button is clicked or released (GLUT_UP or GLUT_DOWN)
													// x and y are the position of the mouse cursor
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//if the left button has been clicked 
	{
		if (clicked == false) {
			isHoldingMouse = true;
			//PlaySound(TEXT("audio/power.wav"), NULL, SND_ASYNC);
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (clicked == false) {
			//PlaySound(TEXT("audio/hit.wav"), NULL, SND_ASYNC);
			isHoldingMouse = false;
			clicked = true;
			kick_power = push_power;
			glutTimerFunc(100, ballAnimation, 0);
		}
	}
	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		switch (ctr) {
		case 0:
			player_2.health = 120; break;
		case 1:
			player_2.health = 100;break;
		case 2:
			player_2.health = 85;break;
		case 3:
			player_2.health = 50;break;
		case 4:
			player_2.health = 10;break;
		case 5:
			player_2.health = 0;break;
		}
		ctr++;
	}
	glutPostRedisplay();//redisplay to update the screen with the new paraeters
}
/***********/

/*Background Lines*/
float y_positions[11] = {
	{100},
	{250},
	{341},
	{20},
	{140},
	{300},
	{360},
	{70},
	{188},
	{466}
};
float x_positions[11][2] = {
	{0, 30},
	{810, 850},
	{200, 260},
	{0, 20},
	{50, 90},
	{450, 470},
	{500, 530},
	{630, 670},
	{20, 50},
	{300,310},
	{741, 763}
};
void drawBackground() {
	glPushMatrix();
	glLineWidth(0.3);
	glBegin(GL_LINES); {
		glColor3f(1, 1, 1);	
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 2; j++) {
				glVertex2f(x_positions[i][j], y_positions[i]);
			}
		}
	}
	glEnd();
	glPopMatrix();

	for (int i = 0; i < 11; i++) 
	{
		for (int j = 0; j < 2; j++) 
		{
			float push = (rand() / (float)RAND_MAX) / 10;
			if (i % 3 == 0) {
				x_positions[i][j] += push;
			}
			else {
				push = (rand() / (float)RAND_MAX) / 10;
				x_positions[i][j] += push;
			}
		}
	}

	if (x_positions[0][0] > WINDOW_WIDTH) {
		int x_positions_again[11][2] = {
			{0, 30},
			{70, 100},
			{200, 260},
			{0, 20},
			{50, 90},
			{30, 60},
			{260, 290},
			{210, 220},
			{20, 50},
			{300,310},
			{350, 400}
		};
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 2; j++) {
				x_positions[i][j] = x_positions_again[i][j];
			}
		}
	}

}
/*********/

void Display() {
	glClearColor(backRed, backBlue, backBlue, 0.0f);//the background color is changed in the timer function after 1 sec by changing these parameters.
	glClear(GL_COLOR_BUFFER_BIT);

	//draw background
	drawBackground();

	player_1.draw(WINDOW_WIDTH - 80, model_a_y);										//draw player 1
	player_2.draw(80, model_a_y);														//draw player 2
	obstacle.draw(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);									//draw wall
	obstacle_2.draw(obstacle_2.ball[0], obstacle_2.ball[1]);							//draw pendulum
	//bars
		//color
	player_1_health.draw(WINDOW_WIDTH - 200, 350, player_1.health, RGB2);				//player 1 health
	player_2_health.draw(50, 350, player_2.health, RGB);								//player 2 health
	if (player_1_turn) {//if player's 1 turn
		player_1_power.draw(WINDOW_WIDTH - 170, 6, push_power, power_color);			//player 1 power
		player_2_power.draw(20, 6, 0, power_color);
	}
	else {
		player_2_power.draw(20, 6, push_power, power_color);							//player 2 power
		player_1_power.draw(WINDOW_WIDTH - 170, 6, 0, power_color);
	}

	//draw BALL
	gameBall.draw();
	glFlush();
}

float jump_speed = 1;
bool up = true;

void gameTime(int val) {
	if (up) {
		player_1.jump(++jump_speed);			//players jump
		player_2.jump(++jump_speed);			//players jump
		if (jump_speed == 5)
			up = false;
	}
	else {
		player_1.jump(--jump_speed);
		player_2.jump(--jump_speed);
		if (jump_speed == -5)
			up = true;
	}

	glutPostRedisplay();
	glutTimerFunc(100, gameTime, 0);
	
}

void onUpdate() {
	if (isHoldingMouse) {
		if (push_power < player_1_power.bar.max) {
			push_power += 0.1;
			R += 0.0007;
			G -= 0.0007;
			power_color[0] = R;
			power_color[1] = G;

		}
	}
	else if (!isHoldingMouse && push_power >= 0) {
		push_power -= 0.1;
		R = 0;
		G = 1;
	}
	glutPostRedisplay();
}
void resize(int width, int height) {
	// we ignore the params and do:
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void main(int argc, char** argr) {
	glutInit(&argc, argr);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(150, 150);

	glutCreateWindow("40-8252 | Dodge Ball");
	glutReshapeFunc(resize);
	glutDisplayFunc(Display);
	glutMouseFunc(mouseAction);					//call the mouse function
	glutTimerFunc(0, gameTime, 0);				//call the timer function (gameTime)
	glutTimerFunc(0, pendulumAnimation, 0);		//call the timer function (pendulum obstacle)
	glutTimerFunc(0, changeColor, 0);
	glutIdleFunc(onUpdate);						//call onUpdate function
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);

	
	
	PlaySound(TEXT("audio/background_music.wav"), NULL, SND_ASYNC | SND_LOOP);
	glutMainLoop();//don't call any method after this line as it will not be reached.
}
