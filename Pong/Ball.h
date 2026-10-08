
class Ball {
public:
  int r;          //ball size
  int x, y;       // ball position
  int dy, dx;     // ball velocity components
  int count = 2;  //count of collisions for accelerating the ball
  bool active = false;

  int width, height;  // temp to be replaced later

  Adafruit_SSD1306 display;

  void start(int X, int vy, int W, int H, Adafruit_SSD1306 screen) {
    width = W;
    r = floor(width/70);
    x = X + r;
    height = H;
    dy = vy;
    dx = 2;
    display = screen;
  }

  void spawn(int Y, int vy, int direction, int X) {
    y = Y;
    x = X; // playerX + playerW + r +1// change to be more dynmaic later TODO
    dy = vy * direction;
    count = 1;
    active = true;
  }  //spawn ball at desired y value

  void show() {  //draw the ball
    if (active) {
      //show ball at x,y with radius r
      display.fillCircle(x, y, r, SSD1306_WHITE);
    }
  }

  void update(boolean collided) {

    if (collided) {  // if the ball hits a paddle change direction
      count++;
      dx = floor(pow(-1, count));
    }

    //update the position of the ball
    x = x + dx;
    y = y + dy;

    //if ball hits the top or bottom of screen, bounce!
    if (y > height || y < 0) {
      dy = dy * -1;  //invert the slope to bounce next frame
    }

    show();
  }

  int getX() {
    return x;
  }

  int getY() {
    return y;
  }

  int getSlope() {
    if (dx != 0) {
      return floor(dy / dx);
    } else
      return 0;
  }
};  // close ball class

Ball ball;  //initialise an instance of the ball class (initialise a ball)
