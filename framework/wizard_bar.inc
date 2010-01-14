<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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
 * 
 */
 
/**
 * The WizardBar class is a user-interface control that
 * draws a stepped nav bar for multi-stage workflows.
 * 
 * 
 */
class WizardBar
{
	var $styles = array("past"			=>	"past_step",
						"current"		=>	"current_step",
						"future"		=>	"future_step",
						"unreachable"	=>	"unreachable_step");

	var $steps  = array();
	var $spacer = " ";
	var $queryString = null;
	var $showNumber = false;
	var $showNext = true;
	var $page = "";
	var $links = true;
	
	/**
	 * Construct a new WizardBar control.
	 * 
	 * @param string $queryString the Query String portion of the page URIs
	 * @param array  $steps the steps in the workflow
	 * @param string $spacer the spacer symbol to put between steps
	 * @param bool   $number flag to indicate whether to display step numbers
	 * @param bool   $showNext flag to indicate whether to display a "Next Page" link
	 */
	function WizardBar($steps = array(), $queryString = "", $spacer = "&nbsp;&raquo;&nbsp;", $number = false, $showNext = false, $useQueryString = true)
	{
		global $_SERVER;
		
		$this->steps          = $steps;
		$this->page           = basename($_SERVER["PHP_SELF"]);
		$this->queryString    = $queryString;
		$this->spacer         = $spacer;
		$this->showNumber     = $number;
		$this->showNext       = $showNext;
		$this->useQueryString = $useQueryString;
		
	}
	
	/**
	 * Writes the HTML for this control to standard output.
	 */
	function writeHTML()
	{
	    $first   = true;
	    $future  = false;
	    $next    = "";
		$count   = 0;
		$getNext = false;
    
	    foreach ($this->steps as $text => $url)
    	{
        	++$count;
        
	        if (!$first)
        	{
            	echo $this->spacer;
        	}
        	else
        	{
            	$first = false;
        	}
         
	        if ($url == $this->page)
	        {
	            $style = $this->styles["current"];
	            $future = true;
	            $getNext = true;
	        }
	        else if ($future)
	        {
	        	if ($this->queryString != "" || !$this->useQueryString)
	        	{
		            $style = $this->styles["future"];
		            if ($getNext)
		            {
		                $next = "$url?{$this->queryString}";
		                $getNext = true;
		            }
	        	}
	        	else
	        	{
	        		$style = $this->styles["unreachable"];
	        	}
	        }
	        else
	        {
	            $style = $this->styles["past"];
	        }
        
        	if ($this->showNumber) $text = "$count.&nbsp;$text";
         
        	if ($this->links)
        	{
	         	if ($this->useQueryString)
	         	{
		        	$dest = ($this->queryString!="")?"$url?{$this->queryString}":"#";
	         	}
	         	else
	         	{
	         		$dest = ($url == $this->page) ? "#" : $url;
	         	}
        	}
        	else
        	{
        		$dest = "#";        
        	}
        	
	        echo "<a href=\"$dest\" class=\"$style\">$text</a>";
    	}
		
		if ($next && $this->showNext) echo $this->spacer."<a href='$next'>Next Page</a>";
    	return $next;		
	}
	
	/**
	 * Appends a query string to the supplied URL
	 *
	 * @param string $url
	 * @param string $qs
	 * @return string
	 */
	function appendQueryString($url, $qs)
	{
		$url .= (strstr($url, "?") !== false) ? "&$qs" : "?$qs";
		return $url;
	}

	/**
	 * Get the next page in the tab list after the current page.
	 *
	 * @return string
	 */
	function getNextPage()
	{
		$found = false;
		
		foreach($this->steps as $text => $url)
		{
			if ($found)
			{
				$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) : $url;
				return $dest;
			}
			else if ($url == $this->page)
			{
				$found = true;
			} 
		}
		
		return "";
	}
	
	/**
	 * Return the URL for the page at the specified tab index.
	 * 
	 * @param string $idx the index of the tab for which we require the URL 
	 */
	function getPage($idx)
	{
		$i = 0;
		
		foreach($this->steps as $text => $url)
		{
			if ($i == $idx) break;
			$i++;
		}

		$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) : $url;
		return $dest;	
	}
	
}
?>