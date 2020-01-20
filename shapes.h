#ifndef SHAPES_H
#define SHAPES_H

struct shape_image {
  uint8_t bitmap[4], w, h;
};

struct shape { // Holds the definition of a tetris shape
  char name;
  struct shape_image rotation[4];
};

struct shape shapes[] = { // The library of shapes
  { .name = 'O',
    .rotation = { // index increases -> shape spins clockwise
     {.bitmap = {B11000000,
                 B11000000}, .w = 2, .h = 2},
     {.bitmap = {B11000000,
                 B11000000}, .w = 2, .h = 2},
     {.bitmap = {B11000000,
                 B11000000}, .w = 2, .h = 2},
     {.bitmap = {B11000000,
                 B11000000}, .w = 2, .h = 2}}},
  { .name = 'I',
    .rotation = {
     {.bitmap = {B11110000}, .w = 4, .h = 1 },
     {.bitmap = {B10000000,
                 B10000000,
                 B10000000,
                 B10000000}, .w = 1, .h = 4 },
     {.bitmap = {B11110000}, .w = 4, .h = 1 },
     {.bitmap = {B10000000,
                 B10000000,
                 B10000000,
                 B10000000}, .w = 1, .h = 4 }}},
  { .name = 'T',
    .rotation = {
     {.bitmap = {B11100000,
                 B01000000}, .w = 3, .h = 2 },
     {.bitmap = {B01000000,
                 B11000000,
                 B01000000}, .w = 2, .h = 3 },
     {.bitmap = {B01000000,
                 B11100000}, .w = 3, .h = 2 },
     {.bitmap = {B10000000,
                 B11000000,
                 B10000000}, .w = 2, .h = 3 }}},
  { .name = 'J',
    .rotation = {
     {.bitmap = {B11100000,
                 B00100000}, .w = 3, .h = 2 },
     {.bitmap = {B01000000,
                 B01000000,
                 B11000000}, .w = 2, .h = 3 },
     {.bitmap = {B10000000,
                 B11100000}, .w = 3, .h = 2 },
     {.bitmap = {B11000000,
                 B10000000,
                 B10000000}, .w = 2, .h = 3 }}},
  { .name = 'L',
    .rotation = {
     {.bitmap = {B11100000,
                 B10000000}, .w = 3, .h = 2 },
     {.bitmap = {B11000000,
                 B01000000,
                 B01000000}, .w = 2, .h = 3 },
     {.bitmap = {B00100000,
                 B11100000}, .w = 3, .h = 2 },
     {.bitmap = {B10000000,
                 B10000000,
                 B11000000}, .w = 2, .h = 3 }}},
  { .name = 'S',
    .rotation = {
     {.bitmap = {B01100000,
                 B11000000}, .w = 3, .h = 2 },
     {.bitmap = {B10000000,
                 B11000000,
                 B01000000}, .w = 2, .h = 3 },
     {.bitmap = {B01100000,
                 B11000000}, .w = 3, .h = 2 },
     {.bitmap = {B10000000,
                 B11000000,
                 B01000000}, .w = 2, .h = 3 }}},
  { .name = 'Z',
    .rotation = {
     {.bitmap = {B11000000,
                 B01100000}, .w = 3, .h = 2 },
     {.bitmap = {B01000000,
                 B11000000,
                 B10000000}, .w = 2, .h = 3 },
     {.bitmap = {B11000000,
                 B01100000}, .w = 3, .h = 2 },
     {.bitmap = {B01000000,
                 B11000000,
                 B10000000}, .w = 2, .h = 3 }}},
};

#endif //SHAPES_H
