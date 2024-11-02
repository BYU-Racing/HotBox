#ifndef HOTBOX_H
#define HOTBOX_H
#include <Adafruit_MLX90640.h>
#include <FlexCAN_T4.h>

class HotBox {
    private:
        int wheel;
        Adafruit_MLX90640 mlx;
        float frame[32*24]; // Full buffer frame for the temperatures
        int waitTime = 500; //Assuming 2hz
        int lastCheck = 0;

        float innerTemp = 0;
        float midTemp = 0;
        float outerTemp = 0;

        FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16>* can;
        CAN_message_t msg;

        bool readyToCheck();

        void calculateTemps();

        void sendCAN();

    public:
        HotBox();
        HotBox(int inWheel);

        void start(FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16>* canIn);

        void readValues();
};

#endif