#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define SPACESHIP_HEIGHT   16
#define SPACESHIP_WIDTH    16
static const unsigned char PROGMEM playerGraphic[] =
{ 0x00, 0x00, 0x10, 0x00, 0x28, 0x00, 0x24, 0x00, 0xC2, 0x00, 0xC1, 0x80, 0x20, 0x78, 0x10, 0x07,
0x10, 0x07, 0x20, 0x78, 0xC1, 0x80, 0xC2, 0x00, 0x24, 0x00, 0x28, 0x00, 0x10, 0x00, 0x00, 0x00 };

#define ENEMY_HEIGHT 8
#define ENEMY_WIDTH 8
static const unsigned char PROGMEM enemyGraphic[] =
{
  0x00, 0x3E, 0x64, 0xC7, 0xC7, 0x64, 0x3E, 0x00
};



#define HEART_HEIGHT 8
#define HEART_WIDTH 8

static const unsigned char PROGMEM heartGraphic[] =
{
  0x66, 
  0x7E , 
  0xDB, 
  0xFF, 
  0xDB, 
  0x66, 
  0x3C, 
  0x18  
};

#define GUN_HEIGHT 8
#define GUN_WIDTH 8

static const unsigned char PROGMEM gunGraphic[] =
{
  0x7E, 
  0xC3 , 
  0x99, 
  0xA5, 
  0xA5, 
  0x99, 
  0xC3, 
  0x7E  
};

#define METEOR_HEIGHT 8
#define METEOR_WIDTH 8

static const unsigned char PROGMEM meteorGraphic[] =
{
  B00000000,
  B01011010,
  B00111100,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};









#define OLED_RESET      4 
const int POT_PIN = A0; 

const int BUTTON_PIN = 5;

const int UP_BUTTON_PIN = 10;
const int SELECT_BUTTON_PIN = 11;
const int DOWN_BUTTON_PIN = 12;

const int LED_PIN_1 = 2;
const int LED_PIN_2 = 3;
const int LED_PIN_3 = 4;

const int LED_PIN_4 = 6;
const int LED_PIN_5 = 7;
const int LED_PIN_6 = 8;

const int BUZZER_PIN = 9;

const int LDR_PIN = A1;

const int DIGIT1_a = 36;
const int DIGIT1_b = 37;
const int DIGIT1_c = 38;
const int DIGIT1_d = 39;
const int DIGIT1_e = 40;
const int DIGIT1_f = 41;
const int DIGIT1_g = 42;


const int DIGIT2_a = 29;
const int DIGIT2_b = 30;
const int DIGIT2_c = 31;
const int DIGIT2_d = 32;
const int DIGIT2_e = 33;
const int DIGIT2_f = 34;
const int DIGIT2_g = 35;


const int DIGIT3_a = 22;
const int DIGIT3_b = 23;
const int DIGIT3_c = 24;
const int DIGIT3_d = 25;
const int DIGIT3_e = 26;
const int DIGIT3_f = 27;
const int DIGIT3_g = 28;


int displayPins[3][7]={

  {22,23,24,25,26,27,28},
  {29,30,31,32,33,34,35},
  {36,37,38,39,40,41,42},
};

byte digits[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

int healthLedPins[] = {LED_PIN_1, LED_PIN_2,LED_PIN_3};


int gunLedPins[] = {LED_PIN_4, LED_PIN_5, LED_PIN_6};

int gunCount = 3;

int buzzer_frequency = 1000;
int buzzer_duration = 50;

int difficultyLevel = 1;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


static unsigned long lastShot = 0; //ms
static unsigned long shotDelay;


#define MIN_Y 0
#define MAX_Y 50
#define MIN_X 0
#define MAX_X 112

class Player {
  private:
    int _x;
    int _y;
    int _score;
    int _lives;
  public:
    Player(int x, int y, int lives) {
      this->_x=x;
      this->_y=y;
      this->_lives=lives;
      this->_score = 0;
    }

    void Move(int dirY) {
      
      _y += dirY;
  
      
      _y = constrain(_y, MIN_Y, MAX_Y); 
    }

    void draw() {
      display.drawBitmap(_x, _y, playerGraphic, SPACESHIP_WIDTH, SPACESHIP_HEIGHT, WHITE);
    }
    void drawBlackPlayer() {
      display.drawBitmap(_x, _y, playerGraphic, SPACESHIP_WIDTH, SPACESHIP_HEIGHT, BLACK);
    }

    int get_lives() {
      return _lives;
    }

    int get_score() {
      return _score;
    }

    int get_x() {
      return _x;
    }

    int get_y() {
      return _y;
    }

    void add_to_score(int val) {
      _score += val; 
    }

    void remove_lives(int val) {
      _lives -= val;
      
    }
     void add_lives(int val) {
      _lives += val;
      
    }

    void reset() {
      _lives = 3;
      _score = 0;
      _x = 0;
      _y = SCREEN_HEIGHT / 2;
    }
};


struct bounds {
  int x;
  int y;
  int w;
  int h;
};

struct bullet {
  int x;
  int y;
  int dir = 1;
  int moveSpeed;
  int maxX = SCREEN_WIDTH;
  bool live;
  bounds boundingBox;

  void Draw() {
    display.drawCircle(x, y, 3, WHITE);
  }
  void DrawBulletBlack(){
    display.drawCircle(x, y, 3, BLACK);
  }

  void Move() {
    if (x >=maxX) {
      live = false;
    }
    x += dir * moveSpeed;
    calc_bounds();
  }

  void calc_bounds() {
    boundingBox.x = x;
    boundingBox.y = y;
    boundingBox.w = ENEMY_WIDTH;
    boundingBox.h = ENEMY_HEIGHT;
  }
};

struct enemy {
  int x;
  int y;
  int dir = -1;
  int moveSpeed;
  bounds boundingBox;

  void Move() {
    x += dir * moveSpeed;
    calc_bounds();
  }

  void Draw() {
    display.drawBitmap(x, y, enemyGraphic, ENEMY_WIDTH, ENEMY_HEIGHT, WHITE);
  }
  void DrawEnemyBlack(){
    display.drawBitmap(x, y, enemyGraphic, ENEMY_WIDTH, ENEMY_HEIGHT, BLACK);
  }

  void calc_bounds() {
    boundingBox.x = x;
    boundingBox.y = y;
    boundingBox.w = ENEMY_WIDTH;
    boundingBox.h = ENEMY_HEIGHT;
  }
};

struct meteor {
  int x;
  int y;
  int dir = -1;
  int moveSpeed;
  bounds boundingBox;

  void Move() {
    x += dir * moveSpeed;
    calc_bounds();
  }

  void Draw() {
    display.drawBitmap(x, y, meteorGraphic, METEOR_WIDTH, METEOR_HEIGHT, WHITE);
  }
  void DrawMeteorBlack(){
    display.drawBitmap(x, y, meteorGraphic, METEOR_WIDTH, METEOR_HEIGHT, BLACK);
  }

  void calc_bounds() {
    boundingBox.x = x;
    boundingBox.y = y;
    boundingBox.w = METEOR_WIDTH;
    boundingBox.h = METEOR_HEIGHT;
  }
};



struct heart {
  int x;
  int y;
  int dir = -1;
  int moveSpeed;
  bounds boundingBox;

  void Move() {
    x += dir * moveSpeed;
    calc_bounds();
  }

  void Draw() {
    display.drawBitmap(x, y, heartGraphic, HEART_WIDTH, HEART_HEIGHT, WHITE);
  }
  void DrawHeartBlack(){
    display.drawBitmap(x, y, heartGraphic, HEART_WIDTH, HEART_HEIGHT, BLACK);
  }

  void calc_bounds() {
    boundingBox.x = x;
    boundingBox.y = y;
    boundingBox.w = HEART_WIDTH;
    boundingBox.h = HEART_HEIGHT;
  }
};

struct gun {
  int x;
  int y;
  int dir = -1;
  int moveSpeed;
  bounds boundingBox;

  void Move() {
    x += dir * moveSpeed;
    calc_bounds();
  }

  void Draw() {
    display.drawBitmap(x, y, gunGraphic, GUN_WIDTH, GUN_HEIGHT, WHITE);
  }
  void DrawGunBlack(){
    display.drawBitmap(x, y, gunGraphic, GUN_WIDTH, GUN_HEIGHT, BLACK);
  }

  void calc_bounds() {
    boundingBox.x = x;
    boundingBox.y = y;
    boundingBox.w = GUN_WIDTH;
    boundingBox.h = GUN_HEIGHT;
  }
};




const int maxBullets = 5;
const int maxEnemies = 7;

const int maxMeteors=3;

const int maxHearts = 1;

const int maxGuns = 1;

int bulletCount;
bullet bullets[maxBullets];

int enemyCount;
enemy enemies[maxEnemies];
int enemySpeed;
int enemySpawnChance;

int meteorCount;
meteor meteors[maxMeteors];
int meteorSpeed;
int meteorSpawnChance;

int heartCount;
heart heartes[maxHearts];
int heartSpeed;
int heartSpawnChance;

int gunexCount;
gun guns[maxGuns];
int gunSpeed;
int gunSpawnChance;


Player player = Player(0, SCREEN_HEIGHT / 2, 3);
int i;
int j;

void initialSetup() {
  bulletCount = 0;
  heartCount=0;
  heartSpeed=1;
  heartSpawnChance=14;
  gunexCount=0;
  gunSpeed=1;
  gunSpawnChance=14;
  enemyCount = 0;
  enemySpeed = 1;
  enemySpawnChance = 92;
  meteorCount=0;
  meteorSpeed=1;
  meteorSpawnChance=47;


  shotDelay = 200;
  lastShot = 0;

  for(i = 0; i < maxBullets; i++) {
    bullets[i].live = false;
  }
  
  for (i = 0; i < maxEnemies; i++) {
    enemies[i].x = 0;
    enemies[i].y = SCREEN_HEIGHT;
  }
   for (i = 0; i < maxHearts; i++) {
    heartes[i].x = 0;
    heartes[i].y = SCREEN_HEIGHT;
  }
  for (i = 0; i < maxGuns; i++) {
    guns[i].x = 0;
    guns[i].y = SCREEN_HEIGHT;
  }
  for (i = 0; i < maxMeteors; i++) {
    meteors[i].x = 0;
    meteors[i].y = SCREEN_HEIGHT;
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.stopscroll();
}

void setup() {
  initialSetup();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(UP_BUTTON_PIN,INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN,INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN,INPUT_PULLUP);
  


  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(DIGIT1_a, OUTPUT);
  pinMode(DIGIT1_b, OUTPUT);
  pinMode(DIGIT1_c, OUTPUT);
  pinMode(DIGIT1_d, OUTPUT);
  pinMode(DIGIT1_e, OUTPUT);
  pinMode(DIGIT1_f, OUTPUT);
  pinMode(DIGIT1_g, OUTPUT);
 

  pinMode(DIGIT2_a, OUTPUT);
  pinMode(DIGIT2_b, OUTPUT);
  pinMode(DIGIT2_c, OUTPUT);
  pinMode(DIGIT2_d, OUTPUT);
  pinMode(DIGIT2_e, OUTPUT);
  pinMode(DIGIT2_f, OUTPUT);
  pinMode(DIGIT2_g, OUTPUT);


  pinMode(DIGIT3_a, OUTPUT);
  pinMode(DIGIT3_b, OUTPUT);
  pinMode(DIGIT3_c, OUTPUT);
  pinMode(DIGIT3_d, OUTPUT);
  pinMode(DIGIT3_e, OUTPUT);
  pinMode(DIGIT3_f, OUTPUT);
  pinMode(DIGIT3_g, OUTPUT);

  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, HIGH);
  digitalWrite(LED_PIN_3, HIGH);

  digitalWrite(LED_PIN_4, HIGH);
  digitalWrite(LED_PIN_5, HIGH);
  digitalWrite(LED_PIN_6, HIGH);

  Serial.begin(9600);


  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Başarısız"));
    for(;;); 
  }
}

void loop() {
  menuLoop();
}



int readValue(int pin, int rangeLow, int rangeHigh, int newRangeLow, int newRangeHigh) {
  int val = analogRead(pin);
  
  if (val >= 480 && val <= 530){
    val = 512;
  }
  return map(val, rangeLow, rangeHigh, newRangeLow, newRangeHigh);
}

// Bullet Things
void createNewBullet() {
  bullets[bulletCount].x = player.get_x() + SPACESHIP_WIDTH;
  bullets[bulletCount].y = player.get_y() + (SPACESHIP_HEIGHT / 2);
  bullets[bulletCount].moveSpeed = 5;
  bullets[bulletCount].live = true;
  bullets[bulletCount].calc_bounds();
  bulletCount++;
  if (bulletCount > maxBullets - 1)
    bulletCount = 0;
}

void createNewEnemy() {
  if (enemies[enemyCount].x > 0) {
    return;
  }
  enemies[enemyCount].x = SCREEN_WIDTH - SPACESHIP_WIDTH / 2;
  enemies[enemyCount].y = random(MIN_Y, 43);
  enemies[enemyCount].moveSpeed = enemySpeed;
  enemies[enemyCount].calc_bounds();
  enemyCount++;
  if (enemyCount > maxEnemies - 1)
    enemyCount = 0;
}

void moveEnemies() {
  for(int i = 0; i < maxEnemies; i++) {
    enemies[i].Move();
  }
}

void createNewHeart() {
  if (heartes[heartCount].x > 0) {
    return;
  }
  heartes[heartCount].x = SCREEN_WIDTH - SPACESHIP_WIDTH / 2;
  heartes[heartCount].y = random(25, 32);
  heartes[heartCount].moveSpeed = heartSpeed;
  heartes[heartCount].calc_bounds();
  heartCount++;
  if (heartCount > maxHearts - 1)
    heartCount = 0;
}
void moveHeartes() {
  for(int i = 0; i < maxHearts; i++) {
    heartes[i].Move();
  }
}

void createNewMeteors() {
  if (meteors[meteorCount].x > 0) {
    return;
  }
  meteors[meteorCount].x = SCREEN_WIDTH - SPACESHIP_WIDTH / 2;
  meteors[meteorCount].y = random(0, 10);
  meteors[meteorCount].moveSpeed = heartSpeed;
  meteors[meteorCount].calc_bounds();
  meteorCount++;
  if (meteorCount > maxMeteors - 1)
    meteorCount = 0;
}
void moveMeteors() {
  for(int i = 0; i < maxMeteors; i++) {
    meteors[i].Move();
  }
}

void createNewGun() {
  if (guns[gunexCount].x > 0) {
    return;
  }
  guns[gunexCount].x = SCREEN_WIDTH - SPACESHIP_WIDTH / 2;
  guns[gunexCount].y = random(35, 45);
  guns[gunexCount].moveSpeed = gunSpeed;
  guns[gunexCount].calc_bounds();
  gunexCount++;
  if (gunexCount > maxGuns - 1)
    gunexCount = 0;
}
void moveGuns() {
  for(int i = 0; i < maxGuns; i++) {
    guns[i].Move();
  }
}

void moveBullets() {
  for(i = 0; i < maxBullets; i++) {
    if (bullets[i].live){
      bullets[i].Move();
    }
  }
}



bounds getPlayerBounds() {
  bounds retBounds{player.get_x(), player.get_y(), SPACESHIP_WIDTH, SPACESHIP_HEIGHT};
  return retBounds;
}

// Collision checking
bool hasCollided(bounds boundsOne, bounds boundsTwo) {
  bool collided = false;
  int xMin1, xMax1, yMin1, yMax1;
  int xMin2, xMax2, yMin2, yMax2;

  xMin1 = boundsOne.x;
  xMax1 = boundsOne.x + boundsOne.w;
  yMin1 = boundsOne.y;
  yMax1 = boundsOne.y + boundsOne.h;

  xMin2 = boundsTwo.x;
  xMax2 = boundsTwo.x + boundsTwo.w;
  yMin2 = boundsTwo.y;
  yMax2 = boundsTwo.y + boundsTwo.h;

  return (xMin1 < xMax2 && xMin2 < xMax1 && yMin1 < yMax2 && yMin2 < yMax1);
}


// Rendering methods
void drawEnemies() {
  for(i = 0; i < maxEnemies; i++) {
    if (enemies[i].y > MIN_Y)
      enemies[i].Draw();
  }  
}

void drawEnemiesBlack(){
  for(i = 0; i < maxEnemies; i++) {
    if (enemies[i].y > MIN_Y)
      enemies[i].DrawEnemyBlack();
  }  
}

void drawHeartes() {
  for(i = 0; i < maxHearts; i++) {
    if (heartes[i].y > MIN_Y)
      heartes[i].Draw();
  }  
}
void drawHeartesBlack() {
  for(i = 0; i < maxHearts; i++) {
    if (heartes[i].y > MIN_Y)
      heartes[i].DrawHeartBlack();
  }  
}

void drawGuns() {
  for(i = 0; i < maxGuns; i++) {
    if (guns[i].y > MIN_Y)
      guns[i].Draw();
  }  
}
void drawGunsBlack() {
  for(i = 0; i < maxGuns; i++) {
    if (guns[i].y > MIN_Y)
      guns[i].DrawGunBlack();
  }  
}





void drawBullets() {
  for(i = 0; i < maxBullets; i++) {
    if (bullets[i].live && bullets[i].x >= SPACESHIP_WIDTH) {
      bullets[i].Draw();
    }  
  }
}

void drawBulletsBlack() {
  for(i = 0; i < maxBullets; i++) {
    if (bullets[i].live && bullets[i].x >= SPACESHIP_WIDTH) {
      bullets[i].DrawBulletBlack();
    }  
  }
}

void drawMeteors() {
  for(i = 0; i < maxBullets; i++) {
    if (meteors[i].y > MIN_Y)
      meteors[i].Draw();
    }  
  }


void drawMeteorsBlack() {
  for(i = 0; i < maxBullets; i++) {
    if (meteors[i].y > MIN_Y)
      meteors[i].DrawMeteorBlack();
    }  
  }





void buzz(int pin, int frequency) {
  tone(pin, frequency, 100);
  delay(100);
  noTone(pin);
}


/*void displayNumber(int display,int number) {
for (int i = 0; i < 7; i++) {
digitalWrite(displayPins[display][i], digits[number][i]);
}*/



void gameLoop() {
  while (true) {

    
    
      
      display.clearDisplay();

      // Should spawn enemy?
      if (random(100) > enemySpawnChance){
        createNewEnemy();
      }
      if (random(100) > heartSpawnChance){
        createNewHeart();
      }
      if (random(100) > gunSpawnChance){
        createNewGun();
      }
      if (random(100) > meteorSpawnChance){
        createNewMeteors();
      }
      int potValue = analogRead(POT_PIN);
      Serial.println(potValue);
      

      int lightLevel = analogRead(LDR_PIN);

      if (lightLevel > 512) {
        display.fillScreen(WHITE);
        player.drawBlackPlayer();
        drawEnemiesBlack();
        drawBulletsBlack();
        drawHeartesBlack(); 
        drawGunsBlack();
        drawMeteorsBlack();
      }
      else {
        display.fillScreen(BLACK);
        player.draw();
        drawBullets();
        drawEnemies();
        drawHeartes();
        drawGuns();
        drawMeteors();
        
      } 

        
      
      int newY = readValue(POT_PIN, 0, 1023, 10, -10);
      
      player.Move(newY);

      if (digitalRead(BUTTON_PIN) == 0) {
        if (gunCount > 0) {
        gunCount--;
        digitalWrite(gunLedPins[gunCount], LOW);
          if (millis() >= lastShot) {
          lastShot = millis() + shotDelay;;
          createNewBullet();
          
        
      }
      }
      }

      moveBullets();
      moveEnemies();
      moveHeartes();
      moveGuns();
      moveMeteors();
      

      // çarpışma
      for(i = 0; i < maxEnemies; i++){
        if(enemies[i].x > 0) {
          if (hasCollided(enemies[i].boundingBox, getPlayerBounds())){
            buzz(BUZZER_PIN, 1000);
            enemies[i].x = 0;
            enemies[i].y = 0;
            player.remove_lives(1);
            digitalWrite(healthLedPins[player.get_lives()], LOW);
                       
            if (player.get_lives() < 0) {
              
              player.reset();
              menuLoop();
              initialSetup();
              continue;
            }
          }
        }
        
        for(j = 0; j < maxBullets; j++) {
          if(bullets[j].live && enemies[i].x > 0){
            if (hasCollided(bullets[j].boundingBox, enemies[i].boundingBox)){
              enemies[i].x = 0;
              enemies[i].y = 0;
              bullets[j].live = false;

             
            }
          }
        }
        
      }

      for(i = 0; i < maxMeteors; i++){
        if(meteors[i].x > 0) {
          if (hasCollided(meteors[i].boundingBox, getPlayerBounds())){
            buzz(BUZZER_PIN, 1000);
            meteors[i].x = 0;
            meteors[i].y = 0;
            player.remove_lives(1);
            digitalWrite(healthLedPins[player.get_lives()], LOW);
                       
            if (player.get_lives() < 0) {
              
              player.reset();
              menuLoop();
              initialSetup();
              continue;
            }
          }
        }
        
        for(j = 0; j < maxBullets; j++) {
          if(bullets[j].live && meteors[i].x > 0){
            if (hasCollided(bullets[j].boundingBox, meteors[i].boundingBox)){
              meteors[i].x = 0;
              meteors[i].y = 0;
              bullets[j].live = false;

             
            }
          }
        }
        
      }






      for(i = 0; i < maxHearts; i++){
        if(heartes[i].x > 0) {
          if (hasCollided(heartes[i].boundingBox, getPlayerBounds())){
            
            heartes[i].x = 0;
            heartes[i].y = 0;
            player.add_lives(1);
            digitalWrite(healthLedPins[player.get_lives()], HIGH);
                       
            if (player.get_lives() < 0) {
              
              player.reset();
              menuLoop();
              initialSetup();
              continue;
            }
          }
        }}
          for(i = 0; i < maxGuns; i++){
        if(guns[i].x > 0) {
          if (hasCollided(guns[i].boundingBox, getPlayerBounds())){
            
            guns[i].x = 0;
            guns[i].y = 0;
            gunCount++;
            digitalWrite(gunLedPins[gunCount], HIGH);
                       
            
          }
        }}

      
        
      
      display.display();
      delay(50);
  }
}

  


void menuLoop() {
  while (true) {
    drawMenu();
    if (digitalRead(UP_BUTTON_PIN) == LOW) {
      if (difficultyLevel < 2) {
        difficultyLevel++;
      }
    }
    if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
      if (difficultyLevel > 1) {
        difficultyLevel--;
      }
    }
    if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
      gameLoop();
      
    }
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Main Menu");
  display.setCursor(0, 10);
  display.setCursor(0, 20);
  display.print("Difficulty: ");
  display.println(difficultyLevel);
  display.display();
}

void displayNumber(int display,int number) {
for (int i = 0; i < 7; i++) {
digitalWrite(displayPins[display][i], digits[number][i]);
}
}