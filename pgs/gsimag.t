#
# GSIMAG.T - image conversion template
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
# 

#--------------------------------------------------------------------------

template<char|long double>
void _PG_map_type_image(PG_device *d, unsigned char *bf, int km, int lm,
			int n, int nc, void *z, double zmn, double zmx)
    {int i, cmid;
     double sc;
     <TYPE> *lz, lzt, lzmn, lzmx;

     lz = (<TYPE> *) z;

     if (zmx <= zmn)
        {lzmn = <MAX>;
         lzmx = <MIN>;
         for (i = 0; i < n; i++)
             {lzt  = lz[i];
              lzmn = min(lzmn, lzt);
              lzmx = max(lzmx, lzt);};
         zmn = lzmn;
         zmx = lzmx;};

     sc   = (zmn == zmx) ? HUGE : 0.9999*nc/(zmx - zmn);
     cmid = 0.5*nc + 2;

/* Note that if z is within range the scaled pixel value is shifted up by
 * two colors accounting for the black and white mandatory in each palette
 */
     for (i = 0; i < n; i++, lz++)
         {if (sc == HUGE)
             bf[i] = cmid;
          else
             bf[i] = sc*(*lz - zmn) + 2;};

     return;}

#--------------------------------------------------------------------------
