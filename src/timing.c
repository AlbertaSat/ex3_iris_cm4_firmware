
#include "gpio.h"
#include "spi_iris.h"
#include "main.h"
#include "timing.h"

#include <sys/time.h>
#include <time.h>
#define TIME_SYNC_LOOP_MAX 1000



double get_time_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1.0e9;
}

void set_time_seconds(double setTime) {
    struct timespec ts;
    ts.tv_sec = setTime;
    ts.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &ts);
}


// Captures a 'reference time' from the Raspberry Pi Internal Clock to 'sync' the clock between
// the OBC and Iris
uint64_t time_sync(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer){

    bool event = false;
    int event_amt = EDGE_EVENT_BUFF_SIZE;
    uint64_t time_sync;
    struct gpiod_edge_event *event_str;

    // Clears out event buffer of any remaining events
    if (gpiod_line_request_wait_edge_events(request, 0)){
        // Clear event buffer by reading over it
        //*Limit MAX_ITERATIONS so it can't loop forever
        //! IF THE NUMBER OF EVENTS EQUALS THE BUFFER SIZE (EDGE_EVENT_BUFF_SIZE) THAN ON NEXT gpiod_lione_request_read_edge_events IT WILL GET STUCK
        for(int index = 0; (index < MAX_ITERATIONS) && (event_amt >= EDGE_EVENT_BUFF_SIZE); index++){
            event_amt = gpiod_line_request_read_edge_events(request, event_buffer, EDGE_EVENT_BUFF_SIZE);
        }
    }

    // Loop for 'TIME_SYNC_DELAY_NS' waiting for OBC to send pulse (SET LOW)
    event = gpiod_line_request_wait_edge_events(request, TIME_SYNC_DELAY_NS); 

    // Dont need to clear event buffer since no event occured
    if(event == false){
        return NULL;
    }

    event_amt = gpiod_line_request_read_edge_events(request, event_buffer, EDGE_EVENT_BUFF_SIZE);
    event_str = gpiod_edge_event_buffer_get_event(event_buffer, 0);
    time_sync = gpiod_edge_event_get_timestamp_ns(event_str);

    // Clears out event buffer of any remaining events
    event_amt = EDGE_EVENT_BUFF_SIZE;
    if (gpiod_line_request_wait_edge_events(request, 0)){
        // Clear event buffer by reading over it
        //*Limit MAX_ITERATIONS so it can't loop forever
        //! IF THE NUMBER OF EVENTS EQUALS THE BUFFER SIZE (EDGE_EVENT_BUFF_SIZE) THAN ON NEXT gpiod_lione_request_read_edge_events IT WILL GET STUCK
        for(int index = 0; (index < MAX_ITERATIONS) && (event_amt >= EDGE_EVENT_BUFF_SIZE); index++){
            event_amt = gpiod_line_request_read_edge_events(request, event_buffer, EDGE_EVENT_BUFF_SIZE);
        }
    }

    return time_sync;

}