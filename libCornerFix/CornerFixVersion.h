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
 * CornerFixVersion.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 22 Jun 2007 : 0.9.0.6
 * 1 Jul 2007 : 0.9.0.7
 * 1 Jul 2007 : 0.9.1.0
 * 1 Sept 2007 : 0.9.1.0 Mac version
 * 4 Sept 2007 : 0.9.1.1
 * 5 Sept 2007 : 0.9.1.2
 * 11 Sept 2007 : 0.9.1.3
 * 21 Sept 2007 : 0.9.1.4
 * 23 Oct 2007 : 0.9.1.5
 * 8 Nov 2007 : 0.9.1.6
 * 29 Nov 2007 : 0.9.1.7
 * 20 Dec 2007 : 0.9.1.8
 * 22 Dec 2007 : 0.9.1.9
 * 12 Jan 2008 : 0.9.2.0
 * 2 December 2008 : 1.0.0.0
 * 12 June 2009 : 1.1.0.0
 * 14 July 2009 : 1.1.0.1
 * 1 Sept 2009 : 1.1.1.0
 * 12 Sept 2009 : 1.2.0.0
 * 21 Oct 2009 : 1.3.0.0
 * 21 Oct 2009 : 1.3.0.1
 * 22 Oct 2009 : 1.3.0.2
 * 9 Nov 2009 : 1.3.0.3
 * 28 Nov 2009 : 1.3.0.4
 * 29 Jan 2010 : 1.3.0.5
 * 9 Mar 2010 : 1.3.0.6
 * 29 Mar 2010 : 1.3.0.7
 * 17 May 2010 : 1.3.0.8
 * 12 Sept 2010 : 1.3.0.9
 * 6 October 2010 : 1.4.0.0
 * 15 October 2010 : 1.4.0.1
 * 22 October 2010 : 1.4.0.2
 * 26 October 2010 : 1.4.0.3
 * 23 January 2011 : 1.4.1.0
 * 4 April 2011 : 1.4.1.1
 * 4 July 2011 : 1.4.2.0
 * 12 July 2012 : 1.4.2.1
 * 31 July 2012 : 1.4.2.2
 * 18 Sept 2013 : 1.4.2.3
 * 5 October 2013 : 1.4.2.4
 * 17 October 2013 : 1.4.2.5
 * 4 December 2013 : 1.4.2.6
 *
 */


#define CornerFixVersion "1.4.2.6"
#if qWinOS
#ifdef DMR_HACK
#define CornerFixNameString "DMR2M8 for Windows"
#else
#define CornerFixNameString "CornerFix for Windows"
#endif
#else
#ifdef DMR_HACK
#define CornerFixNameString "DMR2M8 for the Mac"
#else
#define CornerFixNameString "CornerFix for the Mac"
#endif
#endif

