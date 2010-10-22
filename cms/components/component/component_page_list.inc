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

Fakoli::using("component", "role", "site");
Fakoli::usingFeature("data_view");

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
		
		if (!$component)
		{
			$this->table->column("Component", array($this, formatComponent), true);
		}
			
		$this->table->column("Page", "<a href='/admin/component_page_form?component_page_id={component_page_id}'>{identifier}</a>", true)
			 		->column("Template", array($this, formatTemplate), true)
			 		->column("Role(s)", array($this, formatRole), true, "vertical-align: middle")
			 		->column("Enabled", array($this, formatEnabled), false, "width: 10%; text-align: center");
		$this->table->sortable = true;
		$this->table->filter = true;
		$this->table->pageSize = 20;		
		
		$this->templates = $this->readTemplates();
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

		return "<img src='/fakoli/images/$img' alt='$alt' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer' onclick='toggleEnabled(this, {$page->component_page_id})'/>";
	}
	
	function formatTemplate($page)
	{
		$out = "<select onchange='changeTemplate(this.value, {$page->component_page_id})' id='select_{$page->identifier}'>\n";
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
		$out = "<button onclick=\"rolePopup('{$page->identifier}_roles', {$page->component_page_id}, this); return false\" class='button small' style='float: right'>...</button>";
		$out .= "<span id=\"{$page->identifier}_roles\">$page->role</span>";

		return $out;
	}
	
	function writeScript()
	{
		$roles = SiteRole::getRolesArray();
		
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
  <button class='button' style='float: right; margin-bottom: 4px' onclick='updateRoles()'>Update Roles</button>
 </div>
</div>
<?		
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type='text/javascript'>
function toggleEnabled(img, id)
{
	var enable;
	if (img.alt == "Enabled")
	{
		enable = 0;
	}
	else
	{
		enable = 1;
	}
	
	var request = new Request({
									url: "/action/component/update_page?component_page_id=" + id +"&enabled=" + enable,
									method: 'get',
									onSuccess: function (response) 
									{ 
										if (response == 1) 
										{
											img.src = enable ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
											img.alt = enable ? "Enabled" : "Disabled";
										}
									},
									onFailure: function () {},
									async: true
								});
	request.send();
}

function changeTemplate(template, id)
{
	var request = new Request({
		url: "/action/component/update_page?component_page_id=" + id +"&template=" + encodeURIComponent(template),
		method: 'get',
		onSuccess: function (response) {},
		onFailure: function () {},
		async: true
	});
	request.send();
}


var rolePopupDialog;

function rolePopup(roles, id, source)
{
	var popup = $('rolePopup');
	
	if (!rolePopupDialog)
	{
		rolePopupDialog = new FloatingDialog('rolePopup', {'closeLink': $('closeRolePopup'), 'position': 'absolute'});
	}

	popup.getElements("input[type='checkbox']").each(function(e) { e.checked = false; });

	roles = $(roles);
	var val = roles.get('text');
	var tokens = val.split(',');

	tokens.each(function(t) { if (t != '') $('role_' + t).checked = true; });

	rolePopupDialog.targetID = id;
	rolePopupDialog.roles = roles;
	rolePopupDialog.top = source.getCoordinates().top;
	rolePopupDialog.left = source.getCoordinates().left - 204;
	rolePopupDialog.show();
	
}

function updateRoles()
{
	var id = rolePopupDialog.targetID;
	var popup = $('rolePopup');

	var roles = [];
	
	popup.getElements("input[type='checkbox']").each(function(e) { if (e.checked) roles.push(e.value); });

	var role = roles.join(",");

	var request = new Request({
		url: "/action/component/update_page?component_page_id=" + id +"&role=" + encodeURIComponent(role),
		method: 'get',
		onSuccess: function (response) 
		{
			if (response == 1)
			{
				rolePopupDialog.roles.set('text', role);
				rolePopupDialog.roles = Class.Empty;
				rolePopupDialog.targetID = 0;
				rolePopupDialog.hide();
			}
		},
		onFailure: function () {},
		async: true
	});
request.send();
}

</script>
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
