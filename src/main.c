#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define LED_PIN 17

int main(void)
{
    double low = 68, high = 76;  
    double *temps = NULL;        
    int count = 0;
    int choice = 0;
    double t, sum;
    int i, warning;
    char *status;
    FILE *f;

    if (gpioInitialise() < 0)
    {
        printf("pigpio failed. Run with sudo.\n");
        return 1;
    }
    gpioSetMode(LED_PIN, PI_OUTPUT);
    gpioWrite(LED_PIN, 0);

    while (choice != 3)
    {
        printf("\n1) Enter temperature\n2) Change range\n3) Quit\nChoice: ");
        if (scanf("%d", &choice) != 1)
            break;

        if (choice == 1)
        {
            printf("Enter temperature (F): ");
            scanf("%lf", &t);

            /* grow the array by one and store the reading */
            temps = realloc(temps, (count + 1) * sizeof(double));
            temps[count] = t;
            count++;

            /* check range */
            if (t >= low && t <= high)
            {
                status = "NORMAL";
                warning = 0;
            }
            else
            {
                status = "WARNING";
                warning = 1;
            }
            gpioWrite(LED_PIN, warning);

            /* average */
            sum = 0;
            for (i = 0; i < count; i++)
                sum += temps[i];

            printf("%.1fF - %s - LED %s\n", t, status, warning ? "ON" : "OFF");
            printf("Average: %.2fF\n", sum / count);

            /* save to files */
            f = fopen("data/temperature_log.csv", "a");
            if (f != NULL)
            {
                fprintf(f, "%.1f,%s,%s\n", t, status, warning ? "ON" : "OFF");
                fclose(f);
            }
            f = fopen("status.txt", "w");
            if (f != NULL)
            {
                fprintf(f, "%s\n", status);
                fclose(f);
            }
        }
        else if (choice == 2)
        {
            printf("New minimum: ");
            scanf("%lf", &low);
            printf("New maximum: ");
            scanf("%lf", &high);
            printf("Range is now %.1f to %.1f\n", low, high);
        }
    }

    gpioWrite(LED_PIN, 0);
    gpioTerminate();
    free(temps);
    return 0;
}

