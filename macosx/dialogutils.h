/*
 *  dialogutils.h
 *  np2
 *
 *  Created by tk800 on Sat Oct 25 2003.
 *
 */


void endLoop(WindowRef window);
ControlRef getControlRefByID(OSType sign, int id, WindowRef win);
void SetInitialTabState(WindowRef theWindow, UInt16 pane, int max);
short changeTab(WindowRef window, UInt16 pane);
