<?php
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

/**
 * Panel provides a reloadable panel tied to an action handler.
 * A reload() method is automatically attached to the panel's DOM
 * and can be called from Javascript to refresh the panel contents
 * via an asynchronous request (i.e. without refreshing the page).
 * Additionally the panel is filled by default using the ComponentManager's
 * local action dispatch mechanism. Panels can be used to easily provide
 * updatable content areas without having to re-implement too much plumbing.
 *
 * @author Andy Green
 */
class Panel
{
	var $id;
	var $handler;
	
	function __construct($id, $handler)
	{
		$this->id = $id;
		$this->handler = $handler;
	}
	
	function draw()
	{
		$mgr = new ComponentManager();
		echo "<div id='{$this->id}' data-url='{$this->handler}'>";
		echo $mgr->dispatchLocalAction($this->handler);
		echo "</div>";
	}
}

?>