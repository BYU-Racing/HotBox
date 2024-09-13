#include <HotBox.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <FlexCAN_T4.h>


HotBox::HotBox() {
    wheel = 1;
}

HotBox::HotBox(int inWheel) {
    wheel = inWheel;
}

void HotBox::start() {
    if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        Serial.println("CAMERA NOT FOUND");
        while(1) delay(10); //keep trying
    }

    mlx.setMode(MLX90640_CHESS);

    mlx.setResolution(MLX90640_ADC_18BIT);

    mlx.setRefreshRate(MLX90640_2_HZ);

    
    can.begin();
    can.setBaudRate(250000);
}

bool HotBox::readyToCheck() {
    return (millis() - lastCheck >= waitTime);
}

void HotBox::readValues() {
    if(!readyToCheck()) {
        return;
    }

    lastCheck = millis();

    if(mlx.getFrame(frame) != 0) {
        Serial.println("FAILED TO GET NEW FRAME");
        return;
    }

    calculateTemps();
    sendCAN();
}

void HotBox::calculateTemps() {
    float innerSum = 0.0f, midSum = 0.0f, outerSum = 0.0f;
    
    // Counts for each slice
    int innerCount = 0, midCount = 0, outerCount = 0;

    // Loop through each row (24 rows)
    for (int row = 0; row < 24; ++row) {
        // Outer slice (columns 0 to 3 and 29 to 31)
        for (int col = 0; col <= 3; ++col) {
            outerSum += frame[row * 32 + col];
            ++outerCount;
        }
        for (int col = 29; col <= 31; ++col) {
            outerSum += frame[row * 32 + col];
            ++outerCount;
        }

        // Mid slice (columns 4 to 10 and 22 to 28)
        for (int col = 4; col <= 10; ++col) {
            midSum += frame[row * 32 + col];
            ++midCount;
        }
        for (int col = 22; col <= 28; ++col) {
            midSum += frame[row * 32 + col];
            ++midCount;
        }

        // Inner slice (columns 11 to 21)
        for (int col = 11; col <= 21; ++col) {
            innerSum += frame[row * 32 + col];
            ++innerCount;
        }
    }

    // Check for division by zero (though it shouldn't happen)
    if (innerCount > 0) {
        innerTemp = innerSum / innerCount;
    } else {
        innerTemp = 0.0f;  // Handle edge case
    }

    if (midCount > 0) {
        midTemp = midSum / midCount;
    } else {
        midTemp = 0.0f;  // Handle edge case
    }

    if (outerCount > 0) {
        outerTemp = outerSum / outerCount;
    } else {
        outerTemp = 0.0f;  // Handle edge case
    }

    return;
}



void HotBox::sendCAN() {
    msg.id = 49 + wheel; // Placeholder ID

    int16_t scaledInnerTemp = static_cast<int16_t>(innerTemp * 100);
    int16_t scaledMidTemp = static_cast<int16_t>(midTemp * 100);
    int16_t scaledOuterTemp = static_cast<int16_t>(outerTemp * 100);

    msg.buf[0] = scaledInnerTemp & 0xFF;        // Lower byte of innerTemp
    msg.buf[1] = (scaledInnerTemp >> 8) & 0xFF; // Upper byte of innerTemp

    msg.buf[2] = scaledMidTemp & 0xFF;          // Lower byte of midTemp
    msg.buf[3] = (scaledMidTemp >> 8) & 0xFF;   // Upper byte of midTemp

    msg.buf[4] = scaledOuterTemp & 0xFF;        // Lower byte of outerTemp
    msg.buf[5] = (scaledOuterTemp >> 8) & 0xFF; // Upper byte of outerTemp

    // Send the CAN message
    can.write(msg);
}

