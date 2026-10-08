# Circuits and Systems Design – Arduino Pong

A handheld Pong game for Arduino, drawn on a 128×64 SSD1306 OLED display. You play against a bot that predicts where the ball will go.

[![Watch the finished console on YouTube](https://img.youtube.com/vi/TQeReYXM4l0/hqdefault.jpg)](https://youtu.be/TQeReYXM4l0)

▶ **[Watch the finished product on YouTube](https://youtu.be/TQeReYXM4l0)**

> **Note:** The video shows the finished console. The code in this repository is the **first iteration**, which uses a wired analog joystick, and does not yet match what's in the video. See [Project status](#project-status) below.

## Project status

| Stage | Input | Connection | In this repo? |
| --- | --- | --- | --- |
| **1. First iteration** | Analog joystick (Y axis + push-button) | Wired to the main Arduino | ✅ Yes (`Pong/`) |
| **2. Planned / finished build** | Gyroscopic sensor (tilt to move) + push-button (serve) | Arduino Nano with Wi-Fi, wireless link to the console | ❌ Not yet |

The plan is to replace the joystick with a handheld controller: a **gyroscopic sensor** that moves the paddle when you tilt it, and a **push-button** to serve, both connected to an **Arduino Nano with Wi-Fi**. The controller sends its input to the console wirelessly, so the player isn't tied to the board by cables. The `Networking/` sketch is the starting point for that wireless link.

The rest of this README describes the code as it stands (the joystick version).

## Repository layout

| Path | Description |
| --- | --- |
| `Pong/` | Main single-player Pong game (player vs. bot), joystick version |
| `Networking/` | Wi-Fi server sketch (Arduino) and a Processing client excerpt, the starting point for the wireless controller |
| `fritzing/Console Wireing.fzz` | Fritzing wiring diagram for the console |

---

## Pong

### Hardware (first iteration)

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

See `fritzing/Console Wireing.fzz` for the full wiring diagram.

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

In the planned build, step 3 becomes tilting the gyroscopic controller, and step 2 becomes pressing the controller's push-button. The serve and movement logic stays the same, but its input arrives over Wi-Fi instead of from the analog pin and interrupt.

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

This folder is a reference sketch based on the Wi-Fi protocol from an earlier buggy project. It is the starting point for the wireless link between the gyroscopic controller and the console. The buggy-specific calls are left in as comments.

### Arduino side (`Networking.ino`, `functions.ino`)

- Uses the `WiFiS3` library, so it currently targets the **Arduino UNO R4 WiFi**. The Wi-Fi library calls will need updating to match the Wi-Fi Nano used for the controller.
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

### Protocol (from the buggy project)

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
