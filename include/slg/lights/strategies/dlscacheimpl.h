/***************************************************************************
 * Copyright 1998-2018 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxCoreRender.                                   *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#ifndef _SLG_LIGHTSTRATEGY_DLSCACHEIMPL_H
#define	_SLG_LIGHTSTRATEGY_DLSCACHEIMPL_H

#include <vector>

#include "luxrays/utils/mcdistribution.h"
#include "slg/slg.h"
#include "slg/bsdf/bsdf.h"
#include "slg/scene/scene.h"
#include "slg/samplers/sampler.h"
#include "slg/utils/pathdepthinfo.h"

namespace slg {
	
//------------------------------------------------------------------------------
// DLSCacheEntry
//------------------------------------------------------------------------------

class DLSCacheEntry {
public:
	DLSCacheEntry(const luxrays::Point &pnt, const luxrays::Normal &nml,
			const PathVolumeInfo &vi) :
			p(pnt), n(nml), volInfo(vi), distributionIndexToLightIndex(0),
			lightsDistribution(NULL), disableDirectLightSampling(false) {
	}
	~DLSCacheEntry() {
		delete lightsDistribution;
	}
	
	// Point information
	luxrays::Point p;
	luxrays::Normal n;
	PathVolumeInfo volInfo;
	
	// Cache information
	std::vector<u_int> distributionIndexToLightIndex;
	luxrays::Distribution1D *lightsDistribution;

	bool disableDirectLightSampling;
};

//------------------------------------------------------------------------------
// Direct light sampling cache
//------------------------------------------------------------------------------

class Scene;
class DLSCOctree;

class DirectLightSamplingCache {
public:
	DirectLightSamplingCache();
	virtual ~DirectLightSamplingCache();

	void Build(const Scene *scene);
	
	const DLSCacheEntry *GetEntry(const luxrays::Point &p, const luxrays::Normal &n) const;

	u_int maxSampleCount, maxDepth, maxEntryPasses;
	float targetCacheHitRate;
	float entryRadius, entryNormalAngle;

private:
	void GenerateEyeRay(const Camera *camera, luxrays::Ray &eyeRay,
			PathVolumeInfo &volInfo, Sampler *sampler, SampleResult &sampleResult) const;
	
	void BuildCacheEntries(const Scene *scene);
	void FillCacheEntry(const Scene *scene, DLSCacheEntry *entry);
	void FillCacheEntries(const Scene *scene);

	DLSCOctree *octree;
};

}

#endif	/* _SLG_LIGHTSTRATEGY_DLSCACHEIMPL_H */
