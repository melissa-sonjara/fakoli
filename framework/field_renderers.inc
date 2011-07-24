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

require_once realpath(dirname(__FILE__)."/data_item.inc");

Fakoli::using("context_help");

/**
 * FieldRenderer is the abstract base class for all FieldRenderers. 
 * FieldRenderers are used by the AutoForm class to provide the editing/display
 * controls for the fields in the form.
 * 
 * The FieldRenderer class is itself just the base class from which specific
 * FieldRenderers can be derived. FieldRenderers can be specialized based on the
 * data type of the field they are renderering, or may provide more specific UI,
 * such as dropdown lists providing options or related objects in the database, etc.
 * 
 * Applications can also provide their own custom FieldRenderers to provide specific
 * behavior within a form when the default behavior is not suitable.
 *
 */
class FieldRenderer
{
	var $parent = null;
	var $labelSuffix = "";
	var $colspan = 1;
	var $annotateBefore = false;
	var $annotateNextLine = true;
	var $hideLabel = false;
	
	/**
	 * Constructor
	 *
	 * @param AutoForm $parent - the parent form for this FieldRenderer object
	 * @return FieldRenderer
	 */
	function FieldRenderer($parent)
	{
		global $auto_form_defaults;
		$this->parent =& $parent;
		if (isset($auto_form_defaults["labelSuffix"]))
		{
			$this->labelSuffix = $auto_form_defaults["labelSuffix"];
		}		
	}
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function _printLabel($field, $colspan=1, $styles = "", $annotation = "")
	{		
		if ($this->hideLabel && $colspan > 1 && !$annotation) return;
		
		$label = $this->_getLabel($field);
		
		echo "<td colspan='$colspan'";
		if ($this->parent->labelCSS) echo " class='{$this->parent->labelCSS}'";
		if ($styles) echo " style='$styles'";
		echo ">";
		
		if (!$this->hideLabel) echo "<label for='$field'>$label</label> ";
		
		echo "$annotation</td>\n";
	}

	function _getLabel($field, $addSuffix = true)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if ($this->parent->markRequiredFields &&
		    $this->parent->isRequired($field))
		{
	    	$label .= "*";
	    }

	    $label .= $this->labelSuffix;
		
	    if ($this->parent->onFormatLabel)
	    {
	    	$fn = $this->parent->onFormatLabel;
	    	$label = $fn($this->parent, $field, $label);
	    }
	    
	    return $label;
	}
	
	/**
	 * Internal method to generate the starting HTML for the field (including the label)
	 *
	 * @param string $field the field name
	 * @param unknown_type $styles optional special styles that need to be applied
	 */
	function _startField($field, $styles = "")
	{
		if ($this->colspan > 1) $styles = "text-align: left;$styles";
		echo "<tr>\n";
		$this->_printLabel($field, $this->colspan, $styles, ($this->colspan > 1) ? $this->parent->annotations[$field] : "");
		
		if ($this->colspan > 1)
		{
			echo "</tr>\n";
			echo "<tr>\n";
		}
		echo "<td colspan='{$this->colspan}'";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";
		
		if ($this->annotateBefore && $this->parent->annotations[$field] && $this->colspan == 1)
		{
			echo $this->parent->annotations[$field]."<br/>";
		}
	}
	
	/**
	 * Interneal method to generate the closing HTML for the field.
	 *
	 * @param string $field the field name
	 */
	function _endField($field)
	{
		if (!$this->annotateBefore && $this->colspan == 1 && $this->parent->annotations[$field])
		{
			if ($this->annotateNextLine)
			{
				echo "<br/>"; 
			}
			else 
			{
				echo "&nbsp;&nbsp;";
			}
			echo $this->parent->annotations[$field];
		}
		
		echo "</td>\n</tr>\n";
	}

	
	/**
	 * This method is called by the AutoForm to add any default input validators that are required
	 * by the FieldRenderer.
	 * 
	 * Fields that need custom validation or a custom required validator 
	 * should override this function.
	 *
	 * @param string $field the field name
	 * 
	 * @param boolean $required - whether the field is required
	 */
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
			$this->parent->validator->add(new RequiredValidator($field, $label));
	}
	
	
	/**
	 * Formats the given DataItem based on the supplied format string. This can either
	 * be a string containing the name of the field to return, or it can be a more
	 * complex format string using the syntax provided by DataItem::format().
	 *
	 * @param DataItem $item the item to format
	 * @param string $name the name field or format string, as described above.
	 * @return string the formatted text
	 */
	function formatName($item, $name)
	{		
		if (strpos($name, "{") !== false)
		{
			$name = $item->format($name);
		}
		else
		{
			$name = $item->$name;
		}
		return $name;
	}
	
	/**
	 * FieldRenderers can override this method to provide any Javascript
	 * that their control requires for an edit form.
	 *
	 * @param string $field the field name
	 */
	function renderScript($field)
	{
		// No output by default	
	}
	
	/**
	 * FieldRenderers must override this method to provide the HTML implementation
	 * of the control used to edit the field.
	 *
	 * @param string $field the field name
	 */
	function renderField($field)
	{
		// No output by default
	}
	
	/**
	 * FieldRenderers can override this method to provide any Javascript that 
	 * the control requires when being used in a search form.
	 *
	 * @param string $field the field name
	 * @param string $mode the search mode for the specific field ('equal', 'like', 'from', 'to', 'range')
	 */
	function renderSearchScript($field, $mode)
	{
		// No output by default
	}
	
	/**
	 * FieldRenderers must override this method to provide the HTML implementation
	 * of the control displayed for the field in a search form.
	 *
	 * @param string $field the field name
	 */
	function renderSearchField($field, $mode)
	{
		// No output by default
	}
	
	/**
	 * FieldRenderers can override this method to provide any Javascript that must be executed 
	 * when the form is submitted on the client. See HTMLFieldRenderer for an example of how 
	 * this is used.
	 *
	 * @param string $field the field name
	 */
	function renderOnSubmitHandler($field)
	{
		// No output by default	
	}
	
	/**
	 * FieldRenderers can override this method to provide behavior that occurs prior to the
	 * saving of the parent form's target object to the database. For example, the FileUploadFieldRenderer
	 * overrides this method to process the uploading of the file and then store the location
	 * in the associated field in the target object.
	 *
	 * @param string $field the field name
	 */
	function preProcess($field = "")
	{
		// Nothing to do by default
	}
	
	/**
	 * FieldRenderers can override this method to provide behavior that occurs after the parent form's
	 * target object has been saved to the database. For example, the CrossReferenceSelectFieldRenderer
	 * overrides this method to update crossference tables based on the user's selection.
	 *
	 * @param unknown_type $field
	 */
	function postProcess($field = "")
	{
		// Nothing to do by default
	}
}

require_once realpath(dirname(__FILE__))."/field_renderers/boolean_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/cross_reference_select_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/currency_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/date_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/datetime_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/file_upload_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/html_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/number_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/password_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/radio_button_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/rating_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/related_image_select_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/related_item_radio_button_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/related_item_select_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/select_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/string_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/text_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/time_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/tree_select_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/checklist_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/timezone_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/zipcode_field_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/select_field_view_source_renderer.inc";
require_once realpath(dirname(__FILE__))."/field_renderers/data_list_field_renderer.inc";

?>