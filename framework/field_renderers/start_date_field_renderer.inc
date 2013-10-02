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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * When a form, such as event form, has both a start and end date,
 * this field renderer can be used instead of DateTimeFieldRenderer
 * for the start date so that an empty end date field will be updated
 * to match the start date.
 * 
 * @author janice
 */
class StartDateTimeFieldRenderer extends DateTimeFieldRenderer
{
	var $endDateField;
	
	/**
	 * 
	 * @param obj $parent instance of AutoForm
	 * @param string $field the start date field
	 * @param string $label
	 * @param string $endDateField the end date field
	 */
	function __construct($parent, $field, $label = "", $endDateField = "end_date")
	{		
		$this->FieldRenderer($parent);
		if ($parent->data->hasField($field))
		{
			$parent->override($field, $label, $this);
		}
		else
		{
			$parent->add($this, $field);
			$parent->overrides[$field]['label'] = $label;
		}
		$this->endDateField = $endDateField;
	}
	
	function renderScript($field)
	{	
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";
		
		$startField = "{$this->parent->id}_{$field}";
		$endField = "{$this->parent->id}_{$this->endDateField}";
		
		parent::renderScript($field);
		
		echo "<script type='text/javascript'>\n";
		echo "window.addEvent('domready', function()\n";
		echo "{\n";
		echo "\t\t$('{$startField}').addEvent('blur', function(e) { {$field}_onChangeStartDate(); });\n";
		echo "\t\t$('{$endField}').addEvent('focus', function(e) { {$field}_onChangeStartDate(); });\n";
		echo "\n";
		echo "});\n";
			
?>	
		start_date_calendar.calendar.addEvent('mouseout', function(e) { <?php echo $field ?>_onChangeStartDate(); });
		function <?echo $field ?>_onChangeStartDate()
		{
			var start_date_elt = $('<?php echo $startField ?>');		
			if(!start_date_elt || !start_date_elt.get("value")) return;
			var end_date_elt = $('<?php echo $endField ?>');
			if(!end_date_elt) return;
			var startObj = new Date(start_date_elt.get("value"));
			var endObj = null;
			if(end_date_elt.get("value"))
			{
				endObj = new Date(end_date_elt.get("value"));
			}
			if(!endObj || endObj.valueOf() < startObj.valueOf())
			{
				end_date_elt.set("value", start_date_elt.get("value"));
			}
		}	
	<?php
		echo "</script>\n";
	
	}
}


?>