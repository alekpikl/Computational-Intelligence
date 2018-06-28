//
//  fuzzy_1.c
//  CI
//
//  Created by Alek  on 17/02/2018.
//  Copyright © 2018 private. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define D 0.16

struct robot {
    double X_r, Y_r, Theta_r; // r - robot
    double X_c, Y_c, Theta_c; // c - current position estimated by the sensors
    double err_de;
    double err_thetae;
};

double k_de[4] = {0.25,0.25,0.50,0.50};
double k_te[4] = {0.12,0.25,0.12,0.25};

struct output {
    double vl, vr;
};

struct membership_fnc {
    double mu_s_de, mu_l_de, mu_s_te, mu_l_te;
};

double min(double a, double b){
    if (a < b)
        return a;
    return b;
}

double max(double a, double b){
    if (a > b)
        return a;
    return b;
}

int main(int argc, char* argv[]) {
    // read in
    struct robot Robot[1000];
    struct membership_fnc Mu[1000];
    struct output Output[1000];
    float v_r = 0.0, omega_r = 0.0;
    
    int index = 0;
    while (scanf(" %lf,%lf,%lf", &Robot[index].X_r, &Robot[index].Y_r, &Robot[index].Theta_r) != EOF) {
        if (index == 0) {
            Robot[0].X_c = Robot[0].X_r;
            Robot[0].Y_c = Robot[0].Y_r;
            Robot[0].Theta_c = Robot[0].Theta_c;
        }
        Robot[index].err_de = sqrtf((Robot[index].X_r - Robot[index].X_c)*(Robot[index].X_r - Robot[index].X_c) + (Robot[index].Y_r - Robot[index].Y_c)*(Robot[index].Y_r - Robot[index].Y_c));
        Robot[index].err_thetae = Robot[index].Theta_r - Robot[index].Theta_c;
        
        Mu[index].mu_s_de = min(max(0, (Robot[index].err_de - 80.0)/-160.0), 1);
        Mu[index].mu_l_de = 1 - Mu[index].mu_s_de;
        
        Mu[index].mu_s_te = (-fabs(Robot[index].err_thetae) + 180.0 ) / 180.0;
        Mu[index].mu_l_te = 1 - Mu[index].mu_s_te;
        
        double out_tmp[2] = {0};
        Output[index].vl = 0.0;
        Output[index].vr = 0.0;
        
        for (int j = 0; j < 4; ++j) {
            out_tmp[0] = k_de[j] * Robot[index].err_de + k_te[j] * Robot[index].err_thetae;
            out_tmp[1] = k_de[j] * Robot[index].err_de - k_te[j] * Robot[index].err_thetae;
            double w[2] = {0.0};
            switch (j) {
                case 0:
                    w[0] = Mu[index].mu_s_de;
                    w[1] = Mu[index].mu_s_te;
                    break;
                case 1:
                    w[0] = Mu[index].mu_s_de;
                    w[1] = Mu[index].mu_l_te;
                    break;
                case 2:
                    w[0] = Mu[index].mu_l_de;
                    w[1] = Mu[index].mu_s_te;
                    break;
                case 3:
                    w[0] = Mu[index].mu_l_de;
                    w[1] = Mu[index].mu_l_te;
                    break;
            }
            Output[index].vl += w[0] * w[1] * out_tmp[0];
            Output[index].vr += w[0] * w[1] * out_tmp[1];
        }
        // Kinematics
        v_r = (Output[index].vr + Output[index].vl) / 2.0 ;
        omega_r = (Output[index].vl - Output[index].vr) / D ;
        
        Robot[index+1].X_c = Robot[index].X_c + v_r * cos(Robot[index].Theta_c);
        Robot[index+1].Y_c = Robot[index].Y_c + v_r * sin(Robot[index].Theta_c);
        Robot[index+1].Theta_c = Robot[index].Theta_c + omega_r;
        
        ++index;
        printf("%lf,%lf,%lf\n",Robot[index].X_c,Robot[index].Y_c,Robot[index].Theta_c);
    }
    return 0;
}

