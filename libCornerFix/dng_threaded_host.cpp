/* ======================================
 * CornerFix - Fixing the M8 Cyan Corners
 * ======================================
 *
 * Project Info:  http://sourceforge.net/projects/cornerfix
 * Project Lead:  Sandy McGuffog (sandy.cornerfix@gmail.com);
 *
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * ---------------
 * dng_threaded_host.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 16 Sept 2009 : 1.3.0.0
 * 29 Jan 2010 : 1.3.0.5
 *
 */

/*
 * Implements multithreading for dng_host
 */

#include "dng_abort_sniffer.h"
#include "dng_area_task.h"
#include "dng_negative.h"
#include "dng_threaded_host.h"


#define kLocalUseThreads

#if defined(kLocalUseThreads)
#if qWinOS
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#endif

enum threadVals {
	kMaxLocalThreads = 16,
};

#if defined(kLocalUseThreads)
//////////////////////////////////////////////////////////////
//
// cppThread class
//
//////////////////////////////////////////////////////////////
// cppThread abstracts most of the ugly nasy thread stuff into 
// one relatively small place, hiding all the Win32/OS X/Linux
// specific stuff
// Classes that inherit from cppThread can be platform idependant

class cppThread {
#if qWinOS
	HANDLE thread;
#else
	pthread_attr_t threadAttr;
	pthread_t thread;
#endif
#if qWinOS
	static unsigned __stdcall thread_func(void* d) { 
#else
	static void * thread_func(void *d){
#endif
		((cppThread *)d)->run();
		return NULL;
	}
	
public:
	cppThread() :
	thread(NULL)
	{
#if qWinOS
#else
		// Make very sure the thread is joinable....this is the default on most systems,
		// but you never know
		// Initialize and set thread joinable attribute
		pthread_attr_init(&threadAttr);
		pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
		// Default stack size under OS X is 524K
		if (stackSize() != 0) {
			pthread_attr_setstacksize (&threadAttr, stackSize());
		}
#endif		
	}
	~cppThread(){
#if qWinOS
		if (thread != NULL) CloseHandle(thread);
#else
		pthread_attr_destroy(&threadAttr);
#endif
	}
		
	size_t stackSize() {
		// 0 gives the default size for the platform
		// Override if we know something better
		return 0;
	}
	
	virtual void run(){
	}
	
	int start(){
#if qWinOS
		// Note this creates a thread with a stack the same size as the main thread;
		// probably wasteful, but we have no idea what tasks might run here
		// Under Windows VC++ 6 that is 1MB 
		thread = (HANDLE)_beginthreadex(NULL, (unsigned) stackSize(), cppThread::thread_func, (void*)this, 0, NULL);
		return thread == NULL ? -1 : 0;
#else
		return pthread_create(&thread, &threadAttr, cppThread::thread_func, (void*)this);
#endif
	}
	
	int wait(){
#if qWinOS
		int retVal = 0;
		if (WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) {
			retVal = -1;
		}
		CloseHandle(thread);
		thread = NULL;
		return retVal;
#else
		return pthread_join(thread, NULL);
#endif
	}
};


//////////////////////////////////////////////////////////////
//
// areaThread class
//
//////////////////////////////////////////////////////////////
// areaThread uses cppThread to execute a single area task
	
class areaThread : public cppThread {
public:
	areaThread(dng_area_task &taskVal,
			   uint32 threadIndexVal,
			   const dng_rect &threadAreaVal,
			   const dng_point &tileSizeVal,
			   dng_abort_sniffer *snifferVal) : 
	task(taskVal),
	threadIndex(threadIndexVal),
	threadArea(threadAreaVal),
	tileSize(tileSizeVal),
	sniffer(snifferVal)
	{
		
	}
	
	void run() {
		task.ProcessOnThread (threadIndex, threadArea, tileSize, sniffer);
	}	
	
private:
	dng_area_task &task;
	uint32 threadIndex;
	dng_rect threadArea;
	dng_point tileSize;
	dng_abort_sniffer *sniffer;
};

#endif
	
//////////////////////////////////////////////////////////////
//
// dng_threaded_host
//
//////////////////////////////////////////////////////////////
// Adds threading to dng_host

void dng_threaded_host::PerformAreaTask (dng_area_task &task,
							  const dng_rect &area) {

	dng_point tileSize (task.FindTileSize (area));
	
#if defined(kLocalUseThreads)
	// Now we need to do some resource allocation
	// We start by assuming one tile per thread, and work our way up
	// Note that a thread pool is technically a better idea, but very complex
	// in a Mac/Windows environment; this is the cheap and cheerful solution
	
	int vTilesPerThread = 1;
	int hTilesPerThread = 1;
	int vTilesinArea = area.H()/tileSize.v;
	vTilesinArea = vTilesinArea*((int) tileSize.v) < ((int) area.H()) ? vTilesinArea + 1 : vTilesinArea;
	int hTilesinArea = area.W()/tileSize.h;
	hTilesinArea = hTilesinArea*((int) tileSize.h) < ((int) area.W()) ? hTilesinArea + 1 : hTilesinArea;
	dng_point vInc = tileSize;
	dng_point hInc = tileSize;
	areaThread *localThreads[kMaxLocalThreads];
	
	// Ensure we don't exceed maxThreads for this task
	while (((vTilesinArea+vTilesPerThread-1)/vTilesPerThread)*((hTilesinArea+hTilesPerThread-1)/hTilesPerThread) > 
		   Min_int32(task.MaxThreads (), kMaxLocalThreads)) {
		// Here we want to increase the number of tiles per thread
		// So do we do that in the V or H dimension?
		if (vTilesinArea / vTilesPerThread > hTilesinArea / hTilesPerThread) {
			vTilesPerThread++;
			vInc = vInc + tileSize;
		}
		else {
			hTilesPerThread++;
			hInc = hInc + tileSize;
		}
	}
	vInc.h = 0;
	hInc.v = 0;
	
	dng_rect threadArea(area.t, area.l, vInc.v+area.t, hInc.h+area.l);
	
	task.Start (Min_uint32(task.MaxThreads (), kMaxLocalThreads), tileSize, &Allocator (), Sniffer ());
	int vIndex = 0;
	int threadIndex = 0;
	while (vIndex < vTilesinArea) {
		int hIndex = 0;
		threadArea.l = area.l;
		threadArea.r = hInc.h+threadArea.l;
		while (hIndex < hTilesinArea) {
			localThreads[threadIndex] = new areaThread(task,
													   threadIndex,
													   threadArea,
													   tileSize,
													   Sniffer ());
			if (localThreads[threadIndex] != NULL) {
				if (localThreads[threadIndex]->start() != 0) {
					delete localThreads[threadIndex];
					localThreads[threadIndex] = NULL;
				}
			}
			if (localThreads[threadIndex] == NULL) {
				// If something goes bad, do this non-threaded
				task.ProcessOnThread (threadIndex, threadArea, tileSize, Sniffer ());
			}
			threadArea = threadArea + hInc;
			threadArea.r = Min_int32 (threadArea.r, area.r); 
			hIndex += hTilesPerThread;
			threadIndex++;
		}
		threadArea = threadArea + vInc;
		threadArea.b = Min_int32 (threadArea.b, area.b);
		vIndex += vTilesPerThread;
	}
	for (int i = 0; i < threadIndex; i++) {
		if (localThreads[i] != NULL) {
			// There's nothing we can do if join fails; just hope for the best(!)
			localThreads[i]->wait();
			delete localThreads[i];
		}
	}
	task.Finish (Min_uint32(task.MaxThreads (), kMaxLocalThreads));
#else
	task.Start (1, tileSize, &Allocator (), Sniffer ());
	task.ProcessOnThread (0, area, tileSize, Sniffer ());	
	task.Finish (1);
#endif
	
}