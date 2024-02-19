#include "game-private.h"

// Global variables
// =================================================
static enum States state = STOPPED;

static struct Puck *puck;
static struct Disk *disk1, *disk2;

static unsigned int highScore1 = 0;
static unsigned int highScore2 = 0;

// External modifications of the FSM
// =================================================
void gameInit()
{
    state = STOPPED;

    puck = malloc(sizeof(struct Puck));
    disk1 = malloc(sizeof(struct Disk));
    disk2 = malloc(sizeof(struct Disk));

    highScore1 = 0;
    highScore2 = 0;

    resetGame();

    drawGame();
    keyBoardStart();
}

void gameStart()
{
    resetGame();
    keyBoardStop();

    highScore1 = 0;
    highScore2 = 0;

    state = PLAYING;
    graphicCleared();
}

void gameStop()
{
    resetGame();
    keyBoardStop();

    state = STOPPED;
    graphicCleared();
}

enum States gameState()
{
    return state;
}

// FSM
// =================================================
void gameUpdate()
{
    enum States oldState = state;
    switch (state)
    {
    case STOPPED:
        state = updateStopped();
        break;

    case PLAYING:
    {
        struct InputEvent keyInput = keyBoardPoll();
        state = updatePlaying(&keyInput);

        if (state != PLAYING)
        {
            resetGame();
            keyBoardStart();
        }
    }
    break;

    case WIN:
        state = updateWin();
        break;

    case LOST:
        state = updateLost();
        break;
    }

    if (oldState != state)
    {
        drawGame();
    }

    if (state == PLAYING)
    {
        drawGame();
    }
}

void resetGame()
{
    puck->x = FIELD_WIDTH / 2 - PUCK_WIDTH / 2;
    puck->y = FIELD_HEIGHT / 2 - PUCK_HEIGHT / 2;
    puck->vx = 0;
    puck->vy = 0;

    disk1->x = 0;
    disk1->goalX = disk1->x;
    disk1->y = FIELD_HEIGHT / 2 - DISK_HEIGHT / 2;
    disk1->goalY = disk1->y;
    disk1->vx = 0;
    disk1->vy = 0;
    disk1->angle = 0;

    disk2->x = FIELD_WIDTH - DISK_WIDTH;
    disk2->goalX = disk2->x;
    disk2->y = FIELD_HEIGHT / 2 - DISK_HEIGHT / 2;
    disk2->goalY = disk2->y;
    disk2->vx = 0;
    disk2->vy = 0;
    disk2->angle = 180;
}

enum States updateStopped()
{
    return STOPPED;
}

enum States updateWin()
{
    return WIN;
}

enum States updateLost()
{
    return LOST;
}

enum States updatePlaying(struct InputEvent *inputEvent)
{
    if (puck->y <= FIELD_HEIGHT / 2 + SLOT_HEIGHT / 2 - PUCK_HEIGHT && puck->y >= FIELD_HEIGHT / 2 - SLOT_HEIGHT / 2)
    {
        if (puck->x <= SLOT_WIDTH)
        {
            highScore2++;
            resetGame();
            return PLAYING;
        }

        if (puck->x + PUCK_WIDTH >= FIELD_WIDTH - SLOT_WIDTH)
        {
            highScore1++;
            resetGame();
            return PLAYING;
        }

        if (highScore1 >= 3)
        {
            soundWin();
            return WIN;
        }

        if (highScore2 >= 3)
        {
            soundLost();
            return LOST;
        }
    }

    interactionPuckDisk(puck, disk1);
    interactionPuckDisk(puck, disk2);

    if (inputEvent->touchPressed)
    {
        changeGoalDiskUser(inputEvent->x, inputEvent->y);
    }

    if (inputEvent->keyPressed)
    {
        switch (inputEvent->key)
        {
        case KEY_UP:
            stepDiskUser(0, -DISK_STEP);
            break;

        case KEY_DOWN:
            stepDiskUser(0, DISK_STEP);
            break;

        case KEY_RIGHT:
            stepDiskUser(DISK_STEP, 0);
            break;

        case KEY_LEFT:
            stepDiskUser(-DISK_STEP, 0);
            break;
        }
    }

    makeDecision();

    moveDisk(disk1, DISK_VELOCITY_USER);
    moveDisk(disk2, DISK_VELOCITY_BOT);

    movePuck(puck);

    return PLAYING;
}

// Physics functions for movement / interactions
// =================================================
void interactionPuckDisk(struct Puck *puck, struct Disk *disk)
{
    // Checking if there is a rectangle boundary interaction before long calculations
    if (puck->x > disk->x + DISK_WIDTH || puck->x + PUCK_WIDTH < disk->x || puck->y > disk->y + DISK_HEIGHT || puck->y + PUCK_HEIGHT < disk->y)
    {
        return;
    }

    // Checking if there is a circular boundary interaction
    double puck_center_x = puck->x + PUCK_WIDTH / 2;
    double puck_center_y = puck->y + PUCK_HEIGHT / 2;

    double disk_center_x = disk->x + DISK_WIDTH / 2;
    double disk_center_y = disk->y + DISK_HEIGHT / 2;

    unsigned int distance = (puck_center_x - disk_center_x) * (puck_center_x - disk_center_x) + (puck_center_y - disk_center_y) * (puck_center_y - disk_center_y);
    if (distance <= (PUCK_RADIUS + DISK_RADIUS) * (PUCK_RADIUS + DISK_RADIUS))
    {
        double normal_x = puck_center_x - disk_center_x;
        double normal_y = puck_center_y - disk_center_y;

        double normal_magnitude = sqrt(pow(normal_x, 2) + pow(normal_y, 2));

        normal_x /= normal_magnitude;
        normal_y /= normal_magnitude;

        double rel_velocity_x = puck->vx - disk->vx;
        double rel_velocity_y = puck->vy - disk->vy;

        double impact_velocity = rel_velocity_x * normal_x + rel_velocity_y * normal_y;

        // Check there approaching each other
        if (impact_velocity < 0)
        {
            puck->vx -= (1 + COLLISION_FACTOR) * impact_velocity * normal_x;
            puck->vy -= (1 + COLLISION_FACTOR) * impact_velocity * normal_y;
        }
    }
}

void movePuck(struct Puck *puck)
{
    int next_x = round((double)puck->x + puck->vx);
    int next_y = round((double)puck->y + puck->vy);

    puck->x = next_x;
    puck->y = next_y;

    if (next_x <= 0)
    {
        puck->x = 0;
        puck->vx *= -1;
    }

    if (next_x >= (int)(FIELD_WIDTH - PUCK_WIDTH))
    {
        puck->x = FIELD_WIDTH - PUCK_WIDTH;
        puck->vx *= -1;
    }

    if (next_y <= 0)
    {
        puck->y = 0;
        puck->vy *= -1;
    }

    if (next_y >= (int)(FIELD_HEIGHT - PUCK_HEIGHT))
    {
        puck->y = FIELD_HEIGHT - PUCK_HEIGHT;
        puck->vy *= -1;
    }

    puck->vx -= PUCK_FRICTION * puck->vx;
    puck->vy -= PUCK_FRICTION * puck->vy;

    if (puck->vx > 0)
    {
        puck->angle += -4 * sqrt(puck->vx * puck->vx + puck->vy * puck->vy);
    }
    else
    {
        puck->angle += 4 * sqrt(puck->vx * puck->vx + puck->vy * puck->vy);
    }
}

void moveDisk(struct Disk *disk, const unsigned int velocity)
{
    double distance_x = (double)disk->goalX - (double)disk->x - DISK_WIDTH / 2;
    double distance_y = (double)disk->goalY - (double)disk->y - DISK_HEIGHT / 2;

    double distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));

    if (distance <= DISK_PRECISION)
    {
        if (disk->vx != 0 && disk->vy != 0)
        {
            disk->angle = calculateAngle(disk->vx, -disk->vy);
        }

        disk->vx = 0;
        disk->vy = 0;

        return;
    }

    disk->vx = velocity * (distance_x / distance);
    disk->vy = velocity * (distance_y / distance);

    disk->angle = calculateAngle(disk->vx, disk->vy);

    disk->x += disk->vx;
    disk->y += disk->vy;
}

// Bot functions
// =================================================
void makeDecision()
{
    unsigned int goalX;
    unsigned int goalY;
    // If in the bot-half's field attack
    if (puck->x >= FIELD_WIDTH / 2 - PUCK_WIDTH / 2)
    {
        if (puck->x >= disk2->x)
        {
            goalX = puck->x + PUCK_WIDTH;
        }
        else
        {
            goalX = puck->x;
        }

        if (puck->y >= disk2->y)
        {
            goalY = puck->y + PUCK_HEIGHT;
        }
        else
        {
            goalY = puck->y;
        }
    }
    // If not protect the slot
    else
    {
        goalX = FIELD_WIDTH - DISK_WIDTH;
        goalY = FIELD_HEIGHT / 2 - DISK_HEIGHT / 2;
    }

    // Boundaries checking
    if (goalX <= FIELD_WIDTH / 2)
    {
        goalX = FIELD_WIDTH / 2;
    }

    if (goalX >= FIELD_WIDTH - DISK_WIDTH)
    {
        goalX = FIELD_WIDTH;
    }

    if (goalY >= FIELD_HEIGHT - DISK_HEIGHT)
    {
        goalY = FIELD_HEIGHT;
    }

    disk2->goalX = goalX;
    disk2->goalY = goalY;
}

// User functions
// =================================================
void changeGoalDiskUser(unsigned int pos_x, unsigned pos_y)
{
    int newGoalX = pos_x - (SCREEN_WIDTH - FIELD_WIDTH) / 2;
    int newGoalY = pos_y - (SCREEN_HEIGHT - FIELD_HEIGHT) / 2;

    if (newGoalX < 0)
    {
        newGoalX = 0;
    }

    if (newGoalY < 0)
    {
        newGoalY = 0;
    }

    if (newGoalX > FIELD_WIDTH / 2)
    {
        newGoalX = FIELD_WIDTH / 2;
    }

    if (newGoalY > FIELD_HEIGHT - DISK_HEIGHT / 2)
    {
        newGoalY = FIELD_HEIGHT - DISK_HEIGHT / 2;
    }

    disk1->goalX = newGoalX;
    disk1->goalY = newGoalY;
}

void stepDiskUser(int step_x, int step_y)
{
    if ((step_x < 0 && disk1->goalX >= abs(step_x)) || (step_x > 0 && disk1->goalX <= FIELD_WIDTH / 2 - DISK_WIDTH / 2 - abs(step_x)))
    {
        disk1->goalX += step_x;
    }

    if ((step_y < 0 && disk1->goalY >= abs(step_y)) || (step_y > 0 && disk1->goalY <= FIELD_HEIGHT - DISK_HEIGHT - abs(step_y)))
    {
        disk1->goalY += step_y;
    }
}

// Graphics
// =================================================
void drawGame()
{
    graphicDrawPuck(puck->x, puck->y, puck->angle);
    graphicDrawDisk1(disk1->x, disk1->y, -disk1->angle);
    graphicDrawDisk2(disk2->x, disk2->y, 180 - disk2->angle);
    graphicDrawHighScore(highScore2, highScore1);

    switch (state)
    {

    case LOST:
        graphicShowLost();
        break;

    case WIN:
        graphicShowWin();
        break;
    }
}

// Internal miscellaneous functions
// =================================================
int calculateAngle(double vx, double vy)
{
    return round(atan2(vy, vx) * 180 / M_PI);
}