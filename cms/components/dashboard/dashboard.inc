<?php
/**\addtogroup dashboard
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

interface DashboardWidget
{
	function writeScript();
	function drawWidget();
}

class DashboardView
{
	var $topWidgets;
	var $leftWidgets;
	var $rightWidgets;
	var $bottomWidgets;
	var $id;
		
	function DashboardView($id)
	{
		$this->id = $id;
		$this->topWidgets = array();
		$this->leftWidgets = array();
		$this->rightWidgets = array();
		$this->bottomWidgets = array();
	}

	function top($widget)
	{
		$this->topWidgets[] = $widget;
	}
	
	function left($widget)
	{
		$this->leftWidgets[] = $widget;
	}
	
	function right($widget)
	{
		$this->rightWidgets[] = $widget;
	}
	
	function bottom($widget)
	{
		$this->bottomWidgets[] = $widget;
	}
	
	
	function writeScript()
	{
		$script = "";
		
		foreach($this->topWidgets as $w)
		{
			$script .= $w->writeScript();
		}
		
		foreach($this->leftWidgets as $w)
		{
			$script .= $w->writeScript();
		}
		
		foreach($this->rightWidgets as $w)
		{
			$script .= $w->writeScript();
		}
		
		foreach($this->bottomWidgets as $w)
		{
			$script .= $w->writeScript();
		}	

		return $script;
	}
	
	function drawView()
	{
?>
		<div id="<?echo $this->id?>" class="dashboard">
		 <div class="dashboard_top" style="width: 100%">
<?
		foreach($this->topWidgets as $w) $w->drawWidget();
?>
		 </div>
		 <div class="dashboard_left" style="clear:both;width:50%;float:left">
<?
		foreach($this->leftWidgets as $w) $w->drawWidget();
?>
		 </div>
		 <div class="dashboard_right" style="width:50%;float:left">
<?
		foreach($this->rightWidgets as $w) $w->drawWidget();
?>
		 </div>
		 <div class="dashboard_bottom" style="clear:both;width: 100%">
<?
		foreach($this->bottomWidgets as $w) $w->drawWidget();
?>
		 </div>
		</div>
<?		 
	}
}
/**@}*/?>