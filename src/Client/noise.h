//=============================================================================
//
// Perlin Noise
// Copyright (C) 2004  Julien Guertault
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=============================================================================
//
// Tools
//
// noise.hh : made by Zavie (Julien Guertault)
//
//=============================================================================

#ifndef		NOISE_HH
# define	NOISE_HH

extern "C" void	InitNoise (void);
extern "C" float Noise (const float, const float, const float, const float);

#endif		// NOISE_HH
