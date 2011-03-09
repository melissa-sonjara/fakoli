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

if (!$component_id) $component_id = $components[0]->component_id;

$component = new Component($component_id);

if ($method == "POST")
{
	foreach($_POST["settings"] as $settings_id => $value)
	{
		$setting = new Settings($settings_id);
		$setting->value = $value;
		$setting->save();
		Cache::put("setting_{$setting->component}_{$setting->name}", $setting);
	}
}

$settings = Query::create(Settings, "WHERE component=:component ORDER BY name")
				->bind(":component", $component->name)
				->execute();
				
$tabs = new DataItemTabBar("tabs", $components);

$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<form method="POST" action="?component_id=<?echo $component_id?>">
<table class="form">
<?
	foreach($settings as $setting)
	{
?>
 <tr>
  <td class="label"><?echo prettify($setting->name)?></td>
  <td class="value"><input type="text" style="width: 300px" name="settings[<?echo $setting->settings_id?>]" value="<?echo $setting->value?>"/></td>
 </tr>
<?
	}
?>
</table>
<br/><br/>
<input type="submit" class="button" value=" Save Changes "/>
</form>
</div>
<?

?>