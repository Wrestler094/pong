#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void clearScreen(void);
void printWinner(int leftPlayerPoints, int rightPlayerPoints);
void printField(
    int leftPlayerPoints,
    int rightPlayerPoints,
    int ballPositionH,
    int ballPositionW,
    int leftRocketPosition,
    int rightRocketPosition
);
int isKeyboardPressed(void);

int main(void) {
    int leftRocketPosition = 11;
    int rightRocketPosition = 11;

    int ballPositionH = 11;
    int ballPositionW = 40;
    int ballDirectionH = 1;
    int ballDirectionW = 1;

    int leftPlayerPoints = 0;
    int rightPlayerPoints = 0;

    while (1) {
        if (leftPlayerPoints == 21 || rightPlayerPoints == 21) {
            clearScreen();
            printWinner(leftPlayerPoints, rightPlayerPoints);
            break;
        }

        printField(
            leftPlayerPoints,
            rightPlayerPoints,
            ballPositionH,
            ballPositionW,
            leftRocketPosition,
            rightRocketPosition);

        struct timespec beginTime, currentTime;
        int timeDelta = 0;

        clock_gettime(CLOCK_REALTIME, &beginTime);

        while (timeDelta < 100000000) {
            clock_gettime(CLOCK_REALTIME, &currentTime);
            timeDelta = 1000000000 * (currentTime.tv_sec - beginTime.tv_sec) + \
            (currentTime.tv_nsec - beginTime.tv_nsec);

            if (isKeyboardPressed()) {
                char c;
                c = getchar();

                // Обработка дествий игроков
                if (c == 'a' && leftRocketPosition != 2) {
                    leftRocketPosition -= 1;
                } else if (c == 'z' && leftRocketPosition != 22) {
                    leftRocketPosition += 1;
                }

                if (c == 'k' && rightRocketPosition != 2) {
                    rightRocketPosition -= 1;
                } else if (c == 'm' && rightRocketPosition != 22) {
                    rightRocketPosition += 1;
                }

                printField(
                    leftPlayerPoints,
                    rightPlayerPoints,
                    ballPositionH,
                    ballPositionW,
                    leftRocketPosition,
                    rightRocketPosition);
            }
        }

        // Отскок мяча от верхней и нижней границы
        if (ballPositionH == 1) {
            ballPositionH = ballPositionH + 1;
            ballDirectionH = 1;
        } else if (ballPositionH == 23) {
            ballPositionH = ballPositionH - 1;
            ballDirectionH = -1;
        } else {
            ballPositionH = ballPositionH + ballDirectionH;
        }

        // Отскок мяча от ракетки
        if (ballPositionW == 1) {
            if (
                ballPositionH == leftRocketPosition ||
                ballPositionH == leftRocketPosition - 1||
                ballPositionH == leftRocketPosition + 1
            ) {
                ballPositionW = ballPositionW + 1;
                ballDirectionW = 1;
            } else {
                rightPlayerPoints++;
                ballPositionH = 11;
                ballPositionW = 20;
                ballDirectionW = 1;
            }
        } else if (ballPositionW == 78) {
            if (
                ballPositionH == rightRocketPosition ||
                ballPositionH ==rightRocketPosition - 1 ||
                ballPositionH == rightRocketPosition + 1
            ) {
                ballPositionW = ballPositionW - 1;
                ballDirectionW = -1;
            } else {
                leftPlayerPoints++;
                ballPositionH = 11;
                ballPositionW = 58;
                ballDirectionW = -1;
            }
        } else {
            ballPositionW = ballPositionW + ballDirectionW;
        }
    }

    return 0;
}

void clearScreen(void) {
    printf("\033[2J");
    printf("\033[0;0f");
}

void printWinner(int leftPlayerPoints, int rightPlayerPoints) {
    for (int i = 0; i < 35; i++) {
        printf("%c", ' ');
    }

    printf("%d - %d\n", leftPlayerPoints, rightPlayerPoints);

    for (int i = 0; i < 30; i++) {
        printf("%c", ' ');
    }

    if (leftPlayerPoints == 21) {
        printf("%s", "Left player win!\n");
    } else {
        printf("%s", "Right player win!\n");
    }
}

void printField(
    int leftPlayerPoints,
    int rightPlayerPoints,
    int ballPositionH,
    int ballPositionW,
    int leftRocketPosition,
    int rightRocketPosition
) {
    clearScreen();

    // Вывод счёта по середине
    if (leftPlayerPoints > 9) {
        for (int i = 0; i < 38; i++) {
            printf("%s", " ");
        }
    } else {
        for (int i = 0; i < 39; i++) {
            printf("%s", " ");
        }
    }

    printf("%d %d\n", leftPlayerPoints, rightPlayerPoints);

    // Вывод поля
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            if (i == ballPositionH && j == ballPositionW) {
                printf("%s", "o");
            } else if (j == 0 && (
                    i == leftRocketPosition ||
                    i == leftRocketPosition - 1 ||
                    i == leftRocketPosition + 1)) {
                printf("%s", "|");
            } else if (j == 79 && (
                    i == rightRocketPosition ||
                    i == rightRocketPosition - 1 ||
                    i == rightRocketPosition + 1)) {
                printf("%s", "|");
            } else if (i == 0 || i == 24) {
                printf("%s", "#");
            } else if (j == 40) {
                printf("%s", "|");
            } else {
                printf("%s", " ");
            }
        }

    printf("%s", "\n");
    }
}

int isKeyboardPressed(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
