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

Fakoli::using("settings");
Fakoli::usingFeature("data_tabs", "data_view");

$component_id = checkNumeric($_GET["component_id"]);

$title = "Application Settings";
$menu_item = "Application Settings";

$components = Settings::getComponents();

if (count($components) == 0)
{
	echo "<p><em>There are no application settings defined.</em></p>";
	return;
}

if (!$component_id) 
	$component_id = $components[0]->component_id;

$component = new Component($component_id);
				
$form = new SettingAutoForm($component);

if ($method == "POST")
{
	if ($form->save())
	{
	}
	Fakoli::notify("Settings have been saved");
}

$tabs = new DataItemTabBar("tabs", $components);

$script .= $form->writeScript();
$script .= $tabs->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">

<?php 
$form->drawForm();
?></div>