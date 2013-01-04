#include <string>
namespace luxrays { namespace ocl {
std::string KernelSource_mc_funcs = 
"#line 2 \"mc_funcs.cl\"\n"
"\n"
"/***************************************************************************\n"
" *   Copyright (C) 1998-2010 by authors (see AUTHORS.txt )                 *\n"
" *                                                                         *\n"
" *   This file is part of LuxRays.                                         *\n"
" *                                                                         *\n"
" *   LuxRays is free software; you can redistribute it and/or modify       *\n"
" *   it under the terms of the GNU General Public License as published by  *\n"
" *   the Free Software Foundation; either version 3 of the License, or     *\n"
" *   (at your option) any later version.                                   *\n"
" *                                                                         *\n"
" *   LuxRays is distributed in the hope that it will be useful,            *\n"
" *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n"
" *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n"
" *   GNU General Public License for more details.                          *\n"
" *                                                                         *\n"
" *   You should have received a copy of the GNU General Public License     *\n"
" *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *\n"
" *                                                                         *\n"
" *   LuxRays website: http://www.luxrender.net                             *\n"
" ***************************************************************************/\n"
"\n"
"void ConcentricSampleDisk(const float u0, const float u1, float *dx, float *dy) {\n"
"	float r, theta;\n"
"	// Map uniform random numbers to $[-1,1]^2$\n"
"	float sx = 2.f * u0 - 1.f;\n"
"	float sy = 2.f * u1 - 1.f;\n"
"	// Map square to $(r,\\theta)$\n"
"	// Handle degeneracy at the origin\n"
"	if (sx == 0.f && sy == 0.f) {\n"
"		*dx = 0.f;\n"
"		*dy = 0.f;\n"
"		return;\n"
"	}\n"
"	if (sx >= -sy) {\n"
"		if (sx > sy) {\n"
"			// Handle first region of disk\n"
"			r = sx;\n"
"			if (sy > 0.f)\n"
"				theta = sy / r;\n"
"			else\n"
"				theta = 8.f + sy / r;\n"
"		} else {\n"
"			// Handle second region of disk\n"
"			r = sy;\n"
"			theta = 2.f - sx / r;\n"
"		}\n"
"	} else {\n"
"		if (sx <= sy) {\n"
"			// Handle third region of disk\n"
"			r = -sx;\n"
"			theta = 4.f - sy / r;\n"
"		} else {\n"
"			// Handle fourth region of disk\n"
"			r = -sy;\n"
"			theta = 6.f + sx / r;\n"
"		}\n"
"	}\n"
"	theta *= M_PI_F / 4.f;\n"
"	*dx = r * cos(theta);\n"
"	*dy = r * sin(theta);\n"
"}\n"
"\n"
"float3 CosineSampleHemisphere(const float u0, const float u1) {\n"
"	float x, y;\n"
"	ConcentricSampleDisk(u0, u1, &x, &y);\n"
"\n"
"	const float z = sqrt(fmax(0.f, 1.f - x * x - y * y));\n"
"\n"
"	return (float3)(x, y, z);\n"
"}\n"
"\n"
"float3 CosineSampleHemisphereWithPdf(const float u0, const float u1, float *pdfW) {\n"
"	float x, y;\n"
"	ConcentricSampleDisk(u0, u1, &x, &y);\n"
"\n"
"	const float z = sqrt(fmax(0.f, 1.f - x * x - y * y));\n"
"\n"
"	*pdfW = z * M_1_PI_F;\n"
"\n"
"	return (float3)(x, y, z);\n"
"}\n"
"\n"
"int Mod(int a, int b) {\n"
"	if (b == 0)\n"
"		b = 1;\n"
"\n"
"	a %= b;\n"
"	if (a < 0)\n"
"		a += b;\n"
"\n"
"	return a;\n"
"}\n"
"\n"
"float PowerHeuristic(const float fPdf, const float gPdf) {\n"
"	const float f = fPdf;\n"
"	const float g = gPdf;\n"
"\n"
"	return (f * f) / (f * f + g * g);\n"
"}\n"
"\n"
"float PdfWtoA(const float pdfW, const float dist, const float cosThere) {\n"
"    return pdfW * fabs(cosThere) / (dist * dist);\n"
"}\n"
"\n"
"float PdfAtoW(const float pdfA, const float dist, const float cosThere) {\n"
"    return pdfA * dist * dist / fabs(cosThere);\n"
"}\n"
; } }
