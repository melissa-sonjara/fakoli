<?php

/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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
 * Field renderer for string data fields. Renders as a single-line text field.
 */
class StringFieldRenderer extends FieldRenderer
{	
	var $size = 50;
	var $limit = 0; // optional - restricts users from entering character count over limit
	

	function StringFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
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
		var form = $('<?echo $this->parent->id ?>');
		
		var ctrl = form['<?echo $field ?>'];
		var len = ctrl.value.length;
		if (len >= <?echo $this->limit?>) 
		{
			ctrl.value = ctrl.value.substring(0, <?echo $this->limit ?>);
			len = <?echo $this->limit ?>;
		}
		
		var count = document.getElementById('<?echo $field ?>_length');

		// JDG 2/1/10 must check count to fix IE7 bug
		if(count)
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
		
		echo "<input type='text' name='$field' value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."' size='{$this->size}' $onkeypress/>";

		if ($this->limit)
		{
			echo "</div>";
		}
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">".$this->parent->data->get($field)."</td>\n";		
		echo "</tr>\n";	
	}

	function renderSearchField($field, $mode = "equal")
	{
		if ($mode == "range")
		{
			$from = htmlspecialchars($this->parent->params->get($field, "from"), ENT_QUOTES, 'UTF-8');
			$to =  htmlspecialchars($this->parent->params->get($field, "to"), ENT_QUOTES, 'UTF-8');
			
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>From <input type='text' id='$field' name='$field:from' value='{$from}' size='25'> to <input type='text' name='$field:to' value='{$to}' size='25'></td>\n";		
			echo "</tr>\n";
		}
		else
		{
			$value =  htmlspecialchars($this->parent->params->get($field, $mode), ENT_QUOTES, 'UTF-8');
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td><input type='text' id='$field' name='$field:$mode' value='{$value}' size='50'></td>\n";		
			echo "</tr>\n";
		}		
	}
	
}

?>