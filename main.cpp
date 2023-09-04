#include <M5Core2.h>

void createMenuButton(int,int,String,uint16_t);
void createMenu();
void drawGame(int,int,int);
void drawPlayer(int,int);
void drawPowerMeter(int);
void getPoints(int, int);
void shootDart(int,int,int,int);

enum STATES {menu, game, scores, turnoff};
STATES currentState;

// BUTTON ZONES
HotZone startBtn(85, 30, 235, 70);
HotZone scoresBtn(85, 100, 235, 140);
HotZone exitBtn(85, 170, 235, 210);

// SCREEN INITIATED
bool menuInitiated = false;
bool scoresInitiated = false;
bool targetInitiated = false;
bool gameInitiated = false;
bool gameScreen = false;
bool powerInitiated = false;
bool playerInitiated = false;

// TARGET
int xTarget;
int yTarget;
int rTarget = 80;

// GAME
int gamenr = 0;
int shotCount;
int points;
int score;
int scoreList[8];

// POWER METER
int powerMeter;
int increase;
int xShot;
int yShot;

// PLAYER
int xPos;
int yPos;
int xAim;
int yAim;
int moveLeft = 0;
int moveRight = 0;

void setup(){
  M5.begin();
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextFont(1);

  currentState = menu;

}

void loadMenuScreen(){
if (!menuInitiated) {
    menuInitiated = true;
    gameInitiated = false;
    M5.Lcd.clear(WHITE);
    createMenu();
  }
  TouchPoint_t pressedPos = M5.Touch.getPressPoint();
  if (startBtn.inHotZone(pressedPos)) {
    currentState = game;
  }
  if (scoresBtn.inHotZone(pressedPos)) {
    currentState = scores;
  }
  if (exitBtn.inHotZone(pressedPos)) {
    M5.shutdown();
  }
}

void loadScoreScreen(){
  if (!scoresInitiated){
    scoresInitiated = true;
    menuInitiated = false;
    M5.Lcd.clear(WHITE);
    int scoreTextYValue = 20;
    createMenuButton((M5.Lcd.width()/2)-75, 170, "MENU", RED);
    M5.Lcd.textcolor = BLUE;
    for (int i = 0; i < sizeof(scoreList)/4; i++){
      if (scoreList[i] != 0){
        M5.Lcd.setCursor(160-(M5.Lcd.textWidth("Game #00: 00")/2), scoreTextYValue);
        M5.Lcd.printf("Game #%d: %d\n",i+1, scoreList[i]);
        scoreTextYValue += 20;
      }
    }
  }
  TouchPoint_t pressedPos = M5.Touch.getPressPoint();
  if (exitBtn.inHotZone(pressedPos)) {
    currentState = menu;
    scoresInitiated = false;
  }
}

void loadGameScreen(){
  // initiate game
  if (!gameInitiated){
    menuInitiated = false;
    gameInitiated = true;
    gameScreen = true;
    shotCount = 0;
    points = 0;
    score = 0;
    drawGame(moveLeft,moveRight,increase);
  }
  else if (shotCount >= 1 && !gameScreen) {
    drawGame(moveLeft,moveRight,increase);
    gameScreen = true;
  }
  // get touched location on screen
  TouchPoint_t pressedPos = M5.Touch.getPressPoint();
  // press left button to aim
  if ((pressedPos.x > 50 && pressedPos.x < 100) && (pressedPos.y > 175 && pressedPos.y < 225)){
    moveLeft = 1;
    drawGame(moveLeft, moveRight, increase);
    moveLeft = 0;
  }
  // press right button to aim
  else if ((pressedPos.x > 245 && pressedPos.x < 295) && (pressedPos.y > 175 && pressedPos.y < 225)){
    moveRight = 1;
    drawGame(moveLeft, moveRight, increase);
    moveRight = 0;
  }
  // press target to shoot
  else if ((pressedPos.x > (xTarget-60) && pressedPos.x < (xTarget+60)) && (pressedPos.y > (yTarget-60) && pressedPos.y < (yTarget+60))) {
    M5.Lcd.clear(WHITE);
    // set speed of powermeter
    increase = 1.5;
    drawGame(moveLeft,moveRight,increase);
    increase = 0;
  }
  // when charge throw button is released
  else if (powerMeter != 0){
    delay(200);
    shotCount += 1;

    shootDart(xPos,yPos,xAim,yAim);

    M5.Lcd.setCursor(0,50);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.printf("Points: %d", points);
    delay(3000);

    // if 3 throws been made, return to menu screen
    if (shotCount == 3){
      scoreList[gamenr] = score;
      gamenr += 1;
      currentState = menu;
    }

    points = 0;
    powerMeter = 0;
    playerInitiated = false;
    powerInitiated = false;
    targetInitiated = false;
    gameScreen = false;

  }
}

void loop(){
  M5.update();
  while(currentState == menu){
    loadMenuScreen();
  }

  while(currentState == scores){
    loadScoreScreen();
  }

  while(currentState == game){
    loadGameScreen();
  }
}

void createMenuButton(int x, int y, String text, uint16_t color){
  // Button layout
  int buttonWidth = 150;
  int buttonHeight = 40;
  int buttonRadius = 10;

  int innerWidth = buttonWidth - 10;
  int innerHeight = buttonHeight - 10;
  int innerX = x+5;
  int innerY = y+5;

  // Button design
  M5.Lcd.fillRoundRect(x, y, buttonWidth, buttonHeight, buttonRadius, color);
  M5.Lcd.fillRoundRect(innerX, innerY, innerWidth, innerHeight, buttonRadius, WHITE);

  // Button text
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(color,WHITE);

  int text_x = x + ((buttonWidth/2) - (M5.Lcd.textWidth(text)/2));
  int text_y = y + ((buttonHeight/2) - (M5.Lcd.fontHeight()/2));

  M5.Lcd.setCursor(text_x, text_y);
  M5.Lcd.println(text);
  M5.Lcd.setCursor(0, 0);

}

void createMenu(){
  createMenuButton((M5.Lcd.width()/2)-75, 30, "START", BLUE);
  createMenuButton((M5.Lcd.width()/2)-75, 100, "SCORES", DARKGREEN);
  createMenuButton((M5.Lcd.width()/2)-75, 170, "TURN OFF", RED);
}

void drawTarget(){
  // set random position of target when created
  if (!targetInitiated) {
    targetInitiated = true;
    xTarget = random(80,240);
    yTarget = random(80,100);
  }
  int x = xTarget;
  int y = yTarget;
  int r = rTarget;

  // OUTER CIRCLE
  M5.Lcd.fillCircle(x,y,r,RED);
  M5.Lcd.fillCircle(x,y,r-5,WHITE);
  // MIDDLE CIRCLE
  M5.Lcd.fillCircle(x,y,r-15,RED);
  M5.Lcd.fillCircle(x,y,r-35,WHITE);
  // INNER CIRCLE
  M5.Lcd.fillCircle(x,y,r-50,RED);
  M5.Lcd.fillCircle(x,y,r-65,WHITE);
  // BULLSEYE
  M5.Lcd.fillCircle(x,y,r-75,RED);
}

void drawPlayer(int moveLeft, int moveRight){
  // First time drawing player position
  if (!playerInitiated){
    playerInitiated = true;
    xPos = random(60,260);
    yPos = 239;
    xAim = xPos;
    yAim = yPos - 30;
  }

  // adjust aim
  if (moveLeft) {
    xAim -= 1;
    if (xAim <= xPos) {
      yAim += 1;
    } else if (xAim > xPos) {
      yAim -= 1;
    }
  } else if (moveRight) {
    xAim += 1;
    if (xAim >= xPos) {
      yAim += 1;
    } else if (xAim < xPos) {
      yAim -= 1;
    }
  }

  M5.Lcd.drawLine(xPos,yPos,xAim,yAim,BLACK);

  int rightButton [] = {270,200};
  int leftButton [] = {50,200};

  // draw control buttons
  M5.Lcd.fillCircle(rightButton[0],rightButton[1],25,LIGHTGREY);
  M5.Lcd.fillCircle(leftButton[0],leftButton[1],25,LIGHTGREY);

}

void drawPowerMeter(int increasePower) {
  // reset meter first time drawing
  if (!powerInitiated){
    powerInitiated = true;
    powerMeter = 0;
  }

  //increase meter settings (increases faster the more power there is)
  if (powerMeter >= 80) {
    powerMeter = 0;
  }else if (powerMeter <70) {
    powerMeter += increasePower * 5;
  }else if ( powerMeter < 50) {
    powerMeter += increasePower * 4;
  }else {
    powerMeter += increasePower * 3;
  }

  // check on what side target is placed and set meter position to the other
  if (xTarget >= 160) {
    M5.Lcd.drawRect(10,10,80,25, BLACK);
    M5.Lcd.fillRect(11,11,powerMeter,25,GREEN);
  } else {
    M5.Lcd.drawRect(230,10,80,25, BLACK);
    M5.Lcd.fillRect(231,11,powerMeter,25,GREEN);
  }
}

// print total game score on the bottom of the screen
void drawTotalScore(){
  M5.Lcd.setCursor(160-(M5.Lcd.textWidth("TOTAL: 00")/2), 190);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("TOTAL: %d", score);
}

void drawGame(int moveLeft, int moveRight, int power){
  M5.Lcd.clear(WHITE);
  drawTarget();
  drawPowerMeter(power);
  drawPlayer(moveLeft,moveRight);
  drawTotalScore();
}

// Calculate and add points depending on where the player hits the target
void getPoints(int hitX, int hitY){
  if ((hitX > xTarget-5 && hitX < xTarget+5) && (hitY > yTarget - 5 && hitY < yTarget + 5)){
        points = 10;
        score += points;
      } else if ((hitX > xTarget-15 && hitX < xTarget+15) && (hitY > yTarget - 15 && hitY < yTarget + 15)){
        points = 5;
        score += points;
      } else if ((hitX > xTarget-30 && hitX < xTarget+30) && (hitY > yTarget - 30 && hitY < yTarget + 30)){
        points = 4;
        score += points;
      } else if ((hitX > xTarget-45 && hitX < xTarget+45) && (hitY > yTarget - 45 && hitY < yTarget + 45)){
        points = 3;
        score += points;
      } else if ((hitX > xTarget-55 && hitX < xTarget+55) && (hitY > yTarget - 55 && hitY < yTarget + 55)){
        points = 2;
        score += points;
      } else if ((hitX > xTarget-80 && hitX < xTarget+80) && (hitY > yTarget - 80 && hitY < yTarget + 80)){
        points = 1;
        score += points;
      }
}

void shootDart(int positionX, int positionY, int aimX, int aimY){
  // calculate linear vector and add power from player input to get hit zone.
  int m = powerMeter/7;
  int oa [] = {positionX, 240-positionY};
  int ab [] = {aimX-positionX, (240-aimY)-(240-positionY)};
  int hitx = oa[0] + (m*ab[0]);
  int hity = 240 - (m*ab[1]);
  getPoints(hitx, hity);
  drawGame(moveLeft,moveRight,increase);
  M5.Lcd.fillCircle(hitx, hity, 5, BLACK);
}
