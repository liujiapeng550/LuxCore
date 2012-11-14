/***************************************************************************
 *   Copyright (C) 1998-2010 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of LuxRays.                                         *
 *                                                                         *
 *   LuxRays is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   LuxRays is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   LuxRays website: http://www.luxrender.net                             *
 ***************************************************************************/

#ifndef _LUXRAYS_INTERSECTIONDEVICE_H
#define	_LUXRAYS_INTERSECTIONDEVICE_H

#include "luxrays/luxrays.h"
#include "luxrays/core/device.h"
#include "luxrays/core/geometry/raybuffer.h"

namespace luxrays {

class IntersectionDevice : public Device {
public:
	const DataSet *GetDataSet() const { return dataSet; }

	virtual RayBuffer *NewRayBuffer() = 0;
	virtual RayBuffer *NewRayBuffer(const size_t size) = 0;
	virtual void PushRayBuffer(RayBuffer *rayBuffer) = 0;
	virtual RayBuffer *PopRayBuffer() = 0;
	virtual size_t GetQueueSize() = 0;

	double GetTotalRaysCount() const { return statsTotalRayCount; }
	double GetPerformance() const {
		const double statsTotalRayTime = WallClockTime() - statsStartTime;
		return (statsTotalRayTime == 0.0) ?	1.0 : (statsTotalRayCount / statsTotalRayTime);
	}
	void ResetPerformaceStats() {
		statsStartTime = WallClockTime();
		statsTotalRayCount = 0;
	}
	virtual double GetLoad() const = 0;

	void SetMaxStackSize(const size_t s) {
		stackSize = s;
	}

	unsigned int GetForceWorkGroupSize() const { return forceWorkGroupSize; }
	void SetForceWorkGroupSize(const unsigned int size) const { forceWorkGroupSize = size; }

	friend class Context;
	friend class VirtualM2OHardwareIntersectionDevice;
	friend class VirtualM2MHardwareIntersectionDevice;

protected:
	IntersectionDevice(const Context *context, const DeviceType type, const size_t index);
	virtual ~IntersectionDevice();

	virtual void SetDataSet(const DataSet *newDataSet);
	virtual void UpdateDataSet() { }
	virtual void Start();

	const DataSet *dataSet;
	double statsStartTime, statsTotalRayCount, statsDeviceIdleTime, statsDeviceTotalTime;

	size_t stackSize;

	mutable unsigned int forceWorkGroupSize;
};

class HardwareIntersectionDevice : public IntersectionDevice {
protected:
	HardwareIntersectionDevice(const Context *context,
		const DeviceType type, const size_t index) :
		IntersectionDevice(context, type, index) { }
	virtual ~HardwareIntersectionDevice() { }

	virtual void SetExternalRayBufferQueue(RayBufferQueue *queue) = 0;

	virtual double GetLoad() const {
		return (statsDeviceTotalTime == 0.0) ? 0.0 : (1.0 - statsDeviceIdleTime / statsDeviceTotalTime);
	}

	friend class VirtualM2OHardwareIntersectionDevice;
	friend class VirtualM2MHardwareIntersectionDevice;
};

//------------------------------------------------------------------------------
// Native thread devices
//------------------------------------------------------------------------------

class NativeThreadIntersectionDevice : public HardwareIntersectionDevice {
public:
	NativeThreadIntersectionDevice(const Context *context,
		const size_t threadIndex, const size_t devIndex);
	virtual ~NativeThreadIntersectionDevice();

	virtual void SetDataSet(const DataSet *newDataSet);
	virtual void Start();
	virtual void Interrupt();
	virtual void Stop();

	virtual RayBuffer *NewRayBuffer();
	virtual RayBuffer *NewRayBuffer(const size_t size);
	virtual size_t GetQueueSize() { return rayBufferQueue.GetSizeToDo(); }
	virtual void PushRayBuffer(RayBuffer *rayBuffer);
	virtual RayBuffer *PopRayBuffer();

	static size_t RayBufferSize;

	friend class Context;

protected:
	virtual void SetExternalRayBufferQueue(RayBufferQueue *queue);

private:
	static void IntersectionThread(NativeThreadIntersectionDevice *renderDevice);
	boost::thread *intersectionThread;
	RayBufferQueueO2O rayBufferQueue;
	RayBufferQueue *externalRayBufferQueue;
	bool reportedPermissionError;
};

}

#endif	/* _LUXRAYS_INTERSECTIONDEVICE_H */
