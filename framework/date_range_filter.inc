<?php
/*
 * Provides from and to date selection with Previous and 
 * Next month/year buttons.
 * 
 * usage examples:
 *
 * $dateRangeFilter = new DateRangeFilter("startDate", "endDate");
 * $dateRangeFilter->calculateDateRange();
 * 
 * use calculated sql dates in a query on a datamodel object:
 * datefield >= '{$dateRangeFilter->from}' and datefield <= '{$dateRangeFilter->to}
 * 
 * You can draw both date input fields and Prev/Next buttons or
 * just one of the two:
 * 
 * To draw the input fields for entering custom dates:
 * $dateRangeFilter->writeFilter();
 * 
 * To draw the Prev/Next buttons:
 * $dateRangeFilter->writeButtons();
 * 
 * For Prev/Next,the default stepping mode is by month. To set to 
 * by year, call as
 * 
 * $dateRangeFilter = new DateRangeFilter("startDate", "endDate", "year");
 *
 * @author: Andy Green
 * 
 * year: 2011
 */
require_once realpath(dirname(__FILE__))."/validation.inc";

class FakoliDateRangeFilter
{
	var $startParam;
	var $endParam;
	var $stepMode; // month, year or fiscal_year
	
	var $startDate;
	var $endDate;
	
	var $defaultStart;
	var $default_end;
	var $stepLabel;
	var $method;
	
	var $hiddens = array();
	
	function FakoliDateRangeFilter($startParam = "startDate", $endParam = "endDate", $stepMode = "month")
	{
		$this->startParam = $startParam;
		$this->endParam = $endParam;
		$this->stepMode = $stepMode;
		
		$this->defaultStartDate = null;
		$this->defaultEndDate   = null;
		
		$this->method = "POST";
	}
	
	function calculateDateRange()
	{
		$this->startDate = $_REQUEST[$this->startParam];
		$this->endDate   = $_REQUEST[$this->endParam];
		
		if ($this->startDate || $this->endDate)
		{
			$validator = new DateValidator($this->startParam, "From");
			$validator->required = false;
			$this->msg = $validator->validate();
			
			if ($this->msg) 
			{
				$startDate = null;
			}
			
			$validator = new DateValidator($this->endParam, "To");
			$validator->required = false;
			
			$this->msg = $validator->validate();
		
			if ($this->msg)
			{
				$endDate = null;
			}
		
			if ($this->startDate) $this->startDate = new DateTime($this->startDate);
			if ($this->endDate) $this->endDate = new DateTime($this->endDate);
			
			if ($this->startDate && $this->endDate) $this->subTitle = $this->startDate->format("jS F Y")." to ".$this->endDate->format("jS F Y");
		
			$this->subTitle = $this->startDate->format("F d, Y") . " &ndash; " .
				 $this->endDate->format("F d, Y");
			
			if($this->endDate)
				$this->endDate->modify("+1 days"); // Inclusive of last day...

			$this->month = $this->startDate->format("m");
			$this->year = $this->startDate->format("Y");
		}
	
		$this->month = checkNumeric($_REQUEST['month']);
		$this->year = checkNumeric($_REQUEST['year']);

		if (!$this->year) $this->year = date("Y");

		// JDG 11/2011 - need to set step mode whether we have a 
		// custom start/end date or not
		if ($this->stepMode == "year")
		{
			// calendar year stepping mode
			$this->setStepModeYear();
		}
		else
		{
			// Monthly stepping mode
			$this->setStepModeMonth();
		}

		$this->from = $this->startDate->format("Y-m-d");
		$this->to = $this->endDate->format("Y-m-d");
		$this->calculated = true;
	}
	
	function setStepModeMonth()
	{
		if (!$this->month)
		{
			$this->year = date("Y");
			$this->month = date("m");
		}
				
		$this->prevMonth = $this->month - 1;
		$this->prevYear = $this->year;
		if ($this->prevMonth == 0)
		{
			$this->prevYear--;
			$this->prevMonth = 12;
		}
				
		$this->nextMonth = $this->month + 1;
		$this->nextYear = $this->year;
		if ($this->nextMonth > 12)
		{
			$this->nextMonth = 1;
			$this->nextYear++;
		}
				
		if(!$this->startDate)
			$this->startDate = new DateTime("{$this->year}-{$this->month}-01T00:00:00");
			
		if(!$this->endDate)
		{
			$this->endDate = clone $this->startDate;
			$this->endDate->modify("+1 months");
		}

		if(!$this->subTitle)
			$this->subTitle = $this->startDate->format("F Y");	
	}
	
	function setStepModeYear($startMonth = 1)
	{
		$this->month = $startMonth;
				
		$this->prevMonth = $startMonth;
		$this->prevYear = $this->year - 1;
				
		$this->nextMonth = $startMonth;
		$this->nextYear = $this->year + 1;
		$this->subTitle=  $this->year;
		
		if(!$this->startDate)
			$this->startDate = new DateTime("{$this->year}-{$this->month}-01T00:00:00");
			
		if(!$this->endDate)
		{
			$this->endDate = clone $this->startDate;
			$this->endDate->modify("+1 years");
		}
	}
	
	function writeScript()
	{
		$script .= <<<ENDSCRIPT

<script src='/fakoli/calendar/sonjara_calendar.js' type='text/javascript'></script>
<link href='/fakoli/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'></link>
<script type="text/javascript">
var startDateCalendar = new Calendar('startDateCalendar', 'dateRangeForm', 'startDate');
var endDateCalendar = new Calendar('endDateCalendar', 'dateRangeForm', 'endDate');
</script>

ENDSCRIPT;
		
		return $script;
	}

	function writeFilter()
	{
		if (!$this->calculated) $this->calculateDateRange();
		$endDate = clone $this->endDate;
		$endDate->modify("-1 days"); // Display original date range
?>
<form id="dateRangeForm" method="<?echo $this->method?>" action="">
<?php 
if(count($this->hiddens) > 0)
{
	foreach($this->hiddens as $name => $value)
	{
		echo "<input type=\"text\" name=\"$name\" value=\"$value\"";
	}
}
?>
 <label>Reporting Period From: </label>
 <input type="text" size="10" name="startDate" value="<?echo $this->startDate->format("m/d/Y")?>">&nbsp;
 <img src='/fakoli/calendar/calendar.gif' alt='Popup Calendar' style='vertical-align: middle; border:0' onclick='startDateCalendar.toggle(this)'/>
 <label> To: </label><input type="text" size="10" name="endDate" value="<?echo $endDate->format("m/d/Y")?>">&nbsp;
 <img src='/fakoli/calendar/calendar.gif' alt='Popup Calendar' style='vertical-align: middle; border:0' onclick='endDateCalendar.toggle(this)'/>
 &nbsp;<input type="submit" class="button" value=" Update View "/>
</form>
<?
	}
	
	function writeButtons()
	{
		if (!$this->calculated) $this->calculateDateRange();
		
		$queryString = $_SERVER["QUERY_STRING"];
		
		$queryString = preg_replace("/year=\\d+\\&?/", "", $queryString);
		$queryString = preg_replace("/month=\\d+\\&?/", "", $queryString);
		
		$prevLink = appendToQueryString($queryString, "year=$this->prevYear&month=$this->prevMonth");
		$nextLink = appendToQueryString($queryString, "year=$this->nextYear&month=$this->nextMonth");
		
		$stepLabel = ($this->stepLabel) ? $this->stepLabel : prettify($this->stepMode);
?>
<table width="100%" border="0">
 <tr>
  <td align="left"><input type="button" class="button" value="  &laquo; Previous <?echo $stepLabel ?>   " onclick="go('<?echo $prevLink?>');"></td>
  <td align="right"><input type="button" class="button" value="  Next <?echo $stepLabel ?> &raquo;  " onclick="go('<?echo $nextLink?>');"></td>
 </tr>
</table>
<?
	}
}
?>