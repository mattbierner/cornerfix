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
* CFViewController.h
* ---------------
* (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
*
* Original Author:  Sandy McGuffog;
* Contributor(s):   -;
*
*
* Changes
* -------
* 7 Sept 2007 : 0.9.1.3
* 4 July 2011 : 1.4.2.0
*
*/

/*
 * This sorts out some of the more egregiously bad behavior of the NSSplitterView class
 * It functions as a delegate. In most languages, we would subclass, but this objective-C(!)
 */

@interface CFViewController : NSObject <NSSplitViewDelegate> {
	float splitFactor;
}



@end
