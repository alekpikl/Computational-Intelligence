//
//  som_tsp.c
//  CI
//
//  Created by Alek  on 23/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define PI 3.1415
/* ********************* PARAM *************************/
#define NEURON_FACTOR 1.2 // nr_of_cities * NEURON_FACTOR = nr_of_neurons
#define LR_0 0.300 // starting learning rate
#define EPOCHS 100000
#define R_DECAY_RATE 0.999 // ignore
#define EXP_DECAY 1 // ignore
#define INIT_SCALE 0.67 // not using it
#define INIT_SCALE 0.67 // not using it
static double learning_rate = 0.3; // learning rate as a global variable
static double radius_d = 20.0; // double radius -> gets used to decay the int radius
static int radius_i = 20; // integer radius -> nr of neighbouring neurons
static int R_0 = 20; // Starting radius as a global variable
/* ******************* END OF PARAM ********************/

/* for writing in files -> UNCOMMENT */
FILE *f = NULL; // write to file

typedef struct coordiates {
    double x,y, ID;
} CITY;

typedef struct neuron {
    double x,y;
} NEURON;

typedef struct scl {
    double min_x, max_x, min_y, max_y;
} SCALE;

typedef CITY* CITY_MAP;
typedef NEURON* NEURON_MAP;

static CITY_MAP _city_map = NULL;
static NEURON_MAP _neuron_map = NULL;
static int nr_of_cities = 0;
static int nr_of_neurons = 0;
static SCALE scale = {10000, 0, 10000, 0};

void get_tsp_list(void);

void readCities(void) {
    char puffer[1000][30];
    while (scanf(" %s", puffer[nr_of_cities]) != EOF) {
        ++nr_of_cities;
    }
    _city_map = (CITY_MAP)calloc(nr_of_cities, sizeof(CITY));
    for (int i = 0; i < nr_of_cities; ++i) {
        sscanf(puffer[i], "%lf,%lf,%lf", &_city_map[i].ID, &_city_map[i].x, &_city_map[i].y);
        if(_city_map[i].x > scale.max_x)
            scale.max_x = _city_map[i].x;
        if (_city_map[i].x <= scale.min_x)
            scale.min_x = _city_map[i].x;
        if (_city_map[i].y > scale.max_y)
            scale.max_y = _city_map[i].y;
        if (_city_map[i].y <= scale.min_y)
            scale.min_y = _city_map[i].y;
    }
    nr_of_neurons = (int)(nr_of_cities * NEURON_FACTOR);
}
// Not using it -> scaling the initialises values instead
void scaleInput(void) {
    for (int i = 0; i < nr_of_cities; ++i) {
        if (scale.max_x > scale.max_y) {
            _city_map[i].x = (_city_map[i].x)/(scale.max_x-scale.min_x);
            _city_map[i].y = (_city_map[i].y)/(scale.max_x-scale.min_x);
        } else {
        _city_map[i].x = (_city_map[i].x)/(scale.max_y-scale.min_y);
        _city_map[i].y = (_city_map[i].y)/(scale.max_y-scale.min_y);
        }
    }
}
// between 0 and max -> negative values?

void initNeurons(void) {
    _neuron_map = (NEURON_MAP)calloc(nr_of_neurons, sizeof(NEURON));
    for (int neuron_ = 0; neuron_ < nr_of_neurons; ++neuron_) {
        // x between min_x and max_x -> range: min_x
        _neuron_map[neuron_].x = scale.min_x + rand()%(int)((scale.max_x-scale.min_x));
        _neuron_map[neuron_].y = scale.min_y + rand()%(int)((scale.max_y-scale.min_y));
    }
    // EXPERIMENT WITH THIS VALUE -> radius_0
    R_0 = radius_i = radius_d = nr_of_neurons / 3 - 2;
}

// Init in circle
/*
void initNeurons(void) {
    _neuron_map = (NEURON_MAP)calloc(nr_of_neurons, sizeof(NEURON));
    double radius = 0.0;
    if ((scale.max_x+scale.min_x) < (scale.max_y+scale.min_y))
        radius = (scale.max_x+scale.min_x)/2 * INIT_SCALE;
    else
        radius = (scale.max_y+scale.min_y)/2;
    
    for (int neuron_ = 0; neuron_ < nr_of_neurons; ++neuron_) {
        // x between min_x and max_x -> range: min_x
        _neuron_map[neuron_].x = (scale.min_x+scale.max_x)/2 + cos((double)neuron_/(double)nr_of_neurons*2*PI)*radius;
        _neuron_map[neuron_].y = (scale.min_y+scale.max_y)/2 + sin((double)neuron_/(double)nr_of_neurons*2*PI)*radius;
    }
    R_0 = radius_i = nr_of_neurons / 2 - 2;
}
 */

void printCities(void) {
    for (int city = 0; city < nr_of_cities; ++city) {
        printf("%lf,%lf,%lf\n", _city_map[city].ID, _city_map[city].x, _city_map[city].y);
    }
}

void printNeurons(void) {
    for (int neuron = 0; neuron < nr_of_neurons; ++neuron) {
        printf("x:%lf,y:%lf\n", _neuron_map[neuron].x, _neuron_map[neuron].y);
    }
}

double distance(CITY _city, NEURON _neuron) {
    double dist = pow(_city.x - _neuron.x,2) + pow(_city.y - _neuron.y,2);
    dist = sqrt(dist);
    return dist;
}

double distance_neurons(NEURON _neuron1, NEURON _neuron2) {
    double dist = pow(_neuron1.x - _neuron2.x,2) + pow(_neuron2.y - _neuron2.y,2);
    dist = sqrt(dist);
    return dist;
}
// how much are the neighbouring neurons taken into consideration
double neighbourhood(int radius_i, int distance) {
    int temp = radius_i*radius_i;
    if (temp < 1) // == 0 for abs double
        return 1;
    double ret = exp(-(double)(distance*distance)/(double)(2*temp));
    return ret;
}

// GEILER TRICK!!! LEARNING RATE WIRD GRÖSSER!!! (max 1 am Ende)
void learning_rate_decay(double iteration) {
    double alpha = EPOCHS / log(LR_0); //
    learning_rate = LR_0 * exp(-iteration/alpha);
}

void radius_decay(double iteration) {
    if (EXP_DECAY) {
        double alpha = EPOCHS / log(R_0);
        radius_d = R_0 * exp(-iteration*2/alpha);
        radius_i = (int)radius_d;
    }
}
// Modulo -> circle the neurons n-1, n, 0, 1, 2...
int modulo(int value) {
    if (value < 0)
        return nr_of_neurons-value-1;
    else if (value >= nr_of_neurons)
        return value%nr_of_neurons;
    return value;
}

void som_iteration(int iteration) {
    int winning_neuron_ID = 0;
    double winning_neuron_distance = 10000.0;
    // pick a random city
    int city_ID = rand()%nr_of_cities;
    // find the closest neuron
    for (int _neuron = 0; _neuron < nr_of_neurons; ++_neuron) {
        double temp = distance(_city_map[city_ID], _neuron_map[_neuron]);
        if (temp < winning_neuron_distance) {
            winning_neuron_ID = _neuron; winning_neuron_distance = temp;
        }
    }

    // update the winner neuron
    _neuron_map[winning_neuron_ID].x += learning_rate * 1 * (_city_map[city_ID].x - _neuron_map[winning_neuron_ID].x);
    _neuron_map[winning_neuron_ID].y += learning_rate * 1 * (_city_map[city_ID].y - _neuron_map[winning_neuron_ID].y);
    // update the neighbours
    for (int _neuron = 1; _neuron <= radius_i; ++_neuron) {
        int _index_plus = modulo(winning_neuron_ID + _neuron);
        int _index_minus = modulo(winning_neuron_ID - _neuron);
        double nbh = neighbourhood(radius_i, _neuron);
        _neuron_map[_index_plus].x += learning_rate * nbh * (_city_map[city_ID].x - _neuron_map[_index_plus].x);
        _neuron_map[_index_plus].y += learning_rate * nbh * (_city_map[city_ID].y - _neuron_map[_index_plus].y);
        _neuron_map[_index_minus].x += learning_rate * nbh * (_city_map[city_ID].x - _neuron_map[_index_minus].x);
        _neuron_map[_index_minus].y += learning_rate * nbh * (_city_map[city_ID].y - _neuron_map[_index_minus].y);
    }
    learning_rate_decay(iteration);
    radius_decay(iteration);
    printf("Learning rate: %lf\n", learning_rate);
}

void som(void) {
    time_t begin = time(NULL), end = time(NULL);
    int iteration = 0;
    while (iteration < EPOCHS && (end-begin) < 300) {
        som_iteration(iteration);
        ++iteration;
        /* for writing in files -> UNCOMMENT */
        
        if (!(iteration%200)) {
            f = fopen("som_tsp.txt", "w");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            for (int _neuron = 0; _neuron < nr_of_neurons; ++_neuron) {
                fprintf(f, "%lf,%lf\n", _neuron_map[_neuron].x, _neuron_map[_neuron].y);
            }
            fclose(f);
        }
        
        end = time(NULL);
    }
}

int winning_neuron(int city_ID) {
    int winning_neuron_ID = 0;
    double winning_neuron_distance = 10000.0;
    // find the closest neuron
    for (int _neuron = 0; _neuron < nr_of_neurons; ++_neuron) {
        double temp = distance(_city_map[city_ID], _neuron_map[_neuron]);
        //printf("TEMP: %lf, WND: %lf\n", temp, winning_neuron_distance);
        if (temp < winning_neuron_distance) {
            winning_neuron_ID = _neuron;
            winning_neuron_distance = temp;
        }
    }
    //printf("Winning n ID: %d\n", winning_neuron_ID);
    return winning_neuron_ID;
}

// is index in the array - DELETE??
bool in_array(int* array, int array_length, int index) {
    for (int i = 0; i < array_length; ++i)
        if (array[i] == index)
            return 1;
    return 0;
}
/* OUTPUT -> NEURON 2 CITY then PRINT all the cities */
void get_tsp_list(void) {
    int* nr_of_cities_for_neuron = (int*)calloc(nr_of_neurons, sizeof(int)); // index -> 0-x
    int neuron_city[1000][1000] = {0}; // closest city to neuron 0, neuron 1...
    for (int city = 0; city < nr_of_cities; ++city) {
        int closest_neuron = 0;
        // find nearest Neuron
        closest_neuron = winning_neuron(city);
        //printf("City: %d, closest_neuron: %d\n", city, closest_neuron);
        neuron_city[closest_neuron][nr_of_cities_for_neuron[closest_neuron]] = city;
        ++nr_of_cities_for_neuron[closest_neuron];
    }
    for (int neuron = 0; neuron < nr_of_neurons; ++neuron) {
        for (int city = 0; city < nr_of_cities_for_neuron[neuron]; ++city) {
            // +1 -> 1-nr_of_cities
            int idx_out = neuron_city[neuron][city] + 1;
            printf("%d\n",idx_out);
        }
    }
    /*
    for (int neuron = 0; neuron < nr_of_neurons; ++neuron) {
        printf("Neuron: %d - x: %lf, y: %lf\n", neuron,_neuron_map[neuron].x, _neuron_map[neuron].y);
    }
    printf("Learning rate: %lf, radius: %lf\n", learning_rate, radius);
     */
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));
    readCities();
    //scaleInput();
    /* for writing in files -> UNCOMMENT */
    
    f = fopen("cities.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int _city = 0; _city < nr_of_cities; ++_city) {
        fprintf(f, "%lf,%lf\n", _city_map[_city].x, _city_map[_city].y);
    }
    fclose(f);
    
    //printCities();
    //printf("Nr of Cities read: %d\n", nr_of_cities);
    initNeurons();
    //printNeurons();
    //printCities();
    som();
    
    get_tsp_list();
    //printNeurons();
    return 0;
}
