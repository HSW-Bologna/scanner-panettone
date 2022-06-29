/*
 * File:   gt_reset_bruciatore.c
 * Author: Virginia
 *
 * Created on 19 luglio 2021, 9.39
 */

#include <stdint.h>
#include <stdlib.h>

#include "gt_reset_bruciatore.h"

#include "model/model.h"
#include "peripherals/digin.h"
#include "peripherals/digout.h"
#include "gel/timer/stopwatch.h"
#include "peripherals/timer.h"



stopwatch_t ct_reset_blocco = STOPWATCH_NULL;



void gt_reset_bruciatore(model_t *p, uint32_t timestamp) {
    static uint8_t nfx_all_blocco_bruciatore = 0;     // - ToDO Valutare per "gt_reset_bruciatore_extra"

    static uint8_t nf_all_blocco_bruciatore = 0;
    static uint8_t cnt_blocchi_bruciatore   = 0;



    // ====================================================================== //
    if (model_not_in_test(p) && nfx_all_blocco_bruciatore == 0) {
        if (p->pmac.abilita_gas != 0 && p->pmac.numero_tentativi_reset_gas != 0)     // CP se MACCINA A GAS
        {


            if ((digin_get(BLOCCO_BRUCIATORE) == 1) && (model_get_status_stopped(p)))     // 04/01/21
            {
                p->status.f_all_blocco_bruciatore = 0;
                return;
            }



            if ((digin_get(BLOCCO_BRUCIATORE) == 1)) {
                if (model_get_status_not_work(p))     // 10/09/2020
                {
                    p->status.f_all_blocco_bruciatore = 1;
                    return;
                }




                if (nf_all_blocco_bruciatore == 0 && p->status.f_all_blocco_bruciatore == 0 &&
                    model_get_status_work(p)) {
                    nf_all_blocco_bruciatore = 1;
                    cnt_blocchi_bruciatore++;

                    if (cnt_blocchi_bruciatore >= p->pmac.numero_tentativi_reset_gas) {
                        p->status.f_all_blocco_bruciatore = 1;

                        nf_all_blocco_bruciatore = 0;
                        cnt_blocchi_bruciatore   = 0;
                    }
                }
            }

            if (nf_all_blocco_bruciatore == 1)     // ------------------------------ //
            {
                stopwatch_set(&ct_reset_blocco, 2 * 1000UL);
                stopwatch_start(&ct_reset_blocco, timestamp);
                nf_all_blocco_bruciatore = 2;
            }

            if (nf_all_blocco_bruciatore == 2 &&
                stopwatch_is_timer_reached(&ct_reset_blocco, timestamp))     // -------- //
            {
                stopwatch_init(&ct_reset_blocco);
                stopwatch_set(&ct_reset_blocco, 3 * 1000UL);
                set_digout(RESET_BRUCIATORE);
                nf_all_blocco_bruciatore = 3;
            }

            if (nf_all_blocco_bruciatore == 3 &&
                stopwatch_is_timer_reached(&ct_reset_blocco, timestamp))     // -------- //
            {
                stopwatch_init(&ct_reset_blocco);
                stopwatch_set(&ct_reset_blocco, 5 * 1000UL);
                clear_digout(RESET_BRUCIATORE);
                nf_all_blocco_bruciatore = 4;
            }

            if (nf_all_blocco_bruciatore == 4 &&
                stopwatch_is_timer_reached(&ct_reset_blocco, timestamp))     // -------- //
            {
                nf_all_blocco_bruciatore = 0;
            }
        } else {
            nf_all_blocco_bruciatore          = 0;
            cnt_blocchi_bruciatore            = 0;
            p->status.f_all_blocco_bruciatore = 0;
        }
    }
}
