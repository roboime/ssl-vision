//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    capture_thread.h
  \brief   C++ Interface: CaptureThread
  \author  Stefan Zickler, (C) 2008
*/
//========================================================================

#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H
#ifdef USE_DC1394
#include "capturedc1394v2.h"
#endif
#ifdef USE_MMF
#include "capturemf.h"
#endif
#include "capturefromfile.h"
#include "capture_generator.h"
#include <QThread>
#include "ringbuffer.h"
#include "framedata.h"
#include "framecounter.h"
#include "visionstack.h"
#include "capturestats.h"
#ifndef _WIN32
//TODO: make a windows equivalent for this
#include "affinity_manager.h"
#endif

/*!
  \class   CaptureThread
  \brief   A thread for capturing and processing video data
  \author  Stefan Zickler, (C) 2008
*/
class CaptureThread : public QThread
{
Q_OBJECT
protected:
  QMutex stack_mutex; //this mutex protects multi-threaded operations on the stack
  QMutex capture_mutex; //this mutex protects multi-threaded operations on the capture control
  VisionStack * stack;
  FrameCounter * counter;
  CaptureInterface * capture;
#ifdef USE_DC1394
  CaptureInterface * captureDC1394;
#endif
#ifdef USE_MMF
  CaptureInterface * captureMF;
#endif
  CaptureInterface * captureFiles;
  CaptureInterface * captureGenerator;
#ifdef USE_AFFINITY_MANAGER
  AffinityManager * affinity;
#endif
  FrameBuffer * rb;
  bool _kill;
  int camId;
  VarList * settings;
#ifdef USE_DC1394
  VarList * dc1394;
#endif
#ifdef USE_MMF
  VarList * mmf;
#endif
  VarList * generator;
  VarList * fromfile;
  VarList * control;
  VarTrigger * c_start;
  VarTrigger * c_stop;
  VarTrigger * c_reset;
  VarTrigger * c_refresh;
  VarBool * c_auto_refresh;
  VarStringEnum * captureModule;
  Timer timer;

public slots:
  bool init();
  bool stop();
  bool reset();
  void refresh();
  void selectCaptureMethod();

public:
  void setFrameBuffer(FrameBuffer * _rb);
  FrameBuffer * getFrameBuffer() const;
  void setStack(VisionStack * _stack);
  VisionStack * getStack() const;
  void kill();
  VarList * getSettings();
#ifdef USE_AFFINITY_MANAGER
  void setAffinityManager(AffinityManager * _affinity);
#endif
  CaptureThread(int cam_id);
  ~CaptureThread();

  virtual void run();

};








#endif
