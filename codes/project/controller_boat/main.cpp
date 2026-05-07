#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/sync.h"
#include "config.h"
#include "generic_functions.h"
#include "ILI9341_function.h"
#include "RF24.h"

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};

//#define DEBUG_EN
//#define SHOW_ONLY_TX_FAIL // comment this define if you want only see when the transmission fail
//#define DISABLE_RF // comment to enable transmission via RF
//#define EN_DEMO // comment this define if you want to override the demo of display

volatile int32_t encoderCount = 0;
volatile int8_t encoderDirection = 0;
volatile uint8_t lastEncoded = 0;

static const int8_t enc_table [16] = 
{
    0,-1, 1, 0,
    1, 0, 0,-1,
    -1, 0, 0, 1,
    0, 1,-1, 0
};

void encoder_isr (uint gpio, uint32_t events) {
    uint8_t encoded = (gpio_get (ENCODER_CLK_PIN) << 1) | gpio_get (ENCODER_DT_PIN);
    uint8_t sum = (lastEncoded << 2) | encoded;
    encoderCount += enc_table [sum];
    encoderDirection = enc_table [sum];
    lastEncoded = encoded;
}

int main() {

    #ifdef DEBUG_EN
      stdio_init_all();
      sleep_ms(2000);
      printf ("Ready");
    #endif
    

    #ifndef DISABLE_RF
        if (!radio.begin()) { // if the RF module is not connected
          #ifdef DEBUG_EN
            printf("NRF24 not found\n");
          #endif
          while (1);
        }

        #ifdef DEBUG_EN
          printf("NRF24 found!\n");
        #endif

        radio.setChannel(CHANNEL_RF);
        radio.setDataRate(RF24_250KBPS); // for RF24_PA_MAX set RF24_250KBPS, not RF24_1MBPS
        radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

        radio.enableAckPayload();
        radio.enableDynamicPayloads();

        radio.openWritingPipe(address);
        radio.stopListening();

        #ifdef DEBUG_EN
          printf("TX ready...\n");
        #endif
    #endif


    adc_init ();    

    // pin init
    adc_gpio_init (CONTROLLER_BATTERY_PIN);
    adc_gpio_init (ANALOG_SPEED_PIN);
    gpio_init (F_B_SELECT_PIN);
    gpio_set_dir (F_B_SELECT_PIN, GPIO_IN);
    gpio_pull_up (F_B_SELECT_PIN);
    gpio_init (C_D_SELECT_PIN);
    gpio_set_dir (C_D_SELECT_PIN, GPIO_IN);
    gpio_pull_up (C_D_SELECT_PIN);
    gpio_init (O_S_SELECT_PIN);
    gpio_set_dir (O_S_SELECT_PIN, GPIO_IN);
    gpio_pull_up (O_S_SELECT_PIN);
    gpio_init (ENCODER_SW_PIN);
    gpio_set_dir (ENCODER_SW_PIN, GPIO_IN);
    gpio_init (ENCODER_DT_PIN);
    gpio_set_dir (ENCODER_DT_PIN, GPIO_IN);
    gpio_init (ENCODER_CLK_PIN);
    gpio_set_dir (ENCODER_CLK_PIN, GPIO_IN);

    lastEncoded = (gpio_get (ENCODER_CLK_PIN) << 1) | gpio_get (ENCODER_DT_PIN);

    gpio_set_irq_enabled_with_callback (
        ENCODER_CLK_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &encoder_isr
    );

    gpio_set_irq_enabled (
        ENCODER_DT_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );

    init_gui (spi0, FREQ_SPI, SCK_TFT, MOSI_TFT, ILI9341_CS, ILI9341_DC); 
    //sleep_ms (1000);   

    #ifndef DEBUG_EN
        #ifdef EN_DEMO
            if (gpio_get(ENCODER_SW_PIN)) // if the switch is not pressed start the demo
                demo ();
        #endif
    #endif


    uint16_t data_sent = 0x0000;
    uint32_t data_received = 0x000000;

    int32_t countSnapshot;

    uint32_t current_time, last_time_controller_battery_read;

    // RX side
    uint8_t current_angle_boat = 0, previous_angle_boat = 0;
    uint8_t current_battery_boat_level = 0, previous_battery_boat_level = 0;
    uint8_t current_level_speed_motor_left [2] = {0, 0}, previous_level_speed_motor_left [2] = {0, 0}; // level, direction
    uint8_t current_level_speed_motor_right [2] = {0, 0}, previous_level_speed_motor_right [2] = {0, 0}; // level, direction

    uint8_t current_speed_level, previous_speed_level, speed_direction, previous_speed_direction;
    uint8_t current_angle_setpoint = 0, previous_angle_setpoint = 0;
    uint8_t current_select_value [3], previous_select_value [3]; // SELECT_F_B; SELECT_C_D; SELECT_O_S;
    uint8_t current_level_controller_battery, previous_level_controller_battery;
    uint8_t command_reset_compass = 0;
    uint8_t wireless_status, connection_attempts = 0;

    uint8_t current_connection_attempts = 0, connection_status = true;


    /* ===== Start set initial value controller side ===== */

    // read the initial speed value
    speed_direction = gpio_get (F_B_SELECT_PIN);
    previous_speed_direction = speed_direction;
    previous_speed_level = 0;
    current_speed_level = read_speed_level ();
    draw_tachimeter_level (current_speed_level, previous_speed_level, speed_direction);
    previous_speed_level = speed_direction;

    // read the initial setpoint value
    current_select_value [INDEX_F_B] = gpio_get (F_B_SELECT_PIN);
    current_select_value [INDEX_C_D] = gpio_get (C_D_SELECT_PIN);
    current_select_value [INDEX_O_S] = gpio_get (O_S_SELECT_PIN);
    uint8_t value_select_index = (current_select_value [INDEX_F_B] == 0) ? SELECT_LEFT : SELECT_RIGHT;
    draw_select (SELECT_F_B, value_select_index); // in the init_gui the first value read
    previous_select_value [INDEX_F_B] = current_select_value [INDEX_F_B];
    value_select_index = (current_select_value [INDEX_C_D] == 0) ? SELECT_LEFT : SELECT_RIGHT;
    draw_select (SELECT_C_D, value_select_index); // in the init_gui the first value read
    previous_select_value [INDEX_C_D] = current_select_value [INDEX_C_D];
    value_select_index = (current_select_value [INDEX_O_S] == 0) ? SELECT_LEFT : SELECT_RIGHT;
    draw_select (SELECT_O_S, value_select_index); // in the init_gui the first value read
    previous_select_value [INDEX_O_S] = current_select_value [INDEX_O_S];

    // read the battery level
    previous_level_controller_battery = read_controller_battery_level ();
    last_time_controller_battery_read = to_ms_since_boot (get_absolute_time ());
    draw_level_controller_battery (previous_level_controller_battery);

    /* ===== End set initial value controller side ===== */

    draw_compass (current_angle_boat, previous_angle_boat); // tmp

    current_time = to_ms_since_boot (get_absolute_time ());

    for (;;) {

        /* ===== Start read speed value ===== */ 

        current_speed_level = read_speed_level ();
        if ((previous_speed_level != current_speed_level) || (previous_speed_direction != speed_direction)) {
            previous_speed_direction = speed_direction;
            draw_tachimeter_level (current_speed_level, previous_speed_level, speed_direction);
            previous_speed_level = current_speed_level;
        }

        /* ===== End read speed value ===== */ 
      
        /* ===== Start read select value ===== */  

        if (current_speed_level == SPEED_0_INDEX) {
            current_select_value [INDEX_F_B] = gpio_get (F_B_SELECT_PIN);
            current_select_value [INDEX_C_D] = gpio_get (C_D_SELECT_PIN);
            current_select_value [INDEX_O_S] = gpio_get (O_S_SELECT_PIN);
            if (previous_select_value [INDEX_F_B] != current_select_value [INDEX_F_B]) {
                uint8_t value_select_index = (current_select_value [INDEX_F_B] == 0) ? SELECT_LEFT : SELECT_RIGHT;
                draw_select (SELECT_F_B, value_select_index);
                previous_select_value [INDEX_F_B] = current_select_value [INDEX_F_B];
                speed_direction = current_select_value [INDEX_F_B];
            }
            if (previous_select_value [INDEX_C_D] != current_select_value [INDEX_C_D]) {
                uint8_t value_select_index = (current_select_value [INDEX_C_D] == 0) ? SELECT_LEFT : SELECT_RIGHT;
                draw_select (SELECT_C_D, value_select_index);
                previous_select_value [INDEX_C_D] = current_select_value [INDEX_C_D];
            }
            if (previous_select_value [INDEX_O_S] != current_select_value [INDEX_O_S]) {
                uint8_t value_select_index = (current_select_value [INDEX_O_S] == 0) ? SELECT_LEFT : SELECT_RIGHT;
                draw_select (SELECT_O_S, value_select_index);
                previous_select_value [INDEX_O_S] = current_select_value [INDEX_O_S];
            }   
        }

        /* ===== End read select value ===== */ 

        /* ===== Start read setpoint value ===== */ 

        if (gpio_get(ENCODER_SW_PIN) == false) { // reset encoderCount if (gpio_get(ENCODER_CLK_PIN) == false) { 
            //TODO fai in modo tale che se tieni premuto il pulsante dell'encoder e da open_loop passi a servo e poi di nuovo a open loop setti il compass sull'angolo del setpoint
            uint32_t irq = save_and_disable_interrupts();
            uint8_t reset_compass = false;
            if (gpio_get (O_S_SELECT_PIN) != 0) {
                while ((gpio_get(ENCODER_SW_PIN) == false) && (reset_compass == false)) {
                    // if, while the encoder switch is pressed and servo mode is selected:
                    // if the selection switch from servo to open loop and to servo we reset the compass
                    while (true) {
                        if (gpio_get(ENCODER_SW_PIN) == true) break;
                        if (gpio_get (O_S_SELECT_PIN) == 0) { // change from servo to open loop
                            reset_compass = true;
                            break;
                        }
                    }
                    if (reset_compass) draw_setpoint (current_angle_setpoint, previous_angle_setpoint, true);
                    while (reset_compass) {
                        if (gpio_get (O_S_SELECT_PIN) != 0) // change from open loop to servo
                            break;
                        if (gpio_get(ENCODER_SW_PIN) == true) {
                           reset_compass = false;
                           break;
                        }
                    }
                    
                    draw_setpoint (current_angle_setpoint, previous_angle_setpoint, false);
                }
            }
            if (reset_compass == false) {
                uint8_t value_select_index = (current_select_value [INDEX_O_S] == 0) ? SELECT_LEFT : SELECT_RIGHT;
                encoderCount = (value_select_index == SELECT_LEFT) ? 0 : ((current_angle_boat) << 2); //TODO fai in modo che al reset si allinei con il compass
                lastEncoded = 0;
            }
            while (gpio_get(ENCODER_SW_PIN) == false);
            command_reset_compass = reset_compass;
            restore_interrupts(irq);
        }   
        int32_t encoderSnapshot;
        uint32_t irq = save_and_disable_interrupts();
        encoderSnapshot = encoderCount;
        countSnapshot = (encoderSnapshot >> 2); // encoderCount / 4
        countSnapshot = ((abs (encoderSnapshot) % 4) >= 2) ? ((encoderSnapshot > 0) ? (countSnapshot + 1) : (countSnapshot - 1)) : countSnapshot;
        countSnapshot %= COEFF_SCALE_ANGLE;
        countSnapshot = (countSnapshot < 0) ? (COEFF_SCALE_ANGLE + countSnapshot) : countSnapshot;
        current_angle_setpoint = countSnapshot; 
        if (current_select_value [INDEX_O_S] == 0) { // open loop mode, check if the setpoint is in the range from -45° (305°) to 45°
            if ((current_angle_setpoint < MIN_ANGLE_INDEX_OPEN_LOOP_MODE) && (current_angle_setpoint > HALF_ANGLE_INDEX)) {
                current_angle_setpoint = MIN_ANGLE_INDEX_OPEN_LOOP_MODE;
                encoderCount = (MIN_ANGLE_INDEX_OPEN_LOOP_MODE << 2);
            }
            else if ((current_angle_setpoint > MAX_ANGLE_INDEX_OPEN_LOOP_MODE) && (current_angle_setpoint <= HALF_ANGLE_INDEX)) {
                current_angle_setpoint = MAX_ANGLE_INDEX_OPEN_LOOP_MODE;
                encoderCount = (MAX_ANGLE_INDEX_OPEN_LOOP_MODE << 2);
            } 
        }
        restore_interrupts(irq);
        if (previous_angle_setpoint != current_angle_setpoint) {
            draw_setpoint (current_angle_setpoint, previous_angle_setpoint, false);
            previous_angle_setpoint = current_angle_setpoint;
        }

        /* ===== End read setpoint value ===== */  

        /* ===== Start read battery value ===== */ 

        if ((to_ms_since_boot (get_absolute_time ()) - last_time_controller_battery_read) >= TIME_CONTROLLER_BATTERY) {
            current_level_controller_battery = read_controller_battery_level ();
            if (previous_level_controller_battery != current_level_controller_battery) {
                draw_level_controller_battery (current_level_controller_battery);
                previous_level_controller_battery = current_level_controller_battery;
            }
            last_time_controller_battery_read = to_ms_since_boot (get_absolute_time ());
        }

        /* ===== End read battery value ===== */  
        
        data_sent = 
              DATA_VALID_TX |
              ((command_reset_compass & RESET_COMPASS_BOAT_MASK) << RESET_COMPASS_BOAT_SHIFT) |
              ((current_select_value [INDEX_O_S] & OPEN_LOOP_SERVO_MASK) << OPEN_LOOP_SERVO_SHIFT) |
              ((current_select_value [INDEX_C_D] & COMMON_DIFF_MASK) << COMMON_DIFF_SHIFT) |
              ((current_select_value [INDEX_F_B] & DIRECTION_MASK) << DIRECTION_SHIFT) |
              ((current_speed_level & SPEED_MASK) << SPEED_SHIFT) |
              (current_angle_setpoint & ANGLE_TO_DO_MASK);  

        command_reset_compass = false;


        #ifndef DISABLE_RF
            uint8_t success_sent = radio.write(&data_sent, sizeof(data_sent));

            if (success_sent) {
                if (radio.isAckPayloadAvailable()) {
                  radio.read(&data_received, sizeof(data_received));
                  current_angle_boat = (data_received & ANGLE_BOAT_MASK);
                  current_battery_boat_level = ((data_received & BATTERY_BOAT_MASK) >> BATTERY_BOAT_SHIFT);
                  current_level_speed_motor_left [0] = ((data_received & MOTOR_LEFT_SPEED_MASK) >> MOTOR_LEFT_SPEED_SHIFT);
                  current_level_speed_motor_left [1] = ((data_received & MOTOR_LEFT_DIR_MASK) >> MOTOR_LEFT_DIR_SHIFT);
                  current_level_speed_motor_right [0] = ((data_received & MOTOR_RIGHT_SPEED_MASK) >> MOTOR_RIGHT_SPEED_SHIFT);
                  current_level_speed_motor_right [1] = ((data_received & MOTOR_RIGHT_DIR_MASK) >> MOTOR_RIGHT_DIR_SHIFT);
                  if (previous_battery_boat_level != current_battery_boat_level) {
                    draw_battery_level (current_battery_boat_level, previous_battery_boat_level);
                    previous_battery_boat_level = current_battery_boat_level;
                  }
                  if (previous_angle_boat != current_angle_boat) {
                    draw_compass (current_angle_boat, previous_angle_boat);
                    previous_angle_boat = current_angle_boat;
                  }
                  if ((previous_level_speed_motor_left [0] != current_level_speed_motor_left [0]) || (previous_level_speed_motor_left [1] != current_level_speed_motor_left [1])) {
                    draw_level_speed_motor_left (current_level_speed_motor_left [0], current_level_speed_motor_left [1]);
                    previous_level_speed_motor_left [0] = current_level_speed_motor_left [0];
                    previous_level_speed_motor_left [1] = current_level_speed_motor_left [1];
                  }
                  if ((previous_level_speed_motor_right [0] != current_level_speed_motor_right [0]) || (previous_level_speed_motor_right [1] != current_level_speed_motor_right [1])) {
                    draw_level_speed_motor_right (current_level_speed_motor_right [0], current_level_speed_motor_right [1]);
                    previous_level_speed_motor_right [0] = current_level_speed_motor_right [0];
                    previous_level_speed_motor_right [1] = current_level_speed_motor_right [1];
                  }
                  current_connection_attempts = 0;
                  if (!connection_status) {
                    connection_status = true;
                    draw_wireless_logo (connection_status);
                  }
                }
              } 

              else {
                #ifdef DEBUG_EN
                  //printf("TX FAIL (MAX_RT)\n");
                #endif
                data_received = 0x0000;
                radio.flush_tx();
                current_connection_attempts = 
                    (current_connection_attempts == MAX_CONNECTION_ATTEMPTS) ? current_connection_attempts : (current_connection_attempts + 1);
                if ((current_connection_attempts == MAX_CONNECTION_ATTEMPTS) && (connection_status == true)) {
                    connection_status = false;
                    draw_wireless_logo (connection_status);
                }
            }   
            #ifdef DEBUG_EN 
              #ifndef SHOW_ONLY_TX_FAIL

                  if (previous_angle_boat != current_angle_boat) {
                      printf ("***\n");
                      printf("angle: %d\n", current_angle_boat);
                      printf ("***\n\n\n");
                      previous_angle_boat = current_angle_boat;
                  }


                  if (previous_battery_boat_level != current_battery_boat_level) {
                      printf ("***\n");
                      printf("batt: %d\n", current_battery_boat_level);
                      printf ("***\n\n\n");
                      previous_battery_boat_level = current_battery_boat_level;
                  }


                  if (previous_level_speed_motor_left [0] != current_level_speed_motor_left [0]) {
                      printf ("***\n");
                      printf("speed_l: %d\n", current_level_speed_motor_left [0]);
                      printf ("***\n\n\n");
                      previous_level_speed_motor_left [0] = current_level_speed_motor_left [0];
                  }


                  if (previous_level_speed_motor_left [1] != current_level_speed_motor_left [1]) {
                      printf ("***\n");
                      printf("dir_l: %d\n", current_level_speed_motor_left [1]);
                      printf ("***\n\n\n");
                      previous_level_speed_motor_left [1] = current_level_speed_motor_left [1];
                  }


                  if (previous_level_speed_motor_right [0] != current_level_speed_motor_right [0]) {
                      printf ("***\n");
                      printf("speed_r: %d\n", current_level_speed_motor_right [0]);
                      printf ("***\n\n\n");
                      previous_level_speed_motor_right [0] = current_level_speed_motor_right [0];
                  }


                  if (previous_level_speed_motor_right [1] != current_level_speed_motor_right [1]) {
                      printf ("***\n");
                      printf("speed_r: %d\n", current_level_speed_motor_right [1]);
                      printf ("***\n\n\n");
                      previous_level_speed_motor_right [1] = current_level_speed_motor_right [1];
                  }
              #endif
            #endif  
        #endif
        uint32_t time_to_sleep = to_ms_since_boot (get_absolute_time ()) - current_time;
        if (time_to_sleep <= TIME_UPDATE_TRANSMISSION)
            sleep_ms (TIME_UPDATE_TRANSMISSION - time_to_sleep);
        current_time = to_ms_since_boot (get_absolute_time ()); 


    }
}


/*
draw_tachimeter_level = 2 ms
draw_setpoint = 0 ms
draw_select = 4 ms
draw_select = 5 ms
draw_select = 4 ms
draw_level_controller_battery = 2 ms
draw_wireless_logo = 165 ms (non used in sum of delay)
draw_battery_level = 1 ms
draw_compass = 6 ms
draw_level_speed_motor_left = 2 ms
draw_level_speed_motor_right = 2 ms

total (without draw_wireless_logo) ~ 30 ms
*/

/*
        current_angle_boat = (data_received & ANGLE_BOAT_MASK);
        current_battery_boat_level = (data_received & BATTERY_BOAT_MASK) >> BATTERY_BOAT_SHIFT;
        current_level_speed_motor_right [0] = (data_received & MOTOR_RIGHT_SPEED_MASK) >> MOTOR_RIGHT_SPEED_SHIFT;
        current_level_speed_motor_right [1] = (data_received & MOTOR_RIGHT_DIR_MASK) >> MOTOR_RIGHT_DIR_SHIFT;
        current_level_speed_motor_left [0] = (data_received & MOTOR_LEFT_SPEED_MASK) >> MOTOR_LEFT_SPEED_SHIFT;
        current_level_speed_motor_left [1] = (data_received & MOTOR_LEFT_DIR_MASK) >> MOTOR_LEFT_DIR_SHIFT;

        data_to_send = 
            ((current_select_value [2] & OPEN_LOOP_SERVO_MASK) << OPEN_LOOP_SERVO_SHIFT) |
            ((current_select_value [1] & COMMON_DIFF_MASK) << COMMON_DIFF_SHIFT) |
            ((current_select_value [0] & DIRECTION_MASK) << DIRECTION_SHIFT) |
            ((current_speed_level & SPEDD_MASK) << SPEDD_SHIFT) |
            (current_angle_setpoint & ANGLE_TO_DO_MASK);
    */
