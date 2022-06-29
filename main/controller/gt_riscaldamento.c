/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: gt_riscaldamento.c                                                */
/*                                                                            */
/*      gestione attivazione riscaldamento                                    */
/*                                                                            */
/*  Autore: Maldus (Mattia MALDINI) & Virginia NEGRI & Massimo ZANNA          */
/*                                                                            */
/*  Data  : 19/07/2021      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 16/02/2022      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "model/model.h"

#include "controller/gt_riscaldamento.h"
#include "controller/ciclo.h"

#include "peripherals/digout.h"
#include "peripherals/timer.h"
#include "gel/timer/stopwatch.h"


#define ISTERESI_TEMPERATURA 2



void gt_riscaldamento(model_t *p, unsigned long timestamp)
{
    if (model_get_status_stopped(p) || model_get_status_pause(p))
    {
        if (p->status.f_in_test == 0)
        {
            clear_digout(RISCALDAMENTO);
        }
    }



    if (model_get_status_work(p))
    {
        static unsigned char f_in_temp = 0;
        
        if (p->status.f_ventilazione == 0 ||  p->status.stato_step != STATO_STEP_ASC)
        {
            clear_digout(RISCALDAMENTO);
        }
        else if (p->status.f_ventilazione == 1 && p->status.stato_step == STATO_STEP_ASC)
        {
            if (f_in_temp == 0)
            {
                if (model_get_temperatura_corrente(p) < (model_temperatura_aria_ciclo(p) +  model_ciclo_corrente(p)->offset_temperatura_aria_alto))
                {
                    set_digout(RISCALDAMENTO);
                }
                else
                {
                    clear_digout(RISCALDAMENTO);
                    f_in_temp = 1;
                }
            }
            else if (f_in_temp == 1)
            {
                if (model_get_temperatura_corrente(p) < (model_temperatura_aria_ciclo(p) - model_ciclo_corrente(p)->offset_temperatura_aria_basso))
                {
                    set_digout(RISCALDAMENTO);
                    f_in_temp = 0;
                }
            }
        }
    }
}
