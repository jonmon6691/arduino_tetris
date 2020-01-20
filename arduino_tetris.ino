/*************************************************** 
  Tetris!
  
  (c) 2019 Jon Wallace - jonmon6691@gmail.com
 ****************************************************/

#include "Adafruit_LEDBackpack.h"
#include "pRNG.h"

#include "shapes.h"

#define RB (14)
#define DB (15)
#define UB (16)
#define LB (17)

uint8_t db[4] = {0}; // super simple rising edge detector

// For randomly initializing the falling piece
pRNG prng;

// Main device driver
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

// Duplicate driver to stage the falling piece and check for collisions
Adafruit_BicolorMatrix scratch = Adafruit_BicolorMatrix();

uint16_t zone[8] = {0}; // Duplicate frame buffer to save the state of the stationary peices

struct { // Holds the state of the currently falling piece
  struct shape* shape;
  int x, y, color, rot;
} faller, proposal;

void new_shape()
{
  // (re)Initialize the faller with entropy
  uint8_t rnd = prng.getRndByte();
  faller.shape = &shapes[rnd % 7]; // 3ish bits
  faller.rot = prng.getRndByte() % 4;
  faller.x = (rnd >> 3) % (9 - faller.shape->rotation[faller.rot].w); // 3ish bits
  faller.y = -faller.shape->rotation[faller.rot].h;
  faller.color = (rnd >> 6) % 3 + 1; // 2 bits
}

void save_zone()
{
  for (int i = 0; i < 8; i++) zone[i] = matrix.displaybuffer[i];
}

void draw_zone()
{
  for (int i = 0; i < 8; i++) matrix.displaybuffer[i] = zone[i];
}

void clear_zone()
{
  for (int i = 0; i < 8; i++) zone[i] = 0;
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
uint8_t get_zone_px(int x, int y) 
{
  switch (matrix.getRotation()) {
  case 1:
    _swap_int16_t(x, y);
    x = 8 - x - 1;
    break;
  case 2:
    x = 8 - x - 1;
    y = 8 - y - 1;
    break;
  case 3:
    _swap_int16_t(x, y);
    y = 8 - y - 1;
    break;
  }
  return ((zone[y] >> x) & 0b1) | ((zone[y] >> (x+7)) & 0b10);
}
void set_zone_px(int x, int y, uint8_t val)
{
  switch (matrix.getRotation()) {
  case 1:
    _swap_int16_t(x, y);
    x = 8 - x - 1;
    break;
  case 2:
    x = 8 - x - 1;
    y = 8 - y - 1;
    break;
  case 3:
    _swap_int16_t(x, y);
    y = 8 - y - 1;
    break;
  }
  uint16_t mask = ~((1 << (x+8)) | (1 << x));
  zone[y] = (zone[y] & mask) | ((val & 0b10) << (x+7)) | ((val & 0b01) << x);
}

void clear_rows()
{
  int yt = 0;
  for(int y = 0; y < 8; y++) { // rows
    bool filled = true;
    for(int x = 0; x < 8; x++) { //cols
      uint8_t val = get_zone_px(x,7-y);
      if(y != yt) set_zone_px(x, 7-yt, val); 
      if(val == 0) filled = false;
    }
    if(!filled) yt++;
  }
  for(; yt < 8; yt++) {
    for(int x = 0; x < 8; x++) {
      set_zone_px(x, 7-yt, 0);
    }
  }
}

// TODO: Make this respect the getRotation() value
// BUG: When rolling down a shape that was cut off will remain cut off, need a virtual buffer with neg y
void roll_zone()
{
  for (int i = 7; i > 0; i--) zone[i] = zone[i - 1];
  zone [0] = 0;
}

bool would_collide()
{
  // Use a scratch frame buffer to place the peice
  scratch.clear();
  struct shape_image* image = &proposal.shape->rotation[proposal.rot];
  scratch.drawBitmap(
    proposal.x,
    proposal.y, // Simulate next time step to check for collision
    image->bitmap,
    image->w,
    image->h,
    LED_YELLOW); // Set both color bits so either or both can be detected

  // Check if the frame buffers overlap
  for (int i = 0; i < 8; i++)
    if (scratch.displaybuffer[i] & matrix.displaybuffer[i])
      return true;

  // Check for screen boundaries
  if (proposal.y + image->h > 8) return true;
  if (proposal.x < 0) return true;
  if (proposal.x + image->w > 8) return true;

  // No collision
  return false;
}

void draw_piece()
{
  struct shape_image* image = &faller.shape->rotation[faller.rot];
  matrix.drawBitmap(
    faller.x,
    faller.y,
    image->bitmap,
    image->w,
    image->h,
    faller.color);
}

bool move_down()
{
  proposal = faller;
  proposal.y += 1;
  bool move = !would_collide();
  if(move) {
    faller = proposal;
  }
  return move;
}

bool move_left()
{
  proposal = faller;
  proposal.x -= 1;
  bool move = !would_collide();
  if(move) faller = proposal;
  return move;
}

bool move_right()
{
  proposal = faller;
  proposal.x += 1;
  bool move = !would_collide();
  if(move) faller = proposal;
  return move;
}

bool rotate_cw()
{
  proposal = faller;
  proposal.rot = (proposal.rot + 1) % 4;
  bool move = !would_collide();
  if(move) faller = proposal;
  return move;
}

bool rotate_ccw()
{
  proposal = faller;
  proposal.rot = (proposal.rot + 3) % 4;
  bool move = !would_collide();
  if(move) faller = proposal;
  return move;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Tetris!");
  
  //TODO: Add controls
  // Serial.println("Controls: wasd");
  pinMode(RB, INPUT_PULLUP);
  pinMode(DB, INPUT_PULLUP);
  pinMode(UB, INPUT_PULLUP);
  pinMode(LB, INPUT_PULLUP);
  
  matrix.begin(0x70);  // pass in the address
  matrix.setBrightness(15); // Avoid overcurrent problems when running the display through USB

  // Match the way the screen is soldered in
  matrix.setRotation(1);
  scratch.setRotation(1);

  new_shape(); // First piece! Have fun :)
}

void gravity_step()
{
  matrix.clear();  
  draw_zone();

  if (!move_down()) { // Piece has hit the bottom
    draw_piece();
    save_zone(); // Bake the peice into the zone
    clear_rows();
    matrix.clear();
    draw_zone();
    new_shape();
    //roll_zone(); // Optional: Makes a cool screen saver
    if (!move_down()) { // Screen's full
      clear_zone();
    }
  }
  
  // Keep moving the piece down
  draw_piece();
  
  matrix.writeDisplay();
}

void loop()
{
  if(millis() % 600 == 0) {
    gravity_step();
  }

  if(!digitalRead(RB)) {
    if (!db[RB%4]) {
      db[RB%4] = true;
      matrix.clear();  
      draw_zone();
      move_right();
      draw_piece();
      matrix.writeDisplay();
    }
  } else {db[RB%4] = false;}

  if(!digitalRead(LB)) {
    if (!db[LB%4]) {
      db[LB%4] = true;
      matrix.clear();  
      draw_zone();
      move_left();
      draw_piece();
      matrix.writeDisplay();
    }
  } else {db[LB%4] = false;}

  if(!digitalRead(UB)) {
    if (!db[UB%4]) {
      db[UB%4] = 5;
      matrix.clear();  
      draw_zone();
      rotate_ccw();
      draw_piece();
      matrix.writeDisplay();
    }
  } else {if(db[UB%4]>0) db[UB%4]--;}

  
  if(!digitalRead(DB)) {
    if (!db[DB%4]) {
      db[DB%4] = true;
      matrix.clear();  
      draw_zone();
      move_down();
      draw_piece();
      matrix.writeDisplay();
    }
  } else {db[DB%4] = false;}
}
