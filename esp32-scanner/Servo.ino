
// ------------------------------------------------------------ //
// Servo.ino
// ------------------------------------------------------------ //

#include <ESP32Servo.h>

Servo servoX;
Servo servoY;

#define SERVO_X_MIN 0
#define SERVO_X_MAX 120
#define SERVO_Y_MIN 0
#define SERVO_Y_MAX 120

#define NUMBER_X 10
#define NUMBER_Y 10

#define SERVO_X_PIN 13
#define SERVO_Y_PIN 12

#define SERVO_MOVE_TIME 50

int servoPosX = 0;
int servoPosY = 0;
int servoCount = 0;
bool servoStart = false;

void servo_start()
{
    servoPosX = 0;
    servoPosY = 0;
    servoCount = 0;
    servoStart = true;
}

int minPosX = 0;
int maxPosX = 180;
int minPosY = 70;
int maxPosY = 120;

int qtdTotalCoordenadas = 1000;
int scanIncrement = 1;

int lastPosX = 0;
int lastPosY = 0;
int radius = 0;
int lastRadius = 0;
boolean scanning = true;
boolean scanDirection = false;
int posX = (maxPosX + minPosX) / 2;
int posY = (maxPosY + minPosY) / 4;
float pi = 3.14159265;
float deg2rad = pi / 180.0;

void servo_setup()
{
    servoX.setPeriodHertz(50);
    servoX.attach(SERVO_X_PIN, 500, 2400);

    servoY.setPeriodHertz(50);
    servoY.attach(SERVO_Y_PIN, 500, 2400);

    servo_move();
    servo_start();
}

void servo_loop()
{
    if (servoStart)
    {
        if (servo_runEvery(SERVO_MOVE_TIME))
        {
            servoCount++;
            if (servoCount > (NUMBER_X + 1) * (NUMBER_Y + 1))
            {
                servoStart = false;
                return;
            }

            String body = "";
            body += ";";
            body += get_servo_x(servoPosX);
            body += "|";
            body += get_servo_x(servoPosY);
            body += "|";
            body += millis() / 1000;
            add_body_now(body);

            Serial.println(String(servoCount-1));
            servoPosX = (servoPosX + 1) % (NUMBER_X + 1);
            if (servoPosX == 0)
                servoPosY = (servoPosY + 1) % (NUMBER_Y + 1);
            servo_move();
        }
    }
}

void servo_move()
{
    int posX = get_servo_x(servoPosX);
    int posY = get_servo_y(servoPosY);
    servoX.write(posX);
    servoY.write(posY);
}

int get_servo_x(int i)
{
    float step = (SERVO_X_MAX - SERVO_X_MIN) / (NUMBER_X);
    int servo = SERVO_X_MIN + step * i;
    return servo;
}

int get_servo_y(int i)
{
    float step = (SERVO_Y_MAX - SERVO_Y_MIN) / (NUMBER_Y);
    int servo = SERVO_Y_MIN + step * i;
    return servo;
}

boolean servo_runEvery(unsigned long interval)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

bool moveServos()
{
    bool moved = false;
    static int lastPosX;
    static int lastPosY;
    int delta = 0;
    if (posX != lastPosX)
    {
        delta += abs(posX - lastPosX);
        servoX.write(posX);
        lastPosX = posX;
        moved = true;
    }
    if (posY != lastPosY)
    {
        delta += abs(posY - lastPosY);
        servoY.write(posY);
        lastPosY = posY;
        moved = true;
    }
    delay(50);
    return moved;
}

void displayPosition()
{
    static int lastPosX;
    static int lastPosY;
    if (posX != lastPosX)
    {
        //Serial.println("X: " + String(posX) + "  ");
        lastPosX = posX;
    }
    if (posY != lastPosY)
    {
        //Serial.println("Y: " + String(posY) + "  ");
        lastPosY = posY;
    }
}
