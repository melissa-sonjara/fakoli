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

 
/**
 * The DataItemTabBar class is a user-interface control that
 * manages a line of tabs for multi-page dialogs. Unlike the regular
 * TabBar control, a DataItemTabBar is bound to a collection
 * of DataItem objects.
 */
class DataItemTabBar
{
	var $id;	///< The DOM ID of the DataItemTabBar container element.
	
	var $items;
	var $format;
	
	var $uri;
	var $primary_key;
	var $spacer;
	
	function DataItemTabBar($id, $items, $format = null, $uri = "")
	{
		$this->id = $id;
		$this->items = $items;
		$this->format = $format;
		if (count($this->items))
		{
			$this->primary_key = $this->items[0]->getPrimaryKey();
		}
		
		$this->uri = $uri;
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
		if ($url[0] == "&") 
		{
			$url = $qs.$url;
		}
		else
		{
			$url .= (strstr($url, "?") !== false) ? "&$qs" : "?$qs";
		}
		return $url;
	}

	/**
	 * Writes the HTML for this control to standard output.
	 */
	function writeHTML()
	{
		if (array_key_exists($this->primary_key, $_GET))
		{
			$key = $_GET[$this->primary_key];
		}
		else
		{
			$key = $this->items[0]->get($this->primary_key);
		}
	
	    $first   = true;
	    $future  = false;
    
	    if ($this->anchor) echo "<a name='<?echo $this->anchor?>'></a>";
	    
	    echo "<div id='{$this->id}'";
	    
	    if ($this->cssClass) echo " class='{$this->cssClass}'";
	    
	    echo "><ul>";
	    
	    $count = 0;
	    
	    foreach ($this->items as $item)
    	{
        	++$count;
        	$active = "";
        	$style = "";

        	$pk = $item->get($this->primary_key);

        	$url = $this->uri . "?" . $this->primary_key . "=" . urlencode($pk);

        	$text = $item->format($this->format);
        	
	        if (!$first)
        	{
            	echo $this->spacer;
        	}
        	else
        	{
            	$first = false;
        	}
         
           	if ($pk == $key)
	        {
	        	$style = " class='current'";
	        }
        
        	if ($this->showNumber) $text = "$count.&nbsp;$text";

        	$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) : $url;
        
         	if ($this->anchor)
         	{
         		$dest = ($dest != "#") ? "$dest#{$this->anchor}" : "#{$this->anchor}";
         	}
         	
         	if ($this->showStates)
         	{
	         	if (array_key_exists($url, $this->flags))
	         	{
	         		$text = "<img src='{$this->states[$this->flags[$pk]]}' style='border: none; display: inline-block;vertical-align: middle'/>&nbsp;$text";         	
	         	}
	         	else if ($this->defaultStateImage)
	         	{
	         		$text = "<img src='{$this->defaultStateImage}' style='border: none; display: inline-block;vertical-align: middle'/>&nbsp;$text";
	         	}
         	}
	        echo "<li$style><a href=\"$dest\"$active>$text</a></li>";
    	}
		
		echo "</ul></div>\n";
	}
}
?>