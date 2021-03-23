<?php
/**\addtogroup ui
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
 * The TabBar class is a user-interface control that
 * manages a line of tabs for multi-page dialogs.
 */
class TabBar
{
	var $id;					///< The DOM ID of the TabBar container element.
	var $tabs  = array();		///< The tab records in this tab bar
	var $queryString = null;	///< The query string parameters to append to the navigation links
	var $useQueryString = true;	///< Whether a query string is being used as the navigation key
	var $page = "";				///< The current page (for display purposes). This can be set explicitly to force the correct tab display when in a sub-page, for instance
	var $showNumber;			///< True to display the step number, false if not
	var $states = array();		///< Array of display states to apply, indexed by tab
	var $flags = array();		///< Array of flags to apply, indexed by tab
	var $anchor = "";			///< Anchor name to generate (leave empty for no anchor tag)
	var $cssClass = "";			///< CSS class to apply to the top level container for the tabs
	var $defaultStateImage;		///< Image to show in the default tab state
	var $showStates = true;		///< true to show tab states, false to ignore them
	
	var $dynamic = false;				///< true to indicate dynamic (AJAX-based) loading
	var $container = "";				///< The DOM ID of the container that will contain dynamically loaded content
	var $dynamicLoadHandler = null;		///< Javascript function to call to handle dynamic content load (optional)
	var $dynamicLoadFirstTab = true;	///< By default, dynamically loaded tab bars will do a request for the content of the first tab on page load. Set to false to disable this.
	
	var $disabled = false;		///< Set to true to generate disabled tab navigation
	
	/**
	 * Construct a new TabBar control.
	 * 
	 * @param string $queryString the Query String portion of the page URIs
	 * @param array  $tabs the steps in the workflow
	 * @param string $queryString the query string to append to the links
	 * @param bool   $useQueryString flag to indicate whether a query string is required.
	 *               If so, and none is provided, then all tabs except the first are disabled.
	 * @param bool   $showNumber flag to indicate whether to display step numbers
	 */
	function TabBar($id, $tabs, $queryString = "", $useQueryString = true, $showNumber = false)
	{
		global $config;
		global $isAdmin;
		
		$this->id			  = $id;
		$this->tabs           = $tabs;
		$this->page           = ($config['prettyURLs'] === false) ? basename($_SERVER["PHP_SELF"]) :  $_REQUEST['identifier'];
		if ($isAdmin) $this->page = "/admin/{$this->page}";
		$this->queryString    = $queryString;
		$this->useQueryString = $useQueryString;
		$this->showNumber     = $showNumber;
		trace("PAGE: **** {$this->page}", 4);
	}
	
	/**
	 * Preserve the current query string in tab links
	 */
	function preserveQueryString()
	{
		$qs = getCleanQueryString();
		if ($qs)
		{
			$this->queryString = $qs;
			$this->useQueryString = true;
		}
		else
		{
			$this->queryString = null;
			$this->useQueryString = false;
		}
	}
	
	function dynamicLoad($container)
	{
		$this->container = $container;
		$this->dynamic = true;
	}
	
	function writeScript()
	{
		$script = "";
		if (!$this->dynamic) return $script;
		
		$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		document.id('{$this->id}').getElements("a").each(function(a)
		{
			a.loadTab = function(override)
ENDSCRIPT;
		if ($this->dynamicLoadHandler)
		{
			$script .= <<<ENDSCRIPT
			{
				var url = override ? override : this.href;
				{$this->dynamicLoadHandler}(url);
			};	
ENDSCRIPT;
		}
		else
		{
			$script .= <<<ENDSCRIPT
			{
				var request = new Request.HTML(
				{
					evalScripts: false,
					evalResponse: false,
					method: 'get', 
					url: override ? override : this.href, 
					onSuccess: function(tree, elements, html, script) 
					{ 
						var container = document.id('{$this->container}');
						container.set('text', '');
						container.adopt(tree);
		
						Browser.exec(script);

					}.bind(this)
				});
				
				request.send();
			};
ENDSCRIPT;
		}

		$script .= <<<ENDSCRIPT

			a.addEvent('click', function(e) 
			{
				new DOMEvent(e).stop();
				var url = a.href;
				var parent = a.getParent();
				document.id('{$this->id}').getElements("li").each(function(l) { l.removeClass("current"); });
				parent.addClass('current');
				
				a.loadTab();			
			});
		});
ENDSCRIPT;

		if ($this->dynamicLoadFirstTab)
		{
			$script .= <<<ENDSCRIPT
		document.id('{$this->id}').getElements("li.current a").each(function(a)
		{
			a.loadTab();
		});
ENDSCRIPT;
		}

		$script .= <<<ENDSCRIPT
	});
	</script>	
ENDSCRIPT;
		return $script;
	}
	/**
	 * Writes the HTML for this control to standard output.
	 */
	function writeHTML()
	{
		foreach($this->tabs as $text => $entry)
		{
			if (is_object($entry) && get_class($entry) == "TabBar")
			{
				$entry->page = $this->page;
				$entry->queryString = $this->queryString;
			}
		}
		
	    $first   = true;
	    $future  = false;
    
	    if ($this->anchor) echo "<a name='<?echo $this->anchor?>'></a>";
	    
	    echo "<div id='{$this->id}'";
	    
	    if ($this->cssClass) echo " class='{$this->cssClass}'";
	    
	    echo "><ul>";
	    
	    foreach ($this->tabs as $text => $entry)
    	{
        	++$count;
        	$active = "";
        	$style = "";
        	
        	//AJG - make it easier to have conditional tabs. 
        	if ($entry === null) continue;
        	
        	if (is_string($entry))
        	{
        		$url = $entry;
        	}
        	else if (is_object($entry) && get_class($entry) == "TabBar")
        	{
        		if ($entry->findPage($this->page))
        		{
        			$url = $this->page;
        		}
        		else
        		{
        			$url = $entry->getPage(0, false);
        		}
        	}
        	
	        if (!$first)
        	{
            	echo $this->spacer;
        	}
        	else
        	{
            	$first = false;
        	}
         
        	$isMatch = $this->comparePaths($url, $this->page);
        	// JDG 12/01/09, get the basename of the url, in case we need a
        	// longer path in the tab include file, as with digital movies
        	if ($isMatch)
	        {
	        	$style = " class='current'";
	        }

        	if (($this->disabled || ($this->queryString == "" && $this->useQueryString)) && $count > 1)
        	{
        		$active = " class='disabled'";
	        }
        
        	if ($this->showNumber) $text = "$count.&nbsp;$text";
         
         	if ($this->useQueryString)
         	{
	        	$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) :"#";
         	}
         	else
         	{
         		$dest = ($isMatch && !$this->dynamic) ? "#" : $url;
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
	        echo "<li$style><a href=\"$dest\"$active>$text</a></li>";
    	}
		
		echo "</ul></div>\n";
		
		foreach($this->tabs as $text => $entry)
		{
			if (is_object($entry) && get_class($entry) == "TabBar" &&
				$entry->findPage($this->page))
			{
				$entry->useQueryString = $this->useQueryString;
				$entry->writeHTML();
			}
		}
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
	 * Get the next page in the tab list after the current page.
	 *
	 * @return string
	 */
	function getNextPage()
	{
		$found = false;
		
		foreach($this->tabs as $text => $url)
		{
			trace("getNextPage::text is $text and url is $url; this page is {$this->page}", 3);
			if ($found)
			{
				$dest = ($this->queryString!="") ? $this->appendQueryString($url, $this->queryString) : $url;
				trace("getNextPage::found; dest is$dest", 3);
				return $dest;
			}
			else if ($this->comparePaths($url, $this->page))
			{
				$found = true;
				trace("getNextPage::url is this page", 3);	
			} 
		}

		return ($found) ? "" : false;
	}
	
	/**
	 * Return the URL for the page at the specified tab index.
	 * 
	 * @param string $idx the index of the tab for which we require the URL 
	 */
	function getPage($idx, $appendQueryString = true)
	{
		$i = 0;
		
		foreach($this->tabs as $text => $entry)
		{
			if (is_object($entry) && get_class($entry) == "TabBar")
			{
				$num = count($entry->tabs);
				
				if ($num >= $idx - $i)
				{
					$entry->queryString = $this->queryString;
					return $entry->getPage($idx - $i);
				}
				else
				{
					$i += $num;
					continue;
				}
			}
			else
			{
				$url = $entry;
			}
					
			if ($i == $idx) break;
			$i++;
		}
		
		$dest = ($appendQueryString && $this->queryString!="") ? $this->appendQueryString($url, $this->queryString) : $url;
		return $dest;	
	}
	
	private function comparePaths($url, $page)
	{
		trace("COMPARE PATHS: $url, $page", 2);
		
		if (strpos($url, "/") === false)
		{
			trace("COMPARING PATHS: $url, ".basename($page)." == " . (basename($page) == $url), 2);
			return basename($page) == $url;
		}
		else
		{
			return $page == $url;
		}
	}
	
	/**
	 * Finds the tab containing the specified URL
	 * @param $page The page to find
	 * @return mixed returns the name of the tab if found, false otherwise
	 */
	function findPage($page)
	{
		foreach($this->tabs as $text => $url)
		{
			if ($this->comparePaths($url, $page))
			{
				return $text;
			}
		}
		
		return false;
	}
	
	/**
	 * Move to the next page in the tab list. If the current tab is the
	 * last in the set, redirect to the first tab in the list (wraparound).
	 */
	function next()
	{
		$next = $this->getNextPage();
		if (!$next)
		{
			$next = $this->getPage(0);
		}

		/* 
		 * JDG 6/15/2010, insert "/" before identifier but not if
		 * the page is *.php
		 */
		//if(!preg_match("/.php/i", $next))
		//	$next = "/". $next;
		trace("tabbar::next:: next is $next",3);
		redirect($next);	
	}
	
	function drawCheckList()
	{
	?>
		   <table class="checklist">
	<?
		foreach($this->tabs as $title => $form)
		{
			$this->drawCheckListEntry($title, $form);
		}
	?>
	</table>
	<?
	}
	
	function drawCheckListEntry($title, $form)
	{
		if (get_class($form) == "TabBar")
		{
?>
 <tr>
  <td colspan="2" class="title"><b><?echo $title?></b></td>
 </tr>
<?
			foreach($form->tabs as $t => $f)
			{
				$this->drawCheckListEntry($t, $f);
			}
		}
		else
		{
			$status = $this->flags[$form];
			 
			if ($status && $this->states[$status])
			{
				$img = "<img src='{$this->states[$status]}' alt='$status'/>";
			}
			else if ($this->defaultStateImage)
			{
				$img = "<img src='{$this->defaultStateImage}' alt=''/>";
			}
	?>
	 <tr>
	  <td class="status"><?echo $img ?></td>
	  <td class="link"><a href="<?echo $form ?>?<?echo $this->queryString?>"><?echo $title ?></a></td>
	 </tr>
	<?
			
		}
	}
	
	function setDefaultStateImage($img)
	{
		foreach($this->tabs as $text => $entry)
		{
			if (is_object($entry) && get_class($entry) == "TabBar")
			{
				$entry->defaultStateImage = $img;
			}
		}
	
		$this->defaultStateImage = $img;
	}	
	
	
}



/**
 * The TabBar class is a user-interface control that
 * manages 2 lines of tabs for multi-page dialogs.
 * 
 * Example code the include file calling script:
 * 	$tabs = getTabs();
 *	$qs = ($key) ? "{primary key}=$key" : "";
 *	$tabBar = new TwoLevelTabBar($tabs, $qs);
 *	$tabBar->setStates(array("submitted"	=>	"{icon path}"));
 *	$tabBar->setDefaultStateImage("{icon_path}");
 *	
 *	return $tabBar;
 *
 * function getTabs()
 * {
 *	
 *	$tabs = array(	"First Top Level Tab"	=>
 *					array(	"First Lower Tab"			=>	"{identifier}",
 *					  		"Second Lower Tab"		=>	"{identifier"),
 *
 *					
 *					"2nd Top Level Tab"	=>
 *					array(	"Lower tab"			=>	"{identifier",
 *							"Lower tab"			=>	"{identifier") 
 *
 *	);
 *					 
 *	return $tabs;
 *}
 *
 * 
 */

class TwoLevelTabBar extends TabBar
{
	function TwoLevelTabBar($tabs, $qs)
	{
		foreach($tabs as $text => $entry)
		{
			if (is_array($entry))
			{
				$tabs[$text] = new TabBar("subtabs", $entry, $qs);
			}
		}
		
		$this->TabBar("tabs", $tabs, $qs);
		$this->showStates = false;
	}
	
	function setFlags($flags)
	{
		foreach($this->tabs as $text => $entry)
		{
			if (get_class($entry) == "TabBar")
			{
				$entry->flags = $flags;
			}
		}
		
		$this->flags = $flags;
	}
	
	function setStates($states)
	{		
		foreach($this->tabs as $text => $entry)
		{
			if (get_class($entry) == "TabBar")
			{
				$entry->states = $states;
			}
		}
		
		$this->states = $states;
	}
	
	
	function getNextPage()
	{
		$jump = false;
		
		foreach($this->tabs as $text => $entry)
		{
			if (get_class($entry) == "TabBar")
			{
				$entry->queryString = $this->queryString;
				
				if ($jump) return $entry->getPage(0);

				$next = $entry->getNextPage();
		
				if ($next === false) continue;
				if ($next === "")
				{
					$jump = true;
					continue;
				}
				
				return $next;
			}
		}
			
	}

	/**
	 * Finds the tab containing the specified URL
	 * @param $page The page to find
	 * @return mixed returns the name of the tab if found, false otherwise
	 */
	function findPage($page)
	{
		// remove query string if given;
		$page = preg_replace('/\?.*/', '', $page);

		foreach($this->tabs as $text => $entry)
		{
			if (get_class($entry) == "TabBar")
			{
				$found = $entry->findPage($page);	
				if($found)
				{
					return $found;
				}
			}
		}
		
		return false;
	}
	
		
	function drawHorizontalCheckList()
	{
?>
     <table class="checklist">
     <tr>
<?
		foreach($this->tabs as $title => $steps)
		{
?>
  <td colspan="2" class="title"><b><?echo $title?></b></td> 
<?
		}
?>
     </tr>
<?		
		$idx = 0;
		
		do
		{
			$found = false;
			$row = "";

			foreach($this->tabs as $title => $tabBar)
			{
				$steps = $tabBar->tabs;
				$keys = array_keys($steps);
				$title = $keys[$idx];
				if (isset($steps[$title]))
				{
					$form = $steps[$title];
					$found = true;
					$status = $tabBar->flags[$form];
					 
					if ($status && $this->states[$status])
					{
						$img = "<img src='{$this->states[$status]}' alt='$status'/>";
					}
					else if ($this->defaultStateImage)
					{
						$img = "<img src='{$this->defaultStateImage}' alt=''/>";
					}
					
					$row .= "<td class=\"status\">$img</td>";
	  				$row .= "<td class=\"link\"><a href=\"$form?{$this->queryString}\">$title</a></td>";
				}
				else
				{
					$row .= "<td class=\"status\"></td>";
      				$row .= "<td class=\"link\"></td>"; 

				}
			}

			if ($found)
			{			
?>
     <tr>
      <?echo $row?>
     </tr>
<?
			}
			
			$idx++;
		}
		while($found);
?>
     </table>
<?			
	}
	
	function setColor($color)
	{
		foreach($this->tabs as $text => $entry)
		{
			if (get_class($entry) == "TabBar")
			{
				$entry->cssClass = "{$color}_subtabs";
			}
		}
		
		$this->cssClass = "{$color}_tabs";
	}
}


/**@}*/?>