<?php

/**
 * AutoFormLayout provides the abstract base class for form layout engine classes. When a form is constructed
 * as an AutoForm object, the layout engine provides the HTML structure that the form fields are rendered within.
 * Fakoli provides a number of layout engines for different application styles. You can specify a default layout
 * in your applications config or settings, and can override this value on any AutoForm via the AutoForm::setLayout()
 * method.
 *  
 * @author andy
 *
 */
abstract class AutoFormLayout
{	
	var $form;

	var $externalErrorBox = false; ///< Flag indicating whether the error box should be rendered inside the form grouping
	
	function AutoFormLayout($form)
	{
		$this->form = $form;
	}
	
	abstract function startGroup($name);
	abstract function endGroup();

	abstract function startField($field, $renderer, $colspan = 1, $styles = "");
	abstract function endField($field, $renderer);
	
	abstract function printLabel($field, $renderer, $styles = "", $annotation = "");
	
	function getLabel($field, $renderer, $addSuffix = true)
	{
		$label = isset($renderer->label) ? $renderer->label : $this->form->prettifyFieldName($field);
		
		if ($renderer->parent->markRequiredFields &&
		    $renderer->parent->isRequired($field))
		{
	    	$label .= "*";
	    }

	    $label .= $renderer->labelSuffix;
		
	    if ($renderer->parent->onFormatLabel)
	    {
	    	$fn = $renderer->parent->onFormatLabel;
	    	$label = $fn($renderer->parent, $field, $label);
	    }
	    
	    return $label;
	}
	
	function getLabelID($field)
	{
		return "{$this->form->id}_{$field}_label";		
	}
	
	abstract function startUngrouped();
	abstract function endUngrouped();
	
	abstract function startButtonLine();
	abstract function endButtonLine();
	
	abstract function errorBox();
	abstract function requiredFields($text);
	
	static function create($type, $form)
	{
		switch($type)
		{
		case "mobile":
			return new MobileFormLayout($form);

		case "table_header":
			return new TableHeaderFormLayout($form);
			
		case "table_row":
			return new TableRowFormLayout($form);
		
		case "selectable_table_header":
			return new SelectableTableHeaderFormLayout($form);
			
		case "selectable_table_row":
			return new SelectableTableRowFormLayout($form);
			
			
		case "spreadsheet":
			return new SpreadsheetFormLayout($form);
			
		case "simple":
			return new SimpleFormLayout($form);
			
		case "table":
	
		default:
			return new TableBasedFormLayout($form);
		}
	}
	
	function finalizeLayout()
	{
	}
}

/**
 * Lays out the AutoForm using table-based layout. 
 */
class TableBasedFormLayout extends AutoFormLayout
{
	function TableBasedFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name, $collapsible = false, $collapsed = false)
	{
		$class = "";
		if ($collapsible)
		{
			$class = "collapsible";
			$class .= $collapsed ? " collapsed" : " expanded";
			
			$class = " class='$class'";
		}
		
		$id = $this->form->id . "_" . codify($name) . "_group";
		
		echo "<fieldset id='$id'$class><legend>$name</legend>";
		
		$desc = $this->form->getGroupDescription($name);
		if ($desc)
		{
			echo "<div id='{$id}_description' class='{$this->form->groupDescriptionCSS}'>{$desc}</div>";
		}
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
		echo "</fieldset>";
	}
	
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		if ($renderer->colspan > 1) $styles = "text-align: left;$styles";
		
		echo "<tr class='{$renderer->parent->id}_{$field}_field'>\n";
		
		$this->printLabel($field, $renderer, $styles, ($renderer->annotateBefore && $renderer->colspan > 1) ? "<br/>".$renderer->parent->annotations[$field] : "");
		
		if ($renderer->colspan > 1)
		{
			echo "</tr>\n";
			echo "<tr class='{$renderer->parent->id}_{$field}_field'>\n";
		}
		echo "<td colspan='{$renderer->colspan}'";
		if ($renderer->parent->valueCSS) echo " class='{$renderer->parent->valueCSS}'";
		echo ">";
		
		if ($renderer->annotateBefore && $renderer->parent->annotations[$field] && $renderer->colspan == 1)
		{
			echo $renderer->parent->annotations[$field]."<br/>";
		}
	}
	
	function endField($field, $renderer)
	{
		if (!$renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			if ($renderer->annotateNextLine)
			{
				echo "<br/>"; 
			}
			else 
			{
				echo "&nbsp;&nbsp;";
			}
			echo $renderer->parent->annotations[$field];
		}
		
		echo "</td>\n</tr>\n";
	}
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{		
		$colspan = $renderer->colspan;
		if ($renderer->hideLabel && $colspan > 1 && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		echo "<td colspan='$colspan'";
		if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
		if ($styles) echo " style='$styles'";
		echo ">";
		
		if (!$renderer->hideLabel) echo "<label id='".$this->getLabelID($field)."' for='$field'>$label</label> ";
		
		echo "$annotation</td>\n";
	}

	function startUngrouped()
	{
		echo "<table";
		if ($this->form->formCSS) echo " class='{$this->form->formCSS}'";
		if ($this->form->style) echo " style='{$this->form->style}'";
		echo ">\n";
	}
	
	function endUngrouped()
	{
		echo "</table>";
	}
	
	function startButtonLine()
	{
		echo "<tr>\n";
		echo " <td colspan='2' class='submit' style='text-align: {$this->form->buttonAlignment}'><br/>";	
	}
	
	function endButtonLine()
	{
		echo "</td></tr>";
	}
	
	function errorBox()
	{
		echo "<tr>\n <td colspan='2' id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</td></tr>\n";
	}	
	
	function requiredFields($text)
	{
		echo "<tr>\n <td colspan='2'><b>{$text}</b></td></tr>\n";
	}
}

/**
 * Used when you want layout to provide field ordering but
 * don't want groups or borders around the group(s).
 */
class UngroupedFormLayout extends TableBasedFormLayout
{
	function UngroupedFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name)
	{
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
	}	
}

/**
 * Mobile version of UngroupedFormLayout
 */
class MobileUngroupedFormLayout extends MobileFormLayout
{
	function MobileUngroupedFormLayout($form)
	{
		$this->MobileFormLayout($form);
	}
	
	function startGroup($name)
	{
		//$this->startUngrouped();
	}
	
	function endGroup()
	{
		//$this->endUngrouped();
	}
}
/**
 * Used for the header row in
 * SpreadSheetAutoForm.
 */
class TableHeaderFormLayout extends AutoFormLayout
{
	function TableHeaderFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}

	function startGroup($name)
	{
		$this->startUngrouped();
	}

	function endGroup()
	{
		$this->endUngrouped();
	}

		
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		echo "<td ";
		if ($renderer->parent->valueCSS) echo " class='{$renderer->parent->valueCSS}'";
		echo ">";

		if ($renderer->annotateBefore && $renderer->parent->annotations[$field] && $renderer->colspan == 1)
		{
			echo $renderer->parent->annotations[$field]."<br/>";
		}
	}

	function endField($field, $renderer)
	{
		echo "</td>\n";
	}

	function requiredFields($text)
	{
		// empty - want displayed once at top not for every form in the spreadsheet
	}

	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{
		//if ($renderer->hideLabel && $renderer->colspan > 1 && !$annotation) return;

		$label = $this->getLabel($field, $renderer);

		echo "<th ";
		if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
		if ($styles) echo " style='$styles'";
		echo ">";

		echo "<label id='".$this->getLabelID($field)."' for='$field'>$label</label> ";

		echo "$annotation</th>\n";
	}

	function getLabel($field, $renderer, $addSuffix = true)
	{
		$obj = $renderer->parent->getData();
		 $label = isset($renderer->label) ? $renderer->label : $renderer->parent->prettifyFieldName($field);

		if ($renderer->parent->markRequiredFields &&
				$renderer->parent->isRequired($field))
		{
		$label .= "*";
		}

		$label .= $renderer->labelSuffix;

		if ($renderer->parent->onFormatLabel)
		{
		$fn = $renderer->parent->onFormatLabel;
		$label = $fn($renderer->parent, $field, $label);
		}
	  
		return $label;
	}

	function startUngrouped()
	{
		echo "<tr id='{$this->form->id}_container' class='{$this->form->containerClass}'>\n";
	}

	function endUngrouped()
	{
		echo "</tr>\n";
	}

	function startButtonLine()
	{
		// nothing to do
	}

	function endButtonLine()
	{
		// nothing to do
	}

	function errorBox()
	{
		echo "<tr>\n <td colspan='{$this->form->parent->colCount}' id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</td></tr>\n";
	}
}


/**
 * Overrides the default table-row form layout to provide a selection checkbox
 * for SpreadsheetForms with selectable sub-forms.
 * @author andy
 */
class SelectableTableHeaderFormLayout extends TableHeaderFormLayout
{
	function SelectableTableHeaderFormLayout($form)
	{
		$this->TableHeaderFormLayout($form);
	}

	function startUngrouped()
	{
		echo "<tr id='{$this->form->id}_container' class='{$this->form->containerClass}'><th style='text-align: center'>&nbsp;</th>\n";
	}

}



/**
 * Used for each subordinate form row in 
 * SpreadSheetAutoForm.
 */
class TableRowFormLayout extends AutoFormLayout
{
	var $externalErrorBox = true;
	
	function TableRowFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name)
	{
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
	}
	
			
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		echo "<td ";
		if ($renderer->parent->valueCSS) echo " class='{$renderer->parent->valueCSS}'";
		echo ">";
		
		if ($renderer->annotateBefore && $renderer->parent->annotations[$field] && $renderer->colspan == 1)
		{
			echo $renderer->parent->annotations[$field]."<br/>";
		}		
	}
	
	function endField($field, $renderer)
	{
		echo "</td>\n";
	}
	
	function requiredFields($text)
	{
		// empty - want displayed once at top not for every form in the spreadsheet
	}	
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{		
		// Nothing to do
	}
	
	function getLabel($field, $renderer, $addSuffix = true)
	{
		return "";
	}
	
	function startUngrouped()
	{
		echo "<tr id='{$this->form->id}_container' class='{$this->form->containerClass}'>\n";
	}
	
	function endUngrouped()
	{		
		echo "</tr>\n";
	}
	
	function startButtonLine()
	{
		// nothing to do
	}
	
	function endButtonLine()
	{
		// nothing to do
	}
	
	function errorBox()
	{
		echo "<tr>\n <td colspan='{$this->form->parent->colCount}' id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</td></tr>\n";
	}	
}

/**
 * Overrides the default table-row form layout to provide a selection checkbox
 * for SpreadsheetForms with selectable sub-forms.
 * @author andy
 */
class SelectableTableRowFormLayout extends TableRowFormLayout
{
	function SelectableTableRowFormLayout($form)
	{
		$this->TableRowFormLayout($form);
	}
	
	function startUngrouped()
	{
		$selected = $this->form->isSelected ? " checked='checked'" : "";
		
		echo "<tr id='{$this->form->id}_container' class='{$this->form->containerClass}'><td style='text-align: center'><input type='checkbox' class='checkbox selector'$selected id='{$this->form->id}__selected' name='#selected' value='1'/></td>\n";
	}
	
}

/**
 * Used for the overall layout handling of SpreadsheetAutoForm
 * instance.
 */
class SpreadsheetFormLayout extends AutoFormLayout
{
	function SpreadsheetFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name)
	{
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
	}
	
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		// nothing do to
	}
	
	function endField($field, $renderer)
	{
		// nothing do to
	}
	
	function requiredFields($text)
	{
		echo "<p><b>$text</b></p>";	
	}	
	
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{
		// nothing to do		
	}
	
	function getLabel($field, $renderer, $addSuffix = true)
	{
		return ""; // nothing to do
	}
	
	function startUngrouped()
	{
		echo "<table ";
		if ($this->form->formCSS) echo " class='{$this->form->formCSS}'";
		if ($this->form->style) echo " style='{$this->form->style}'";
		echo ">\n";
	}
	
	function endUngrouped()
	{
		echo "</table>";
	}
	
	function startButtonLine()
	{
		echo "<tr>\n";
		echo " <td colspan='{$this->form->colCount}' class='submit' style='text-align: {$this->form->buttonAlignment}'><br/>";	
	}
	
	function endButtonLine()
	{
		echo "</td></tr>";
	}
	
	function errorBox()
	{
		// nothing to do
	}	
}

/**
 * MobileFormLayout provides a linear form layout suitable for mobile devices.
 */
class MobileFormLayout extends AutoFormLayout
{
	function MobileFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name, $collapsible = false, $collapsed = false)
	{
		$class = "";
		if ($collapsible)
		{
			$class = "collapsible";
			$class .= $collapsed ? " collapsed" : " expanded";
			
			$class = " class='$class'";
		}
		
		$id = $this->form->id . "_" . codify($name) . "_group";
		
		echo "<fieldset id='$id'$class><legend>$name</legend>";
		
		$desc = $this->form->getGroupDescription($name);
		if ($desc)
		{
			echo "<div id='{$id}_description' class='{$this->form->groupDescriptionCSS}'>{$desc}</div>";
		}
		$this->startUngrouped();
		
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
		echo "</fieldset>";
	}
	
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{	
		echo "<div class='field {$renderer->parent->id}_{$field}_field'>\n";
		
		$this->printLabel($field, $renderer, $styles, $this->parent->annotations[$field]);
		
		if (!$renderer->hideLabel) echo "<br/>\n";
		
		if ($renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			echo $renderer->parent->annotations[$field]."<br/>";
		}
	}
	
	function endField($field, $renderer)
	{
		if (!$renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			if ($renderer->annotateNextLine)
			{
				echo "<br/>"; 
			}
			else 
			{
				echo "&nbsp;&nbsp;";
			}
			echo $renderer->parent->annotations[$field];
		}
		
		echo "</div>\n";
	}
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{		
		$colspan = $renderer->colspan;
		if ($renderer->hideLabel && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		if (!$renderer->hideLabel)
		{
			echo "<label id='".$this->getLabelID($field)."' for='$field'";
			if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
			if ($styles) echo " style='$styles'";
		
			echo ">$label</label> ";
		}
		
		echo "$annotation\n";
	}

	function startUngrouped()
	{
		echo "<div";
		if ($this->form->formCSS) echo " class='{$this->form->formCSS}'";
		if ($this->form->style) echo " style='{$this->form->style}'";
		echo ">\n";
	}
	
	function endUngrouped()
	{
		echo "</div>";
	}
	
	function startButtonLine()
	{
		echo "<br/>\n";
		echo " <div class='submit' style='text-align: {$this->form->buttonAlignment}'><br/>";	
	}
	
	function endButtonLine()
	{
		echo "</div><br/>";
	}
	
	function errorBox()
	{
		echo "<div id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</div>\n";
	}	
	
	function requiredFields($text)
	{
		echo "<p style='font-weight: bold'>{$text}</p>\n";
	}
}




/**
 * SimpleFormLayout provides a form with minimal HTML, and does not employ table-based layout.
 */
class SimpleFormLayout extends AutoFormLayout
{
	function MobileFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name, $collapsible = false, $collapsed = false)
	{
		$class = "";
		if ($collapsible)
		{
			$class = "collapsible";
			$class .= $collapsed ? " collapsed" : " expanded";
			
			$class = " class='$class'";
		}
		
		$id = $this->form->id . "_" . codify($name) . "_group";
		
		echo "<fieldset id='$id'$class><legend>$name</legend>";

		$desc = $this->form->getGroupDescription($name);
		if ($desc)
		{
			echo "<div id='{$id}_description' class='{$this->form->groupDescriptionCSS}'>{$desc}</div>";
		}
		$this->startUngrouped();
		
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
		echo "</fieldset>";
	}
	
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{	
		echo "<br/>\n";
		
		$this->printLabel($field, $renderer, $styles, $this->parent->annotations[$field]);
		
		if ($renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			echo $renderer->parent->annotations[$field]." ";
		}

		if ($colspan == 2) echo "<br/>\n";
	}
	
	function endField($field, $renderer)
	{
		if (!$renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			if ($renderer->annotateNextLine)
			{
				echo "<br/>"; 
			}
			else 
			{
				echo "&nbsp;&nbsp;";
			}
			echo $renderer->parent->annotations[$field];
		}
		
		echo "<br/>\n";
	}
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{		
		$colspan = $renderer->colspan;
		if ($renderer->hideLabel && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		if (!$renderer->hideLabel)
		{
			echo "<label id='".$this->getLabelID($field)."' for='$field'";
			if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
			if ($styles) echo " style='$styles'";
		
			echo ">$label</label> ";
		}
		
		echo "$annotation\n";
	}

	function startUngrouped()
	{
		echo "<div";
		if ($this->form->formCSS) echo " class='{$this->form->formCSS}'";
		if ($this->form->style) echo " style='{$this->form->style}'";
		echo ">\n";
	}
	
	function endUngrouped()
	{
		echo "</div>";
	}
	
	function startButtonLine()
	{
		echo "<br/>\n";
		echo " <div class='submit' style='text-align: {$this->form->buttonAlignment}'><br/>";	
	}
	
	function endButtonLine()
	{
		echo "</div><br/>";
	}
	
	function errorBox()
	{
		echo "<div id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</div>\n";
	}	
	
	function requiredFields($text)
	{
		echo "<p style='font-weight: bold'>{$text}</p>\n";
	}
}

/**
 * Layout for list filtering using filter_form.inc.
 * This layout is not table-based; it used float left
 * divs to layout the filter fields left to right.
 * 
 * @author janice
 */
class FilterFormLayout extends AutoFormLayout
{
	function FilterFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name, $collapsible = false, $collapsed = false)
	{
		$class = "";
		if ($collapsible)
		{
			$class = "collapsible";
			$class .= $collapsed ? " collapsed" : " expanded";
			
			$class = " class='$class'";
		}
		
		$id = $this->form->id . "_" . codify($name) . "_group";
		
		$this->startUngrouped();
	}
	
	function endGroup()
	{
		$this->endUngrouped();
	}
	
	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		echo "<div class='{$renderer->cssClass}'>\n";
		$this->printLabel($field, $renderer, $styles, "");
	}
	
	function endField($field, $renderer)
	{
		echo "</div>\n";
	}
	
	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{		
		if ($renderer->hideLabel) return;
		
		$label = $this->getLabel($field, $renderer);
		
		if (!$renderer->hideLabel) echo "<label id='".$this->getLabelID($field)."' for='$field'>$label</label> ";
	}

	function startUngrouped()
	{
		// nothing to do
	}
	
	function endUngrouped()
	{
		// nothing to do
	}
	
	function startButtonLine()
	{
		// nothing to do
	}
	
	function endButtonLine()
	{
		// nothing to do
	}
	
	function errorBox()
	{
		// nothing to do
	}	
	
	function requiredFields($text)
	{
		// nothing to do
	}
}

/**
 * TemplateFormLayout provides support for custom form layouts.
 * @author andy
 */
class TemplateFormLayout extends AutoFormLayout
{
	var $template;
	var $output;
	var $showButtons = true;
	
	function TemplateFormLayout($form, $template, $showButtons = true)
	{
		trace("Using form template {$template}", 3);
		$this->template = file_get_contents($template);
		$this->showButtons = $showButtons;
		$this->AutoFormLayout($form);
	}

	function startGroup($name, $collapsible = false, $collapsed = false)
	{
		// Groups are ignored when dealing with template form layouts
		$this->startUngrouped();
	}

	function endGroup()
	{
		$this->endUngrouped();
	}

	function startField($field, $renderer, $colspan = 1, $styles = "")
	{
		$annotations = $renderer->annotateBefore ? $renderer->parent->annotations[$field] : "";
		$this->printLabel($field, $renderer, $styles, $annotations);
		
		ob_start();
	}

	function endField($field, $renderer)
	{
		$fieldContent = ob_get_contents();
		
		ob_end_clean();
		
		if (!$renderer->annotateBefore && $renderer->parent->annotations[$field])
		{
			if ($renderer->annotateNextLine)
			{
				$fieldContent .= "<br/>";
			}
			else
			{
				$fieldContent .= "&nbsp;&nbsp;";
			}
			
			$fieldContent .= $renderer->parent->annotations[$field];
		}
		
		$this->template = str_replace("{".$field."}", $fieldContent, $this->template);		
	}

	/**
	 * Internal method to generate the HTML for the field label
	 *
	 * @param string $field the field name
	 * @param int $colspan the number of columns to span
	 * @param string $styles optional special styles that need to be applied
	 * @param string $annotation annotation text for the field
	 */
	function printLabel($field, $renderer, $styles = "", $annotation = "")
	{
		if ($renderer->hideLabel && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		$labelHTML = "";
		
		if (!$renderer->hideLabel)
		{
			$labelHTML .= "<label id='".$this->getLabelID($field)."' for='$field'";
			if ($renderer->parent->labelCSS) $labelHTML .=  " class='{$renderer->parent->labelCSS}'";
			if ($styles) $labelHTML .=  " style='$styles'";
		
			$labelHTML .=  ">$label</label> ";
		}
		
		$labelHTML .=  "$annotation\n";
		
		$this->template = str_replace("{label:".$field."}", $labelHTML, $this->template);		
	}

	function startUngrouped()
	{
		if (!$this->renderingForm)
		{
			$this->output .= "{FORM}";
			$this->renderingForm = true;
		}
	}

	function endUngrouped()
	{
	}

	function startButtonLine()
	{
		$this->output .= "<br/>\n";
		$this->output .= " <div class='submit' style='text-align: {$this->form->buttonAlignment}'><br/>";
		
		ob_start();
		
	}
	
	function endButtonLine()
	{
		if ($this->showButtons) 
		{
			$this->output .= ob_get_contents();
		}
		ob_end_clean();
		
		$this->output .=  "</div><br/>";
	}
	
	function errorBox()
	{
		$this->output .=  "<div id='{$this->form->id}__error' class='error' style='display: ".($this->form->msg ? 'table-cell' : 'none')."'>{$this->form->msg}</div>\n";
	}
	
	function requiredFields($text)
	{
		$this->output .=  "<p style='font-weight: bold'>{$text}</p>\n";
	}
	
	function finalizeLayout()
	{
		// Clear any unmapped fields
		foreach($this->form->data->getFields() as $field => $type)
		{
			$this->template = str_replace(array("{{$field}}", "{label:{$field}}"), array("", ""), $this->template);
		}
			
		$this->output = str_replace("{FORM}", $this->template, $this->output);
		echo $this->output;
	}
}
