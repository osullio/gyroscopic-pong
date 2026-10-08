# Circuits and Systems Design – Arduino Pong

A handheld Pong game for Arduino, drawn on a 128×64 SSD1306 OLED display and controlled with an analog joystick. You play against a bot that predicts where the ball will go. The repo also contains a networking sketch, carried over from an earlier buggy project, to use as a starting point for talking to a laptop over Wi-Fi.

## Repository layout

| Path | Description |
| --- | --- |
| `Pong/` | Main single-player Pong game (player vs. bot) |
| `Networking/` | Wi-Fi server sketch (Arduino) and a Processing client excerpt |
| `Console Wireing.fzz` | Fritzing wiring diagram for the console |

---

## Pong

### Hardware

- **Board:** Arduino (needs interrupt support on pin 2)
- **Display:** SSD1306 128×64 OLED, connected over software SPI
- **Input:** Analog joystick (Y axis plus its push-button)

| Signal | Arduino pin |
| --- | --- |
| OLED MOSI | 5 |
| OLED CLK | 6 |
| OLED DC | 11 |
| OLED CS | 12 |
| OLED RESET | 13 |
| Joystick Y axis | A1 |
| Joystick click | 2 (`INPUT_PULLUP`, interrupt) |

See `Console Wireing.fzz` for the full wiring diagram.

### Dependencies

Install these through the Arduino Library Manager:

- `Adafruit GFX Library`
- `Adafruit SSD1306`

### Files

- **`Pong.ino`**: Entry point. Holds the global game state (paddle sizes and positions, ball speed, spawn direction) and contains `setup()` and `loop()`.
- **`functions.ino`**: Game logic: paddle drawing, player and bot movement, collision detection, drawing the map, and spawning the ball.
- **`Ball.h`**: The `Ball` class, which tracks position, velocity, radius, bouncing and drawing.
- **`screen.h`**: OLED pin definitions, the global `display` object and `setupOLED()`.

### How it plays

1. When the game starts, it draws both paddles and a dividing line down the middle of the screen.
2. **Click the joystick** to serve. The click is caught by an interrupt (`joystickClick`), and on the next frame `spawnBall()` launches the ball from the front of your paddle. The ball moves vertically in the direction you last moved (`d`).
3. **Push the joystick up or down** to move your paddle. Readings above 550 move it up and readings below 450 move it down; anything in between is a dead zone. The paddle cannot leave the screen.
4. The ball bounces off the top and bottom edges. When it hits a paddle, its horizontal direction flips.
5. If the ball leaves the left or right edge of the screen, it is removed and you can serve again.

### Game loop

Each frame, `loop()` does the following:

1. Clears the display buffer.
2. Serves a new ball if the joystick was clicked.
3. Updates the player paddle (`movementPlayer`) and the bot paddle (`movementBot`).
4. Draws the centre line and both paddles.
5. Checks for a collision (`collided`) and calls `ball.update(collision)`, which moves and draws the ball.
6. Pushes the buffer to the screen with `display.display()`.

### The bot

`movementBot()` predicts where the ball will cross the bot's paddle using the line equation

```
targetY = slope * (botX - ballX - playerW) + ballY + playerH / 2
```

where `slope = dy / dx` comes from `ball.getSlope()`. The target is clamped to the screen, and the bot moves toward it at `vy` pixels per frame. When no ball is in play, the bot moves back to the centre. The bot can only move at a limited speed, so a steep shot can beat it.

### Scaling

Paddle and ball sizes are calculated from the screen dimensions in `setupPlayers()` and `Ball::start()`, rather than hard-coded:

- Paddle height = `2 * floor(width / 17.5)`
- Paddle width = `0.5 * floor(0.5 * paddleHeight)`
- Paddle inset = `floor(width / 35)`
- Ball radius = `floor(width / 70)`

### Building

Open `Pong/Pong.ino` in the Arduino IDE. The IDE loads `functions.ino` as an extra tab automatically, and the `.h` files are included. Select your board and port, then upload.

---

## Networking

This folder is a reference sketch based on the Wi-Fi protocol from an earlier buggy project. It is meant as a template for adding laptop communication to this project. The buggy-specific calls are left in as comments.

### Arduino side (`Networking.ino`, `functions.ino`)

- Uses the `WiFiS3` library, so it targets the **Arduino UNO R4 WiFi**.
- Joins the network set by `ssid` and `pass`, then starts a `WiFiServer` on **port 5200**.
- On every loop it checks for a connected client (`laptop`):
  - `sendUpdate()` sends `'u'` to announce an update. The buggy version then sent the wheel count, obstacle range, buggy speed and target speed.
  - `checkInput()` reads a new speed from the laptop. If that speed is `0`, it replies with the average wheel count.
  - It also writes `'T'` while connected, which was a leftover connection indicator.

### Processing side (`Networking.pde`)

This file is an excerpt from the Processing GUI client, not a complete sketch. It relies on globals defined elsewhere (`buggy`, `speed`, `dist` and others).

- `getInteger()` waits until `read()` returns something other than `-1`, then returns that value.
- `dataEval(char)` handles the incoming message codes listed below.
- `send()` sends the speed only when it has changed. When it sends `0`, it waits for the wheel count so it can update the distance travelled (`count * π * 6.5`).

### Protocol

| Direction | Message | Meaning |
| --- | --- | --- |
| Arduino → PC | `u` + 4 ints | Update: wheel count, range, current speed, target speed |
| Arduino → PC | `f` | Entered follow mode (GUI light on) |
| Arduino → PC | `n` | Left follow mode (GUI light off) |
| PC → Arduino | speed byte | Set a new speed |
| Arduino → PC | int | Wheel count, sent in reply to a speed of `0` |

> **Note:** The Wi-Fi credentials in `Networking.ino` are placeholders. Change them to match your network before uploading.

---

## Notes

- The bottom of the developer's OLED has dead pixels, and the top band shows in yellow. Two-colour SSD1306 modules have a yellow strip at the top, so this is expected on that hardware.
- When the ball hits a paddle, its horizontal speed is set to ±1 (`pow(-1, count)`). This means the ball moves slower after its first bounce than when it is served (`dx = 2`).
- In `setup()`, `Serial.println("radius is: " + ball.r)` uses pointer arithmetic on the string instead of joining the text and the number. Wrap the text in `String(...)` to print the radius correctly.
