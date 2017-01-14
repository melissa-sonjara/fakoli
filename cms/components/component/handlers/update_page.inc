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

Fakoli::assertRole("admin");

$component_page_ids = checkNumeric($_GET["component_page_id"]);

if (!$component_page_ids) die("");

if (!is_array($component_page_ids))
{
	$component_page_ids = array($component_page_ids);
}

foreach($component_page_ids as $component_page_id)
{
		
	$page = new ComponentPage($component_page_id);
	
	$filter = new InclusionFilter("component_page_id");
	
	if (isset($_GET["enabled"]))
	{
		$filter->add("enabled");
		$page->enabled = checkNumeric($_GET["enabled"]);
	}
	
	if (isset($_GET["role"]))
	{
		$filter->add("role");
		$page->role = mysqli_escape_string($_GET["role"]);
	}

	if (isset($_GET["site_id"]))
	{
		$filter->add("site_id");
		$page->site_id = checkNumeric($_GET["site_id"]);
	}
	
	if (isset($_GET["template"]))
	{
		$filter->add("template");
		$page->template = mysqli_escape_string($_GET["template"]);
	}
	
		
	$page->filter = $filter;
	$page->save();
}

echo "1";
?>