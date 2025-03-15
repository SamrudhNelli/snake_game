#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <vector>

#define n 40

#ifdef _WIN32
    #include <conio.h>
    #include <Windows.h>
    #define slp() sleep();
    #define device 1

#elif __linux__
    #include <unistd.h>
    #include <stdio.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #define device 2

    int kbhit() 
    {
        static const int STDIN = 0;
        static bool initialized = false;

        if (! initialized) {
            // Use termios to turn off line buffering
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }

    void sleep_ms(int milliseconds)
    {
        // Convert milliseconds to microseconds
        usleep(milliseconds * 1000);
    }

#else
    #error "Unknown OS used!!"

#endif


class game
{
    public:
    int f_x, f_y;
    int head_x = 20, head_y = 20, snake_length = 3, snake_dir = 1, snake_speed = 300; // 1->right, -1->left, 2->up, -2->down
    std::vector<int> snake_x, snake_y;
    char A[n][n];
    void fruit();
    void init();
    int snake(int,int,int,int);
    int snake_game();
};


void game::fruit()
{
    snake_x.resize(this->snake_length);
    snake_y.resize(this->snake_length);
    bool fruit_exists = false;
    while(!fruit_exists)
    {
        srand(time(NULL));
        int x = rand() % 40;
        int y = rand() % 40;
        int token = 1;
        for(int i = 0; i < this->snake_length; i++)
        {
            if(x == snake_x[i] || y == snake_y[i])
            {
                token = 0;
                break;
            }
        }
        if(x == 0 || x == n-1 || y == 0 || y == n-1)
            token = 0;
        if(token)
        {
            A[x][y] = '@';
            f_x = x;
            f_y = y;
            fruit_exists = true;
        }
    }
}

void game::init()
{
    snake_x.resize(this->snake_length);
    snake_y.resize(this->snake_length);
    for(int i = 0; i < snake_length; i++)
    {
        snake_x[i] = head_x;
        snake_x[i] = head_x;
        snake_y[i] = head_y - i;
        if(!i)
            A[snake_x[i]][snake_y[i]] = '>';
        else
            A[snake_x[i]][snake_y[i]] = '-';
    }
    this->fruit();
}


int game::snake(int w, int a, int s, int d)
{

    if(w)
        snake_dir = 2;
    if(a)
        snake_dir = -1;
    if(s)
        snake_dir = -2;
    if(d)
        snake_dir = 1;

    int tx = snake_x[snake_length-1], ty = snake_y[snake_length-1];
    
    char t = A[tx][ty];
    
    for(int i = snake_length-1; i > 0; i--)
    {
        snake_x[i] = snake_x[i-1];
        snake_y[i] = snake_y[i-1];
    }
    A[tx][ty] = ' ';

    if(snake_dir == 1)
    {
        snake_y[0]++;
        A[snake_x[0]][snake_y[0]] = '>';
        A[snake_x[1]][snake_y[1]] = '-';
    }
    if(snake_dir == -1)
    {
        snake_y[0]--;
        A[snake_x[0]][snake_y[0]] = '<';
        A[snake_x[1]][snake_y[1]] = '-';
    }
    if(snake_dir == 2)
    {
        snake_x[0]--;
        A[snake_x[0]][snake_y[0]] = '^';
        A[snake_x[1]][snake_y[1]] = '|';
    }
    if(snake_dir == -2)
    {
        snake_x[0]++;
        A[snake_x[0]][snake_y[0]] = 'v';
        A[snake_x[1]][snake_y[1]] = '|';
    }
    if(snake_x[0] == f_x && snake_y[0] == f_y)
    {
        this->fruit();
        snake_length ++;
        snake_x.resize(this->snake_length);
        snake_y.resize(this->snake_length);
        snake_x[snake_length-1] = tx;
        snake_y[snake_length-1] = ty;
        A[tx][ty] = t;
    }
    for(int i = 1; i < snake_length; i++)
    {
        if(snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i])
        {
            return 0;
        }
    }
    if(snake_x[0] == 0 || snake_x[0] == n-1 || snake_y[0] == 0 || snake_y[0] == n-1)
    {
        return 0;
    }
    return 1;
    
}

int game::snake_game()
{
    FILE *fptr;
    fptr = fopen("snake_game_leaderboard.txt","a+");
    fseek(fptr, 0, SEEK_SET);

    int c = fgetc(fptr);
    if (c == EOF) 
    {
        fprintf(fptr,"Highscore none 0\n");
    }
    else
    {
        ungetc(c, fptr);
    }
    fseek(fptr,0,SEEK_SET);

    int highscore, score;
    char user[100], tuser[100];
    fscanf(fptr,"%*s %s %d",user,&highscore);
    fseek(fptr,0,SEEK_SET);


    int cont = 1;
    for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
    {
    if(i == 0 || i == n-1)
        A[i][j] = '*';
    else if(j == 0 || j == n-1)
        A[i][j] = '*';
    else
        A[i][j] = ' ';
    }
    this->init();
    while(cont)
    {
        while(!kbhit() && cont)
        {
            for(int i = 0; i < n; i++)
            {
                for(int j = 0; j < n; j++)
                    printf("%c",A[i][j]);
                if(i == 8)
                    printf("                                     !!Help!! :      ");
                if(i == 9)
                    printf("                           1) Move UP : w / W / ^ arrow");
                if(i == 10)
                    printf("                           2) Move Left : a / A / v arrow ");
                if(i == 11)
                    printf("                           3) Move Down : s / S / < arrow ");
                if(i == 12)
                    printf("                           4) Move Right : d / D / > arrow ");
                if(i == 13)
                    printf("                           5) Increase snake speed : f ");
                if(i == 14)
                    printf("                           6) Decrease snake speed : g ");
                if(i == 15)
                    printf("                           7) Quit : q / Enter ");
                
                printf("\n");
            }
            printf("     Last Highscore: %d by %s \n",highscore,user);
            cont = this->snake(0,0,0,0);
            if(device == 1)
            {
                sleep(snake_speed);
                system("cls");
            }
            else if(device == 2)
            {
                sleep_ms(snake_speed);
                system("clear");
            }
        }
        if(cont)
        {
            char ch;
            scanf("%c", &ch);
            int k = 0;
            if(ch == 27)
            {
                scanf("%c", &ch);
                k++;
            }
            if(k && ch == 91)
            {
                scanf("%c", &ch);
                k++;
            }
            if(k == 2)
            {
                if(ch == 65)
                    cont = this->snake(1,0,0,0);
                else if(ch == 68)
                    cont = this->snake(0,1,0,0);
                else if(ch == 66)
                    cont = this->snake(0,0,1,0);
                else if(ch == 67)
                    cont = this->snake(0,0,0,1);
            }
            else
            {
                if(ch == 'w' || ch == 'W')
                    cont = this->snake(1,0,0,0);
                if(ch == 'a' || ch == 'A')
                    cont = this->snake(0,1,0,0);
                if(ch == 's' || ch == 'S')
                    cont = this->snake(0,0,1,0);
                if(ch == 'd' || ch == 'D')
                    cont = this->snake(0,0,0,1);
                if(ch == 'f')
                    snake_speed /= 2;
                if(ch == 'g')
                    snake_speed *= 2;
                if(ch == 'q' || (int)ch == 13)
                    cont = 0;
            }
        }
        
    }

    score = snake_length - 3;

    printf("****************************************\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*            --------------            *\n");
    printf("*            |            |            *\n");
    printf("*            |            |            *\n");
    printf("*            ------------>|            *\n");
    printf("*                         |            *\n");
    printf("*                         |            *\n");
    printf("*                       ---            *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*          You Failed!!!!!!            *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*          Your Score :                *\n");
    printf("*%16d                      *\n",score);
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("*                                      *\n");
    printf("****************************************\n");
    printf("****************************************\n");

    char d = 'y';

    if(score > highscore)
    {
        printf("\nCongratulations on beating the highscore!!\n");
        printf("Your name please : ");
        scanf("%s",tuser);
        printf("Thank You for enjoying the game!!\n");
        
        int line_count = 0;

        FILE *tfp = fopen("temp.txt","w");
        fprintf(tfp,"Highscore %s %d",tuser,score);

        while(d != EOF)
        {
            d = getc(fptr);
            if(line_count != 0)
            {
                if(d != '\n')
                    fprintf(tfp,"%c",d);
            }
            if(d == '\n')
            {
                fprintf(tfp,"%c",d);
                line_count++;
            }

        }
        
        fprintf(tfp,"%s %d\n",tuser,score);
        fclose(fptr);
        fclose(tfp);
        remove("snake_game_leaderboard.txt");
        rename("temp.txt","snake_game_leaderboard.txt");
    }
    else
    {
        printf("\nWould you like to save your result ? (y/n) : ");
        scanf("%c",&d);
        if(d == 'y')
        {
            printf("\nEnter the player name : ");
            scanf("%s",tuser);
            printf("\nSaved successfully !!");
        }
        printf("\nThanks for playing the game!!\n");
        fseek(fptr,0,SEEK_END);
        fprintf(fptr,"%s %d\n",tuser,score);
    }

    char e;
    printf("\nWould you like to play again? (y/n) : ");
    scanf(" %c",&e);
    if(e == 'y' || e == 'Y')
        return 1;
    return 0;
}

