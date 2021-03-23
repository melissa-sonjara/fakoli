<?php
/**\addtogroup field_renderers
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


Fakoli::usingFeature("field_renderers");
Fakoli::using("site");

class TemplateSelectFieldRenderer extends SelectFieldRenderer
{
	static $templateMapWritten = false;
	var $mode = "site"; ///< Dynamic update mode, either 'site', 'theme' or 'none'
	var $site_id = 0;
	
	function TemplateSelectFieldRenderer(&$form, $field, $label, $mode = "site", $onChange = "")
	{
		$this->mode = $mode;
		$this->site_id = $site_id;
			
		$options = $this->getInitialOptions($form);
			
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
	}
	
	private function getInitialOptions($form)
	{
		$site = $this->getSite($form);
		$directory = $site->getThemeDirectory();
		$options = $this->readFiles($directory, "");
		$options = array_merge(array("" => ""), $options);
		
		return $options;
	}
	
	private function getSite($form = null)
	{
		if (!$form) $form = $this->parent;
		
		if ($form->data->hasField("site_id"))
		{
			$site_id = $form->data->get("site_id");
			if ($site_id)
			{
				$site = new Site($site_id);
				return $site;
			}
		}
		
		$site = Site::getSite();
		return $site;
	}
	
	private function buildTemplateMap()
	{
		$map = array();

		switch($this->mode)
		{
		case "site":

			$sites = Query::create(Site, "")->execute();
	
			foreach($sites as $site)
			{
				$directory = $site->getThemeDirectory();
			
				$map[$site->site_id] = $this->readFiles($directory, "");
			}
			break;

		case "theme":
			
			$themes = SiteManager::getThemes();
			
			$map[""] = $this->readFiles(SiteManager::getThemeDirectory(""));
			
			foreach($themes as $component => $theme)
			{
				$map[$component] = $this->readFiles($theme["directory"]);
			}
			
			break;
		}
		
		return $map;
	}
		
	function readFiles($home, $dir = "")
	{
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
						$options[$l] = $l;
					}
	    		}        	
	    	}
	    	closedir($handle);
		}
	    return $options;
	}
	
	function renderScript($field)
	{
		if (!TemplateSelectFieldRenderer::$templateMapWritten)
		{
?>
<script type='text/javascript'>
var TemplateSelectFieldRenderer_templateMap = <?echo json_encode($this->buildTemplateMap());?>;
</script>
<?
			TemplateSelectFieldRenderer::$templateMapWritten = true;
		}
		
		switch($this->mode)
		{
		case "site":
				
			if ($this->parent->data->hasField("site_id"))
			{
?>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	var siteSelect = document.id("<?echo $this->parent->id?>_site_id");
	siteSelect.addEvent('change', function()
	{
		var site_id = siteSelect.value;
		var templateSelect = document.id("<?echo $this->parent->id?>_<?echo $field?>");
		var val = templateSelect.value;

		var html = "";
		
		var options = TemplateSelectFieldRenderer_templateMap[site_id];
		for (var o in options)
		{
			if (o == val)
			{
				html += "<option value='" + o + "' selected='selected'>" + o + "</option>";
			}
			else
			{
				html += "<option value='" + o + "'>" + o + "</option>";
			}
		}
			
		templateSelect.set('html', html);
	});
});
</script>			
<?
			}
			break;

		case "theme":
			
			if ($this->parent->data->hasField("theme"))
			{
?>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	var themeSelect = document.id("<?echo $this->parent->id?>_theme");
	themeSelect.addEvent('change', function()
	{
		var theme = themeSelect.value;
		var templateSelect = document.id("<?echo $this->parent->id?>_<?echo $field?>");
		var val = templateSelect.value;

		var html = "";
		
		var options = TemplateSelectFieldRenderer_templateMap[theme];
		for (var o in options)
		{
			if (o == val)
			{
				html += "<option value='" + o + "' selected='selected'>" + o + "</option>";
			}
			else
			{
				html += "<option value='" + o + "'>" + o + "</option>";
			}
		}
			
		templateSelect.set('html', html);
	});
});
</script>			
<?
			}
			break;
			
		case "none":
		default:
			break;
		}
		
		return parent::renderScript(field);
	}
}
/**@}*/?>