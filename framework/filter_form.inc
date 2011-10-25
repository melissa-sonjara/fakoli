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

Fakoli::usingFile("framework/search_form.inc");
Fakoli::usingFile("framework/field_renderers/select_field_renderer.inc");
Fakoli::usingFile("framework/field_renderers/date_field_renderer.inc");

/*
 * Filter form adds a select field renderer to a page that allows
 * the user to select one item from the filter and the page reloads,
 * querying the list of items using the filter selection.
 * 
 * The concept is that a list page of objects of class myItem, can be 
 * filtered by creating an empty instance of class myItem, 
 * assigning the "GET" filter values to fields in that instance. 
 * Only fields in myItem that are useful for the filter would be 
 * included in its object filter. 
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 4/2011
 * 
 * Example of incorporating this feature into a form that lists
 * budget table data where budget obj has field "year" and we show
 * a year drop down.
 * 
 * Fakoli::usingFeature("filter_form");
 * 
 * $year = checkNumeric($_GET["year"]);
 * 
 * if(!$year) // get your default year
 * 	$year = FiscalYear::currentYear()->year;
 * 
 * $budget = new Budget();
 * $budget->year = $year;
 * $budget->filter = new InclusionFilter("year");
 * $filterForm = new FilterForm($budget);
 * 
 * 	$yearSelect = new FilterFieldRenderer($filterForm, "year", "Year", Budget::getYearOptions());
 *  $yearSelect->all = false; // we don't want to show "All" option in the filter
 *  
 *  $constraint = $filterForm->getConstraint();
 *  
 *  $filterForm->drawForm();
 *  
 *  * * * * * * * * *
 *  
 *  Example of filtering application objects:
 *  
 *  $application = new Application();
 *  $application->status = checkNumeric($_GET["status"]);
 *  $application->round_id = $_GET["round_id"];
 *  $application->country_id = $_GET["country_id"];
 *  $application->filter = new InclusionFilter("status", "country_id", "round_id");
 *  $filterForm = new FilterForm($application);
 *  $statusSelect = new FilterFieldRenderer($filterForm, "status", "Status", $application->statusTypes);
 *  $roundSelect = new FilterFieldRenderer($filterForm, "round_id", "Round", FundingRound::getRoundList());
 *  $countrySelect = new FilterFieldRenderer($filterForm, "country_id", "Country", Country::getList("WHERE country_id in (SELECT country_id FROM application)"));
 *  
 *  $applications = query(Application, $filterForm->getConstraint()." ORDER BY round_key DESC");
 *	
 *  $filterForm->drawForm();
 *  
 *  Example when using CompositeDataItems:
 *  Must use methods get and set on the CompositeDataItem
 *  
 *  $filterForm->data->set("event_type", $_GET["event_type"]);
 *  
 *  $te = 	new CompositeTrainingEvent();
 *  $te->filter = new InclusionFilter("event_type");
 *  $te->set("event_type", $_GET["event_type"]);
 *  $filterForm = new FilterForm($te);
 *  $typeSelect = new FilterFieldRenderer($filterForm, "event_type", "Event Type", array(TrainingEvent::formatEventTypes()));
 *  
 *  $constraint = $filterForm->getConstraint();
 *  
 *  Use constraint to query on your target object's table, 
 *  e.g., "Events"
 *  
 *  $filterForm->drawForm();
 *  
 */
class FilterForm extends SearchForm
{	
	var $search;
	
	function FilterForm($target, $method="GET", $action="", $id="")
	{
		parent::SearchForm($target, $method, $action, $id);
		$this->formCSS = "filter";
		$this->setFilterParameters();
	}
		
		
	function drawForm()
	{
		$obj =& $this->data;
		$pk = $obj->getPrimaryKey();

		echo "<form id='{$this->id}_filter' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'>\n";
		echo "<div";
 		if ($this->formCSS) echo " class='{$this->formCSS}'";
		if ($this->style) echo " style='{$this->style}'";
		echo ">\n";
		
		foreach(array_keys($this->hidden) as $hidden)
		{
			echo "<input id='{$this->id}_{$hidden}' type='hidden' name='$hidden' value='".htmlSafe($obj->get($hidden))."'/>\n";
		}

		$this->renderSearchFields();
		
		echo "</div>\n";
		echo "</form>\n";
	}

	/*
	 * Override an existing SearchParameter constraint
	 * clause handler or add one. 
	 * 
	 * @handler - callback function that creates the
	 * constraint clause for the given field
	 */
	function setHandler($field, $handler)
	{
		$this->search->paramHandlers[$field] = $handler;
	}
	
	
	/*
	 * Default for all parameters included in filter is equal
	 * Additional search parameters can be added by calling script with
	 * the syntax:
	 * $filterForm->search->setParam("round_id", "member", implode(",", array_keys($rounds)));
	 * Do not include these additional parameters in the object filter.
	 */
	function setFilterParameters()
	{
		$obj = $this->data;
		
		// JDG 9/2011 handle CompositeDataItems
		$fields = $obj->getFields();
		$this->search = new SearchParameters($obj);

		foreach(array_keys($fields) as $field)
		{
			if (!array_key_exists($field, $this->hidden) &&
				!($obj->filter && $obj->filter->isExcluded($field))
				)
			{
				$value = $obj->get($field);
				if($value && !is_int($value))
					$value = urlencode($value);
				if($value)
					$this->search->setParam($field, "equal", $value);
			}
		}
	}
	
	function getConstraint($first = true, $firstText = "WHERE")
	{
		return $this->search->generateConstraint($first, $firstText);
	}
	
	function setParam($field, $mode, $value)
	{
		$this->search->setParam($field, $mode, $value);
	}
	
}

/*
 * Renders a select field drop down with the option
 * set given. When one option is selected, the page
 * reloads with the selected item as the parameter 
 * to be retrieved with "GET".
 * 
 */
class FilterFieldRenderer extends SelectFieldRenderer
{
	var $options;
	var $label;
	var $all = true;
	var $all_value = "";
	
	function FilterFieldRenderer(&$form, $field, $label = "", $options)
	{
		$this->options = $options;
		$this->label = ($label) ? $label : prettify($field);
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
		}		
	}
	
	
	function renderField($field)
	{
	   	$label = $this->label;
		
		?>
  		<div style="float: left">
		<label for='<?php echo $field?>' class="<?php echo $this->parent->labelCSS ?>"><?php echo $label ?></label>		
    	<select name="<?php echo $field ?>" onchange="this.form.submit();">
    	<?php 
    	if($this->all)
    	{
    	?>
		<option value="<?php echo $this->all_value ?>">All</option>  
		<?
    	}
    	
     	$fieldValue = $this->parent->data->get("$field");
     	foreach($this->options as $value => $name)
		{
  			$selected = (!strcmp($value, $fieldValue)) ? " selected" : "";
			echo "<option value='{$value}' $selected>$name</option>\n";
		}
		?>
   		</select>&nbsp;</div>
  		<?		
	}
	
	function renderSearchField($field)
	{
		$this->renderField($field);
	}
	
} // end class FilterFieldRenderer


/*
 * For Boolean checkboxes, the unchecked value "0" is not
 * included in the parameter string. This is a problem when
 * we want the field to be checked on initial page load or
 * by default because we can't identify when the box is
 * unchecked b/c the user unchecked it or because
 * we just loaded. To fix this problem, we write the checkbox
 * as field name "field" + "_box" and create a hidden field
 * with the field name. This hidden field will appear in the 
 * url as 0 if user unchecked the box and 1 if checked.
 * The hidden field is set using onchange event on the
 * checkbox html.
 * 
 * If checked is the desired default, set to 1 in the calling script:
 * e.g.,
 * 
	$unbilledCheckbox = new BooleanFilterFieldRenderer($filterForm, "ledger_id", "Show Unbilled Only");
	$unbilledCheckbox->default = 1;
 *
 * For BooleanFilters, we also may want to use a callback to
 * generate the clause for this field. E.g:
 * 
	// Set custom callback handler to generate the constraint for ledger_id
	$filterForm->setHandler("ledger_id", array(ProjectManager, getLedgerClause));
	
	function getLedgerClause($name, $value)
	{
		if(!$value && is_numeric($value))
			return "";
		else
			return "{$name}=0";
	}
 */
class BooleanFilterFieldRenderer extends BooleanFieldRenderer
{
	var $default = 0; // initial setting when page loaded
	
	function BooleanFilterFieldRenderer(&$parent, $field = "", $label = "")
	{
		$this->label = ($label) ? $label : prettify($field);
		
		$this->FieldRenderer($parent);
		if ($parent->data->hasField($field))
		{
			$parent->override($field, $label, $this);
		}
		else
		{
			$parent->add($this, $field);
			$parent->overrides[$field]['label'] = $this->label;
		}

		$this->parent->search->setParam($field, "equal", $this->parent->data->get($field));
	}
	
	/*
	 * When the checkbox is checked or unchecked,
	 * update the value of the hidden field.
	 */
	function renderSearchScript($field)
	{
		$selChanged = "selChanged_" . $field . "_box";

?>
<script type="text/javascript">

function <? echo $selChanged ?>(elt)
{
  	var value_field = $('<?php echo $field ?>');
  	var new_value = "0";
  
  	if(value_field && elt)
  	{
 	  	if(elt.checked == true)
  	  		new_value = "1";
   	 	
   	  	value_field.set("value", new_value);
  	}
	elt.form.submit();
}
</script>
<?	
	}
	
	function renderSearchField($field)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		echo "<input type='checkbox' value='1' name='{$field}_box'";

		$value = $this->parent->data->get($field);

		if(!$value && !is_numeric($value))
			$value = $this->default;
	
		if ($value)
		{
			echo " checked";
		}
		echo " onchange='selChanged_{$field}_box(this);'\">&nbsp;\n";
				
		if(!$value) $value = 0;
		echo "<input id='{$field}' type='hidden' name='{$field}' value='$value'/>\n";
		echo $label;
	}
}

class RadioButtonFilterFieldRenderer extends RadioButtonFieldRenderer
{
	function RadioButtonFieldRenderer(&$parent, $field, $label, $options)
	{
		parent::RadioButtonFieldRenderer($parent, $field, $label, $options);
	}
	
	function renderSearchField($field)
	{
		$onchange = " onchange='this.form.submit();'";
		
		$searchValue = $this->parent->data->get($field);
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td>\n";
		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue == $value) ? " checked" : "";
			echo "<input style='border: none' type='radio' name='$field' value='$value'$selected$onchange>$name</option>";
			echo ($this->horizontal) ? "&nbsp;&nbsp;" : "<br/>";
		}
		echo "</td></tr>\n";
		
	}
}

class DateFilterFieldRenderer extends DateFieldRenderer
{
	var $mode;
	
	function DateFilterFieldRenderer(&$form, $field, $label, $mode)
	{
		$this->mode = $mode;
		$this->FieldRenderer($form);
		
		$this->label = ($label) ? $label : prettify($field);
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
		}
		
		$form->search->setParam($field, $mode, $form->data->$field);
		$form->modifiers[$field] = $mode;
	}	
	
	function renderSearchScript($field, $mode)
	{
		$script = "";
		
		if (!$this->_includedDateScript)
		{
			echo "<script src='{$this->parent->componentPath}/calendar/sonjara_calendar.js' type='text/javascript'></script>\n";
			echo "<link href='{$this->parent->componentPath}/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'>\n";
			$this->_includedDateScript = true;
		}
		
		if ($mode == "range")
		{
			$calendar_from = "{$field}_calendar_from";
			$calendar_to = "{$field}_calendar_to";
			
			echo "<script type='text/javascript'>\n";
			echo "var $calendar_from = new Calendar('$calendar_from', '{$this->parent->id}', '$field:from');\n";
			echo "var $calendar_to = new Calendar('$calendar_to', '{$this->parent->id}', '$field:to');\n";
			echo "</script>\n";
		}
		else
		{
			$calendar = "{$field}_calendar_{$mode}";
			echo "<script type='text/javascript'>\n";
			echo "var $calendar = new Calendar('$calendar', '{$this->parent->id}', '$field:$mode');\n";
			echo "</script>\n";
		}
	}
	
	
	function renderField($field, $mode)
	{
		$onchange = "onchange='this.form.submit()'";
		
		if ($mode == "range")
		{
			$date_from = $this->parent->search->get($field, "from");
			$date_to = $this->parent->search->get($field, "to");
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>";
			echo "<input type='text' name='$field:from' value='$date_from' size='12' $onchange>&nbsp;
			<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_from.toggle(this)' $onchange>";
			echo " to <input type='text' name='$field:to' value='$date_to' size='12' $onchange>&nbsp;
			<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_to.toggle(this)' $onchange></td>\n";
			echo "</td>\n";
			echo "</tr>\n";	
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>";
			echo "<input type='text' name='$field:$mode' value='$date' size='12' $onchange>&nbsp;
			<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_{$mode}.toggle(this)' $onchange></td>\n";
			echo "</td>\n";
			echo "</tr>\n";			
		}
	}
	
	function renderSearchField($field)
	{
		$this->renderField($field, $this->mode);
	}	
}

?>