/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright (c) 2012,2013,2014.2015, by the GROMACS development team, led by
 * Mark Abraham, David van der Spoel, Berk Hess, and Erik Lindahl,
 * and including many others, as listed in the AUTHORS file in the
 * top-level source directory and at http://www.gromacs.org.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * http://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at http://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out http://www.gromacs.org.
 */
/*
 * Note: this file was generated by the GROMACS c kernel generator.
 */
#include "gmxpre.h"
#include "config.h"
#include <math.h>

#include "../../../fda/FDA.h"
#include "../nb_kernel.h"
#include "gromacs/gmxlib/nrnb.h"

/*
 * Gromacs nonbonded kernel:   nb_kernel_ElecRFCut_VdwNone_GeomP1P1_VF_c
 * Electrostatics interaction: ReactionField
 * VdW interaction:            None
 * Geometry:                   Particle-Particle
 * Calculate force/pot:        PotentialAndForce
 */
void
nb_kernel_ElecRFCut_VdwNone_GeomP1P1_VF_c
                    (t_nblist                    * gmx_restrict       nlist,
                     rvec                        * gmx_restrict          xx,
                     rvec                        * gmx_restrict          ff,
                     t_forcerec                  * gmx_restrict          fr,
                     t_mdatoms                   * gmx_restrict     mdatoms,
                     nb_kernel_data_t gmx_unused * gmx_restrict kernel_data,
                     t_nrnb                      * gmx_restrict        nrnb)
{
    int              i_shift_offset,i_coord_offset,j_coord_offset;
    int              j_index_start,j_index_end;
    int              nri,inr,ggid,iidx,jidx,jnr,outeriter,inneriter;
    real             shX,shY,shZ,tx,ty,tz,fscal,rcutoff,rcutoff2;
    int              *iinr,*jindex,*jjnr,*shiftidx,*gid;
    real             *shiftvec,*fshift,*x,*f;
    int              vdwioffset0;
    real             ix0,iy0,iz0,fix0,fiy0,fiz0,iq0,isai0;
    int              vdwjidx0;
    real             jx0,jy0,jz0,fjx0,fjy0,fjz0,jq0,isaj0;
    real             dx00,dy00,dz00,rsq00,rinv00,rinvsq00,r00,qq00,c6_00,c12_00,cexp1_00,cexp2_00;
    real             velec,felec,velecsum,facel,crf,krf,krf2;
    real             *charge;

    struct FDA *fda = fr->fda;

    x                = xx[0];
    f                = ff[0];

    nri              = nlist->nri;
    iinr             = nlist->iinr;
    jindex           = nlist->jindex;
    jjnr             = nlist->jjnr;
    shiftidx         = nlist->shift;
    gid              = nlist->gid;
    shiftvec         = fr->shift_vec[0];
    fshift           = fr->fshift[0];
    facel            = fr->epsfac;
    charge           = mdatoms->chargeA;
    krf              = fr->ic->k_rf;
    krf2             = krf*2.0;
    crf              = fr->ic->c_rf;

    /* When we use explicit cutoffs the value must be identical for elec and VdW, so use elec as an arbitrary choice */
    rcutoff          = fr->rcoulomb;
    rcutoff2         = rcutoff*rcutoff;

    outeriter        = 0;
    inneriter        = 0;

    /* Start outer loop over neighborlists */
    for(iidx=0; iidx<nri; iidx++)
    {
        /* Load shift vector for this list */
        i_shift_offset   = DIM*shiftidx[iidx];
        shX              = shiftvec[i_shift_offset+XX];
        shY              = shiftvec[i_shift_offset+YY];
        shZ              = shiftvec[i_shift_offset+ZZ];

        /* Load limits for loop over neighbors */
        j_index_start    = jindex[iidx];
        j_index_end      = jindex[iidx+1];

        /* Get outer coordinate index */
        inr              = iinr[iidx];
        i_coord_offset   = DIM*inr;

        /* Load i particle coords and add shift vector */
        ix0              = shX + x[i_coord_offset+DIM*0+XX];
        iy0              = shY + x[i_coord_offset+DIM*0+YY];
        iz0              = shZ + x[i_coord_offset+DIM*0+ZZ];

        fix0             = 0.0;
        fiy0             = 0.0;
        fiz0             = 0.0;

        /* Load parameters for i particles */
        iq0              = facel*charge[inr+0];

        /* Reset potential sums */
        velecsum         = 0.0;

        /* Start inner kernel loop */
        for(jidx=j_index_start; jidx<j_index_end; jidx++)
        {
            /* Get j neighbor index, and coordinate index */
            jnr              = jjnr[jidx];
            j_coord_offset   = DIM*jnr;

            /* load j atom coordinates */
            jx0              = x[j_coord_offset+DIM*0+XX];
            jy0              = x[j_coord_offset+DIM*0+YY];
            jz0              = x[j_coord_offset+DIM*0+ZZ];

            /* Calculate displacement vector */
            dx00             = ix0 - jx0;
            dy00             = iy0 - jy0;
            dz00             = iz0 - jz0;

            /* Calculate squared distance and things based on it */
            rsq00            = dx00*dx00+dy00*dy00+dz00*dz00;

            rinv00           = 1.0/sqrt(rsq00);

            rinvsq00         = rinv00*rinv00;

            /* Load parameters for j particles */
            jq0              = charge[jnr+0];

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            if (rsq00<rcutoff2)
            {

            qq00             = iq0*jq0;

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = qq00*(rinv00+krf*rsq00-crf);
            felec            = qq00*(rinv00*rinvsq00-krf2);

            /* Update potential sums from outer loop */
            velecsum        += velec;

            fscal            = felec;

            /* Calculate temporary vectorial force */
            tx               = fscal*dx00;
            ty               = fscal*dy00;
            tz               = fscal*dz00;

            /* Update vectorial force */
            fix0            += tx;
            fiy0            += ty;
            fiz0            += tz;
            f[j_coord_offset+DIM*0+XX] -= tx;
            f[j_coord_offset+DIM*0+YY] -= ty;
            f[j_coord_offset+DIM*0+ZZ] -= tz;

            /* pairwise forces */
            fda_add_nonbonded_coulomb(fda, inr+0, jnr+0, felec, dx00, dy00, dz00);
            fda_virial_bond(fda, inr+0, jnr+0, fscal, dx00, dy00, dz00);

            }

            /* Inner loop uses 32 flops */
        }
        /* End of innermost loop */

        tx = ty = tz = 0;
        f[i_coord_offset+DIM*0+XX] += fix0;
        f[i_coord_offset+DIM*0+YY] += fiy0;
        f[i_coord_offset+DIM*0+ZZ] += fiz0;
        tx                         += fix0;
        ty                         += fiy0;
        tz                         += fiz0;
        fshift[i_shift_offset+XX]  += tx;
        fshift[i_shift_offset+YY]  += ty;
        fshift[i_shift_offset+ZZ]  += tz;

        ggid                        = gid[iidx];
        /* Update potential energies */
        kernel_data->energygrp_elec[ggid] += velecsum;

        /* Increment number of inner iterations */
        inneriter                  += j_index_end - j_index_start;

        /* Outer loop uses 14 flops */
    }

    /* Increment number of outer iterations */
    outeriter        += nri;

    /* Update outer/inner flops */

    inc_nrnb(nrnb,eNR_NBKERNEL_ELEC_VF,outeriter*14 + inneriter*32);
}
/*
 * Gromacs nonbonded kernel:   nb_kernel_ElecRFCut_VdwNone_GeomP1P1_F_c
 * Electrostatics interaction: ReactionField
 * VdW interaction:            None
 * Geometry:                   Particle-Particle
 * Calculate force/pot:        Force
 */
void
nb_kernel_ElecRFCut_VdwNone_GeomP1P1_F_c
                    (t_nblist                    * gmx_restrict       nlist,
                     rvec                        * gmx_restrict          xx,
                     rvec                        * gmx_restrict          ff,
                     t_forcerec                  * gmx_restrict          fr,
                     t_mdatoms                   * gmx_restrict     mdatoms,
                     nb_kernel_data_t gmx_unused * gmx_restrict kernel_data,
                     t_nrnb                      * gmx_restrict        nrnb)
{
    int              i_shift_offset,i_coord_offset,j_coord_offset;
    int              j_index_start,j_index_end;
    int              nri,inr,ggid,iidx,jidx,jnr,outeriter,inneriter;
    real             shX,shY,shZ,tx,ty,tz,fscal,rcutoff,rcutoff2;
    int              *iinr,*jindex,*jjnr,*shiftidx,*gid;
    real             *shiftvec,*fshift,*x,*f;
    int              vdwioffset0;
    real             ix0,iy0,iz0,fix0,fiy0,fiz0,iq0,isai0;
    int              vdwjidx0;
    real             jx0,jy0,jz0,fjx0,fjy0,fjz0,jq0,isaj0;
    real             dx00,dy00,dz00,rsq00,rinv00,rinvsq00,r00,qq00,c6_00,c12_00,cexp1_00,cexp2_00;
    real             velec,felec,velecsum,facel,crf,krf,krf2;
    real             *charge;

    struct FDA *fda = fr->fda;

    x                = xx[0];
    f                = ff[0];

    nri              = nlist->nri;
    iinr             = nlist->iinr;
    jindex           = nlist->jindex;
    jjnr             = nlist->jjnr;
    shiftidx         = nlist->shift;
    gid              = nlist->gid;
    shiftvec         = fr->shift_vec[0];
    fshift           = fr->fshift[0];
    facel            = fr->epsfac;
    charge           = mdatoms->chargeA;
    krf              = fr->ic->k_rf;
    krf2             = krf*2.0;
    crf              = fr->ic->c_rf;

    /* When we use explicit cutoffs the value must be identical for elec and VdW, so use elec as an arbitrary choice */
    rcutoff          = fr->rcoulomb;
    rcutoff2         = rcutoff*rcutoff;

    outeriter        = 0;
    inneriter        = 0;

    /* Start outer loop over neighborlists */
    for(iidx=0; iidx<nri; iidx++)
    {
        /* Load shift vector for this list */
        i_shift_offset   = DIM*shiftidx[iidx];
        shX              = shiftvec[i_shift_offset+XX];
        shY              = shiftvec[i_shift_offset+YY];
        shZ              = shiftvec[i_shift_offset+ZZ];

        /* Load limits for loop over neighbors */
        j_index_start    = jindex[iidx];
        j_index_end      = jindex[iidx+1];

        /* Get outer coordinate index */
        inr              = iinr[iidx];
        i_coord_offset   = DIM*inr;

        /* Load i particle coords and add shift vector */
        ix0              = shX + x[i_coord_offset+DIM*0+XX];
        iy0              = shY + x[i_coord_offset+DIM*0+YY];
        iz0              = shZ + x[i_coord_offset+DIM*0+ZZ];

        fix0             = 0.0;
        fiy0             = 0.0;
        fiz0             = 0.0;

        /* Load parameters for i particles */
        iq0              = facel*charge[inr+0];

        /* Start inner kernel loop */
        for(jidx=j_index_start; jidx<j_index_end; jidx++)
        {
            /* Get j neighbor index, and coordinate index */
            jnr              = jjnr[jidx];
            j_coord_offset   = DIM*jnr;

            /* load j atom coordinates */
            jx0              = x[j_coord_offset+DIM*0+XX];
            jy0              = x[j_coord_offset+DIM*0+YY];
            jz0              = x[j_coord_offset+DIM*0+ZZ];

            /* Calculate displacement vector */
            dx00             = ix0 - jx0;
            dy00             = iy0 - jy0;
            dz00             = iz0 - jz0;

            /* Calculate squared distance and things based on it */
            rsq00            = dx00*dx00+dy00*dy00+dz00*dz00;

            rinv00           = 1.0/sqrt(rsq00);

            rinvsq00         = rinv00*rinv00;

            /* Load parameters for j particles */
            jq0              = charge[jnr+0];

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            if (rsq00<rcutoff2)
            {

            qq00             = iq0*jq0;

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = qq00*(rinv00*rinvsq00-krf2);

            fscal            = felec;

            /* Calculate temporary vectorial force */
            tx               = fscal*dx00;
            ty               = fscal*dy00;
            tz               = fscal*dz00;

            /* Update vectorial force */
            fix0            += tx;
            fiy0            += ty;
            fiz0            += tz;
            f[j_coord_offset+DIM*0+XX] -= tx;
            f[j_coord_offset+DIM*0+YY] -= ty;
            f[j_coord_offset+DIM*0+ZZ] -= tz;

            /* pairwise forces */
            fda_add_nonbonded_coulomb(fda, inr+0, jnr+0, felec, dx00, dy00, dz00);
            fda_virial_bond(fda, inr+0, jnr+0, fscal, dx00, dy00, dz00);

            }

            /* Inner loop uses 27 flops */
        }
        /* End of innermost loop */

        tx = ty = tz = 0;
        f[i_coord_offset+DIM*0+XX] += fix0;
        f[i_coord_offset+DIM*0+YY] += fiy0;
        f[i_coord_offset+DIM*0+ZZ] += fiz0;
        tx                         += fix0;
        ty                         += fiy0;
        tz                         += fiz0;
        fshift[i_shift_offset+XX]  += tx;
        fshift[i_shift_offset+YY]  += ty;
        fshift[i_shift_offset+ZZ]  += tz;

        /* Increment number of inner iterations */
        inneriter                  += j_index_end - j_index_start;

        /* Outer loop uses 13 flops */
    }

    /* Increment number of outer iterations */
    outeriter        += nri;

    /* Update outer/inner flops */

    inc_nrnb(nrnb,eNR_NBKERNEL_ELEC_F,outeriter*13 + inneriter*27);
}
