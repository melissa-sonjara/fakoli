<?php
/**\addtogroup util
 * @{
 */

/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 * DateUtils - provides support for date/time manipulation.
 */

class DateUtils
{
	function DateUtils()
	{
		
	}
	
	/**
 	* Returns the number of days in the specified month
 	* @param number $m the month number
 	* @param number $y the year
	* @return number the number of days in that month
	* 
 	*/
	static function getMonthDays($m, $y)
	{
		$monthDays = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
		$d = $monthDays[$m - 1];
		if ($m == 2 && ($y % 4) == 0 && ($y % 100) != 0) $d++;
	
		return $d;
	}
	

	/**
 	* Get the number of week days in the specified month
 	* @param number $m the month number
 	* @param number $y the year
 	* @return number the number of week days in that month
 	*/
	static function getMonthWeekDays($month, $year)
	{
		$weekDaysInMonth = array
		(
			28	=>	array(20, 20, 20, 20, 20, 20, 20),
			29	=>	array(21, 21, 21, 21, 21, 20, 20),
			30	=>	array(22, 22, 22, 22, 21, 20, 21),
			31	=>	array(23, 23, 23, 22, 21, 21, 22)
		);
	
		$days = getMonthDays($month, $year);
		$date = strtotime("$year-$month-01");
	
		$first = date("N", $date) - 1;
		return $weekDaysInMonth[$days][$first];
	}	
	
	/*
 	* If start and end dates are the same date 
 	* then show the date followed by start and end time.
 	* Otherwise, show start date and time to end date
 	* and time. 
 	* 
 	* $start - datetime
 	* $end - datetime
 	*/
	static function formatStartDateToEndDate($start, $end)
	{
		$yyyy = substr($start,0,4);
		$yyyy_2 = substr($end,0,4);
		$mm   = substr($start,5,2);
		$mm_2   = substr($end,5,2);
		$dd   = substr($start,8,2);
		$dd_2   = substr($end,8,2);
			
		if($yyyy != $yyyy_2 || $mm != $mm_2 || $dd != $dd_2)
		{
			return formatDateTime12Hr($start) ." &ndash; ". formatDateTime12Hr($end);
		}
		else
		{
			$hh   = substr($start,11,2);
			$mi   = substr($start,14,2);
			$hh_2   = substr($end,11,2);
			$mi_2   = substr($end,14,2);
			$s_time = date("g:ia", mktime($hh,$mi));
			$e_time = date("g:ia", mktime($hh_2,$mi_2));
			return formatDateShort($start) . " $s_time &ndash; $e_time";
		}	
	}
	
/**
	 * DateTime will fail to increment month correctly when current date
	 * is last day of 31 day month. Use addMonths instead.
	 * 
	 * @param DateTime object $date
	 * @param Number $months (can be positive or negative)
	 */
  	static function addMonths(&$date, $months)
  	{
        $init = clone $date;
        $modifier = $months.' months';
        $back_modifier = -$months.' months';
       
        $date->modify($modifier);
        $back_to_init = clone $date;
        $back_to_init->modify($back_modifier);
       
        while($init->format('m') != $back_to_init->format('m'))
        {
        	$date->modify('-1 day');
        	$back_to_init = clone $date;
        	$back_to_init->modify($back_modifier);   
		}
  	}
  	
    /**
	 * DateTime will fail to increment year correctly in some
	 * instances.
	 * 
	 * @param DateTime object $date
	 * @param Number $years (can be positive or negative)
	 */
    static function addYears(&$date, $years)
    {   
        $init = clone $date;
        $modifier = $years.' years';
        $date->modify($modifier);
       
        while($date->format('m')!=$init->format('m'))
        {
        	$date->modify('-1 day');
        }
    }	
    
    /**
     * Fuzzy approximates of some time in the past
     * @param unknown $date
     * @return string
     */
    static function fuzzyDate($date)
    {
    	$time = strtotime($date);
    	$now = time();
    	$ago = $now - $time;
    	if ($ago < 60)
    	{
    		$when = round($ago);
    		$s = ($when == 1)?"second":"seconds";
    		return "$when $s ago";
    	}
    	else if ($ago < 3600)
    	{
    		$when = round($ago / 60);
    		$m = ($when == 1)?"minute":"minutes";
    		return "$when $m ago";
    	}
    	else if ($ago >= 3600 && $ago < 86400)
    	{
		    $when = round($ago / 60 / 60);
		    $h = ($when == 1)?"hour":"hours";
		    return "$when $h ago";
    	}
    	else if ($ago >= 86400 && $ago < 2629743.83)
    	{
		    $when = round($ago / 60 / 60 / 24);
		    $d = ($when == 1)?"day":"days";
		    return "$when $d ago";
    	}
    	else if ($ago >= 2629743.83 && $ago < 31556926)
    	{
		    $when = round($ago / 60 / 60 / 24 / 30.4375);
		    $m = ($when == 1)?"month":"months";
		    return "$when $m ago";
    	}
    	else
    	{
		    $when = round($ago / 60 / 60 / 24 / 365);
		    $y = ($when == 1)?"year":"years";
		    return "$when $y ago";
    	}
    }
    
    /**
     * Return an array of month names in the currently selected locale, indexed by month number.
     * This is a format suitable for use with a SelectFieldRenderer
     * @return array month names indexed by month number
     */
    static function getMonths()
    {
    	$months = array();
    	for($i = 1; $i < 13; ++$i)
    	{
    		$months[$i] = date("F", mktime(0, 0, 0, $i, 10));
    	}
    	return $months;
    }
}
/**@}*/