<?php

abstract class AutoFormLayout
{	
	var $form;
	
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
		case "table_row":
			return new TableRowFormLayout($form);
			
		case "spreadsheet":
			return new SpreadsheetFormLayout($form);
			
		case "table":
	
		default:
			return new TableBasedFormLayout($form);
		}
	}
}

class TableBasedFormLayout extends AutoFormLayout
{
	function TableBasedFormLayout($form)
	{
		$this->AutoFormLayout($form);
	}
	
	function startGroup($name)
	{
		echo "<fieldset><legend>$name</legend>";
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
		
		echo "<tr>\n";
		
		$this->printLabel($field, $renderer, $styles, ($renderer->colspan > 1) ? $this->parent->annotations[$field] : "");
		
		if ($renderer->colspan > 1)
		{
			echo "</tr>\n";
			echo "<tr>\n";
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
		if (!$renderer->annotateBefore && $renderer->colspan == 1 && $renderer->parent->annotations[$field])
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
		if ($renderer->hideLabel && $renderer->colspan > 1 && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		echo "<td colspan='$colspan'";
		if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
		if ($styles) echo " style='$styles'";
		echo ">";
		
		if (!$renderer->hideLabel) echo "<label for='$field'>$label</label> ";
		
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

/*
 * Used for each subordinate form row in 
 * SpreadSheetAutoForm.
 */
class TableRowFormLayout extends AutoFormLayout
{
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
		if ($renderer->hideLabel && $renderer->colspan > 1 && !$annotation) return;
		
		$label = $this->getLabel($field, $renderer);
		
		echo "<th ";
		if ($renderer->parent->labelCSS) echo " class='{$renderer->parent->labelCSS}'";
		if ($styles) echo " style='$styles'";
		echo ">";
		
		if (!$renderer->hideLabel) echo "<label for='$field'>$label</label> ";
		
		echo "$annotation</th>\n";
	}
	
	function getLabel($field, $renderer, $styles, $annotation)
	{
		$obj = $renderer->parent->getData();
		$label = isset($renderer->label) ? $renderer->label : $obj->prettifyFieldName($field);
		
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

/*
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
	
	function getLabel($field, $renderer, $styles, $annotation)
	{
		// nothing to do
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
