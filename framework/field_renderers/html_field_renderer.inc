<?php
/**\addtogroup fieldrenderers
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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Field renderer for HTML data fields. Renders with the Sonjara
 * Rich Text Editor, which can be extended to provide custom toolbar
 * buttons for application-specific features and enhancements.
 * .
 */
class HTMLFieldRenderer extends FieldRenderer
{
	var $colspan = 2;
	var $hideStyleBar = false;
	var $width = "540px";
	var $height = "200px";
	var $featureButtons;
	var $rootBlock = "p";
	var $baseURL = null;
	var $convertURLs = false;
	
	static $_includedRTEScript = false;
	
	public static $actionFilter = null;
	
	var $rteParams= array();
	
	function HTMLFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
		$this->hideStyleBar = $parent->HTMLEditorHideStyleBar;
		$this->width = $parent->HTMLEditorDefaultWidth;
		$this->height = $parent->HTMLEditorDefaultHeight;
		$this->annotateBefore = true;
	}
	
	function setRTEParam($key, $value)
	{
		$this->rteParams[$key] = $value;
	}
	
	function getRTEParam($key)
	{
		return $this->rteParams[$key];
	}
	
	function renderScript($field)
	{
		global $config;
		
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";

		global $tiny_mce_application_plugins;
		global $tiny_mce_application_toolbar_section;
		global $tiny_mce_application_context_menu_section;
		global $tiny_mce_content_css;
		global $tiny_mce_body_class;
		
		$plugins = "advlist,lists,code,visualblocks,table,charmap,paste,contextmenu,searchreplace,fullscreen";
		if ($tiny_mce_application_plugins) $plugins .= ",".$tiny_mce_application_plugins;
		$contextMenu = "cut copy paste pastetext searchreplace removeformat | {$tiny_mce_application_context_menu_section} inserttable cell row column deletetable";
		$toolbar = "undo redo | cut copy paste pastetext searchreplace | styleselect | bold italic removeformat | bullist numlist outdent indent | table charmap | {$tiny_mce_application_toolbar_section} code visualblocks fullscreen";
		
		if (is_callable(HTMLFieldRenderer::$actionFilter))
		{
			$contextMenu = (HTMLFieldRenderer::$actionFilter)($contextMenu);
			$toolbar = (HTMLFieldRenderer::$actionFilter)($toolbar);
		}
		
		$https = Fakoli::checkSSL();
		
		$protocol = $https ? "https" : "http";
		
		$rootBlock = "";
		
		if ($this->rootBlock != "p")
		{
			if (!$this->rootBlock) 
			{ 
				$this->setRTEParam("forced_root_block", false);
			}
			else
			{
				$this->setRTEParam("forced_root_block", $this->rootBlock);
			}
		}

		$baseURL = ($this->baseURL !== null) ? $this->baseURL : "{$protocol}://{$config['http_host']}";
		if (isset($config["convertURLs"]))
		{
			$convertURLs = $config["convertURLs"] ? true : false;
		}
		else
		{			
			$convertURLs = ($this->convertURLs) ? true : false;
		}
		
		if (!HTMLFieldRenderer::$_includedRTEScript)
		{
?>

	<script type='text/javascript'>
		tinymce.baseURL = "<?echo $protocol?>://<?echo $config['http_host']?>/fakoli/tinymce/";
	</script>
<?		
			HTMLFieldRenderer::$_includedRTEScript = true;
		}
		
		$cssFiles = explode("\n", str_replace("\r", "", $tiny_mce_content_css));
		if (count($cssFiles) > 0)
		{
			$this->setRTEParam("content_css", $cssFiles);
		}
		else
		{
			$this->setRTEParam("content_css", "");
		}

		if (endsWith($this->width, "px"))
		{
			$w = substr($this->width, 0, -2);

			$this->setRTEParam("width", $w);
		}
			
		if (endsWith($this->height, "px"))
		{
			$h = substr($this->height, 0, -2);
			$this->setRTEParam("height", $h);
		}

		$this->setRTEParam("valid_elements", '*[*]');
		$this->setRTEParam("selector", "#{$this->parent->id}_{$field}");
		$this->setRTEParam("plugins", $plugins);
		$this->setRTEParam("contextmenu", $contextMenu);
		$this->setRTEParam("skin", 'fakoli');
		$this->setRTEParam("body_class", $tiny_mce_body_class);
		$this->setRTEParam("relative_urls", false);
		$this->setRTEParam("convert_urls", $convertURLs);
		$this->setRTEParam("document_base_url", $baseURL);
		$this->setRTEParam("menubar", false);
		$this->setRTEParam("toolbar", $toolbar);
		$this->setRTEParam("tools", 'inserttable');
		
		if ($this->customConfig) call_user_func($this->customConfig, $this);
?>		
	<script type='text/javascript'>
		tinymce.init(
		{
<?
		echo $this->renderParameters();
?>
 });
	</script>
<?		
	}
	
	private function renderParameters()
	{	
		$params = array();
		foreach($this->rteParams as $key => $value)
		{
			if (is_array($value))
			{
				$v = "['".implode("','", $value)."']";
				$params[] = "\t\t\t$key: $v";
			} 
			else if (is_bool($value))
			{
				$v = $value ? "true" : "false";
				$params[] = "\t\t\t$key: $v";
			}
			else if (is_numeric($value))
			{
				$params[] = "\t\t\t$key: $value";
			}
			else
			{
				$params[] = "\t\t\t$key: '$value'";
			}
		}
	
		return implode(",\n", $params);
	}
	
	function addEditorButton($editor, $button)
	{
		if ($button['url'])
		{
			echo "theEditors.$editor.addToolbarButton('{$button['name']}', '{$button['image']}', '{$button['tooltip']}', '{$button['url']}', '{$button['width']}', '{$button['height']}');\n";
		}
		else
		{
			echo "theEditors.$editor.addToolbarButton('{$button['name']}', '{$button['image']}', '{$button['tooltip']}', {$button['handler']});\n";
		}		
	}
	
	function overrideEditorButton($editor, $button)
	{
		echo "theEditors.$editor.overrideToolbarButton('{$button['name']}', {$button['handler']});\n";
	}
		
	function renderField($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
		echo "<div id='{$field}_container'></div>";
		
		$this->_startField($field);

		echo "<textarea id='{$this->parent->id}_{$field}' name='{$field}' style='width: {$this->width}; height: {$this->height}' class='richtext'>".htmlSafe($this->parent->data->get($field))."</textarea>\n";
		
		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->parent->data->get($field);
		
		$this->_endField($field);
	}	

	function renderSearchField($field, $mode = "equal")
	{
		// AJG - Override default colspan when rendering in a search form
		$this->colspan = 1;
		
		if ($mode == "range")
		{
			$from = $this->parent->params->get($field, "from");
			$to =  $this->parent->params->get($field, "to");
			
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>From <input type='text' name='$field:from' value='{$from}' size='25'> to <input type='text' name='$field:to' value='{$to}' size='25'></td>\n";		
			echo "</tr>\n";
		}
		else
		{
			$value = $this->parent->params->get($field, $mode);
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td><input type='text' name='$field:$mode' value='{$value}' size='50'></td>\n";		
			echo "</tr>\n";
		}		
	}
		
	function renderOnSubmitHandler($field)
	{
		// JDG 3/2/2011., no js for readonly field
		if ($this->parent->isReadOnly($field)) return "";
		
		$editor = $field."_editor";
		
		//echo "\ttheEditors.$editor.prepareForSubmit();\n";
	}
}

/**@}*/?>