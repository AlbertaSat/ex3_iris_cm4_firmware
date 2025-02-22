#ifndef TIMING_H
#define TIMING_H_H

#define TIME_SYNC_DELAY_NS 2000000000 // 2Sec
#define TIME_SYNC_LOOP_MAX 1000


int get_time_seconds(void);
void set_time_seconds(double setTime);
uint64_t time_sync(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer);

#endif //TIMING_H_H