/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: gt_ventilazone.c                                                  */
/*                                                                            */
/*      gestione VENTILAZIONE                                                 */
/*                                                                            */
/*  Autore: Maldus (Mattia MALDINI) & Virginia NEGRI & Massimo ZANNA          */
/*                                                                            */
/*  Data  : 19/07/2021      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 18/02/2022      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "model/model.h"

#include "controller/gt_ventilazione.h"
#include "controller/ciclo.h"

#include "peripherals/digout.h"
#include "peripherals/timer.h"
#include "gel/timer/stopwatch.h"


// ************************************************************************** //
void gt_ventilazione(model_t *p, unsigned long timestamp)
{
    if (model_get_status_stopped(p) || model_get_status_pause(p))
    {
        if (p->status.f_in_test==0)
            ventilazione_stop(p);
    }
    
    if (model_get_status_work(p))
    {
        if (p->status.f_in_test==0)
        {
            if ((p->status.stato_step==STATO_STEP_ASC) || (p->status.stato_step==STATO_STEP_RAF))
            {
                ventilazione_marcia(p);
            }
            else
            {
                if ((rele_get(ORARIO)==0) && (rele_get(ANTIORARIO)==0))
                {
                    ventilazione_stop(p);
                }
                else
                {
                    ventilazione_marcia(p);
                }
            }
        }
    }
    
    
    
    return;
    
    
    
    
    /*
    // GT VEL VENTOLA VENTILAZIONE ========================================== //
    if ((get_digout (VENTILAZIONE)==0) || (ct_sec!=0))
    {
        if (f_in_test==0)
        {
            val_da_v = 0;
        }
    }
    else
    {
        comodo1 = temp_aria_in_1;
        comodo2 = temp_ingresso;
        comodo3 = perc_antic_temp_vent;
        comodo11 = perc_vel_min_vent;
        
        comodo10 = 255;
        
        comodo9 = comodo10 * comodo11; // vel minima percentuale calcolata
        comodo9 = comodo9 / 100; // vel minima percentuale calcolata
        
        if (comodo1 > comodo2) // CP se impostata > rilevata
        {
            comodo5 = ((comodo1 * comodo3) / 100); // gradi "anticipo temperatura"
            comodo6 = comodo1 - comodo5; // temperatura "anticipata"
            
            if (comodo6 >= comodo2) // CP temperatura "anticipata" > rilevata
            {
                comodo7 = (comodo1 - (comodo1 - comodo2));
                comodo8 = ((comodo10 * comodo7) / comodo6);
                
                if (comodo8 < comodo9) // vel CALCOLATA < MINIMA
                {
                    val_da_v = comodo9; // OUT minima
                }
                else
                {
                    val_da_v = comodo8; // OUT calcolata
                }
            }
            else
            {
                val_da_v = comodo10; // OUT max
            }
        }
        else
        {
            val_da_v = comodo10; // OUT max
        }
    }
*/
    
    
}



void ventilazione_stop(model_t *p)
{
    clear_digout(VENTILAZIONE);
   
    pwm_set((p), 0, CH_VEL_VENTILAZIONE);       // % speed , CH ANALOG
}

void ventilazione_marcia(model_t *p)
{
    set_digout(VENTILAZIONE);
   
    if (p->status.stato_step==STATO_STEP_ASC)
    {
        pwm_set((p), 100, CH_VEL_VENTILAZIONE); // % speed , CH ANALOG  # CP SE MACCHINA RE => % VEL @ TEMP -TODO 
    }
    
    if (p->status.stato_step==STATO_STEP_RAF)
    {
        pwm_set((p), 100, CH_VEL_VENTILAZIONE); // % speed , CH ANALOG
    }
    
    if (p->status.stato_step==STATO_STEP_ANT)
    {
        pwm_set((p), 100, CH_VEL_VENTILAZIONE); // % speed , CH ANALOG
    }
}
