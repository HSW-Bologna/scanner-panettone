/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: gt_ciclo.c                                                        */
/*                                                                            */
/*      gestione CICLO                                                        */
/*                                                                            */
/*  Autore: Maldus (Mattia MALDINI) & Virginia NEGRI & Massimo ZANNA          */
/*                                                                            */
/*  Data  : 19/07/2021      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 23/02/2022      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include <xc.h>
#include "controller/ciclo.h"
#include "model/model.h"
#include "view/view_types.h"
#include "view/view.h"
#include "peripherals/timer.h"

extern stopwatch_t ct_moto_cesto;
extern stopwatch_t ct_oblo_open_close_on;

extern stopwatch_t ct_anti_piega_rit;
extern stopwatch_t ct_anti_piega_max;
extern stopwatch_t ct_anti_piega;
extern stopwatch_t ct_anti_piega_pausa;



void gt_ciclo(model_t *p, uint32_t timestamp)
{
    if (model_get_status_stopped(p))
    {
        stopwatch_init(&ct_moto_cesto);
        
        stopwatch_init(&ct_anti_piega_rit);
        stopwatch_init(&ct_anti_piega_max);
        stopwatch_init(&ct_anti_piega);
        stopwatch_init(&ct_anti_piega_pausa);
    }
    
    
    
    // STATO - PAUSA ======================================================== //
    if (model_get_status_pause(p))
    {
        stopwatch_pause(&ct_moto_cesto, get_millis());
        
        stopwatch_pause(&ct_anti_piega_rit, get_millis());
        
        stopwatch_pause(&ct_anti_piega_max, get_millis());
        stopwatch_pause(&ct_anti_piega, get_millis());
        stopwatch_pause(&ct_anti_piega_pausa, get_millis());
        
        model_metti_tempo_lavoro_in_pausa(p, get_millis());
    }
    
    
    
    // STATO - WORK ========================================================= //
    if (model_get_status_work(p))
    {
        switch (p->status.stato_step)
        {
            case STATO_STEP_ASC:
                if (model_tempo_lavoro_scaduto(p, get_millis()))
                {
                    if (model_ciclo_corrente(p)->abilita_raffreddamento)
                    {
                        model_comincia_raffreddamento(p);
                    }
                    else if (model_ciclo_corrente(p)->abilita_antipiega)
                    {
                        model_comincia_antipiega(p);
                    }
                    else
                    {
                        model_set_status_stopped(p);
                    }
                    view_event((view_event_t){.code = VIEW_EVENT_STEP_UPDATE});
                }
                break;
                
            case STATO_STEP_RAF:
                if (model_tempo_lavoro_scaduto(p, get_millis()))
                {
                    if (model_ciclo_corrente(p)->abilita_antipiega)
                    {
                        model_comincia_antipiega(p);
                    }
                    else
                    {
                        model_set_status_stopped(p);
                    }
                    view_event((view_event_t){.code = VIEW_EVENT_STEP_UPDATE});
                }
                break;
                
            case STATO_STEP_ANT:
                if (p->pmac.tempo_max_antipiega==0 && p->pmac.numero_cicli_max_antipiega==0)
                {
                    // -TODO INFINITA !!!!
                }
                else
                {
                    if (p->pmac.numero_cicli_max_antipiega != 0)
                    {
                        if (p->status.cnro_c_anti_piega_max == 0)
                        {
                            Nop();
                        }
                    }
                    else if (p->pmac.tempo_max_antipiega !=0 )
                    {
                        if (stopwatch_is_timer_reached(&ct_anti_piega_max, get_millis()))
                        {
                            //stopwatch_init(&ct_anti_piega_max);

                            Nop();

//                            p->status.nf_anti_piega = ANTIPIEGA_ATTESA_APERTURA_OBLO; // TERMINE TEMPO
//                            p->status.f_anti_piega = 2;   // !!!! ToDO
                        }
                    }
                }
                view_event((view_event_t){.code = VIEW_EVENT_STEP_UPDATE});
                break;
                
            default:
                break;
        }
    }
}
