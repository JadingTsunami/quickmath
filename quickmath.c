/* quickmath.c
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <math.h>

int getNumber() {
    struct termios old, new;
    int nread;
    char num[256] = { 0 };
    int i = 0;

    if (tcgetattr(fileno(stdin), &old) != 0)
        return -1;
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
        return -1;

    do {
        nread = getchar();
        if( i == 0 && nread == '-' ) {
            num[i++] = '-';
            putchar(nread);
            fflush(stdout);
        }
        if( nread >= '0' && nread <= '9' ) {
            num[i++] = nread;
            putchar(nread);
            fflush(stdout);
        } else if( nread == '\n' ) {
            putchar(nread);
            num[i] = '\0';
            nread = EOF;
        }
    } while(nread != EOF);

    tcsetattr(fileno(stdin), TCSAFLUSH, &old);

    return atoi(num);
}

enum OPS {
    ADD,
    SUB,
    MUL,
    DIV
};

#define MAX_STRIKES (3)
#define NUM_ROUNDS  (10)

int main(int argc, char const *argv[]) {
    int val = 0;
    int op = 0;
    char opc = 0;
    int op1,op2;
    int res = 0;
    int done = 0;
    int strikes = 0;
    int rounds = 0;
    int totalstrikes = 0;
    double playtime = 0.0;
    int difficulty = 0;

    time_t begin, end;
    srand(time(NULL));

    time(&begin);
    while(rounds < NUM_ROUNDS) {
        rounds++;
        val = rand() % 100;

        if( val >= 60 )      op = ADD;
        else if( val >= 20 ) op = SUB;
        else if( val >= 10 ) op = MUL;
        else                 op = SUB; // TODO: DIV

        op1 = rand() % ((int)pow(10,1+rand()%2));
        op2 = rand() % ((int)pow(10,1+rand()%2));

        switch(op) {
            case ADD:
                opc = '+';
                difficulty += 1;
                res = op1 + op2;
                break;
            case SUB:
                opc = '-';
                difficulty += 2;
                if( op2 > 10 ) difficulty += 2;
                if( op2 > op1 ) difficulty += 1;
                res = op1 - op2;
                break;
            case MUL:
                opc = '*';
                difficulty += 4;
                if( op1 > 10 ) difficulty += 2;
                if( op2 > 10 ) difficulty += 2;
                if( op1 < 10 && op2 < 10 ) difficulty -=3;
                res = op1 * op2;
                break;
            // TODO: Implement
            case DIV:
                opc = '/';
                res = op1 / op2;
                break;
            default:
                break;
        }

        while( !done ) {
            printf("%d %c %d = ",op1,opc,op2);
            val = getNumber();
            if( val == res )  {
                printf("You got it.\n");
                done = 1;
            } else {
                if( val > res ) {
                    printf("Smaller.\n");
                } else {
                    printf("Larger.\n");
                }
                strikes++;
            }
            if( strikes >= MAX_STRIKES ) {
                printf("Answer: %d\n", res);
                done = 1;
            }
        }
        totalstrikes += strikes;
        done = strikes = 0;
    }
    time(&end);

    playtime = difftime(end,begin);
    printf("\n");
    printf("Difficulty score:\t%d\n", difficulty);
    printf("Strikes:\t\t%d\n",totalstrikes);
    printf("Time:\t\t\t%.2lf seconds\n\n",playtime);
    printf("Total Score: %d\n\n", (int)((difficulty-totalstrikes)*2-playtime) );
    return 0;

}
