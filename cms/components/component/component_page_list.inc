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

Fakoli::using("role");
Fakoli::usingFeature("data_view", "context_menu");

class ComponentPageListView
{
	var $table;
	var $pages;
	var $templates;
	
	function ComponentPageListView($component = null)
	{
		if ($component)
		{
			$this->pages = $component->ComponentPages();		
		}
		else
		{
			$this->pages = Query::create(ComponentPage, "ORDER BY component, identifier")->execute();
		}
		
		$this->table = new DataListView($this->pages, "component_pages");
		$this->table->selector();
		
		$contextMenu = $this->table->commandMenu();
		$contextMenu->command("component_enable", "Enable Pages", "new ComponentPageList().setEnabled(1)", false)
					->command("component_disable", "Disable Pages", "new ComponentPageList().setEnabled(0)", false);
					//->command("component_site",	"Set Page Template", "new ComponentPageList().templatePopup(document.id('component_pages').getSelectedValues())", false)
					//->command("component_roles", "Set Page Roles", "new ComponentPageList().rolePopupMulti(document.id('component_pages').getSelectedValues())", false)
					//->command("component_site",	"Set Page Site", "new ComponentPageList().sitePopup(document.id('component_pages').getSelectedValues())", false);
		
		if (!$component)
		{
			$this->table->column("Component", array($this, formatComponent), true);
		}
			
		$this->table->column("Page", "<a href='/admin/component_page_form?component_page_id={component_page_id}'>{identifier}</a>", true, "width: 50%")
			->column("Page Title", "{page_title}")
	 		->column("Enabled", array($this, formatEnabled), false, "width: 10%; text-align: center");
		$this->table->sortable = true;
		$this->table->filter = true;
		$this->table->pageSize = 20;		
		
		//$this->templates = $this->readTemplates();
	}	
	
	function formatComponent($page)
	{
		$components = Fakoli::getComponents();
		$component = $components[$page->component];
		return "<a href='/admin/component_form?component_id={$component->component_id}'>{$component->name}</a>";
	}
	
	function formatEnabled($page)
	{
		$img = $page->enabled ? "on.png" : "off.png";
		$alt = $page->enabled ? "Enabled" : "Disabled";

		return "<img src='/fakoli/images/$img' id='enable_{$page->component_page_id}' alt='$alt' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer' onclick='new ComponentPageList().toggleEnabled(this, {$page->component_page_id})'/>";
	}
	
	function formatTemplate($page)
	{
		$out = "<select onchange='new ComponentPageList().changeTemplate(this.value, {$page->component_page_id})' id='template_{$page->component_page_id}'>\n";
		$out .= "<option value=''></option>\n";
		foreach($this->templates as $template)
		{
			$out .= "<option value='$template'";
			if ($page->template == $template)
			{
				$out .= " selected";
			}
			$out .= ">$template</option>";
		}
		$out .= "</select>";
		
		return $out;
	}
	
	function formatRole($page)
	{
		$out = "<button onclick=\"new ComponentPageList().rolePopup('roles_{$page->component_page_id}', {$page->component_page_id}, this); return false\" class='button small' style='float: right'>...</button>";
		$out .= "<span id=\"roles_{$page->component_page_id}\">$page->role</span>";

		return $out;
	}
	
	function writeScript()
	{
		$roles = SiteRole::getRolesArray();
		$sites = Site::getSites();
		
		global $dialogs;
		ob_start();
?>
<div class="dialog" id="rolePopup" style="width: 200px">
 <div class="dialog_header" id="rolePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeRolePopup'">Close &times;</a></div>
   <span"style="font-weight: bold">Roles</span>
  </div>	
 </div>
 <div class="dialog_body">
<?
foreach($roles as $role => $name)
{
?>
	<input type="checkbox" id="role_<?echo $role?>" value="<?echo $role?>"/>&nbsp;<?echo $name?><br/>
<?	
}
?>
  <button class='button' style='float: right; margin-bottom: 4px' onclick='new ComponentPageList().updateRoles()'>Update Roles</button>
 </div>
</div>
<div class="dialog" id="sitePopup" style="width: 200px">
 <div class="dialog_header" id="sitePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeSitePopup'">Close &times;</a></div>
   <span"style="font-weight: bold">Sites</span>
  </div>	
 </div>
 <div class="dialog_body">
  <label for="site_id">Select Site: </label>
   <select id="site_id" name="site_id">
<?
	foreach($sites as $s)
	{
		option($s->site_id, $s->site_name, 0);
	}
?>
   </select><br/>
  <button class='button' style='float: right; margin-bottom: 4px' onclick='new ComponentPageList().updateSites()'>Update Templates</button>
 </div>
</div>
<div class="dialog" id="templatePopup" style="width: 200px">
 <div class="dialog_header" id="templatePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeTemplatePopup'">Close &times;</a></div>
   <span"style="font-weight: bold">Templates</span>
  </div>	
 </div>
 <div class="dialog_body">
  <label for="template_id">Select Template: </label>
	<select id='template_id'>
	 <option value=''></option>   
<?

	foreach($this->templates as $template)
	{
		option($template, $template, "");
	}
?>
   </select><br/>
  <button class='button' style='float: right; margin-bottom: 4px' onclick='new ComponentPageList().updateTemplates()'>Update Templates</button>
 </div>
</div>
<?		
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type='text/javascript' src="/components/component/js/component_page_list.js"></script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		$script .= $this->table->writeScript();
		return $script;
	}
	
	function drawView()
	{
		return $this->table->drawView();
	}

	function readTemplates()
	{
		global $config;
		
		$home = $config["homedir"] . DIRECTORY_SEPARATOR . "templates";
		
		trace("Scanning Templates", 3);
		 
		$options = array();
		
		$folders = array("$dir");		
		
		while(($dir = array_shift($folders)) !== null)
		{
			$path = "$home/$dir";
			
			trace("Reading $path", 3);

			$handle = opendir($path);
			if (!$handle)
			{
				trace("Directory read failed for $path", 3);
				continue;
			}
			
			while(false !== ($file = readdir($handle))) 
	    	{
	    		trace("Checking $file", 4);
	    		
	    		if ($file != "." && 
	    			$file != ".." && 
	    			$file != ".svn" && 
	    			$file != "CVS")
	    		{
	    			$f = "$path/$file";
					$l = ($dir != '') ? "$dir/$file" : $file;
					
	    			if (is_dir($f))
	    			{
	    				array_push($folders, $l);
	    			}
					else if (preg_match('/.tpl$/i', $file)) 
					{
						$options[] = $l;
					}
	    		}        	
	    	}
	    	closedir($handle);
		}
	    return $options;
	}
	
}
?>
