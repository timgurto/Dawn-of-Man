#ifndef RGB_H
#define RGB_H

struct RGB{
   int r;
   int g;
   int b;
   RGB(int red, int green, int blue):
      r(red),
      g(green),
      b(blue)
   {}
};

#endif