#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gsl/gsl_fit.h>

int main(void)
{
    char file_name[] = "./data/springData.csv";
    FILE *fp = fopen(file_name, "r");

    if (fp == 0)
    {
        fprintf(stderr, "Cannot open the file! \n");
        return 1;
    }

    char curline[100];
    char *convert = NULL;
    int x = 0, y = 0, flag1, flag2;
    double Xr[100], Yr[100];
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

    double X[rows], Y[rows];

    for (int j = 0; j < rows; j++)
    {
        X[j] = Xr[j] * 9.81;
        Y[j] = Yr[j];
    }

    double c0, c1, cov00, cov01, cov11, sumsq;
    gsl_fit_linear(X, 1, Y, 1, rows, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

    printf("# best fit: Y = %g + %g X\n", c0, c1);
    printf("covariance matrix:\n");
    printf("[ %g, %g\n  %g, %g]\n", cov00, cov01, cov01, cov11);
    printf("sumsq = %g\n", sumsq);
    printf("\n");

    /*
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

        for (i = 0; i < rows; i++)
        {
            fprintf(pipe, "%lf %lf\n", X[i], Y[i]);
        }
        //followed by the letter "e"
        fprintf(pipe, "e");

        // linear fit

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