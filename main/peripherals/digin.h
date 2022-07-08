#ifndef DIGIN_H_INCLUDED
#define DIGIN_H_INCLUDED

#define DIGIN_RESET             DIGIN_IN1
#define DIGIN_STOP              DIGIN_IN2
#define DIGIN_START_CICLO_1     DIGIN_IN3
#define DIGIN_START_CICLO_2     DIGIN_IN4
#define DIGIN_MICRO_S_BASSO     DIGIN_IN5
#define DIGIN_MICRO_S_ALTO      DIGIN_IN6
#define DIGIN_MICRO_0_ROTATION  DIGIN_IN7


typedef enum {
    DIGIN_IN1 = 0,
    DIGIN_IN2,
    DIGIN_IN3,
    DIGIN_IN4,
    DIGIN_IN5,
    DIGIN_IN6,
} digin_t;


void         digin_init(void);
int          digin_get(digin_t digin);
int          digin_take_reading(void);
unsigned int digin_get_inputs(void);
unsigned int digin_read_pulses(void);
void         digin_clear_pulses(void);

#endif /* DIGIN_H_INCLUDED */
