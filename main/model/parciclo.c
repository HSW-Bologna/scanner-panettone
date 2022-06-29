#include <stdio.h>
#include "gel/parameter/parameter.h"
#include "model.h"
#include "parciclo.h"
#include "descriptions/AUTOGEN_FILE_parmac.h"

#define NUM_PARAMETERS 17
#define AL_USER 0x01
#define AL_TECH 0x02

static void    formatta(char *string, const void *arg);
static void    temperatura_aria_check_posizione_sonda(struct _parameter_handle_t *p, void *arg);
static uint8_t get_livello_accesso(uint8_t parametri_ridotti);

extern parameter_handle_t parameters[];

void parciclo_init(model_t *p, tipo_ciclo_t ciclo, int reset) {
#define FINT(i) ((parameter_user_data_t){parmac_descriptions[i], formatta, NULL, NULL})

    size_t j = 0, i = ciclo;
    // clang-format off
    /*                                 Tipo                  Puntatore                                      PMIN    PMAX    MIN  MAX   DEF   STEP  ACCESS       STRINGHE                                                    RUNTIME     ARG */
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tipo_asciugatura_m_a,               NULL,   NULL,   0,   1,    0,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TIPO_ASCIUGATURA_M_A),             NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_durata_asciugatura,           NULL,   NULL,   1,   MINUTI_MASSIMI_ASCIUGATURA,  35,    1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_TEMPO_DURATA_ASCIUGATURA),     NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].abilita_attesa_temperatura,         NULL,   NULL,   0,   1,    0,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_ABILITA_ATTESA_TEMPERATURA),       NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].abilita_inversione_asciugatura,     NULL,   NULL,   0,   1,    1,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_ABILITA_INVERSIONE_ASCIUGATURA),   NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_giro_asciugatura,             NULL,   NULL,   1,   99,   95,   1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_TEMPO_GIRO_ASCIUGATURA),           NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_pausa_asciugatura,            NULL,   NULL,   5,   99,   6,    1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_TEMPO_PAUSA_ASCIUGATURA),          NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].velocita_asciugatura,           &p->pmac.velocita_min_lavoro,   &p->pmac.velocita_max_lavoro,   0,   0,    55,    1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_VELOCITA_ASCIUGATURA),     NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].temperatura_aria_1,                 NULL,   NULL,   0,   100,  50,   1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_TEMPERATURA_ARIA_1),     temperatura_aria_check_posizione_sonda,       p);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].offset_temperatura_aria_alto,       NULL,   NULL,   0,   20,   2,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TEMPERATURA_ARIA_ALTO),            NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].offset_temperatura_aria_basso,      NULL,   NULL,   0,   20,   2,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TEMPERATURA_ARIA_BASSO),           NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].umidita_residua_dry_auto,           NULL,   NULL,   5,   20,   60,   1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_UMIDITA_RESIDUA_DRY_AUTO),         NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].abilita_raffreddamento,             NULL,   NULL,   0,   1,    1,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_ABILITA_RAFFREDDAMENTO),           NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_durata_raffreddamento,        NULL,   NULL,   1,   99,   2,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TEMPO_DURATA_RAFFREDDAMENTO),      NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].abilita_inversione_raffreddamento,  NULL,   NULL,   0,   1,    1,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_ABILITA_INVERSIONE_RAFFREDDAMENTO),NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_giro_raffreddamento,          NULL,   NULL,   1,   99,   35,   1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TEMPO_GIRO_RAFFREDDAMENTO),        NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].tempo_pausa_raffreddamento,         NULL,   NULL,   5,   99,   6,    1,    AL_TECH,     FINT(PARMAC_DESCRIPTIONS_TEMPO_PAUSA_RAFFREDDAMENTO),       NULL,       NULL);
    parameters[j++] = PARAMETER_C99(PARAMETER_TYPE_UINT8, &p->pciclo[i].abilita_antipiega,                  NULL,   NULL,   0,   1,    0,    1,    AL_USER,     FINT(PARMAC_DESCRIPTIONS_ABILITA_ANTIPIEGA),                NULL,       NULL);
    // clang-format on

    parameter_check_ranges(parameters, NUM_PARAMETERS);
    if (reset) {
        parameter_reset_to_defaults(parameters, NUM_PARAMETERS);
    }
}

void parciclo_operation(size_t parameter, int op, model_t *pmodel) {
    parameter_operator(parameter_get_handle(parameters, NUM_PARAMETERS, parameter,
                                            get_livello_accesso(pmodel->pmac.abilita_parametri_ridotti)),
                       op);
}

const char *parciclo_get_description(const model_t *pmodel, size_t parameter) {
    (void)pmodel;
    parameter_user_data_t data = parameter_get_user_data(parameter_get_handle(
        parameters, NUM_PARAMETERS, parameter, get_livello_accesso(pmodel->pmac.abilita_parametri_ridotti)));

    return data.descrizione[pmodel->pmac.lingua];
}

void parciclo_format_value(char *string, size_t parameter, model_t *pmodel) {
    parameter_handle_t   *par  = parameter_get_handle(parameters, NUM_PARAMETERS, parameter,
                                                      get_livello_accesso(pmodel->pmac.abilita_parametri_ridotti));
    parameter_user_data_t data = parameter_get_user_data(par);

    data.format(string, par);
}

size_t parciclo_get_tot_parameters(model_t *pmodel) {
    return parameter_get_count(parameters, NUM_PARAMETERS, get_livello_accesso(pmodel->pmac.abilita_parametri_ridotti));
}

static void formatta(char *string, const void *arg) {
    const parameter_handle_t *par = arg;

    switch (par->type) {
        case PARAMETER_TYPE_UINT8:
            sprintf(string, "%i", *(uint8_t *)par->pointer);
            break;
        default:
            sprintf(string, "Errore!");
            break;
    }
}


static void temperatura_aria_check_posizione_sonda(struct _parameter_handle_t *p, void *arg) {
    model_t *pmodel = arg;
    if (pmodel->pmac.sonda_temperatura_in_out == 0) {
        p->pmax = &pmodel->pmac.temperatura_max_1_in;
    } else {
        p->pmax = &pmodel->pmac.temperatura_max_1_out;
    }
}

static uint8_t get_livello_accesso(uint8_t parametri_ridotti) {
    if (parametri_ridotti == 1)
        return 0b01;
    else
        return 0b11;
}
