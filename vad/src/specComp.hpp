/**********************************************************************
    * specComp.hpp                                          *
    *                                                                 *
    * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
    *          adapted from KisTA: https://github.com/nandohca/kista  *
    *                                                                 *
    * Purpose: The Spectral Comparison task                           *
    *                                                                 *
    * Usage:   The VAD example                                        *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#ifndef SPECTRALCOMPARISON_HPP
#define SPECTRALCOMPARISON_HPP

#include <forsyde.hpp>
#include <src/includes/vad.h>

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void specComp_func(tokens<short>& out,
		   tokens<L_av_t> inp1,
		   tokens<rav1_t> inp2)
{
  // Resize all the vectors to contain 1 element
  // TODO: Generalize
  out.resize(1);
    
  short* in_rav_buff = &get<0,0>(inp2);
  short in_rav_scal  =  get<0,1>(inp2);
    
  short out_stat;
    
#pragma ForSyDe begin specComp_func
    
  int in_L_av[9];
  for (auto i=0;i<9;i++) in_L_av[i] = inp1[0][i];
    
  // Returns flag to indicate spectral stationarity
  out_stat = spectral_comparison(
				 in_rav_buff,    // ACF obtained from L_av1
				 in_rav_scal,    // rav1[] scaling factor
				 in_L_av);       // ACF averaged over last four frames
    
#pragma ForSyDe end
    
  out[0] = out_stat;
}


#endif
