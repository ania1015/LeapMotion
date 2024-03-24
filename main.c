
#include <iostream>
#include <cstring>
#include <windows.h>
#include <string>
#include <math.h>
#include "LeapC.h"
#include "header.h"
#include <math.h>
#include <stdio.h>



int64_t lastFrameID = 0; //The last frame received
uint32_t counter_extended = 0;
bool stop = 0;


double* vector_length(LEAP_HAND* hand, int  i, int bone_number);
double odchylenieXZ(LEAP_HAND* hand);

int main(int argc, char** argv) {

    double lengthZ[2] = { 0 };
    double length0[5] = { 0 };
    double length1[5] = { 0 };
    double length2[5] = { 0 };
    double length3[5] = { 0 };

    double *Results;

    double vectors[5][3][2] = { 0 };
    double vectors_0[5][2][2] = { 0 };
    double vectors_Z[2][2] = { 0 };

    double cos0[5] = { 0 };
    double cos1[5] = { 0 };
    double cos2[5] = { 0 };

    double cos_Z = 0;

    double teta1[5] = { 0 };
    double teta2[5] = { 0 };
    double teta3[5] = { 0 };


    float x2 = 0;
    float x1 = 0;
    float y2 = 0;
    float y1 = 0;
    float x0 = 0;
    float x3 = 0;
    float y3 = 0;
    float y0 = 0;
    float z1 = 0;
    float z2 = 0;
    
    DCB dcbSerialParams = {0};

    OpenConnection();
    while (!IsConnected)
        millisleep(100); //wait a bit to let the connection complete

    printf("Connected.");
    LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
    if (deviceProps)
        printf("Using device %s.\n", deviceProps->serial);
    printf("calibration start");
    //kalibracja
    do {
       
        LEAP_TRACKING_EVENT* frame0 = GetFrame();
        if (frame0 && (frame0->tracking_frame_id > lastFrameID)) {
            lastFrameID = frame0->tracking_frame_id;
           
            for (uint32_t h = 0; h < frame0->nHands; h++) {
                LEAP_HAND* hand1 = &frame0->pHands[h];
                
                for (uint32_t i = 0; i < 5; i++) {
                    
                    if (hand1->digits[i].is_extended) {
                        counter_extended++;
                        printf("\nnumber of extended fingers: %d", counter_extended);
                    }
                }
                if (counter_extended < 5 ) {
                    counter_extended = 0;
                }

                 if (counter_extended == 5){
                    stop = 1;
                    counter_extended = 0;
                }
            }
        }
    } while (stop != 1);

    // writefile wyslac ze wszystkie serva na 0

    lastFrameID = 0; //The last frame received

    WriteFile(hSerial, bytes_to_send, 5, 0, NULL);
    printf("kalibracja zakonczona");

    for (;;) {
        LEAP_TRACKING_EVENT* frame = GetFrame();

        if (frame && (frame->tracking_frame_id > lastFrameID)) {
            lastFrameID = frame->tracking_frame_id;
            printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);

            for (uint32_t h = 0; h < frame->nHands; h++) {
                LEAP_HAND* hand = &frame->pHands[h];
                
               
                printf("    Hand id %i is a %s hand with position (%f, %f, %f).\n",
                    hand->id,
                    (hand->type == eLeapHandType_Left ? "left" : "right"),
                    hand->palm.position.x,
                    hand->palm.position.y,
                    hand->palm.position.z);

                //robimy jedna reke wiec uruchamiamy jedna 
                if (hand->type == eLeapHandType_Right) {


                    printf("jest prawa ręka");


                    // liczymy długości kości,  czyli wektorow 
                    // dla kciuka xyz dla reszty xy 

                    //i kat pomiedzy metacarpal i proximal nie dla kciuka, wiec od i = 1
                    for (int i = 1; i < 5; i++) {
                        
                        Results = vector_length(hand, i, 0);

                        length0[i] = *Results;
                        vectors_0[i][0][0] = *(Results + 1);
                        vectors_0[i][0][1] = *(Results + 2);

                        Results = vector_length(hand, i, 1);

                        length1[i] = *Results;
                        vectors_0[i][1][0] = *(Results + 1); // palec to pierwszy wymiar 2 wymiar to kosc 3 wymiar to wspolrzedna x lub y
                        vectors_0[i][1][1] = *(Results + 2);


                        cos0[i] = acos((vectors_0[i][0][0] * vectors_0[i][1][0] + vectors_0[i][0][1] * vectors_0[i][1][1]) / (length0[i] * length1[i]));
                        cos0[i] = 360 * cos0[i] / (2 * 3.14);
                       // printf("\n katy miedzy metacarpal i proximal = %f", cos0[i]);

                        //czyszczenie tablicy
                        length1[i] = 0;
                    }



                    //bierzemy proximala dlugosc potem intermediate 
                    for (int i = 0; i < 5; i++) {


                        Results = vector_length(hand, i, 1);

                        length1[i] = *Results;
                        vectors[i][0][0] = *(Results + 1); // palec to pierwszy wymiar 2 wymiar to kosc 3 wymiar to wspolrzedna x lub y
                        vectors[i][0][1] = *(Results + 2);



                        Results = vector_length(hand, i, 2);

                        length2[i] = *Results;
                        vectors[i][1][0] = *(Results + 1);
                        vectors[i][1][1] = *(Results + 2);

                        // cosinusy między wektorami
                        cos1[i] = acos((vectors[i][0][0] * vectors[i][1][0] + vectors[i][0][1] * vectors[i][1][1]) / (length1[i] * length2[i]));
                        cos1[i] = 360 * cos1[i] / (2 * 3.14);
                        // printf("\n katy miedzy metacarpal i proximal= %f", cos1[i]);

                        Results = vector_length(hand, i, 3);

                        length3[i] = *Results;
                        vectors[i][2][0] = *(Results + 1);
                        vectors[i][2][1] = *(Results + 2);

                        cos2[i] = acos((vectors[i][1][0] * vectors[i][2][0] + vectors[i][1][1] * vectors[i][2][1]) / (length2[i] * length3[i]));
                        cos2[i] = 360 * cos2[i] / (2 * 3.14);
                        // printf("\n katy miedzy proximal i intermediate = %f", cos2[i]);
                        printf("\n numer palca : %d \n", i+1);
                        printf("\n katy 1: %f, 2: %f, 3: %f", cos0[i], cos1[i], cos2[i]);

                        x1 = hand->digits[i].bones[1].prev_joint.x;
                        x2 = hand->digits[i].bones[1].next_joint.x;
                        y1 = hand->digits[i].bones[1].prev_joint.y;
                        y2 = hand->digits[i].bones[1].next_joint.y;
                        x3 = hand->digits[i].bones[2].next_joint.x;
                        y3 = hand->digits[i].bones[2].next_joint.y;
                        x0 = hand->digits[i].bones[3].next_joint.x;
                        y0 = hand->digits[i].bones[3].next_joint.y;

                        teta2[i] = acos(((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) - length1[i] * length1[i] - length2[i] * length2[i]) / (2 * length1[i] * length2[i]));
                        teta3[i] = acos(((x0 - x2) * (x0 - x2) + (y0 - y2) * (y0 - y2) - length3[i] * length3[i] - length2[i] * length2[i]) / (2 * length3[i] * length2[i]));
                        //printf("\n kat alfa %f", atan(abs(y3 - y1) / abs(x3 - x1)));
                        //printf("\n kat beta %f", asin(length2[i] * sin(teta2[i]) / sqrt((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1))));

                        teta1[i] = atan((asin(length2[i] * sin(teta2[i]) / sqrt((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1)))) + atan(abs(y3 - y1) / abs(x3 - x1)));
                        teta1[i] = 360 * teta1[i] / (2 * 3.14);
                        teta2[i] = 360 * teta2[i] / (2 * 3.14);
                        teta3[i] = 360 * teta3[i] / (2 * 3.14);
                        printf("\n teta1: %f, teta2: %f, teta3: %f", teta1[i], teta2[i], teta3[i]);

                        if (i == 4) {
                            cos_Z = odchylenieXZ(hand);

                            printf("\n kat odchylenia kciuka = %f", cos_Z);
                        }
                        //dla kazdego palca po kolei ???
                       

                    }
                    //Writefile dla kciuka w xz?
                    for (int l = 0; l < 5; l++)
                    {
                        if (teta2[l]<= 80 && teta2[l]>=20) {
                            //lekko zwiniete
                            bytes_to_send[l] = 90;
                        }
                        if (teta2[l] > 80) {
                            //pozycja serwa tak ze piastka
                            bytes_to_send[l] = 179;
                        }
                        if (teta2[l]<20) {
                            //pozycja na 0
                            bytes_to_send[l] = 0;
                        }
                       // printf("\n pozycyja =%c dla l=%d ", bytes_to_send[l], l);
                    }
                }
                else{

                    printf("wrong hand");
                }
            }
        }

        millisleep(1000);
    } //ctrl-c to exit
    return 0;
}



double *vector_length(LEAP_HAND *hand, int  i, int bone_number) {

    double length;
    double difX;
    double difY;

    static double results[3];

    float x1, x2, y1, y2;

    x1 = hand->digits[i].bones[bone_number].prev_joint.x;
    x2 = hand->digits[i].bones[bone_number].next_joint.x;

    y1 = hand->digits[i].bones[bone_number].prev_joint.y;
    y2 = hand->digits[i].bones[bone_number].next_joint.y;

    length = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    difX = x2 - x1;
    difY = y2 - y1;

    results[0] = length;
    results[1] = difX;
    results[2] = difY;

    return results;
}
double odchylenieXZ(LEAP_HAND* hand) {

    //odchylenie kciuka na płaszczyźnie xz

    double x1, x2, z1, z2, lengthZ[2], vectors_Z[2][2], cos_Z;

    x1 = hand->digits[0].bones[1].prev_joint.x;
    x2 = hand->digits[0].bones[1].next_joint.x;

    z1 = hand->digits[0].bones[1].prev_joint.z;
    z2 = hand->digits[0].bones[1].next_joint.z;

    lengthZ[0] = sqrt((x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2));
    vectors_Z[0][0] = x2 - x1;
    vectors_Z[0][1] = z2 - z1;

    //wektor wzgledem ktorego liczymy odchylenie 
    //bedzie to metacarpal palca wskazujacego 

    x1 = hand->digits[1].bones[0].prev_joint.x;
    x2 = hand->digits[1].bones[0].next_joint.x;

    z1 = hand->digits[1].bones[0].prev_joint.z;
    z2 = hand->digits[1].bones[0].next_joint.z;

    lengthZ[1] = sqrt((x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2));
    vectors_Z[1][0] = x2 - x1;
    vectors_Z[1][1] = z2 - z1;

    cos_Z = acos((vectors_Z[0][0] * vectors_Z[1][0] + vectors_Z[0][1] * vectors_Z[1][1]) / (lengthZ[0] * lengthZ[1]));
    cos_Z = 360 * cos_Z / (2 * 3.14);
    
    return cos_Z;
} 
