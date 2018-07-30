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
 * Field renderer for text data fields. Renders as a multi-line text 
 * box area, with optional javascript character count display.
 * 
 * Example for $form= new autoform($document)
 * 
 * $fieldname=$form->getRenderer('field_name');
 * $fieldname->rows=3;
 * 
 */
class TextFieldRenderer extends FieldRenderer
{
	var $rows = 4;			//!< The number of rows to display in the text area
	var $columns = 60;		//!< The number of columns to display in the text area
	var $colspan = 2;		//!< The number of table columns to display across
	var $limit = 0;			//!< Optional limit on the number of characters allowed to be entered (enforced by Javascript). Note - make sure you have at least this many characters in your database field!
	var $disable = false; 	//!< Alternative to readonly; field can be reenabled thru javascript
	var $encoding = "UTF-8";//!< Overrides the default text encoding.
	var $style;				//!< Inline styles for the text area element
	
	/**
	 * Constructs a new TextFieldRenderer object.
	 * @param AutoForm $parent the parent AutoForm
	 */
	function TextFieldRenderer(&$parent)
	{
		global $config;
		
		$this->FieldRenderer($parent);
		
		$this->annotateBefore = true;
		if ($config["encoding"]) $this->encoding = $config["encoding"];
	}
	
	function renderScript($field)
	{
		if ($this->limit)
		{
?>
<script type='text/javascript'>
/* <![CDATA[ */
	function <?echo $field ?>_keyup(event)
	{
		var form = document.getElementById('<?echo $this->parent->id ?>');
		var ctrl = form['<?echo $field ?>'];
		var len = ctrl.value.length;
		if (len >= <?echo $this->limit?>) 
		{
			ctrl.value = ctrl.value.substring(0, <?echo $this->limit ?>);
			len = <?echo $this->limit ?>;
		}

		var count = document.getElementById('<?echo $field ?>_length');
		count.innerHTML = len;
	}
</script>
<?
		}
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		if ($this->limit)
		{
			echo "<div>";
			$onkeypress = " onkeyup='{$field}_keyup(event);'";
		}
		$disable = ($this->disable) ? "disabled='disabled'" : "";	
		
		if ($this->style) $style = "style='{$this->style}'";
		
		echo "<textarea id='{$this->parent->id}_$field' name='$field' rows='{$this->rows}' cols='{$this->columns}' $style $disable $onkeypress>";
		echo cleanHTMLTags($this->parent->data->get($field));
		//echo htmlspecialchars($this->parent->data->get($field), ENT_QUOTES | ENT_IGNORE, $this->encoding);
		echo "</textarea>";
		
		if ($this->limit)
		{
			echo "<br /><div class='character_count' style='clear: left; float: right'><span id='{$field}_length'>";
			echo strlen(trim($this->parent->data->get($field)));
			echo "</span> of $this->limit characters</div></div>";
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		$text = htmlspecialchars($this->parent->data->get($field), ENT_QUOTES | ENT_IGNORE, 'UTF-8');
		$text = str_replace("\n", "<br/>", $text);
		echo $text;

		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode = "like")
	{
		// AJG - Override default colspan when rendering in a search form
		$this->colspan = 1;
		
		if ($mode == "range")
		{
			$from = htmlspecialchars($this->parent->params->get($field, "from"), ENT_QUOTES, 'UTF-8');
			$to =  htmlspecialchars($this->parent->params->get($field, "to"), ENT_QUOTES, 'UTF-8');
			
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>From <input type='text' name='$field:from' value='{$from}' size='25'> to <input type='text' name='$field:to' value='{$to}' size='25'></td>\n";		
			echo "</tr>\n";
		}
		else
		{
			$value =  htmlspecialchars($this->parent->params->get($field, $mode), ENT_QUOTES, 'UTF-8');
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td><input type='text' name='$field:$mode' value='{$value}' size='50'></td>\n";		
			echo "</tr>\n";
		}		
	}	
}

	
?>