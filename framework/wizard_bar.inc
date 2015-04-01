<?php
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

Fakoli::usingFeature("tab_bar");


 
/**
 * The WizardBar class is a user-interface control that
 * draws a stepped nav bar for multi-stage workflows.
 * 
 * 
 */
class WizardBar extends TabBar
{
	var $spacer = "&nbsp;&raquo;&nbsp;";
	
	/**
	 * Construct a new WizardBar control.
	 * 
	 * @param string $queryString the Query String portion of the page URIs
	 * @param array  $tabs the steps in the workflow
	 * @param string $queryString the query string to append to the links
	 * @param bool   $useQueryString flag to indicate whether a query string is required.
	 *               If so, and none is provided, then all tabs except the first are disabled.
	 * @param bool   $showNumber flag to indicate whether to display step numbers
	 */
	function WizardBar($id, $tabs = array(), $queryString = "", $useQueryString = true, $showNumber = false)
	{
		parent::TabBar($id, $tabs, $queryString, $useQueryString, $showNumber);
	}
	
	/**
	 * Writes the HTML for this control to standard output.
	 */
	function writeHTML()
	{
	    $first   = true;
	    $future  = false;
		$count   = 0;

	    echo "<div id='{$this->id}'";
	    
	    if ($this->cssClass) echo " class='{$this->cssClass}'";
	    
	    echo "><ul>";		
		foreach($this->tabs as $text => $url)
	    {
     		
 	    	++$count;
        	$active = "";
        	$style = "";

        	if (!$first)
     		{	
     			$spacer = $this->spacer;
        	}
        	else
        	{
        		$spacer = "";
            	$first = false;
            	$style = "first ";
        	}
      	
          	if ($url == $this->page)
	        {
	        	$style .= "current";
	        	$future = true;	        
	        }
     		elseif ($this->disabled || 
     				(($future && ($this->queryString == "" && $this->useQueryString)) && $count > 1) 
     				|| $this->states[$url] == 'disable')
	        {
	     		$active = " class='disabled'";
	     		$style .= "disabled";
	        }
	  
        	if ($this->showNumber) $text = "$count.&nbsp;$text";
         
         	if ($this->useQueryString)
         	{
	        	$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) :"#";
         	}
         	else
         	{
         		$dest = ($url == $this->page || $this->disabled) ? "#" : $url;
         	}
        
         	if ($this->anchor)
         	{
         		$dest = ($dest != "#") ? "$dest#{$this->anchor}" : "#{$this->anchor}";
         	}
         	
         	if ($this->showStates)
         	{
	         	if (array_key_exists($url, $this->flags))
	         	{
	         		$text = "<img src='{$this->states[$this->flags[$url]]}' style='border: none; display: inline-block;vertical-align: middle'/>&nbsp;$text";         	
	         	}
	         	else if ($this->defaultStateImage)
	         	{
	         		$text = "<img src='{$this->defaultStateImage}' style='border: none; display: inline-block;vertical-align: middle'/>&nbsp;$text";
	         	}
         	}
         	
             echo "<li class='$style'>$spacer<a href=\"$dest\"$active>$text</a></li>";
        	}	
			
    	// close wizard div
    	echo "</ul></div>\n";
 	}
}
?>