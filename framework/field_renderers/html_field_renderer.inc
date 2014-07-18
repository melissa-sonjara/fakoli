<?php

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

	static $_includedRTEScript = false;
	
	function HTMLFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
		$this->hideStyleBar = $parent->HTMLEditorHideStyleBar;
		$this->width = $parent->HTMLEditorDefaultWidth;
		$this->height = $parent->HTMLEditorDefaultHeight;
		$this->annotateBefore = true;
	}
	
	function renderScript($field)
	{
		global $config;
		
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";
		
		global $html_editor_extra_buttons;
		global $html_editor_override_buttons;
		
		$https = $_SERVER['HTTPS'];
		if ($https == "off") $https = false;
		
		$protocol = $https ? "https" : "http";
		
		if (!HTMLFieldRenderer::$_includedRTEScript)
		{
?>

	<script type='text/javascript'>
		tinymce.baseURL = "<?echo $protocol?>://<?echo $config['http_host']?>/fakoli/tinymce/";
	</script>
<?		
			HTMLFieldRenderer::$_includedRTEScript = true;
		}
?>		
	<script type='text/javascript'>
		tinymce.init(
		{
<?
			if (endsWith($this->width, "px"))
			{
				$w = substr($this->width, 0, -2);
?>
			width: <?echo $w?>,
<?
			}
			
			if (endsWith($this->height, "px"))
			{
				$h = substr($this->height, 0, -2);
?>
			height: <?echo $h?>,
<?
			}
?>
			valid_elements: '*[*]',
			selector: '#<?echo "{$this->parent->id}_{$field}"?>',
			plugins: 'advlist,code,visualblocks,table,imagepicker,linkpicker,videopicker,charmap,paste,contextmenu,searchreplace,fullscreen',
			contextmenu: "cut copy paste pastetext searchreplace removeformat | linkpicker imagepicker videopicker | inserttable cell row column deletetable",
			skin: 'fakoli',
			relative_urls: false,
			document_base_url: 'http://<?echo $config['http_host']?>',
			menubar: false,
			toolbar: "undo redo | cut copy paste pastetext searchreplace | styleselect | bold italic removeformat | bullist numlist outdent indent | table charmap | linkpicker imagepicker videopicker | code visualblocks fullscreen",
			tools: 'inserttable' });
	</script>
<?		
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
		
		global $html_editor_extra_buttons;
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

?>