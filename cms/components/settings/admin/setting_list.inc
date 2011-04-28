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

/*
 * Systems Administrator view of settings.
 * 
 * CMS Classification feature for component settings.
 * Lists all component settings and allows user to modify settings
 * definition.
 * 
 */

Fakoli::using("settings");
Fakoli::usingFeature("data_view");

$title = "Settings Options";
$menu_item = "Define Settings Options";

$settings = Query::create(Settings, "ORDER BY component, name")->execute();

$table = new DataListView($settings, "settings");
$table->column("Component", "{component}", true)
	->column("Name", "<a href='/admin/setting_form?settings_id={settings_id}'>{name}</a>", true)
	->column("Field Annotation", "{annotation}", true)
	->column("Field Type", "{field_type}", true)
	;
		  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "There are no application settings defined.";
$script .= $table->writeScript();
?>
<h4>This features allows you to define the system's application setting options. It is designed for use by systems administrators. CMS Users can modify <a href="/admin/settings">Application Settings</a> under CMS Web Site Configuration.</h4>
<p>To create a new setting, perform the following steps:</p>
<ul>
<li>In your component's manifest, subscribe to the event "ComponentScanComplete", calling a function in your component's manager class called "setDefaults"</li>
<li>Add the static function setDefaults to your component's manager class</li>
<li>For each setting, call Settings::setDefaultValue and provide the setting default values</li>
</ul>

<?php
$table->drawView();
?>