/*
 *  dialogutils.h
 *  np2
 *
 *  Created by tk800 on Sat Oct 25 2003.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void endLoop(WindowRef window);
ControlRef getControlRefByID(OSType sign, int id, WindowRef win);
void SetInitialTabState(WindowRef theWindow, UInt16 pane, int max);
short changeTab(WindowRef window, UInt16 pane);
pascal OSStatus changeSlider(ControlRef theControl, WindowRef theWindow, short base);
void uncheckAllPopupMenuItems(OSType ID, short max, WindowRef win);
void setjmper(BYTE *board, BYTE value, BYTE bit);

#ifdef __cplusplus
}
#endif
