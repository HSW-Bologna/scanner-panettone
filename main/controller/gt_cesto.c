/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: gt_cesto.c                                                        */
/*                                                                            */
/*      gestione moto & velocita' cesto                                       */
/*                                                                            */
/*  Autore: Maldus (Mattia MALDINI) & Virginia NEGRI & Massimo ZANNA          */
/*                                                                            */
/*  Data  : 19/07/2021      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 23/02/2022      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "controller/gt_cesto.h"
#include "model/model.h"

#include "controller/ciclo.h"

#include "peripherals/digout.h"
#include "peripherals/timer.h"
#include "gel/timer/stopwatch.h"
#include "peripherals/pwm.h"

stopwatch_t ct_moto_cesto = STOPWATCH_NULL;
stopwatch_t ct_oblo_open_close_on = STOPWATCH_NULL;

stopwatch_t ct_anti_piega_rit = STOPWATCH_NULL;
stopwatch_t ct_anti_piega_max = STOPWATCH_NULL;
stopwatch_t ct_anti_piega = STOPWATCH_NULL;
stopwatch_t ct_anti_piega_pausa = STOPWATCH_NULL;





// * ====================================================================== * //
// *                                                                        * //
// *                                                                        * //
// *    GESTIONE CESTO (con INVERTER e DA velocita' DA 0/10 VDC @ GT PWM    * //
// *                                                                        * //
// *                                                                        * //
// * ====================================================================== * //
//
void gt_cesto(model_t *p, unsigned long timestamp)
{
    if (model_get_status_stopped(p) || model_get_status_pause(p))
    {
        if (model_not_in_test(p))
        {
            cesto_stop(p);
        }
        
        if (model_get_status_stopped(p))
        {
            p->status.f_ava_ind = 0;
            p->status.nf_ava_ind = 0;
        }
    }
    
    
    
    if (model_get_status_work(p))
    {
        
        gt_velocita_cesto(p);
        
        
                
        //stopwatch_init(&ct_anti_piega_rit);
        //stopwatch_set(&ct_anti_piega_rit,p->pmac.tempo_ritardo_antipiega*1000UL);
        //stopwatch_start(&ct_anti_piega_rit, get_millis());

//        if (stopwatch_get_state(&ct_moto_cesto) == TIMER_STOPPED)
//        {
//            stopwatch_init(&ct_moto_cesto);
//            stopwatch_start(&ct_moto_cesto, timestamp);
//            
////            if (p->status.f_ava_ind==1)
////            {
////                cesto_avanti(p);
////            }
////            else if (p->status.f_ava_ind==2)
////            {
////                cesto_indietro(p);
////            }
//        }
        
        
        
        // ================================================================== //
        if (model_get_status_step(p)==STATO_STEP_ASC)
        {
            if (model_ciclo_corrente(p)->abilita_inversione_asciugatura==0) // INV NO
            {
                cesto_avanti(p);
            }
            else // INV SI
            {
                if ((stopwatch_get_state(&ct_moto_cesto) == TIMER_STOPPED) || stopwatch_is_timer_reached(&ct_moto_cesto, timestamp)) 
                {
                    stopwatch_stop(&ct_moto_cesto);

                    if (p->status.nf_ava_ind==0) // avvio prima volta
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_asciugatura*1000UL);
                        cesto_avanti(p);
                        p->status.nf_ava_ind = 1;
                    }
                    else if (p->status.nf_ava_ind==1) // ORARIO
                    {              
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_pausa_asciugatura*1000UL);
                        cesto_stop(p);
                        p->status.f_ava_ind = 0;
                        p->status.nf_ava_ind = 2;
                    }
                    else if (p->status.nf_ava_ind==2) // pausa ORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_asciugatura*1000UL);
                        cesto_indietro(p);
                        p->status.nf_ava_ind = 3;
                    }
                    else if (p->status.nf_ava_ind==3) // ANTIORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_pausa_asciugatura*1000UL);
                        cesto_stop(p);
                        p->status.f_ava_ind = 0;
                        p->status.nf_ava_ind = 4;
                    }
                    else if (p->status.nf_ava_ind==4) // pausa ANTIORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_asciugatura*1000UL);
                        cesto_avanti(p);
                        p->status.nf_ava_ind = 1;
                    }
                    stopwatch_start(&ct_moto_cesto, timestamp);
                }
                else if (stopwatch_get_state(&ct_moto_cesto) == TIMER_PAUSED)
                {
                    if (p->status.nf_ava_ind==0) // avvio prima volta
                    {
                        cesto_avanti(p);
                        p->status.nf_ava_ind = 1;
                    }
                    else if (p->status.nf_ava_ind==1) // ORARIO
                    {              
                        cesto_avanti(p);
                    }
                    else if (p->status.nf_ava_ind==2) // pausa ORARIO
                    {
                        cesto_stop(p);
                    }
                    else if (p->status.nf_ava_ind==3) // ANTIORARIO
                    {
                        cesto_indietro(p);
                    }
                    else if (p->status.nf_ava_ind==4) // pausa ANTIORARIO
                    {
                        cesto_stop(p);
                    }
                    stopwatch_start(&ct_moto_cesto, timestamp);
                }
            }
        }
        
        
        
        // ================================================================== //
        if (model_get_status_step(p)==STATO_STEP_RAF)
        {
            if (model_ciclo_corrente(p)->abilita_inversione_raffreddamento==0) // cp INV: non prevista
            {
                cesto_avanti(p);
            }
            else // INV prevista
            {
                if ((stopwatch_get_state(&ct_moto_cesto) == TIMER_STOPPED) || stopwatch_is_timer_reached(&ct_moto_cesto, timestamp)) 
                {
                    stopwatch_stop(&ct_moto_cesto);

                    if (p->status.nf_ava_ind==0) // avvio prima volta
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_raffreddamento*1000UL);
                        cesto_avanti(p);
                        p->status.nf_ava_ind = 1;
                    }
                    else if (p->status.nf_ava_ind==1) // ORARIO
                    {              
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_pausa_raffreddamento*1000UL);
                        cesto_stop(p);
                        p->status.f_ava_ind = 0;
                        p->status.nf_ava_ind = 2;
                    }
                    else if (p->status.nf_ava_ind==2) // pausa ORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_raffreddamento*1000UL);
                        cesto_indietro(p);
                        p->status.nf_ava_ind = 3;
                    }
                    else if (p->status.nf_ava_ind==3) // ANTIORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_pausa_raffreddamento*1000UL);
                        cesto_stop(p);
                        p->status.f_ava_ind = 0;
                        p->status.nf_ava_ind = 4;
                    }
                    else if (p->status.nf_ava_ind==4) // pausa ANTIORARIO
                    {
                        stopwatch_set(&ct_moto_cesto, model_ciclo_corrente(p)->tempo_giro_raffreddamento*1000UL);
                        cesto_avanti(p);
                        p->status.nf_ava_ind = 1;
                    }
                    stopwatch_start(&ct_moto_cesto, timestamp);
                }
            }
        }
        
        
        
        // ************************************************************************** //
        //                                            STOP Min  |  STOP Cicl NO !!!!  //
        //                                            |Ventilo  |  |      Vent FERMA  //
        //                                            ||        |  |      |  NO !!!!  //
        // ab_anti_piega       -AB.ANTIPIEGA (0=NO, 1=MV, 2=MF) |  3=CV, 4=CF) 0-4    //
        //                                                                            //
        // -tempo ritardo antipiega                sec  001-240   //
        // p->pmac.tempo_max_antipiega    -tempo durata antipiega max             min  001-240   //
        // p->pmac.tempo_giro_antipiega    -tempo antipiega cad                    min  01-99     //
        // p->pmac.numero_cicli_max_antipiega-n.ro cicli antipiega                   cic  01-20     //
        // p->pmac.tempo_giro_antipiega   -TEMPO GIRO antipiega                   sec  01-99     //
        // p->pmac.tempo_pausa_antipiega  -TEMPO PAUSA antipiega                  sec  01-99     //
        // vel_anti_piega      -VELOCITA' ANTIPIEGA                  g/min  mmm-MMM   //
        //                                                                            //
        // -------------------------------------------------------------------------- //
        //                                                                            //
        //  Step:                                                                     //
        //                                                                            //
        //  1 - viene valutata PARTENZA RITARDATA o SUBITO                            //
        //  2 - viene valutata DURATA se INFINITA o A TERMINE                         //
        //  3 - viene valutata se con INVERSIONE o NO                                 //
        //                                                                            //
        //  4 - quando VIENE ESEGUITA ANTIPIEGA si vede se FARE PARTIRE VENTILAZIONE  //
        //                                                                            //
        //  5 - NB: nella "gt_cesto" viene valutato se USARE VEL CICLO o ANTIPIEGA    //
        //                                                                            //
        // ************************************************************************** //
        // ************************************************************************** //
        //                                            STOP Min  |  STOP Cicl NO !!!!  //
        //                                            |Ventilo  |  |      Vent FERMA  //
        //                                            ||        |  |      |  NO !!!!  //
        // ab_anti_piega       -AB.ANTIPIEGA (0=NO, 1=MV, 2=MF) |  3=CV, 4=CF) 0-4    //
        //                                                                            //
        // t_anti_piega_rit    -tempo ritardo antipiega                sec  001-240   //
        // t_anti_piega_max    -tempo durata antipiega max             min  001-240   //
        // t_anti_piega_cad    -tempo antipiega cad                    min  01-99     //
        // nro_c_anti_piega_max-n.ro cicli antipiega                   cic  01-20     //
        // t_giro_anti_piega   -TEMPO GIRO antipiega                   sec  01-99     //
        // t_pausa_anti_piega  -TEMPO PAUSA antipiega                  sec  01-99     //
        // vel_anti_piega      -VELOCITA' ANTIPIEGA                  g/min  mmm-MMM   //
        //                                                                            //
        // -------------------------------------------------------------------------- //
        //                                                                            //
        //  Step:                                                                     //
        //                                                                            //
        //  1 - viene valutata PARTENZA RITARDATA o SUBITO                            //
        //  2 - viene valutata DURATA se INFINITA o A TERMINE                         //
        //  3 - viene valutata se con INVERSIONE o NO                                 //
        //                                                                            //
        //  4 - quando VIENE ESEGUITA ANTIPIEGA si vede se FARE PARTIRE VENTILAZIONE  //
        //                                                                            //
        //  5 - NB: nella "gt_cesto" viene valutato se USARE VEL CICLO o ANTIPIEGA    //
        //                                                                            //
        // ************************************************************************** //
        //
        if (model_get_status_step(p)==STATO_STEP_ANT)
        {
            if (p->pciclo[p->status.ciclo].abilita_antipiega==0 || p->status.stato!=2 || p->status.f_in_test!=0 || p->status.f_anti_piega==0)
            {
                return;
            }
            
            
            
            // PARTENZA ANTIPIEGA =========================================== //
            if (p->status.nf_anti_piega==ANTIPIEGA_START)
            {
                if (p->pmac.tempo_ritardo_antipiega != 0)
                {
                    stopwatch_init(&ct_anti_piega_rit);
                    stopwatch_set(&ct_anti_piega_rit,p->pmac.tempo_ritardo_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_rit, get_millis());
                    cesto_stop(p);
                    p->status.nf_anti_piega = ANTIPIEGA_RITARDO;    // SI RIT
                }
                else
                {
                    p->status.nf_anti_piega = ANTIPIEGA_TIPO_DURATA;// NO RIT
                }
            }
            
            // ATTESA RITARDO ANTIPIEGA ===================================== //
            if (p->status.nf_anti_piega==ANTIPIEGA_RITARDO)
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_rit, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_rit);
                    p->status.nf_anti_piega = ANTIPIEGA_TIPO_DURATA;
                }
            }
            
            
            
            /* CP DURATA INFINITA/TERMINE ----------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_TIPO_DURATA)
            {
                if (p->pmac.tempo_max_antipiega==0 && p->pmac.numero_cicli_max_antipiega==0)
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA; // INFINITO
                }
                else
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE; // A TERMINE
                }
            }
            
            
            
            
            
            
            
            
            
            
            // DURATA A TERMINE ===================================================== //
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE)
            {
                if (p->pmac.tempo_max_antipiega != 0)
                {
        //            ct_anti_piega_max = p->pmac.tempo_max_antipiega*60;          
                    
                    //stopwatch_init(&ct_anti_piega_max);
                    //stopwatch_set(&ct_anti_piega_max,p->pmac.tempo_max_antipiega*1000UL);
                    //stopwatch_start(&ct_anti_piega_max, get_millis());

                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_CHECK; // TERMINE TEMPO
                }

                if (p->pmac.numero_cicli_max_antipiega != 0)
                {
                    //p->status.cnro_c_anti_piega_max = p->pmac.numero_cicli_max_antipiega;
                    
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_CHECK; // TERMINE CICLI
                }
            }
            
            
            
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_CHECK) // DURATA A TERMINE - VALUTAZIONE FINE =========== //
            {
                if (p->pmac.numero_cicli_max_antipiega != 0)
                {
                    if (p->status.cnro_c_anti_piega_max == 0)
                    {
                        p->status.nf_anti_piega = ANTIPIEGA_ATTESA_APERTURA_OBLO; // TERMINE CICLI
                        p->status.f_anti_piega = 2;   // !!!! ToDO
                    }
                    else
                    {
                        p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE;
                    }
                }
                else if (p->pmac.tempo_max_antipiega != 0)
                {
                    if (stopwatch_is_timer_reached(&ct_anti_piega_max, get_millis()))
                    {
                        stopwatch_init(&ct_anti_piega_max);

                        p->status.nf_anti_piega = ANTIPIEGA_ATTESA_APERTURA_OBLO; // TERMINE TEMPO
                        p->status.f_anti_piega = 2;   // !!!! ToDO
                    }
                    else
                    {
                        p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE;
                    }
                }
            }
            
            
            
            
            
            // DURATA A TERMINE ============================================= //
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE)
            {
                if (p->pmac.tempo_pausa_antipiega==0) // CP INVERSIONE
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO; // NO INV
                }
                else
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI; // SI INV
                }
            }



            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO) /* TER NO INV ------------------------------------ */
            {
                stopwatch_init(&ct_anti_piega);
                stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                stopwatch_start(&ct_anti_piega, get_millis());
                set_digout (ORARIO);

                p->status.f_ventilazione = 1;

                p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO_GIRO;
            }

            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO_GIRO) /* TER NO INV -------------------------- */
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);
                    clear_digout (ORARIO);

                    p->status.f_ventilazione = 0;

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO_PAUSA;
                }
            }

            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_NO_PAUSA) /* TER NO INV -------------------------- */
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);

                    if (p->status.cnro_c_anti_piega_max != 0)
                    {
                        p->status.cnro_c_anti_piega_max--;
                    }
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_CHECK;
                }
            }





            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI) /* TER SI INV MOTO ------------------------------- */
            {
                stopwatch_init(&ct_anti_piega);
                stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                stopwatch_start(&ct_anti_piega, get_millis());
                set_digout (ORARIO);


                p->status.f_ventilazione = 1;

                p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_GIRO_ORA;
            }



            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_GIRO_ORA) /* TER SI INV PAUSA -------------------- */
            {
                if  (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);
                    clear_digout (ORARIO);

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_PAUSA_ORA;
                }
            }



            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_PAUSA_ORA) /* TER SI INV MOTO INV---------------------------- */
            {
                if  (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);

                    stopwatch_init(&ct_anti_piega);
                    stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega, get_millis());
                    set_digout (ANTIORARIO);

                    p->status.f_ventilazione = 1;

                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_GIRO_ANT;
                }
            }



            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_GIRO_ANT) /* TER SI INV MOTO INV ----------------- */
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);

                    clear_digout (ANTIORARIO);

                    p->status.f_ventilazione = 0;

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_PAUSA_ANT;
                }
            }



            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_TERMINE_GESTIONE_INV_SI_PAUSA_ANT) /* TER SI INV -------------------------- */
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);

                    if (p->status.cnro_c_anti_piega_max != 0)
                    {
                        p->status.cnro_c_anti_piega_max--;
                    }
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_TERMINE_CHECK;
                }
            }
            
            
            
            
            
            // DURATA INFINITA ============================================== //
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA)
            {
                if (p->pmac.tempo_pausa_antipiega==0) // CP INVERSIONE
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_NO; // NO INV
                }
                else
                {
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI; // SI INV
                }
            }

            /* INF NO INV --------------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_NO)
            {
                    stopwatch_init(&ct_anti_piega);
                    stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega, get_millis());
                set_digout (ORARIO);

                p->status.f_ventilazione = 1;

                p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_NO_GIRO;
            }
            
            /* INF NO INV --------------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_NO_GIRO)
            {
                if  (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);
                    clear_digout (ORARIO);

                    p->status.f_ventilazione = 0;

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_NO_PAUSA;
                }
            }
            
            /* INF NO INV --------------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_NO_PAUSA)
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_NO;
                }
            }
            
            /* INF SI INV MOTO ---------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_SI)
            {
                    stopwatch_init(&ct_anti_piega);
                    stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega, get_millis());
                set_digout (ORARIO);

                p->status.f_ventilazione = 1;

                p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI_GIRO_ORA;
            }
            
            
            /* INF SI INV PAUSA --------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_SI_GIRO_ORA)
            {
                if  (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);
                    clear_digout (ORARIO);

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI_PAUSA_ORA;
                }
            }
            
            
            /* INF SI INV MOTO INV------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_SI_PAUSA_ORA)
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);

                    stopwatch_init(&ct_anti_piega);
                    stopwatch_set(&ct_anti_piega,p->pmac.tempo_giro_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega, get_millis());
                    set_digout (ANTIORARIO);

                    p->status.f_ventilazione = 1;

                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI_GIRO_ANT;
                }
            }
            
            
            /* INF SI INV MOTO INV ------------------------------------------ */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_SI_GIRO_ANT)
            {
                if  (stopwatch_is_timer_reached(&ct_anti_piega, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega);
                    clear_digout (ANTIORARIO);

                    p->status.f_ventilazione = 0;

                    stopwatch_init(&ct_anti_piega_pausa);
                    stopwatch_set(&ct_anti_piega_pausa,p->pmac.tempo_pausa_antipiega*1000UL);
                    stopwatch_start(&ct_anti_piega_pausa, get_millis());
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI_PAUSA_ANT;
                }
            }
            
            
            /* INF SI INV --------------------------------------------------- */
            if (p->status.nf_anti_piega==ANTIPIEGA_DURATA_INFINITA_INV_SI_PAUSA_ANT)
            {
                if (stopwatch_is_timer_reached(&ct_anti_piega_pausa, get_millis()))
                {
                    stopwatch_init(&ct_anti_piega_pausa);
                    p->status.nf_anti_piega = ANTIPIEGA_DURATA_INFINITA_INV_SI;
                }
            }
            
            
            
            
            
            /* ATTENDO STOP o OBLO' ========================================= */
            if (p->status.nf_anti_piega==ANTIPIEGA_ATTESA_APERTURA_OBLO)
            {
                // IN LOOP ATTESA APERTURA OBLO'
            }
        }
    }
}





// * ====================================================================== * //
// *                                                                        * //
// *                                                                        * //
// *    GESTIONE VELOCITA'                                                  * //
// *                                                                        * //
// *                                                                        * //
// * ====================================================================== * //
void gt_velocita_cesto (model_t *p)
{
    
        pwm_set((p), model_velocita_ciclo(p), CH_VEL_CESTO); // % speed , CH ANALOG

    
    
////    unsigned long comodo;
////    
////    unsigned long valore_DA_ces = 0;
////    unsigned long vel_mom = 0;
////    unsigned long vel_comodo = 0;
////    
////    if (stato.f_in_test == 1) // SONO IN TEST
////    {
//////        valore_DA_ces = 0;
////    }
////    else if (!stato_in_marcia(&stato)) // stato diverso da MARCIA
////    {
////        valore_DA_ces = 0;
////        set_dac_value(&model, CH_DA_CESTO, valore_DA_ces);
////    }
////    else
////    {
////        if (step_parameters.codice_step==C_SCA || step_parameters.codice_step==C_SRO)
////        {
////            vel_comodo = step_parameters.vel_lav;
////        }
////        
////        if (step_parameters.codice_step==C_ATT)
////        {
////            vel_comodo = step_parameters.vel_lav;
////        }
////        
////        
////        
////        
////        
////        if (step_parameters.codice_step==C_CEN)
////        {
////            if ((stato.sottostato>0 && stato.sottostato<3 && stato.f_sbilanciamento==0) || stato.sottostato==50)
////            {
////                vel_comodo = step_parameters.vel_prep;
////            }
////            
////            if (stato.sottostato>0 && stato.sottostato<3 && stato.f_sbilanciamento==1)
////            {
////                vel_comodo = 0;
////            }
////            
////            if (stato.sottostato==20 || stato.sottostato==100 /*|| stato.sottostato==230 || stato.sottostato==240*/ || stato.stato==STATO_FRENATA_FORZ)
////            {
////                vel_comodo = 0;
////            }
////            
////            if (stato.sottostato==230 || stato.sottostato==240)
////            {
////                // vel_comodo = 0;
////            }
////            
//////            if (stato.sottostato > 199 && stato.sottostato < 210)
//////            {
//////                vel_comodo = step_parameters.vel_prep;
//////            }
////        }
////        
////        
////        
////        
////        
////        if (step_parameters.codice_step==C_AMM || step_parameters.codice_step==C_PRE || step_parameters.codice_step==C_LAV || step_parameters.codice_step==C_RIS)
////        {
////            if (stato.f_moto_cesto_rie_lav_cic==MC_RIE)
////            {
////                vel_comodo = step_parameters.vel_rie;
////            }
////            
////            if (stato.f_moto_cesto_rie_lav_cic==MC_LAV || stato.f_moto_cesto_rie_lav_cic==MC_CIC)
////            {
////                vel_comodo = step_parameters.vel_lav;
////            }
////        }
////        
////        
////        comodo = (1023000 / parmac.vel_max_centr_3);
////        
////        vel_mom = ((comodo * vel_comodo) / 1000);
////        
////        if (vel_mom > 1023)
////        {
////            vel_mom = 1023;
////        }
////        
////        if (vel_mom < 0)
////        {
////            vel_mom = 0;
////        }
////        
////        if (step_parameters.codice_step==C_CEN && ((stato.sottostato>5 && stato.sottostato <11) || stato.sottostato== 230 || stato.sottostato== 240))
////        {
////            return;
////        }
////        else
////        {
////            valore_DA_ces = vel_mom; // vel "normale" su SCHEDA
////            set_dac_value(&model, CH_DA_CESTO, valore_DA_ces);
////        }
////    }
}





void cesto_stop(model_t *p) /* ------------------------------------ */
{
    clear_digout(ORARIO);
    clear_digout(ANTIORARIO);
    
    pwm_set((p), 0, CH_VEL_CESTO); // % speed , CH ANALOG
}

void cesto_avanti(model_t *p) /* ---------------------------------- */
{
    p->status.f_ava_ind = 1;
    
    rele_update(ORARIO, 1);
    pwm_set((p), model_velocita_ciclo(p), CH_VEL_CESTO); // % speed , CH ANALOG
}

void cesto_indietro(model_t *p) /* -------------------------------- */
{
    p->status.f_ava_ind = 2;
    
    set_digout(ANTIORARIO);
    pwm_set((p), model_velocita_ciclo(p), CH_VEL_CESTO); // % speed , CH ANALOG
}
