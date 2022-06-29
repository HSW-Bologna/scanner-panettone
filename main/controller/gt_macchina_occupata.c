/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: gt_macchina_occupata.c                                            */
/*                                                                            */
/*      gestione uscita MACCHINA OCCUPATA                                     */
/*                                                                            */
/*  Autore: Maldus (Mattia MALDINI) & Virginia NEGRI & Massimo ZANNA          */
/*                                                                            */
/*  Data  : 19/07/2021      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 03/08/2021      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "controller/gt_macchina_occupata.h"

#include "model/model.h"
#include "peripherals/digout.h"





// ************************************************************************** //
void gt_macchina_occupata(model_t *p)
{
    if (p->pmac.macchina_libera_off_on==0) // NA - OFF =========================
    {
        if (p->pmac.tipo_out_macchina_occupata==2) // IN MARCIA + TOLTO CON OBLO' APERTO
        {
            if (p->status.f_in_test==0 && p->status.stato!=0)
            {
                if (p->status.f_all==1 && p->status.n_allarme==1) // PORTA APERTA
                {
                    clear_digout (MACCHINA_OCCUPATA);
                }
                else
                {
                    set_digout (MACCHINA_OCCUPATA);
                }
            }
            else if (p->status.f_in_test==0)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
        }
        
        
        
        if (p->pmac.tipo_out_macchina_occupata==1) // SOLO ALL
        {
            if (p->status.f_in_test==0 && p->status.f_all==1 && p->status.n_allarme!=1)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && p->status.f_all==0)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
        }
        
        
        
        if (p->pmac.tipo_out_macchina_occupata==0) // ALL + START + OBLO'
        {
            if (p->status.f_in_test==0 && p->status.f_all==1 && p->status.n_allarme!=1)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && p->status.stato==0 && p->status.f_ok_gettone==1)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && (p->status.stato!=0 || p->status.f_ok_gettone==1) && p->status.f_all==0)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
        }
    }
    
    
    
    
    
    if (p->pmac.macchina_libera_off_on==1) // NC - ON ==========================
    {
        if (p->pmac.tipo_out_macchina_occupata==2) // IN MARCIA + TOLTO CON OBLO' APERTO
        {
            if (p->status.f_in_test==0 && p->status.stato!=0)
            {
                if (p->status.f_all==1 && p->status.n_allarme==1) // PORTA APERTA
                {
                    set_digout (MACCHINA_OCCUPATA);
                }
                else
                {
                    clear_digout (MACCHINA_OCCUPATA);
                }
            }
            else if (p->status.f_in_test==0)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
        }
        
        
        
        
        
        if (p->pmac.tipo_out_macchina_occupata==1) // SOLO ALL
        {
            if (p->status.f_in_test==0 && p->status.f_all==1 && p->status.n_allarme!=1)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && p->status.f_all==0)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
        }
        
        
        
        
        
        if (p->pmac.tipo_out_macchina_occupata==0) // ALL + START + OBLO'
        {
            if (p->status.f_in_test==0 && p->status.f_all==1 && p->status.n_allarme!=1)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && p->status.stato==0 && p->status.f_ok_gettone==1)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0 && (p->status.stato!=0 || p->status.f_ok_gettone==1) && p->status.f_all==0)
            {
                clear_digout (MACCHINA_OCCUPATA);
            }
            else if (p->status.f_in_test==0)
            {
                set_digout (MACCHINA_OCCUPATA);
            }
        }
    }
}
