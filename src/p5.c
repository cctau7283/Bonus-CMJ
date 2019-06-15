#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gsl/gsl_fit.h>

int main(void)
{
    char file_name[] = "./data/springData.csv";
    FILE *fp = fopen(file_name, "r");

    //Check if the file could be succesfully opened
    //If not, end the program and print the message on the screen
    if (fp == 0)
    {
        fprintf(stderr, "Cannot open the file! \n");
        return 1;
    }

    //A char array to put the line in. 100 is a number that is hopefully big enough.
    char curline[100]; 
    //A char array without commas
    char *convert = NULL;
    //flag1 checks which number of the line it is reading.
    //flag2 checks whether the reading procedure goes well.
    int x = 0, y = 0, flag1, flag2;
    //Two large enough arrays to put in the converted numbers.
    double Xr[100], Yr[100];
    //The number of data rows.
    int rows = 0;

    while (fgets(curline, 100, fp) != NULL)
    {
        flag1 = 0;
        convert = strtok(curline, ",");
        while (convert != NULL)
        {
            flag2 = 0;
            if (flag1 == 0)
            {
                if (atof(convert) > 0)
                //Since the first line is also read, it is needed to block it out from the array
                {
                    Yr[y] = atof(convert);
                    y += 1;
                    flag2 += 1;
                }
            }
            else
            {
                if (atof(convert) > 0)
                {
                    Xr[x] = atof(convert);
                    x += 1;
                    rows += 1;
                }
            }
            if (flag2 == 1)
            {
                flag1 += 1;
            }

            convert = strtok(NULL, ",");
        }
    }

    fclose(fp);

    //The array with the correct number of rows
    double X[rows], Y[rows];

    for (int j = 0; j < rows; j++)
    {
        X[j] = Xr[j] * 9.81;
        Y[j] = Yr[j];
    }

    //Linear regression
    double c0, c1, cov00, cov01, cov11, sumsq;
    gsl_fit_linear(X, 1, Y, 1, rows, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

    //Print the calculation result
    printf("# best fit: Y = %g + %g X\n", c0, c1);
    printf("covariance matrix:\n");
    printf("[ %g, %g\n  %g, %g]\n", cov00, cov01, cov01, cov11);
    printf("sumsq = %g\n", sumsq);
    printf("\n");

    /* For checking mistakes
    for (int i = 0; i < 19; i++)
    {
        printf("%f   %f\n", X[i], Y[i]);
    }
    printf("%d\n",rows);
     */

    FILE *pipe = popen("gnuplot -persist", "w"); // Open a pipe to gnuplot

    int i;

    if (pipe)
    {                                   // If gnuplot is found
        fprintf(pipe, "set term wx\n"); // set the terminal
        fprintf(pipe, "set xlabel '|Force| (Newtons)'\n");
        fprintf(pipe, "set ylabel 'Distance (meters)'\n");
        fprintf(pipe, "set yrange [0.05:0.6]\n");
        fprintf(pipe, "set title 'Measured Displacement of Spring, Linear fit:y=%.4f*x+%.4f'\n", c1, c0);

        fprintf(pipe, "plot '-' title '(Force,Distance)' with points  pt 7 lc rgb 'blue',\
                        '-' title 'Distance=f(Force)' with  linespoints  pt  6 lc rgb 'red'\n");

        //Drawing scatter plot
        for (i = 0; i < rows; i++)
        {
            fprintf(pipe, "%lf %lf\n", X[i], Y[i]);
        }
        //followed by the letter "e"
        fprintf(pipe, "e");

        //Drawing linear regression line
        fprintf(pipe, "\n"); // start a new draw item
        fprintf(pipe, "%lf %lf\n", 0.0, c0 + c1 * 0.0);
        for (i = 0; i < rows; i++)
        {
            fprintf(pipe, "%lf %lf\n", X[i], c0 + c1 * X[i]);
        }
        fprintf(pipe, "%lf %lf\n", 10.0, c0 + c1 * 10.0);
        fprintf(pipe, "e");

        fflush(pipe);             //flush pipe
        fprintf(pipe, "exit \n"); // exit gnuplot
        pclose(pipe);             //close pipe
    }
    return 0;
}