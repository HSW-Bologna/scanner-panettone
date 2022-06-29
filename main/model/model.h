#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "gel/timer/stopwatch.h"


#define MINUTI_MASSIMI_ASCIUGATURA 99
//#define TEMPERATURA_MASSIMA_ARIA   100


//#define PARS_SERIALIZED_SIZE         136
#define PARS_SERIALIZED_SIZE         141
#define PRIVATE_PARS_SERIALIZED_SIZE 3
#define PWOFF_SERIALIZED_SIZE        42
#define MAX_PARAMETER_CHUNK          17

#define GETTONIERA_DISABILITATA 0
#define GETTONIERA_NA           1
#define GETTONIERA_NC           2
#define GETTONIERA_INGRESSO     3


#define VISUALIZZAZIONE_GETTONE 0
#define VISUALIZZAZIONE_MONETA  1
#define VISUALIZZAZIONE_CASSA   2


typedef enum {
    MODELLO_MACCHINA_TEST = 0,          // 00

    MODELLO_MACCHINA_EDS_RE_SELF_CA,    // 01
    MODELLO_MACCHINA_EDS_RE_LAB_CA,
    MODELLO_MACCHINA_EDS_RG_SELF_CA,
    MODELLO_MACCHINA_EDS_RG_LAB_CA,
    MODELLO_MACCHINA_EDS_RV_SELF_CA,    // 05

    MODELLO_MACCHINA_EDS_RV_LAB_CA,     // 06
    MODELLO_MACCHINA_EDS_RE_SELF_CC,
    MODELLO_MACCHINA_EDS_RV_SELF_CC,
    MODELLO_MACCHINA_EDS_RE_LAB_CC,
    MODELLO_MACCHINA_EDS_RV_LAB_CC,     // 10

    MODELLO_MACCHINA_EDS_RP_SELF_CA,    // 11
    MODELLO_MACCHINA_EDS_RP_LAB_CA,
    MODELLO_MACCHINA_EDS_RP_SELF_CC,
    MODELLO_MACCHINA_EDS_RP_LAB_CC,     // 14

    MODELLO_MACCHINA_EDS_RE_LAB_TH_CA,  // 15
    MODELLO_MACCHINA_EDS_RG_LAB_TH_CA,
    MODELLO_MACCHINA_EDS_RV_LAB_TH_CA,
    MODELLO_MACCHINA_EDS_RE_LAB_TH_CC,
    MODELLO_MACCHINA_EDS_RV_LAB_TH_CC,  // 19
} modello_macchina_t;


typedef enum {
    CICLO_CALDO,
    CICLO_MEDIO,
    CICLO_TIEPIDO,
    CICLO_FREDDO,
    CICLO_LANA,
    NUM_CICLI,
    CICLO_NESSUNO,
} tipo_ciclo_t;

typedef struct {
    uint8_t lingua;
    uint8_t lingua_max;
    uint8_t logo_ditta;
    uint8_t abilita_visualizzazione_temperatura;
    uint8_t abilita_tasto_menu;
    uint8_t abilita_visualizzazione_cicli_totali;
    uint8_t abilita_parametri_ridotti;
    
    uint8_t modello_macchina;
    uint8_t abilita_autoavvio;
    uint8_t tipo_pausa_asciugatura;
    uint8_t abilita_gas;
    uint8_t numero_tentativi_reset_gas;
    uint8_t abilita_reset_gas_esteso;
    uint8_t tempo_flusso_aria_blocco_gas;   // new TODO
    uint8_t abilita_blocco_no_aria;
    
    uint8_t allarme_inverter_off_on;
    uint8_t allarme_filtro_off_on;
    uint8_t emergenza_na_nc;
    uint8_t macchina_libera_off_on;
    uint8_t tipo_out_macchina_occupata;
    
    uint8_t tempo_uscita_pagine;
    uint8_t tempo_reset_lingua;
    uint8_t tempo_azzeramento_ciclo_pausa;
    uint8_t tempo_azzeramento_ciclo_stop;
    uint8_t abilita_stop_tempo_ciclo;
    uint8_t tempo_attesa_azzeramento_ciclo;
    
    uint8_t sonda_temperatura_in_out;
    uint8_t temperatura_max_1_in;
    uint8_t temperatura_sicurezza_1;
    uint8_t temperatura_max_1_out;
    uint8_t temperatura_sicurezza_1_out;
    uint8_t tempo_allarme_temperatura_1;
    uint8_t tempo_allarme_no_umidita;       // new TODO
    uint8_t tempo_antigelo;
    uint8_t temperatura_stop_tempo_ciclo;
    uint8_t percentuale_anticipo_temperatura_ventola;
    uint8_t percentuale_velocita_min_ventola;
    uint8_t temperatura_raffreddo_allarme;
    uint8_t tempo_allarme_flusso_aria;
    
    uint8_t velocita_min_lavoro;
    uint8_t velocita_max_lavoro;
    
    uint8_t abilita_gettoniera;
    uint8_t numero_gettoni_consenso;
    uint8_t tempo_gettone_1;
    uint8_t tempo_gettone_min_sec;
    uint8_t tipo_visualizzazione_get_mon_cas;
    
    uint8_t numero_cicli_manutenzione;
    uint8_t tempo_durata_avviso_manutenzione;
    uint8_t tempo_cadenza_avviso_manutenzione;

    uint8_t tempo_ritardo_antipiega;
    uint8_t tempo_giro_antipiega;
    uint8_t tempo_pausa_antipiega;
    uint8_t tempo_cadenza_antipiega;
    uint8_t velocita_antipiega;
    uint8_t tempo_max_antipiega;
    uint8_t numero_cicli_max_antipiega;
    
    uint8_t tempo_attesa_partenza_ciclo;
} parmac_t;

typedef struct {
    uint8_t tipo_asciugatura_m_a;
    uint8_t tempo_durata_asciugatura;
    uint8_t abilita_attesa_temperatura;
    uint8_t abilita_inversione_asciugatura;
    uint8_t tempo_giro_asciugatura;
    uint8_t tempo_pausa_asciugatura;
    uint8_t velocita_asciugatura;
    uint8_t temperatura_aria_1; 
    
    uint8_t offset_temperatura_aria_alto;
    uint8_t offset_temperatura_aria_basso;
    uint8_t umidita_residua_dry_auto;
    
    uint8_t abilita_raffreddamento;
    uint8_t tempo_durata_raffreddamento;
    uint8_t abilita_inversione_raffreddamento;
    uint8_t tempo_giro_raffreddamento;
    uint8_t tempo_pausa_raffreddamento;
    
    uint8_t abilita_antipiega;
} parciclo_t;

typedef struct {
    uint8_t contrasto;
} parametri_riservati_t;

typedef enum {
    STATO_STOPPED = 0,
    STATO_PAUSE,
    STATO_WORK,
} stato_t;

typedef enum {
    STATO_STEP_NUL = 0,

    STATO_STEP_ASC,
    STATO_STEP_RAF,
    STATO_STEP_ANT,
} stato_step_t;

typedef struct {
    uint32_t tempo_attivita;
    uint32_t tempo_lavoro;
    uint32_t tempo_moto;
    uint32_t tempo_ventilazione;
    uint32_t tempo_riscaldamento;
    
    uint16_t credito;
    uint16_t cicli_parziali;
    uint16_t cicli_totali;
    uint16_t gettoni;
    
    uint16_t delta_temperatura;
    uint16_t delta_velocita;
} pwoff_data_t;

typedef struct {
    uint8_t  inputs;
    uint8_t  outputs;
    uint16_t ptc_adc;
    int      ptc_temperature;
    int      sht_temperature;
    int      sht_umidity;
    size_t   parchunk;

    int     pwm1;
    int     pwm2;
    uint8_t lingua_temporanea;


    parmac_t              pmac;
    parciclo_t            pciclo[NUM_CICLI];
    parciclo_t            ciclo_corrente;
    parametri_riservati_t hsw;

    struct {
        tipo_ciclo_t ciclo;
        stato_t      stato;
        uint8_t      sottostato;
        uint16_t     temperatura_rilevata;
        stato_step_t stato_step;
        uint8_t      f_in_test;
        uint8_t      f_no_gt_all;
        uint8_t      f_start_ok;
        uint8_t      f_ok_gettone;
        uint8_t      f_ventilazione;
        uint8_t      f_ava_ind;
        uint8_t      nf_ava_ind;
        uint8_t      f_anti_piega;
        uint8_t      nf_anti_piega;
        uint8_t      ct_anti_piega_max;
        uint8_t      cnro_c_anti_piega_max;
        uint8_t      f_all;
        uint8_t      n_allarme;
        uint8_t      n_old_allarme;
        uint8_t      f_pwoff;
        uint8_t      f_errore_ram_ko;
        uint8_t      f_all_pw_off;
        uint8_t      f_all_emergenza;
        uint8_t      f_all_inverter;
        uint8_t      f_all_filtro_aperto;
        uint8_t      f_all_flusso_aria;
        uint8_t      f_all_anomalia_aria;
        uint8_t      f_all_sovratemperatura;
        uint8_t      f_all_blocco_bruciatore;
        stopwatch_t  tempo_asciugatura;
    } status;

    struct {
        uint8_t  used_percentage;
        uint8_t  frag_percentage;
        uint32_t low_watermark;
    } lvgl_mem;

    pwoff_data_t pwoff;
} model_t;

void               model_init(model_t *pmodel);
char              *model_get_output_status(model_t *pmodel, int output);
size_t             model_pars_serialize(model_t *pmodel, uint8_t buff[static PARS_SERIALIZED_SIZE]);
size_t             model_pars_deserialize(model_t *pmodel, uint8_t *buff);
size_t             model_pwoff_serialize(model_t *pmodel, uint8_t buff[static PWOFF_SERIALIZED_SIZE]);
size_t             model_pwoff_deserialize(model_t *pmodel, uint8_t *buff);
void               model_get_pwoff(model_t *pmodel);
void               model_mem_data(model_t *pmodel, lv_mem_monitor_t *mem);
size_t             model_get_lingua(model_t *pmodel);
void               model_cambia_lingua(model_t *pmodel);
stato_t            model_get_stato(model_t *pmodel);
void               model_cambia_stato(model_t *pmodel, int res);
unsigned long      model_get_stato_timer(model_t *pmodel);
parciclo_t        *model_ciclo_corrente(model_t *pmodel);
void               model_set_status_stopped(model_t *p);
int                model_get_status_stopped(model_t *p);
int                model_get_status_not_stopped(model_t *p);
void               model_set_status_pause(model_t *p);
int                model_get_status_pause(model_t *p);
int                model_get_status_not_pause(model_t *p);
void               model_set_status_work(model_t *p);
int                model_get_status_work(model_t *p);
int                model_get_status_not_work(model_t *p);
void               model_set_status_step_nul(model_t *p);
void               model_set_status_step_asc(model_t *p);
void               model_set_status_step_raf(model_t *p);
void               set_status_step_ant(model_t *p);
int                model_get_status_step(model_t *p);
int                model_is_machine_selected(model_t *pmodel);
modello_macchina_t model_get_machine_model(model_t *pmodel);
uint8_t            model_get_logo_ditta(model_t *pmodel);
void               model_reset_parameters(model_t *pmodel);
void               model_init_parametri_ciclo(model_t *pmodel);
int                model_is_in_test(model_t *pmodel);
int                model_not_in_test(model_t *pmodel);
size_t             model_private_parameters_deserialize(model_t *pmodel, uint8_t *buff);
size_t       model_private_parameters_serialize(model_t *pmodel, uint8_t buff[static PRIVATE_PARS_SERIALIZED_SIZE]);
void         model_add_second(model_t *pmodel);
int          model_get_riscaldamento_attivo(model_t *pmodel);
unsigned int model_secondi_durata_asciugatura(model_t *pmodel);
void         model_aggiungi_gettoni(model_t *pmodel, unsigned int gettoniera, unsigned int ingresso);
int          model_consenso_raggiunto(model_t *pmodel);
int          model_tempo_lavoro_scaduto(model_t *pmodel, unsigned long ts);
void         model_metti_tempo_lavoro_in_pausa(model_t *pmodel, unsigned long ts);
void         model_comincia_antipiega(model_t *pmodel);
void         model_comincia_raffreddamento(model_t *pmodel);
void         model_azzera_credito(model_t *pmodel);
void         model_fine_ciclo(model_t *pmodel);
int          model_in_antipiega(model_t *pmodel);
void         model_seleziona_ciclo(model_t *pmodel, tipo_ciclo_t ciclo);
uint16_t     model_velocita_ciclo(model_t *pmodel);
int          model_temperatura_aria_ciclo(model_t *pmodel);
int          model_get_temperatura_corrente(model_t *pmodel);
int          model_ciclo_selezionato(model_t *pmodel);
void         model_modifica_durata_asciugatura(model_t *pmodel, int minuti);
void         model_modifica_temperatura_aria(model_t *pmodel, int gradi);
void         model_modifica_velocita(model_t *pmodel, int giri);
int          model_modifica_abilitata(model_t *pmodel);
int model_allarme_emergenza(model_t *pmodel, int emergenza);

#endif
