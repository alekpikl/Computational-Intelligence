//
//  vq_2.c
//  CI
//
//  Created by Alek  on 25/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct scale {
    int min_x, max_x, min_y, max_y;
} SCALE;

#define EPOCHS 20000
#define TIME 8
#define PI  3.14159265359
#define LR_0 0.01
//#define FILE_OUT
#define MAX_CLUSTERS 20
#define RADIUS_SCALE 5 // average * RADIUS_SCALE
static SCALE scale = {10000, 0, 10000, 0};
static int nrOfInputs = 0;
static int _NR_OF_SAMPLES_ = 9; // nr of samples pro cluster
static int _NR_FREE = 100;
int nrOfClusters =0;
static double LEARNING_RATE;

#ifdef FILE_OUT
    FILE *f = NULL; // write to file
#endif

typedef struct smp {
    double x,y;
    int cluster;
} SAMPLE;

typedef double** MATRIX;
typedef double* VECTOR;

MATRIX input = NULL;
SAMPLE* _input = NULL;
SAMPLE* CLUSTERS = NULL;

void findInputRange(void) {
    //find the max and min
    for (int i=0; i<nrOfInputs; ++i) {
        if (_input[i].x > scale.max_x)
            scale.max_x = _input[i].x;
        else if (_input[i].x < scale.min_x)
            scale.min_x = _input[i].x;
        if (_input[i].y > scale.max_y)
            scale.max_y = _input[i].y;
        else if (_input[i].y < scale.min_y)
            scale.min_y = _input[i].y;
    }
}
VECTOR createVector(int rows, int columns) {
    VECTOR vector = (VECTOR)calloc(rows*columns, sizeof(double));
    return vector;
}
MATRIX createMatrix(int rows, int columns) {
    MATRIX matrix = (MATRIX)calloc(rows, sizeof(VECTOR));
    matrix[0] = (VECTOR)calloc(columns*rows, sizeof(double));
    
    for (int i = 0; i<rows; i++)
        matrix[i] = (*matrix + columns*i);
    
    return matrix;
}
void readCSV() {
    nrOfInputs = 0;
    double buffer[2];
    char peak[50] = {'\0'};
    while (scanf(" %s", peak) != EOF) {
        sscanf(peak,"%lf,%lf", &buffer[0],&buffer[1]);
        _input[nrOfInputs].x = buffer[0];
        _input[nrOfInputs].y = buffer[1];
        _input[nrOfInputs].cluster = MAX_CLUSTERS;
        ++nrOfInputs;
    }
}
double distance_v2(SAMPLE c1, SAMPLE c2) {
    double dist = 0.0;
    dist += pow((c1.x - c2.x),2);
    dist += pow((c1.y - c2.y),2);
    dist = sqrt(dist);
    return dist;
}

bool check_finish(int free_samples, int* index) {
    int nr_free = 0;
    for (int sample = 0; sample < nrOfInputs; ++sample) {
        if (_input[sample].cluster == MAX_CLUSTERS) {
            index[nr_free] = sample;
            ++nr_free;
        }
    }
    _NR_FREE = nr_free;
    //printf("Number of free soldiers: %d\n", nr_free);
    if (nr_free <= free_samples)
        return true;
    else
        return false;
}

void vq(void) {
    int free_samples = 5;
    int cluster = 0;
    
    /******* distances to the cluster c ******/
    VECTOR distances = createVector(nrOfInputs, 1);
    /******* index encoder for the cluster c ******/
    int* dist_idx = (int*)calloc(_NR_OF_SAMPLES_, sizeof(int));
    /********** saved cluster samples *********/
    CLUSTERS = (SAMPLE*)calloc(MAX_CLUSTERS, sizeof(SAMPLE));
    /******** free samples indizes *******/
    int* free_samples_idx = (int*)calloc(nrOfInputs, sizeof(int));
    
    while (/*!check_finish(free_samples, free_samples_idx)*/ cluster < nrOfClusters) {
        // First, take a random sample and find it's neighbours and mark them as CLUSTER 0
        
        int first_cluster = 0;
        do {
            first_cluster = rand() % nrOfInputs;
        } while (_input[first_cluster].cluster != MAX_CLUSTERS);
        CLUSTERS[cluster] = _input[first_cluster];
        
        //printf("%d: %lf,%lf,%d\n", first_cluster,_input[first_cluster].x, _input[first_cluster].y, _input[first_cluster].cluster);
        _input[first_cluster].cluster = cluster;
        // Find the _NR_OF_SAMPLES_ samples with the smallest Distance to the random sample and calculate the average Distance to the Sample
        double min_dist = 10000; int min_dist_idx = 0;
        for (int sample = 0; sample < nrOfInputs; ++sample) {
            distances[sample] = distance_v2(_input[sample], _input[first_cluster]);
            if (distances[sample] < min_dist && sample != first_cluster) {
                min_dist_idx = sample;
                min_dist = distances[sample];
            }
        }
        //printf("Distance: %lf\n\n", distances[min_dist_idx]);
        dist_idx[0] = min_dist_idx;
        _input[min_dist_idx].cluster = cluster;
        for (int idx = 1; idx < _NR_OF_SAMPLES_; ++idx) {
            min_dist = 10000;
            for (int sample = 0; sample < nrOfInputs; ++sample) {
                if ((distances[sample] < min_dist) && (distances[sample] > distances[dist_idx[idx-1]])) {
                    min_dist_idx = sample;
                    min_dist = distances[sample];
                }
            }
            //printf("Distance: %lf\n", distances[min_dist_idx]);
            dist_idx[idx] = min_dist_idx;
            _input[min_dist_idx].cluster = cluster;
        }
        // get all the rest samples from the same cluster -> calculate the average distance and then mark all in that radius as from that cluster
        double average = 0.0;
        // remove the max value
        int idx_max = 0; double value_max = 0.0;
        for (int sample = 0; sample < _NR_OF_SAMPLES_; ++sample) {
            if(distances[dist_idx[sample]] > value_max) {
                idx_max = dist_idx[sample];
                value_max = distances[dist_idx[sample]];
            }
        }
        for (int sample = 0; sample < _NR_OF_SAMPLES_; ++sample) {
            if (dist_idx[sample] != idx_max) {
                average += distances[dist_idx[sample]];
            }
        }
        average = average / (_NR_OF_SAMPLES_ - 1.0);
        //printf("Average: %lf\n", average);
        // find the middle and use the point that is closest to calculate the distances
        SAMPLE middle;
        for (int j = 0; j < 4; j++) {
            middle.x = 0.0; middle.y = 0.0;
            int t = 0;
            for (int i = 0; i < nrOfInputs; ++i) {
                if (_input[i].cluster == cluster) {
                    middle.x += _input[i].x;
                    middle.y += _input[i].y;
                    ++t;
                }
            }
            
            middle.x /= (t);
            middle.y /= (t);
            CLUSTERS[cluster].x = middle.x;
            CLUSTERS[cluster].y = middle.y;
            // printf("t: %d.  Middle_x: %lf, Middle_y: %lf\n", t, middle.x, middle.y);
            // find the closest point to the middle - OR NOT?! just use the middle!!!
            for (int sample = 0; sample < nrOfInputs; ++sample) {
                distances[sample] = distance_v2(_input[sample], middle);
            }
            // Searh for others that are in the average range && have not yet been clustered
            for (int sample = 0; sample < nrOfInputs; ++sample) {
                if ((distances[sample] < average * RADIUS_SCALE) && (_input[sample].cluster == MAX_CLUSTERS)) {
                    _input[sample].cluster = cluster;
                }
            }
        }
        
        
        
        /*
        for (int sample = 0; sample < nrOfInputs; ++sample) {
            distances[sample] = distance_v2(_input[sample], _input[dist_idx[_NR_OF_SAMPLES_-3]]);
        }
        for (int sample = 0; sample < nrOfInputs; ++sample) {
            if ((distances[sample] < average * RADIUS_SCALE) && (_input[sample].cluster == MAX_CLUSTERS)) {
                _input[sample].cluster = cluster;
            }
        }
        */
        if (cluster < nrOfClusters) {
            ++cluster;
        }
        check_finish(free_samples, free_samples_idx);
    }
    //printf("End of while\n");
    free(distances);
    // asign the free samples to the nearest cluster!
    double min_dist = 10000.0, temp = 0;
    for (int not_assigned = 0 ; not_assigned < _NR_FREE; ++not_assigned) {
        min_dist = 10000.0;
        for (int _c = 0; _c < cluster; ++_c) {
            temp = distance_v2(_input[free_samples_idx[not_assigned]], CLUSTERS[_c]);
            if (temp < min_dist) {
                _input[free_samples_idx[not_assigned]].cluster = _c;
                min_dist = temp;
            }
        }
    }
    // give out the clusters -> the middle of the samples
    SAMPLE middle; middle.x = 0; middle.y = 0;
    int nr_of_units = 0;
    for (int c = 0; c < cluster; ++c) {
        middle.x = 0; middle.y = 0;
        nr_of_units = 0;
        //printf("Cluster: %d\n", c);
        for (int sample = 0; sample < nrOfInputs; ++sample) {
            if (_input[sample].cluster == c) {
                //printf("%lf,%lf\n", _input[sample].x, _input[sample].y);
                middle.x += _input[sample].x;
                middle.y += _input[sample].y;
                ++nr_of_units;
            }
        }
        middle.x /= nr_of_units;
        middle.y /= nr_of_units;
        printf("%lf,%lf\n", middle.x, middle.y);
    }
    
}

void reposition(int c) {
        CLUSTERS[c].x = scale.min_x + rand()%(int)((scale.max_x-scale.min_x));
        CLUSTERS[c].y = scale.min_y + rand()%(int)((scale.max_y-scale.min_y));
}
void lvq() {
    int* selected_time = (int*)calloc(nrOfClusters, sizeof(int));
    int nrEpoch = 0;
    while (nrEpoch < EPOCHS) {
        int inputIndex = 0; //indexMin = 0;
        //randomize(input, nrOfInputs);
        while (inputIndex < nrOfInputs) {
            int indexMin = 0;
            double min=(CLUSTERS[0].x-_input[inputIndex].x)*(CLUSTERS[0].x-_input[inputIndex].x)+(CLUSTERS[0].y-_input[inputIndex].y)*(CLUSTERS[0].x-_input[inputIndex].y);
            for (int j = 0; j<nrOfClusters; ++j) {
                if ((CLUSTERS[j].x-_input[inputIndex].x)*(CLUSTERS[j].x-_input[inputIndex].x) + (CLUSTERS[j].y-_input[inputIndex].y)*(CLUSTERS[j].y-_input[inputIndex].y) < min) {
                    min = (CLUSTERS[j].x-_input[inputIndex].x)*(CLUSTERS[j].x-_input[inputIndex].x) + (CLUSTERS[j].y-_input[inputIndex].y)*(CLUSTERS[j].y-_input[inputIndex].y);
                    indexMin = j;
                }
            }
            //printf("Index min: %d\n", indexMin);
            ++selected_time[indexMin];
            _input[inputIndex].cluster = CLUSTERS[indexMin].cluster;
            CLUSTERS[indexMin].x += LEARNING_RATE * (_input[inputIndex].x - CLUSTERS[indexMin].x);
            CLUSTERS[indexMin].y += LEARNING_RATE * (_input[inputIndex].y - CLUSTERS[indexMin].y);
            // update the winning center
            ++inputIndex;
        }
        
        if (LEARNING_RATE > 0.05) {
            double temp = exp(-2*(double)nrEpoch/(double)EPOCHS);
            LEARNING_RATE = LR_0 * temp;
        }
        //printf("Learning Rate: %lf\n", LEARNING_RATE);
        ++nrEpoch;
    }
    
    // print the centers
    for (int i = 0; i < nrOfClusters; ++i) {
        printf("%lf,%lf\n", CLUSTERS[i].x, CLUSTERS[i].y);
    }
}


int main(int argc, char *argv[]) {
    //time_t begin = time(NULL), end = time(NULL);
    srand((unsigned int)time(NULL));
    // read the inputs
    _input = (SAMPLE*)calloc(1000, sizeof(SAMPLE));
    //input = createMatrix(1000,3);
    scanf("%d\n", &nrOfClusters);
    readCSV();
    findInputRange();
    vq();
    lvq();
}
