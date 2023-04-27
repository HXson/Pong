// "Copyright (2022) Team 32"
#include <stdio.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#define WIDTH 81  // ширина поля
#define HEIGHT 26      // Высота поля
#define PADDLE1_X 7  // Значение 1 ракетки по х
#define PADDLE2_X WIDTH - 7  // Значение 2 Ракетки по х
#define BALLSTARTX 40  // Начальная позичия мяча по х
#define BALLSTARTY 13
#define BALLSTARTMOVE 0  // Начальное движение мяча

void graph();  // Функция отрисовки поля со всеми включениями
void ballCalc();  // Функция полета мяча
void paddleCal(char, int *, int *);  // Функция движения ракеток
void set_keypress(void);  // Функция пользовательского ввода, не блокируещего выполнение программы.

int main() {
  char command;  // Значение того, что было нажато на клавиатуре
  int paddle1_y = 13, paddle2_y = 13;  // Начальное положение ракеток по у
  int ballX = BALLSTARTX, ballY = BALLSTARTY;  // Начальное положение мяча
  int move = BALLSTARTMOVE;  // Начальное накправление мяча
  int scoreF = 0, scoreS = 0, goalFlag = 0, winFlag = 0;
  // Начальные значения счета, указатель забит ли мяч и кому , указатель
  // окончания игры
  printf("PRESS ENTER TO START");
  getc(stdin);
  system("stty -icanon");
  fd_set rfds;
  struct timeval tv;
  set_keypress();
  while (1) {
    FD_ZERO(&rfds);  // мониторит на наличие чтения в одном или нескольких дескрипторах
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int retval = select(2, &rfds, NULL, NULL, &tv);
    if (goalFlag == 1) {  // Если забит мяч превому
      goalFlag = 0;  // Изменяет значение опять на не гол
      ballX = 8;  // Вывод положения мяча после гола по х
      ballY = paddle1_y;
      move = 0;  // Назначаем новый способ движения
    }
    if (goalFlag == 2) {
      goalFlag = 0;
      ballX = 73;
      ballY = paddle2_y;
      move = 3;
    }
    { if (retval) {
        command = getc(stdin);
          graph(&ballX, &ballY, &paddle1_y, &paddle2_y, scoreF, scoreS,
                winFlag);
      } else {
        command = ' ';
      }
      usleep(85000);
      // Если нажали какой-нибудь символ или нет переноса строки
            // Проверка будет ли двигаться ракетка
      ballCalc(&ballX, &ballY, &move, paddle1_y, paddle2_y, &scoreF, &scoreS,
               &goalFlag);  // Изменяет положение мяча
      paddleCal(command, &paddle1_y, &paddle2_y);  // Изменяет положение ракетки
      graph(&ballX, &ballY, &paddle1_y, &paddle2_y, scoreF, scoreS,
            winFlag);  // Отрисовывает поле
    }
    if (scoreF == 1) {  // Проверяем значение счета
      winFlag = 21;  // Окончена игра для 1
    }
    if (scoreS == 21) {
      winFlag = 2;
    }
    if (winFlag == 1 || winFlag == 2) {  // Проверка окончена ли игра
      graph(&ballX, &ballY, &paddle1_y, &paddle2_y, scoreF, scoreS, winFlag);
      break;
    }
  }
}
/*
* Mode to move a ball
0 - Right Up
1 - Right Down
2 - Left Donw
3 - Left Up
*/
void ballCalc(int *x, int *y, int *move, int paddle1_y, int paddle2_y,
              int *scoreF, int *scoreS, int *goalFlag) {
  if (*x == PADDLE1_X && (*y < paddle1_y - 1 || *y > paddle1_y + 1)) {
    // Проверяем не попал ли мяч на линию ракетки по х и не попал на ракетку
    (*scoreS)++;  // Увеличиваем счет второго
    *goalFlag = 1;  // Указываем, что гол забит первому
  }
  if (*x == PADDLE2_X && (*y < paddle2_y - 1 || *y > paddle2_y + 1)) {
    (*scoreF)++;
    *goalFlag = 2;
  }
  switch (*move) {  // Обработка отражения
  case 0:
    if (*x == 80 ||
        (*x == PADDLE2_X - 1 && *y >= paddle2_y - 1 && *y <= paddle2_y + 1)) {
      // Проверка врезается в правый край поля или в ракетку 2
      *move = 3;  // Меняет направление: влево вверх
    }
    if (*y == 1) {  // Проверка врезался ли в потолок
      *move = 1;  //  Меняет направление: вправо вниз
    }
    break;
  case 1:
    if (*x == 80 || (*x == PADDLE2_X - 1 &&
                     *y >= paddle2_y - 1
                     // Проверка врезается в правый край или в ракетку 2
                     && *y <= paddle2_y + 1)) {
      *move = 2;
    }
    if (*y == 25) {  // Проверка врезается ли в пол
      *move = 0;
    }
    break;
  case 2:
    if (*x == 1 || (*x == PADDLE1_X + 1 &&
                    *y >= paddle1_y - 1
                    // Проверка врезается в левый край или в ракетку 1
                    && *y <= paddle1_y + 1)) {
      *move = 1;
    }
    if (*y == 25) {  // Проверка врезается ли в пол
      *move = 3;
    }
    break;
  case 3:
    if (*x == 1 ||
        (*x == PADDLE1_X + 1 && *y >= paddle1_y - 1 && *y <= paddle1_y + 1)) {
      *move = 0;
    }
    if (*y == 1) {
      *move = 2;
    }
    break;
  default:  // Выход при ошибке
    break;
  }
  switch (*move) {  // Описание полета мяча
  case 0:
    (*x)++;  // Мяч вправо вниз
    (*y)--;
    break;
  case 1:
    (*x)++;
    (*y)++;
    break;
  case 2:
    (*x)--;
    (*y)++;
    break;
  case 3:
    (*x)--;
    (*y)--;
    break;
  default:
    break;
  }
}
void paddleCal(char mode, int *paddle1_y, int *paddle2_y) {  // Движение ракеток
  if (mode == 'Z') {  // Будет ли двигаться вниз 1 ракетка
    if (*paddle1_y + 2 < HEIGHT) {  // Сможет ли двигаться
      *paddle1_y += 1;  // Меняем положение центра ракетки по у
    }
  } else if (mode == 'A') {
    if (*paddle1_y - 2 > 0) {
      *paddle1_y -= 1;
    }
  } else if (mode == 'M') {
    if (*paddle2_y + 2 < HEIGHT) {
      *paddle2_y += 1;
    }
  } else if (mode == 'K') {
    if (*paddle2_y - 2 > 0) {
      *paddle2_y -= 1;
    }
  }
}
static struct termios stored_settings;

void set_keypress(void) {
  struct termios new_settings;

  tcgetattr(0, &stored_settings);

  new_settings = stored_settings;

  new_settings.c_lflag &= (~ICANON & ~ECHO);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;

  tcsetattr(0, TCSANOW, &new_settings);
  return;
}

void graph(const int *ballX, const int *ballY, const int *paddle1_y, const int *paddle2_y, int scoreF,
           int scoreS, int winFlag) {
  char str[] = "WINNER";
  int i = 0;  // Счетчик для вывода букв
  printf("\e[1;1H\e[2J");
  for (int y = 0; y < 27; y++) {
    for (int x = 0; x < 82; x++) {
      if (((x == 0 || x == 41 || y == 0 || y == 26) && (x % 2 == 0) && (x < 41)) ||
          ((x == 81 || y == 0 || y == 26) && (x % 2 == 1) && (x > 40))) {  // Отрисовка рамки
        printf("·");
      } else if (x == *ballX && y == *ballY) {
        printf("o");
      } else if (((y == *paddle1_y - 1) || (y == *paddle1_y) ||
                  (y == *paddle1_y + 1)) &&
                 (x == PADDLE1_X)) {
        printf("|");
      } else if (((y == *paddle2_y - 1) || (y == *paddle2_y) ||
                  (y == *paddle2_y + 1)) &&
                 (x == PADDLE2_X)) {
        printf("|");
      } else if (x == 40 || x == 41) {  // Отрисовка сетки
        printf(":");
      } else if ((scoreF > 9) && (((x == 37) && (y == 2)) ||
                                  ((x == 38) && (y == 2)))) {  // Отрисовка счета
        if (x == 37) {
          printf("%d", scoreF / 10);
        } else {
          printf("%d", scoreF % 10);
        }
      } else if ((scoreF < 10) && (x == 38) && (y == 2)) {
        printf("%d", scoreF);
      } else if ((scoreS > 9) &&
                 (((x == 43) && (y == 2)) || ((x == 44) && (y == 2)))) {
        if (x == 43) {
          printf("%d", scoreS / 10);
        } else {
          printf("%d", scoreS % 10);
        }
      } else if ((scoreS < 10) && (x == 43) && (y == 2)) {
        printf("%d", scoreS);
      } else if ((winFlag == 1) &&
                 (x ==
                  17 + i)  // Проверка окончания игры и отрисовка поздравления
                 && (y == 12) && (i < 6)) {
        printf("%c", str[i]);
        i++;
      } else if ((winFlag == 2) && (x == 57 + i) && (y == 12) && (i < 6)) {
        printf("%c", str[i]);
        i++;
      } else {  // В других ситуациях пробел
        printf(" ");
      }
    }
    printf("\n");
  }
    printf("The PONG (by Team32)\n");
    printf("Игрок 1 - A/Z.\nИгрок 2 - K/M.\n");
    printf("Игра продолжается до 21 очков.\n");
}
