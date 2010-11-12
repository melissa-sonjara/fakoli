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
		if ($this->parent->isReadOnly) return "";
		
		global $html_editor_extra_buttons;
		
		if (!HTMLFieldRenderer::$_includedRTEScript)
		{
			//echo "<script type='text/javascript' src='{$this->parent->componentPath}/richtext/sonjara_richtext.js'></script>\n";
			echo "<link href='{$this->parent->componentPath}/richtext/sonjara_richtext.css' type='text/css' rel='stylesheet'></link>\n";
			echo "<script type='text/javascript'>\n";
			echo "setRichTextEnv('{$this->parent->componentPath}/richtext/sonjara_richtext.css', '{$this->parent->componentPath}/richtext/images/', '{$this->parent->componentPath}/richtext/');\n";
			echo "</script>\n";

			HTMLFieldRenderer::$_includedRTEScript = true;
		}
				
		echo "<script type='text/javascript'>\n";
		$editor = $field."_editor";
		echo "theEditors.$editor = new RichTextEditor('$editor', '$field', '";
		echo trim(jsSafe($this->parent->data->get($field)));
		echo "', '{$this->width}', '{$this->height}', false, false";
		if ($this->parent->HTMLEditorCSSPath) echo ", '{$this->parent->HTMLEditorCSSPath}'";
		echo ");\n";
		
		if ($this->hideStyleBar)
		{
			echo "theEditors.$editor.showStyleBar = false;\n";
		}
		
		if(is_array($this->featureButtons))
		{
			foreach($this->featureButtons as $button)
			{
				echo "theEditors.$editor.addToolbarButton('{$button['name']}', '{$button['image']}', '{$button['tooltip']}', '{$button['url']}', '{$button['width']}', '{$button['height']}');\n";
			}
		}
		
		if (is_array($html_editor_extra_buttons))
		{
			foreach($html_editor_extra_buttons as $button)
			{
				echo "theEditors.$editor.addToolbarButton('{$button['name']}', '{$button['image']}', '{$button['tooltip']}', '{$button['url']}', '{$button['width']}', '{$button['height']}');\n";
			}
		}
		echo "\nwindow.addEvent('domready', function() {\n";
		echo "theEditors.$editor.draw();\n";
		echo "});\n";
		echo "</script>";
	}
	
	function renderField($field)
	{
		global $html_editor_extra_buttons;
		
		$this->_startField($field);

		echo "<div id='{$field}_container'></div>";
		
		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		if ($this->colspan > 1) $style = "text-align: left";
		echo "<tr>\n";
		$this->_printLabel($field, $this->colspan, $style);
		if ($this->colspan > 1)
		{
			echo "</tr>\n";
			echo "<tr>\n";
		}
		echo "<td colspan='$this->colspan'";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">".$this->parent->data->get($field)."</td>\n";		
		echo "</tr>\n";	
	}	

	function renderSearchField($field, $mode = "equal")
	{
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
		$editor = $field."_editor";
		
		echo "\ttheEditors.$editor.prepareForSubmit();\n";
	}
}

?>