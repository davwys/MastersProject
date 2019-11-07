// Main definitions


/*  ==========================
    Status Enum for game board
    ==========================

    READY       = ready for training or game start
    PRETRAINING   = connected to training dashboard, not training yet (serial port open - prevents game start)
    PREPLAYING   = connected to game dashboard, not playing yet (serial port open)
    TRAINING    = training new data via dashboard (sends sensor inputs to dashboard via specified port)
    UPLOAD      = ready to upload new training data (after training is finished)
    PLAYING     = actively playing the game (outputs API calls for game logic)
 */


enum status {
    READY,
    PRETRAINING,
    PREPLAYING,
    TRAINING,
    UPLOAD,
    PLAYING
};


/* ==================
   Physical pin setup
   ================== */

//LED Pins
int LED_Red = 3;
int LED_Green = 4;

/* =====================
   Logic variables setup
   ===================== */

String receivedData = "";       //Incoming Serial data string
status currentStatus = READY;   //Game board's current status
