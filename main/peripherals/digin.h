#ifndef DIGIN_H_INCLUDED
#define DIGIN_H_INCLUDED

typedef enum {
    DIGIN_IN1=0, DIGIN_IN2, DIGIN_IN3, DIGIN_IN4, DIGIN_IN5, DIGIN_IN6, DIGIN_IN7
} digin_t ;

#define OBLO_APERTO DIGIN_IN7
#define EMERGENZA_STOP DIGIN_IN6
#define FLUSSO_ARIA DIGIN_IN5
#define ALLARME_INVERTER DIGIN_IN4
#define GETTONE1 DIGIN_IN3
#define FILTRO_APERTO DIGIN_IN2
#define BLOCCO_BRUCIATORE DIGIN_IN1

void digin_init(void);
int digin_get(digin_t digin);
int digin_take_reading(void);
unsigned int digin_get_inputs(void);

#endif  /* DIGIN_H_INCLUDED */
