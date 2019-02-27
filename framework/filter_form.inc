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

require_once realpath(dirname(__FILE__)."/search_form.inc");
require_once realpath(dirname(__FILE__)."/auto_form_layout.inc");
require_once realpath(dirname(__FILE__)."/field_renderers/select_field_renderer.inc");
require_once realpath(dirname(__FILE__)."/field_renderers/date_field_renderer.inc");
require_once realpath(dirname(__FILE__)."/field_renderers/boolean_field_renderer.inc");
require_once realpath(dirname(__FILE__)."/field_renderers/checklist_field_renderer.inc");


/**
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
	var $preserveQueryString = false;
 	var $navigationFunction = "go('{url}')";	///< Format specifier for the default javascript navigation function to be used by fields in the FilterForm
 	var $navigationMode = "form"; 				///< Mode used for navigating after filter changed - either "form" or "panel"
 	var $showSubmitButton = false;				///< Whether to show the submit button on a filter form
 	var $submitButtonLabel = "Filter Results";	///< Text to show on the filter form submit button, if present
 	var $formLabel = null;						///< Text to display before the start of the form (such as 'Filter By')
 	var $rememberFilter = false;					///< Set to true to remember the chosen filter parameters and re-apply them if none are set (when returning to a page)
 	
	function FilterForm($target, $method="GET", $action="", $id="")
	{
		parent::SearchForm($target, $method, $action, $id);
		$this->layout = new FilterFormLayout($this);
		$this->formCSS = "filter";
		$this->setFilterParameters();
	}
		
	function getLink($url)
	{
		return str_replace("{url}", $url, $this->navigationFunction);
	}
		
	/**
	 * Sets the navigation mode for the FilterForm. 
	 * @param string $mode Use 'form' for page reloads, or 'panel' for AJAX panel reloads
	 */
	function setNavigationMode($mode)
	{
		$this->navigationMode = $mode;
		if ($mode == "panel")
		{
			$this->navigationFunction = "loadPanel(form, '{url}');";
			$this->onSubmitHandler = "{$this->id}_loadPanel";
			$this->action = Fakoli::scriptName();
		}
	}
	
	function getOnSubmitFunction()
	{
		return "onSubmit{$this->id}";
	}
	
	function writeScript()
	{
		if ($this->navigationMode == "panel")
		{
			$script = <<<ENDSCRIPT
<script type="text/javascript">
function {$this->id}_loadPanel(form)
{
	var form = new Element(form);
	var url = form.action + "?" + form.toQueryString();
	form.loadPanel(url);
	return false;
}
</script>		
ENDSCRIPT;
		}
		return $script . parent::writeScript();	
	}
		
	private function sessionKey()
	{
		$base = preg_replace("/\\?.*/", "", $_SERVER["REQUEST_URI"]);
		return "filter:{$this->id}:{$base}";
	}
	
	private function loadFilterFromSession()
	{
		if (isset($_SESSION[$this->sessionKey()]))
		{
			$this->params->copy(unserialize($_SESSION[$this->sessionKey()]));
			$this->data =& $this->params->target;
		}
	}
	
	private function saveFilterToSession()
	{
		if (!$this->params->isDefaultState)
		{
			$_SESSION[$this->sessionKey()] = serialize($this->params);
		}
		else
		{
			unset($_SESSION[$this->sessionKey()]);
		}
	}
	
	function drawForm()
	{
		$obj =& $this->data;
		$pk = $obj->getPrimaryKey();
		if ($this->preserveQueryString)
		{
			$obj->fromREQUEST();
		}

// 		echo "<p>Empty: {$this->params->empty}</p>";
// 		echo "<p>Default State: {$this->params->isDefaultState}</p>";
// 		echo "<p>".$this->sessionKey()."</p>";
// 		echo "<pre>".$_SESSION[$this->sessionKey()]."</pre>";
		echo "<form id='{$this->id}' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
		if (!$this->ajaxSubmitHandler) 
		{
			echo " onsubmit='return onSubmit{$this->id}(this);'";
		}
		
		if ($this->navigationMode == 'panel')
		{
			echo " data-mode='ajax'";
		}
		echo ">\n";
		
		echo "<div";
 		if ($this->formCSS) echo " class='{$this->formCSS}'";
		if ($this->style) echo " style='{$this->style}'";
		echo ">\n";
		
		foreach(array_keys($this->hidden) as $hidden)
		{
			if ($obj->filter && $obj->filter->isExcluded($hidden)) continue;
			
			echo "<input id='{$this->id}_{$hidden}' type='hidden' name='$hidden' value='".htmlSafe($obj->get($hidden))."'/>\n";
		}
		
		if ($this->preserveQueryString)
		{
			foreach($_GET as $key => $value)
			{
				if ($key == "identifier" || $key == "section" || $key == "submit" || array_key_exists($this->hidden, $key) || $obj->hasField($key)) continue;
				echo "<input id='{$this->id}_{$key}' type='hidden' name='$key' value='".htmlSafe($value)."'/>\n";
			}
		}

		$this->layout->startUngrouped();
		
		if ($this->formLabel) echo "<div class='filter_form_label'><label>{$this->formLabel}</label></div>";
		$this->renderSearchFields();
		if ($this->showSubmitButton)
		{
			echo "<input type='submit' class='button' value='{$this->submitButtonLabel}'/>";
		}
		else
		{
			// Hidden submit button allow filter to be run by hitting enter
			echo "<input type='submit' style='display: none'/>";
		}
		
		$this->layout->endUngrouped();
		
		$this->layout->finalizeLayout();
		echo "</div>\n";
		echo "</form>\n";
		echo "<div style=\"clear: both\">\n</div>\n";
	}
	
	/*
	 * If the form contains a DateRangeFilterRenderer,
	 * it must be rendered last.
	 */
	function renderSearchFields()
	{
		$obj =& $this->data;
		$filter = $obj->getFilter();
		
		if (count($this->groups) > 0)
		{
			$this->layout->endUngrouped();
			
			foreach($this->groups as $legend => $fields)
			{
				$collapsible = array_key_exists($legend, $this->collapsibleGroups);
				if ($collapsible)
				{
					$cf = $this->collapsibleGroups[$legend];
					$collapsed = !$this->data->$cf;
				}
				
				$this->layout->startGroup($legend, $collapsible, $collapsed);
				
				foreach($fields as $field)
				{
					if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
					{
						$this->renderOneSearchField($field);
					}					
				}
				
				$this->layout->endGroup();
			}
			
			$this->layout->startUngrouped();
			
		}
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if(get_class($renderer) == DateRangeFilterRenderer)
				{
					$dateRangeField = $field;
					continue;
				}	
				$this->renderOneSearchField($field);
			}
		}
		
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
			if (!array_key_exists($field, $this->renderedFields))
			{
				if(get_class($renderer) == DateRangeFilterRenderer)
				{
					$dateRangeField = $field;
					continue;
				}	
				$this->renderOneSearchField($field);
			}	
		}
				
		if($dateRangeField)
		{
			$this->renderOneSearchField($dateRangeField);
		}
	}

	/**
	 * Override an existing SearchParameter constraint
	 * clause handler or add one. 
	 * 
	 * @handler - callback function that creates the
	 * constraint clause for the given field
	 */
	function setHandler($field, $handler)
	{
		$this->params->paramHandlers[$field] = $handler;
	}
	
	
	/**
	 * Default for all parameters included in filter is equal
	 * Additional search parameters can be added by calling script with
	 * the syntax:
	 * $filterForm->params->setParam("round_id", "member", implode(",", array_keys($rounds)));
	 * Do not include these additional parameters in the object filter.
	 */
	function setFilterParameters()
	{
		$obj = $this->data;
		
		// JDG 9/2011 handle CompositeDataItems
		$fields = $obj->getFields();
		$this->params = new SearchParameters($obj);

		foreach(array_keys($fields) as $field)
		{
			if (!array_key_exists($field, $this->hidden) &&
				!($obj->filter && $obj->filter->isExcluded($field))
				)
			{
				$value = $obj->get($field);
				// AJG - Removing this logic as it is incorrectly re-encoding parameters leading to bad side-effects
				// Application code that breaks as a result of this should be fixed in the application
				//if($value && !is_int($value))
				//	$value = urlencode($value);
				if($value)
					$this->params->setParam($field, "equal", $value);
			}
		}
	}
	
	function getConstraint($first = true, $firstText = "WHERE")
	{
		if ($this->rememberFilter)
		{
			if ($this->params->isDefaultState)
			{
				$this->loadFilterFromSession();
			}
			else
			{
				$this->saveFilterToSession();
			}
		}
		
		return $this->params->generateConstraint($first, $firstText);
	}
	
	function setParam($field, $mode, $value)
	{
		$this->params->setParam($field, $mode, $value);
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
	var $width = "";
	var $cssClass = 'select';
	
	function FilterFieldRenderer(&$form, $field, $label = "", $options = null)
	{
		$this->options = $options;
		$this->label = ($label) ? $label : prettify($field);
		$this->FieldRenderer($form);
		if ($form->getData()->hasField($field))
		{
			// JDG 5/30/11 - don't crash on additiona/psuedo fields
			// only query if real field
			$form->override($field, $label, $this);
			if ($this->options == null)
			{
				$opt = $form->getData()->distinctValues($field, true);
				if (count($opt) > 0)
				{
					$this->options = array_combine($opt, $opt);
				}
				else
				{
					$this->options = array();
				}
			}
		}
		else
		{
			$form->add($this, $field);
			// JDG 7/22/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}
	}
	
	function setDefault($title, $value)
	{
		$this->default = $title;
		$this->defaultValue = $value;
	}
	
	function renderField($field)
	{
		$this->_startField($field, $this->styles);
		
		if($this->width)
		{
			$styles .= "style='width: {$this->width}'";
		}
			
		$onSubmit = $this->parent->getOnSubmitFunction();
		echo "<select name='{$field}' {$styles} onchange='if ({$onSubmit}(this.form)) this.form.manager.submit();'>";
    	
    	if ($this->default)
    	{
   ?>
		<option value="<?php echo $this->defaultValue ?>"><?echo $this->default?></option>     
   <?
    	}
    	
    	if($this->all)
    	{
    	?>
		<option value="<?php echo $this->all_value ?>">All</option>  
		<?
    	}
    	
     	$fieldValue = $this->parent->data->get("$field");
     	foreach($this->options as $value => $name)
		{
			$name = htmlSafe($name);
			$trunced = $this->max_chars ? ellipsis($name, $this->max_chars, true) : $name;
		
  			$selected = (!strcmp($value, $fieldValue)) ? " selected" : "";
  			$optionTitle = ($name != $trunced) ? " title='$name'" : "";
  			
			echo "<option value='{$value}' $selected>$trunced</option>\n";
		}
		
		echo "</select>&nbsp;";
  		
  		$this->_endField($field);
  	}
	
	function renderSearchField($field)
	{
		$this->renderField($field);
	}
	
	/*
	 * Given an array of dataitem objects, return
	 * an array of options with the key the primary
	 * key of the obj and the name field the param given.
	 */
	static function formatOptions($items, $template)
	{
		$options = array();

		// user sent just the field name
		if(!preg_match("/^\{/", $template))
			$template = "{" . $template . "}";
			
		if(count($items) == 0)
			return $options;
			
		foreach($items as $item)
		{
			$pk = $item->getPrimaryKey();
			$options[$item->$pk] = $item->format("$template");
		}
		return $options;
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
	$unbilledCheckbox = new BooleanFilterFieldRenderer($filterForm, "ledger_id", "Show Unbilled Only", 1);
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
	var $cssClass = "boolean";
	var $default;
	
	/**
	 * 
	 * @param FilterForm $parent the parent FilterForm
	 * @param string $field the name of the field
	 * @param string $label label to display for the field
	 * @param bool $default whether the checkbox should be check or unchecked as the default
	 */
	function BooleanFilterFieldRenderer(&$parent, $field = "", $label = "", $default = 0)
	{
		$this->label = ($label) ? $label : prettify($field);
	
		$this->default = $default;
		
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

		$value = $this->getValue($field);
		// The search parameter value is used for the constraint
		$this->parent->params->clearParam($field);
		$this->parent->params->setParam($field, "checked", $value);
	}
	
	private function getValue($field)
	{
		if ($this->parent->data->hasField($field))
		{
			$value = $this->parent->data->get($field);
			if(!$value && !is_numeric($value))
			{
				return  $this->default;
			}
			else
			{
				return $value;
			}
		}
		
		if (isset($_REQUEST[$field]))
		{
			return checkNumeric($_REQUEST[$field]);
		}
		
		return $this->default;
	}
	
	/*
	 * When the checkbox is checked or unchecked,
	 * update the value of the hidden field.
	 */
	function renderSearchScript($field)
	{
		$selChanged = "selChanged_" . $field . "_box";
		$onSubmit = $this->parent->getOnSubmitFunction();
?>
<script type="text/javascript">

function <? echo $selChanged ?>(elt)
{
  	var value_field = document.id('<?php echo $field ?>');
  	var new_value = "0";

  	if(value_field && elt)
  	{
 	  	if(elt.checked == true)
  	  		new_value = "1";
   	 	
   	  	value_field.set("value", new_value);
  	}
  	if (<?php echo $onSubmit?>(elt.form)) elt.form.manager.submit();
}
</script>
<?	
	}
	
	function renderSearchField($field)
	{
		$this->_startField($field, $this->styles);
			
		echo "<input type='checkbox' value='1' name='{$field}_box'";

		$value = $this->getValue($field);
	
		if ($value)
		{
			echo " checked='checked'";
		}
		echo " onclick='selChanged_{$field}_box(this);'/>\n";
				
		if(!$value) $value = 0;
		echo "<input id='{$field}' type='hidden' name='{$field}' value='$value'/>\n";
		echo "&nbsp;&nbsp;<label for='{$field}'>{$label}</label>";
		
		$this->_endField($field);
	}
}

/**
 * 
 * Usage:
 * 
 * Displays a horizontal set of checkboxes with optional default values
 * checked. Constraint is obj's field value must be a member of the
 * set of checked boxes.
 * 
 * ex:
 * 	$statusChecklist = new CheckListFilterFieldRenderer($filterForm, "status", "Status", Task::$taskStatusOptions, "Not Started Yet,In Progress,Internally Completed");		
 * 
 * @author janice
 *
 */
class CheckListFilterFieldRenderer extends CheckListFieldRenderer
{
	var $cssClass = 'checklist';
	
	/**
	 * 
	 * @param FilterForm $form
	 * @param string $field
	 * @param string $label
	 * @param array $options - array of checklist options
	 * @param string $defaults - list of defaults to be checked in comma delimited string
	 */
	function CheckListFilterFieldRenderer(&$form, $field, $label, $options, $defaults = "")
	{
		parent::CheckListFieldRenderer($form, $field, $label, $options);
		$this->horizontal = true;
		
		$value = $this->parent->data->get($field);
		if(!$value && !is_numeric($value) && ((!isset($_GET["{$field}_submitted"]) || !$_GET["{$field}_submitted"])) && $defaults)
		{
			$defaults = preg_replace("/'/", "", $defaults);
			$defaults = array_flip(explode(",", preg_replace("/,\s*/", ",", $defaults)));
			
			// The form's data value is used to set the selected checkboxes and cannot contain the "'" around the string values
			$this->parent->data->set($field, $defaults);
			
			if(!is_numeric(array_flip($defaults[0])))
			{
				$value = "'" . implode("','", array_keys($defaults)) . "'";
			}
			else
			{
				$value = implode(",", array_keys($defaults));
			}
		}
				
		//AJG - Make sure there is no pre-existing parameter that would cause a duplicate
		$this->parent->params->clearParam($field);
		// The search parameter's value is used in the constraint and must contain "'" around each string value
		$this->parent->params->setParam($field, "member", $value);
	}
	
	/*
	 * Draws a horizontal line of checkboxes.
	 */
	function renderSearchField($field)
	{
		$this->_startField($field, $this->styles);
		
		$searchValues = $this->parent->data->get($field);
		
		if(is_array($searchValues))
		{
			$searchValues = explode(",", implode(",", array_keys($searchValues)));
		}
		else
		{
			$searchValues = array();
		}

		$idx = 0;

		$onSubmit = $this->parent->getOnSubmitFunction();
		if(count($this->options) > 0)
		{
			foreach($this->options as $value => $text)
			{
				trace("CheckListFieldRenderer value $value and text $text", 3);
				echo "<li>\n";
				$onchange = " onchange='if ({$onSubmitFunction}(this.form)) this.form.manager.submit();'";
				$checked = (array_search($value, $searchValues) !== FALSE) ? " checked" : "";
				echo "<input type='checkbox' class='checkbox' name='{$field}[$value]' value='1'$checked$onchange/>&nbsp;$text$divider";
				$idx++;
				echo "</li>\n";
			}
			
			/**
			 * We need to use this hidden field to distinguish between when 
			 * this field is not set in $_GET because the page has just loaded
			 * and no values because the users specifically chose
			 * to deselect any defaults.
			 */
			echo "<input type='hidden' name='{$field}_submitted' value='1' />";
		}

		$this->_endField($field);	
	}
	
	function _startField($field, $styles = "")
	{
		echo "<div class='{$this->cssClass}'>\n";
		echo "<ul>\n";		
		echo "<li>\n";
		$this->parent->layout->printLabel($field, $this, $styles, "");
		echo "</li>\n";	
	}
	
	function _endField($field, $styles = "")
	{
		echo "</ul>\n";
		$this->parent->layout->endField($field, $this);	
		echo "<div style='clear: both'></div>\n";
	}
}

class RadioButtonFilterFieldRenderer extends RadioButtonFieldRenderer
{
	var $cssClass = "radio";
	
	function RadioButtonFieldRenderer(&$parent, $field, $label, $options)
	{
		parent::RadioButtonFieldRenderer($parent, $field, $label, $options);
		$this->horizontal = true;
	}
	
	function renderSearchField($field)
	{
		$this->_startField($field, $this->styles);
		
		$onSubmit = $this->parent->getOnSubmitFunction();
		$onchange = " onchange='if ({$onSubmit}(this.form)) this.form.manager.submit();'";		
		
		$searchValue = $this->parent->data->get($field);

		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue == $value) ? " checked" : "";
			echo "<input style='border: none' type='radio' name='$field' value='$value'$selected$onchange>$name</option>";
			echo ($this->horizontal) ? "&nbsp;&nbsp;" : "<br/>";
		}
		
		$this->_endField($field);
	}

	function _startField($field, $styles = "")
	{
		echo "<div class='{$this->cssClass}'>\n";
		$this->parent->layout->printLabel($field, $this, $styles, "");
		echo "<ul>\n";		
		
	}

	function _endField($field, $styles = "")
	{
		echo "</ul>\n";
		$this->parent->layout->endField($field, $this);	
		echo "<div style='clear: both'></div>\n";
	}	
}

/**
 * Adds a Date Range Filter with start and end dates
 * and Next and Previous buttons to a FilterForm.
 * 
 * Usage: first create the instance of FilterForm,
 * including in the DataItem object filter any other
 * fields that you want to filter on (e.g., status drop 
 * down)
 * 
 * 	$filterForm = new FilterForm($obj);
 *
 * Then add the date range renderer to the form:
 * 
 *  $dateRangeRenderer = new DateRangeFilterRenderer($filterForm, "start_date_field", "end_date_field", "month");
 *  
 *  @param - filterForm - instand of class FilterForm
 *  
 *  @param - start_date_field - the field in the DataItem obj
 *  sent to the FilterForm that is the date to be queried on
 *  (e.g., order_date)
 *  
 *  @param - end_date_field - the field in the DataItem obj sent
 *  to the FilterForm that is the end date to be queried on. If
 *  the query involves just one field, send the same field name
 *  for both. Example where field names are different:
 *  	program_start_date and program_end_date
 *  
 *  @param - stepMode - the increment for the Next and Previous
 *  buttons. Default is "month", can be "year" or "fiscal_year"
 *  
 *  @param - fiscal_year_start - the numeric starting month
 *  of the fiscal_year, if the step mode is fiscal_year.
 *  May be stored in Settings - varies among web sites
 *  so is not queried directly here.
 *  
 *  Retrieve the date range constraint through FilterForm instance
 *  $constraint = $filterForm->getConstraint();
 *  
 *  To show only the Prev/Next buttons and not the
 *  start/end date input fields, hide the startDate
 *  field
 *  
 *  $filterForm->hide("startDate");
 *   
 * @author: Andy Green
 */
class DateRangeFilterRenderer extends FieldRenderer
{
	var $startParam = "startDate";
	var $endParam = "endDate";
	var $stepMode; // month, year or fiscal_year
	var $fiscal_year_start = 1; // month number on which fiscal year begins
	
	var $startDate; // the start date for the filter field
	var $endDate; // the end date for the filter field
	
	// the name of the date fields in the table to be searched
	// if no endDateField, then search will be field BETWEEN startDate and endDate
	var $field; // start date field
	var $endDateField;
	
	var $stepLabel; // e.g., Next/Previous Month 
	var $dateRangeLabel = "Reporting Period";
	var $subTitle;			// Calling code can optionally output a text description of the date range by calling this renderer through the form.
	
	var $handler; // custom parameter handler if override to getConstraint needed
	var $cssClass = "date_range";
	
	function __construct($parent, $field, $endDateField = "", $stepMode = "month", $fiscal_year_start = 1, $startParam = null, $endParam = null)
	{
		$this->stepMode = $stepMode;
		$this->field = $field;

		$this->endDateField = (!$endDateField) ? $field : $endDateField;
		$this->fiscal_year_start = $fiscal_year_start;
		
		if ($startParam) $this->startParam = $startParam;
		if ($endParam) $this->endParam = $endParam;
		
		$this->calculateDateRange();
		
		$this->label = "";
		if ($parent->data->hasField($field))
		{
			$parent->override($field, $label, $this);
		}
		else
		{
			$parent->add($this, $field);
		}		
		$this->FieldRenderer($parent);
		
		// Needs to have a value so that it is included in the constraint
		$parent->setParam($this->startParam, "to", $this->to);
		// override default search constraint generation
		$parent->setHandler($this->startParam, array($this, getConstraint));
	}
	
	function calculateDateRange()
	{
		$this->startDate = $_REQUEST[$this->startParam];
		$this->endDate   = $_REQUEST[$this->endParam];
		
		if ($this->startDate || $this->endDate)
		{
			$validator = new DateValidator($this->startParam, "From");
			$validator->required = false;
			$this->msg = $validator->validate();
			
			if ($this->msg) 
			{
				$startDate = null;
			}
			
			$validator = new DateValidator($this->endParam, "To");
			$validator->required = false;
			
			$this->msg = $validator->validate();
		
			if ($this->msg)
			{
				$endDate = null;
			}
		
			if ($this->startDate) $this->startDate = new DateTime($this->startDate);
			if ($this->endDate) $this->endDate = new DateTime($this->endDate);
			
			if ($this->startDate && $this->endDate) 
			{
				$this->subTitle = $this->startDate->format("jS F Y")." to ".$this->endDate->format("jS F Y");
			}
		
			// JDG 2/12 - remove "inclusive of last day"
	
			// JDG 11/2011 - Need to set this for stepping with Prev Next
			$this->month = $this->startDate->format("m");
			$this->year = $this->startDate->format("Y");
		}
	
		$this->month = checkNumeric($_REQUEST['month']);
		$this->year = checkNumeric($_REQUEST['year']);

		if (!$this->year) $this->year = date("Y");

		// JDG 11/2011 - need to set step mode whether we have a 
		// custom start/end date or not
		if ($this->stepMode == "year")
		{
			// calendar year stepping mode
			$this->setStepModeYear();
		}
		/*
		 * If today's month for this year is less than
		 * the fiscal year month, then show last year's
		 * data
		 */
		elseif($this->stepMode == "fiscal_year")
		{
			if(!checkNumeric($_REQUEST['year']) && $this->fiscal_year_start > date("n"))
				$this->year--;
			$this->setStepModeYear($this->fiscal_year_start);
		}
		else
		{
			// Monthly stepping mode
			$this->setStepModeMonth();
		}

		$this->from = $this->startDate->format("Y-m-d");
		$this->to = $this->endDate->format("Y-m-d");
		$this->calculated = true;
	}
	
	/*
	 * The fiscal year in settings is stored as a string;
	 * use this function to convert to a number.
	 */
	static function convertMonthStringToNumber($month)
	{
		for($i=1;$i<=12;$i++)
		{
			if(date("F", mktime(0, 0, 0, $i, 1, 0)) == $month)
			{
				$month_number = $i;
				break;
			}
		}
		return $month_number;
	}
	
	function setStepModeMonth()
	{
		if (!$this->month)
		{
			$this->year = date("Y");
			$this->month = date("m");
		}
				
		$this->prevMonth = $this->month - 1;
		$this->prevYear = $this->year;
		if ($this->prevMonth == 0)
		{
			$this->prevYear--;
			$this->prevMonth = 12;
		}
				
		$this->nextMonth = $this->month + 1;
		$this->nextYear = $this->year;
		if ($this->nextMonth > 12)
		{
			$this->nextMonth = 1;
			$this->nextYear++;
		}
				
		if(!$this->startDate)
			$this->startDate = new DateTime("{$this->year}-{$this->month}-01T00:00:00");
			
		if(!$this->endDate)
		{
			$this->endDate = clone $this->startDate;
			$this->endDate->modify("+1 months");
			// JDG 2/12 - show last day of month 
			$this->endDate->modify("-1 days");
		}

		if(!$this->subTitle)
		{
			$this->subTitle = $this->startDate->format("F Y");	
		}
	}
	
	function setStepModeYear($startMonth = 1)
	{
		$this->month = $startMonth;
				
		$this->prevMonth = $startMonth;
		$this->prevYear = $this->year - 1;
				
		$this->nextMonth = $startMonth;
		$this->nextYear = $this->year + 1;
		$this->subTitle=  $this->year;
		
		if(!$this->startDate)
		{
			$this->startDate = new DateTime("{$this->year}-{$this->month}-01T00:00:00");
		}	
		
		if(!$this->endDate)
		{
			$this->endDate = clone $this->startDate;
			$this->endDate->modify("+1 years");
			$this->endDate->modify("-1 days");
		}		
	}
	
	function getConstraint()
	{
		if(is_array($this->handler) && method_exists($this->handler[0], $this->handler[1]))
		{
			return call_user_func($this->handler, $this);
		}
		
		//AJG: Cope with date rounding
		$to = $this->to . " 23:59:59.999";
		
		if($this->field == $this->endDateField)
		{
			$name = $this->field;
			$constraint = "$name BETWEEN '{$this->from}' AND '{$to}'";
		}
		else
		{
			$constraint = "{$this->endDateField} <= '{$to}' AND {$this->field} >= '{$this->from}' ";
		}
	
		return $constraint;
	}
	
	function setHandler($handler)
	{
		$this->handler = $handler;
	}
	
	function renderSearchScript($field)
	{
		if (array_key_exists($field, $this->parent->hidden)) return "";
		
		$script .= <<<ENDSCRIPT

<script src='/fakoli/calendar/sonjara_calendar.js' type='text/javascript'></script>
<link href='/fakoli/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'></link>
<script type="text/javascript">
var startDateCalendar = new Calendar('startDateCalendar', '{$this->parent->id}', '{$this->startParam}');
var endDateCalendar = new Calendar('endDateCalendar', '{$this->parent->id}', '{$this->endParam}');
</script>

ENDSCRIPT;
		
		echo $script;
	}
	
	/**
	 * If the user hides the startParm field, then don't display
	 * the calendar picker and input fields for dates.
	 * 
	 * If we don't display the input fields for start and end dates,
	 * then write them as hidden fields so the values are in
	 * $_GET.
	 */
	function renderSearchField($field)
	{

		$this->_startField($field);
		
		if (!array_key_exists($this->startParam, $this->parent->hidden)) 
		{
			$this->writeFilter();
		}
		else
		{
			$this->writeDateHiddenFields();
		}
		
		$this->writeButtons();
		
		$this->_endField($field);
	}
	
	function writeDateHiddenFields()
	{
		if(!$_REQUEST["month"] || !$_REQUEST["year"])
		{
		?>
		<input type="hidden" name="<?php echo $this->startParam ?>" value="<?echo $this->startDate->format("m/d/Y")?>">
		<input type="hidden" name="<?php echo $this->endParam ?>" value="<?echo $this->endDate->format("m/d/Y")?>">
		<?php
		}
		else
		{
		?>
		<input type="hidden" name="year" value="<? echo $this->year ?>">
		<input type="hidden" name="month" value="<? echo $this->month ?>">
		<?php	
		}
	}

	function writeFilter()
	{
		if (!$this->calculated) $this->calculateDateRange();
		$endDate = clone $this->endDate;

?>
<div style="clear: both; float: left; padding-bottom: 4px">
 <label><?php echo $this->dateRangeLabel ?> From: </label>
 <input type="text" size="10" name="<?php echo $this->startParam ?>" value="<?echo $this->startDate->format("m/d/Y")?>">&nbsp;
 <img src='/fakoli/calendar/calendar.gif' alt='Popup Calendar' style='vertical-align: middle; border:0' onclick='startDateCalendar.toggle(this)'/>
 <label> To: </label><input type="text" size="10" name="<?php echo $this->endParam ?>" value="<?echo $endDate->format("m/d/Y")?>">&nbsp;
 <img src='/fakoli/calendar/calendar.gif' alt='Popup Calendar' style='vertical-align: middle; border:0' onclick='endDateCalendar.toggle(this)'/>
 &nbsp;<input type="submit" class="button" value=" Update View "/>
 </div>
<?
	}
	
	function writeButtons()
	{
		if ($this->stepMode == "none") return;
		
		if (!$this->calculated) $this->calculateDateRange();
		
		$queryString = "?".$_SERVER["QUERY_STRING"];
		
		$queryString = preg_replace("/identifier=\\w+\\&?/", "", $queryString);
		$queryString = preg_replace("/year=\\d+\\&?/", "", $queryString);
		$queryString = preg_replace("/month=\\d+\\&?/", "", $queryString);
		$queryString = preg_replace("/startDate=\\d+\\&?/", "", $queryString);
		$queryString = preg_replace("/endDate=\\d+\\&?/", "", $queryString);
		
		$prevLink = appendToQueryString($queryString, "year=$this->prevYear&month=$this->prevMonth");
		$nextLink = appendToQueryString($queryString, "year=$this->nextYear&month=$this->nextMonth");
		
		$stepLabel = ($this->stepLabel) ? $this->stepLabel : prettify($this->stepMode);
?>
<div class="<?php echo $this->cssClass ?>">
<div style="float: right; z-index:2; padding-bottom: 10px;">
<input type="button" class="button" value="  Next <?echo $stepLabel ?> &raquo;  " onclick="go('<?echo $nextLink?>');">
</div>
<input type="button" class="button" value="  &laquo; Previous <?echo $stepLabel ?>   " onclick="go('<?echo $prevLink?>');">
</div>
<?
	}
}


class ProgressiveSearchFilterFieldRenderer extends FieldRenderer
{
	var $cssClass = 'prog_search';
	var $searchOptions = "";
	var $size = 12;
	
	/**
	 * 
	 * 
	 * @param $parent
	 * @param $field
	 * @param $label
	 * @param $searchOptions - ProgressiveSearch options, e.g., 
	 * 'search': '/action/task/task_search_handler?$qs', minimumLength: 1, 'cssClass': 'scrollbox', 'width': '300px'
	 */
	function ProgressiveSearchFilterFieldRenderer(&$parent, $field, $label = "", $searchOptions)
	{
		$this->FieldRenderer($parent);
		if ($field && !$parent->data->hasField($field))
		{
			$parent->add($this, $field);
			$parent->overrides[$field]['label'] = $label;
		}
		else if($label)
		{
			$parent->override($field, $label, $this);	
		}
		
		$this->searchOptions = $searchOptions;
	}
	
	function renderSearchField($field)
	{
		$this->_startField($field, $this->styles);
				
		echo "<input id='$field' type='text' name='$field' 
		value='".$this->parent->data->get($field)."' autocomplete='off' 
		size='{$this->size}'/>";
		
		$this->_endField($field);
	}
	
	function renderSearchScript($field)
	{
?>
<script type="text/javascript">
window.addEvent('domready', function()
{
	new ProgressiveSearch('<?php echo $field ?>', <?php echo $this->searchOptions ?>);
});
</script>
		<?		
	}
	
	
	function renderField($field)
	{
		$this->renderSearchField($field);
	}
}

class RelatedItemSelectFilterFieldRenderer extends RelatedItemSelectFieldRenderer
{
	function __construct(&$form, $field, $label, $relatedClass, $constraint, $nameField, $valueField = "", $maxChars = 80)
	{	
		$this->RelatedItemSelectFieldRenderer($form, $field, $label, $relatedClass, $constraint, $nameField, $valueField = "", false, true, $maxChars);
	}	
	
	
	function renderSearchField($field)
	{
		$onSubmit = $this->parent->getOnSubmitFunction();
		$onchange = " onchange='if ({$onSubmit}(this.form)) this.form.manager.submit();'";
		
		$this->getRelatedItems();
		
		if($this->width)
			$style = "style='width: {$this->width}'";
		
		echo "<div>\n";
		$this->_printLabel($field);
		echo "<select name='{$field}'$onchange $style>\n";
		//trace("valueField: $valueField", 3);
		
		$value = $this->parent->data->get($field);
		
		foreach($this->items as $item)
		{
			$valueField = ($this->valueField != "") ? $this->valueField : $item->getPrimaryKey();
			$name = $this->formatName($item, $this->nameField);

			echo "<option value='{$item->get($valueField)}'";
			if ($item->get($valueField) == $value) echo " selected";
			echo ">".ellipsis($name, $this->max_chars)."</option>\n";			
		}
		echo "</select>&nbsp;";
		echo "</div>";
	}
}
?>