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

require_once realpath(dirname(__FILE__)."/auto_form.inc");

/**
 * Base class for SearchParameterHandlers. Each derived SearchParameterHandler should extend
 * this base class and implement the type-dependent behaviors appropriate for the type being handled.
 * 
 * SearchParameterHandlers are responsible for generating the query constraint expressions 
 * that provide appropriate matching for the search parameters being used to build a query
 * from information entered into a SearchForm.
 * 
 * @author andy
 *
 */
class SearchParameterHandler
{
	/**
	 * Generates the constraint expression for equality matching parameters.
	 * @param $name the name of the field
	 * @param $value the value to match
	 * @return string a SQL expression to perform the match
	 */
	function equal($name, $value)
	{
	}
	
	/**
	 * Generates the constraint expression for 'like' matching parameters, i.e.
	 * matching a substring or partial match.
	 * @param $name the name of the field
	 * @param $value the value to match
	 * @return string a SQL expression to perform the match
	 */
	function like($name, $value)
	{
	}
	
	/**
	 * Generates the constraint expression for 'startsWith' matching parameters, i.e.
	 * matching a substring or partial match at the start of the field only.
	 * @param $name the name of the field
	 * @param $value the value to match
	 * @return string a SQL expression to perform the match
	 */	
	function startsWith($name, $value)
	{
			
	}
	
	/**
	 * Generates the constraint expression for 'from' matching parameters, i.e.
	 * matching values that are greater than or equal to the specified value.
	 * @param $name the name of the field
	 * @param $from the lower bound
	 * @return string a SQL expression to perform the match
	 */
	function from($name, $from)
	{
		
	}
	
	/**
	 * Generates the constraint expression for 'to' matching parameters, i.e.
	 * matching values that are less than or equal to the specified value.
	 * @param $name the name of the field
	 * @param $to the upper bound
	 * @return string a SQL expression to perform the match
	 */
	function to($name, $to)
	{
		
	}

	/**
	 * Generates the constraint expression for 'range' matching parameters, i.e.
	 * matching values that within the range of the two specified values.
	 * @param $name the name of the field
	 * @param $from the lower bound
	 * @param $to the value to match
	 * @return string a SQL expression to perform the match
	 */
	function range($name, $from, $to)
	{
	}
	
	
	/* 
	 * Generate the constraint expression for set membership matching
	 * paramters, matching values in a comma delimite set.
	 * 
	 * @param $name the name of the field
	 * @param $set a comma delimited set of values to match
	 * @return string a SQL expression to peform the match
	 */
	function member($name, $set)
	{
	}
	

	/* 
	 * Generate the constraint expression for matching any
	 * word in a group of words.
	 * 
	 * @param $name the name of the field
	 * @param $value a string that may contain multiple words
	 * @return string a SQL expression to peform the match
	 */
	function any($name, $value)
	{	
	}
	
	/* 
	 * Generate the constraint expression for matching all
	 * words in a group of words but allowing other words
	 * in between.
	 * 
	 * @param $name the name of the field
	 * @param $value a string that may contain multiple words
	 * @return string a SQL expression to peform the match
	 */
	function all($name, $value)
	{	
	}
	
	/*
	 * Generate the constraint expression for matching
	 * the f_name and l_name fields to a given value.
	 * 
	 * @param $f_name the name of the first name field
	 * @param $l_name the name of the last name field
	 * @param $value a string that may contain multiple words
	 * @return string a SQL expression to perform the match
	 */
	function fullName($f_name, $l_name, $value)
	{
		
	}
}

/**
 * NumberSearchParameterHandler is a SearchParameterHandler for numeric database types.
 * @author andy
 *
 */
class NumberSearchParameterHandler extends SearchParameterHandler
{
	function equal($name, $value)
	{
		checkNumeric($value);
		
		return "$name=$value";
	}
	
	function any($name, $value)
	{
		trace("NUMBER: ANY!!!", 3);
		
		checkNumeric($value);
		
		if (is_array($value))
		{
			if (count($value) == 0) return "";
			return "$name in (".implode(",", array_values($value)).") ";
		}
		else return "$name=$value";
	}	
		
	function like($name, $value)
	{
		die("Inappropriate matching mode");
	}

	function startsWith($name, $value)
	{
		die("Inappropriate matching mode");
	}
		
	function from($name, $from)
	{
		checkNumeric($from);
		return "$name >= $from";
	}
	
	function to($name, $to)
	{
		checkNumeric($to);
		return "$name <= $to";
	}
		
	function range($name, $from, $to)
	{
		checkNumeric($from);
		checkNumeric($to);
		
		return "($name >= $from AND $name <= $to)";
	}
	
	/* 
	 * Use when checking if a number is a member of a set.
	 * For example, teachers_key in (8312, 5643, 9893) or
	 * public in (2,4,5). Set can must be comma delimited
	 * or an array where the keys are the values to
	 * be included in the set.
	 * 
	 */
	function member($name, $set)
	{
		$values = $set;
		if(is_array($set))
			$values = implode(",", array_keys($set));
		return "$name IN ($set)";
	}
	
}

/**
 * StringSearchParameterHandler is a SearchParameterHandler for text database types.
 * @author andy
 *
 */
class StringSearchParameterHandler extends SearchParameterHandler
{
	function equal($name, $value)
	{
		$txt = mysql_escape_string($value);
		return "$name='$txt'";
	}
	
	function like($name, $value)
	{
		$txt = mysql_escape_string($value);
		return "$name LIKE '%$txt%'";
	}
	
	function startsWith($name, $value)
	{
		$txt = mysql_escape_string($value);
		return "$name LIKE '$txt%'";
	}
	
	function from($name, $from)
	{
		$from = mysql_escape_string($from);
		return "$name >= '$from'";
	}
	
	function to($name, $to)
	{
		$to = mysql_escape_string($to);
		return "$name <= '$to'";
	}
	
	function range($name, $from, $to)
	{
		$from = mysql_escape_string($from);
		$to = mysql_escape_string($to);
		
		return "($name >= '$from' AND $name <= '$to')";
	}
	
	function any($name, $value)
	{
		$value = mysql_escape_string($value);
		$value = preg_replace("/\\s+/", " ", $value);
		$words = explode(" ", $value);
		
		if (count($words) == 0) return;
				
		$query .= " (";
		$first = true;
		foreach($words as $word)
		{
			if (!$first) $query .= " OR ";
			$query .= "$name LIKE '%$word%'";
			$first = false;
		}
		$query .= ")";	
		
		return $query;
	}
	
	function all($name, $value)
	{
		$value = mysql_escape_string($value);
		$value = preg_replace("/\\s+/", " ", $value);
		
		$words = explode(" ", $value);
			
		if (count($words) == 0) return;
						
		$query .= " (";
		$first = true;
		foreach($words as $word)
		{
			if (!$first) $query .= " AND ";
			$query .= "$name LIKE '%$word%'";
			$first = false;
		}
		$query .= ")";	
		
		return $query;
	}
	

	function fullName($name, $value)
	{
		$value = mysql_escape_string($value);
		$value = preg_replace("/\\s+/", " ", $value);
		$words = explode(" ", $value);
		
		if(count($words) < 2) return;
		
		$idx = 0;
		$query = " (";
		$first = true;
			
		while($idx < count($words)-1)
		{
			if (!$first) $query .= " OR (";
					
			$query .= "first_name LIKE '{$words[$idx]}%' 
			AND last_name LIKE '{$words[$idx+1]}%'";

			$idx += 1;
			$query .= ") ";
			$first = false;
		}

		return $query;
	}	
	
	/* 
	 * Use when checking if a number is a member of a set.
	 * For example, status in ('in_progress', 'submitted')
	 * Set must be comma delimited.
	 */
	function member($name, $set)
	{
		$values = $set;
		if(is_array($set))
			$values = "'". implode("','", array_keys($set)) . "'";
		return "$name IN ($values)";
	}
}


/**
 * DateSearchParameterHandler is a SearchParameterHandler for date database types.
 * @author andy
 *
 */
class DateSearchParameterHandler extends SearchParameterHandler
{
	function reformatToSQLDate($date)
	{
		trace($date, 3);
		$fields = array();
		if (preg_match("|^(\\d+)[\-/](\\d+)[\-/](\\d{4})$|", $date, $fields))
		{
			$date = $fields[3]."-".$fields[1]."-".$fields[2];
			// JDG 5/2012  - need month and day to have "0" if single digit
			$date = date('Y-m-d', strtotime($date));
		}
		// datetime
		else if (preg_match("|^(\\d+)/(\\d+)/(\\d{4})\\s+(\\d+:\\d+:\\d+)|", $date, $fields))
		{
			// JDG 5/2012  - need month and day to have "0" if single digit
			$date = date('Y-m-d', strtotime($date));
			$date = $fields[3]."-".$fields[1]."-".$fields[2]." ".$fields[4];
		}
	
		return mysql_escape_string($date);
	}


	function equal($name, $value)
	{
		$date = $this->reformatToSQLDate($value);
		return "$name='$date'";
	}
	
	function like($name, $value)
	{
		//TODO: Fuzzy Date Matching Logic
		die("Inappropriate matching mode");
	}

	function startsWith($name, $value)
	{
		die("Inappropriate matching mode");
	}
		
	function from($name, $from)
	{
		
		$from = $this->reformatToSQLDate($from);
		return "$name >= '$from'";
	}
	
	function to($name, $to)
	{
		$to = $this->reformatToSQLDate($to);
		return "$name <= '$to'";
	}
	
	function range($name, $from, $to)
	{
		$from = $this->reformatToSQLDate($from);
		$to = $this->reformatToSQLDate($to);
		
		return "($name BETWEEN '$from' AND '$to')";
	}
}

class SearchParameters
{
	var $target;
	var $params;
	var $handlers;
	var $joinWith = "AND"; // AND or OR
	var $paramHandlers = array(); // custom search handlers
	
	function SearchParameters($target)
	{
		$this->target = $target;
		$this->params = array();
		
		$this->handlers = array();

		$this->handlers[String] 	= new StringSearchParameterHandler();
		$this->handlers[HTML] 		= new StringSearchParameterHandler();
		$this->handlers[Date]		= new DateSearchParameterHandler();
		$this->handlers[DateTime]	= new DateSearchParameterHandler();
		$this->handlers[Timestamp]	= new DateSearchParameterHandler();
		$this->handlers[Number]		= new NumberSearchParameterHandler();
		$this->handlers[Currency]	= new NumberSearchParameterHandler();
		$this->handlers[Boolean]	= new NumberSearchParameterHandler();		
	}
	
	function get($field, $mode)
	{
		$value = $this->params["{$field}:$mode"];
		trace("SearchParameters::get({$field}:$mode): $value", 3);
		return $value;
	}
	
	/*
	 * Override default handling by setting
	 * a custom callback to generate the
	 * constraint for a field.
	 * 
	 * @param $field - the field name in the obj to be queried
	 * 
	 * @handler - callback function such as array(MyClass, MyFunction)
	 * 
	 * When the function is called, it is given the 
	 * parameters $name and $value
	 */
	function setHandler($field, $handler)
	{
		$this->paramHandlers[$field] = $handler;	
	}
		
	function fromArray($array)
	{
		$fields = array_keys($this->target->fields);
		foreach($array as $field => $value)
		{
			list($fieldname, $mode) = explode(":", $field);

			if ($this->target->filter && $this->target->filter->isExcluded($fieldname)) continue;
			if (!$mode) continue;
			if ($value != "")
			{
				if (is_array($value))
				{
					$this->params[$field] = implode(",", array_values($value));
				}
				else
				{
					$this->params[$field] = $value;
				}
			}
		}
	}
	
	/*
	 * Set additional parameters that don't come from search
	 * form input.
	 */
	function setParam($field, $mode, $value)
	{
		$this->params["{$field}:{$mode}"] = $value;
	}
	
	function set($field, $mode, $value)
	{
		$this->setParam($field, $mode, $value);
	}
	
	function fromGET()
	{
		$this->fromArray($_GET);
	}

	function fromPOST()
	{
		$this->fromArray($_POST);
	}
	
	function remapField($old, $new)
	{
		foreach($this->params as $key => $value)
		{
			list($field, $mode) = explode(":", $key);
			if ($field == $old)
			{
				$this->params["$new:$mode"] = $value;
				unset($this->params["$old:$mode"]);
				return;
			}
		}
	}
	
	function toQueryString()
	{
		$qs = "";
		
		foreach($this->params as $name => $value)
		{
			if ($value == "") continue;
			
			$qs .= ($qs == "") ? "?" : "&";
			
			$qs .= urlencode($name);
			$qs .= "=";
			$qs .= urlencode($value);	
		}
		
		return $qs;
	}
	
	/*
	 * generateConstraint
	 * 
	 * @first: whether this set of clauses is the first in a group
	 * 
	 * @firstText: the text is this is the first clause: either
	 * WHERE or AND ( 
	 */
	function generateConstraint($first = true, $firstText = "WHERE")
	{
		$constraint = "";
					
		foreach($this->params as $param => $value)
		{
			$joinWith = (!$first) ? $this->joinWith : "";
			 
			if (strpos($param, ':') > 0)
			{
				list($field, $modifier) = explode(":", $param);
			}
			else
			{
				$field = $param;
				$modifier = "equal";
			}
			
			// JDG 10/25/2011 - allow callback function for clauses
			if(!array_key_exists($field, $this->paramHandlers))
			{
				$fields = $this->target->getFields();
				$handler = $this->handlers[$fields[$field]];
				if ($handler == null)
					die("Unknown Type: {$this->target->fields[$field]} for field $field");
				$clause = $this->getClause($handler, $modifier, $field, $value);			
			}
			else
			{
				$handler = $this->paramHandlers[$field];
				if(is_array($handler) && method_exists($handler[0], $handler[1]))
					$clause = call_user_func($handler, $field, $value);
				elseif(method_exists($handler, $modifier))
					$clause = $handler->$modifier($field, $value);
			}
	
			$constraint .= $this->joinClauses($first, $clause, $joinWith, $firstText);
		}
		
		$this->closeClause($constraint, $firstText);
		
		return $constraint;
	}
	
	function getClause($handler, $modifier, $field, $value)
	{
		$clause = "";
			
		switch($modifier)
		{
			case "equal":

				$clause = $handler->equal($field, $value);
				break;
				
			case "like":
				
				$clause = $handler->like($field, $value);
				break;
				
			case "startsWith":
				
				$clause = $handler->startsWith($field, $value);
				break;				
				
			case "from":

				if (array_key_exists("{$field}:to", $this->params))
				{
					trace("date range", 3);
					$to = $this->params["{$field}:to"];
					$clause = $handler->range($field, $value, $to);
				}
				else
				{
					$clause = $handler->from($field, $value);
				}
				break;
				
			case "to":
				
				if (!array_key_exists("{$field}:from", $this->params))
				{
					$clause = $handler->to($field, $value);
				}
				break;
				
			case "member":
				
				$clause = $handler->member($field, $value);
				break;

			case "any":
				
				$clause = $handler->any($field, $value);
				break;				

			case "all":
				
				$clause = $handler->all($field, $value);
				break;
				
			case "fullName":
				
				$clause = $handler->fullName($field, $value);
				break;
			
		} // end case statement
		
		return $clause;
	}	
	
/*
 * joinClauses
 * 
 * @first: whether this is the first clause; if so start with "WHERE"
 * 
 * @clause: the statement, e.g., l_name like 'smith'
 * 
 * @joinWith: specify is statement is to be joined with OR or AND
 * 
 * @firstText: the text to output if first: WHERE or "" - clause may
 * be first in a group "(" rather than first of all clauses.
 */
	static function joinClauses(&$first, $clause, $joinWith = "AND", $firstText)
	{
		if ($clause != "")
		{
			if ($first)
			{
				$constraint = "{$firstText} $clause";
				$first = false;
			}
			else
			{
				$constraint .= " $joinWith $clause";
			}
		}

		return $constraint;
	}
	
	static function closeClause(&$constraint, $firstText)
	{
		/*
		 * If no clauses were created, remove the
		 * $firstText if "AND (" or "OR (".
		 * Close out an opened group of clauses, by
		 * adding the ")";
		 */
		if($constraint == "") return;
		
		if(strpos($firstText, "(") !== false)
		{
			$constraint .= ") ";
		}
	}
}



/**
 * SearchForm generates forms for searching based on a supplied DataItem.
 * 
 * Since SearchForm extends AutoForm, functionality such as alias,
 * hide, layout and field grouping, and custom field renderer are available.
 * 
 * Example to create the search form:
 * 
 * $order = new Order();
 * $order->filter = new InclusionFilter("order_date", "first_name, "last_name);
 *  
 * // on submit, system redirects to "review_orders" identifier and retrives
 * // values through GET
 * $form = new SearchForm($order, "GET", "review_orders");
 * 
 * // default matching mode is equal but can be set to others available
 * // in the SearchParameters
 * $form->setMatchingMode("startsWith", "first_name", "last_name");
 * $form->setMatchingMode("range", "order_date");
 * 
 * $script .= $form->writeScript();
 * $form->drawForm();
 * 
 * ------------------
 * Example of the page that processes the constraint:
 * 
 * $order = new Order();
 * $order->filter = new InclusionFilter("order_date", "first_name, "last_name);
 *  
 * // on submit, system redirects to "review_orders" identifier and retrives
 * // values through GET
 * $search = new SearchParameters($order);
 * $search->fromGET();
 * 
 * $constraint = $search->generateConstraint();
 * 
 * -------------------
 * 
 * Example of drawing a readonly SearchForm on the results page
 * to show the user the criteria on which they searched.
 * 
 * $order = new Order();
 * $order->filter = new InclusionFilter("order_date", "first_name, "last_name);
 * $form = new SearchForm($order, "GET", "review_orders");
 * $form->readOnlyForm = true;
 * $form->button("Search Again", "order_search_form");
 * 
 * $script .= $form->writeScript();
 * $form->drawForm();
 * 
 *
 * @author andy
 */
class SearchForm extends AutoForm
{
	var $modifiers;
	var $params;
	var $hasSearchCriteria = false;
	var $emptyMessage = "No search criteria was provided.";
	
	var $searchModeTypes = array("like"	=>  "exact phrase match",
								 "all"	=>	"match all words in any order",
								 "any"	=>	"match any words");	
	/**
	 * Create a new Search Form.
	 * @param $target the target DataItem object
	 * @param $method (optional) the HTTP method to use for submitting the search request
	 * @param $action (optional) the URL of the recipient script for the search request
	 * @param $id (optional) the DOM ID for the generated search form
	 * @return SearchForm
	 */
	function SearchForm($target, $method="POST", $action="", $id="")
	{
		$this->AutoForm($target, $method, $action, $id);
		$this->modifiers = array();
		$this->params = new SearchParameters($target);
	}
	
	/**
	 * Sets the matching mode for the specified fields.
	 * @param $mode the matching mode, which can be one of "equal", "like", "startsWith", "from" or "to"
	 */
	function setMatchingMode($mode)
	{
		$fields = func_get_args();
		for($i = 1; $i < count($fields); ++$i)
		{
			$this->modifiers[$fields[$i]] = $mode;
		}
		
	}
	
	/**
	 * Retrieve the matching mode that was set for the specified field.
	 * @param $field the field to query
	 * @return string the matching mode set for that field.
	 */
	function getMatchingMode($field)
	{		
		$mode = (array_key_exists($field, $this->modifiers)) ? $this->modifiers[$field] : "equal";
		return $mode;
	}
	
	/**
	 * Populate the search parameters from an HTTP GET request
	 */
	function populateFromGET()
	{
		$this->params->fromGET();
	}
	
	/**
	 * Populate the search parameters from an HTTP POST request
	 */
	function populateFromPOST()
	{
		$this->params->fromPOST();
	}
	
	function populateFromREQUEST()
	{
		$this->params->fromArray($_REQUEST);
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
		$this->params->paramHandlers[$field] = $handler;
	}
	
	/**
	 * Generate any javascript required by the search form.
	 * @see framework/AutoForm#writeScript()
	 */
	function writeScript()
	{
		ob_start();
		
		$obj =& $this->data;
		$script = "";				
		foreach(array_keys($obj->fields) as $field)
		{
			trace("SearchForm writeScript field $field", 3);
			if ($field != $pk&& 
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$mode = $this->getMatchingMode($field);
				
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->renderSearchScript($field, $mode);
				}
			}		
		}
		
		// JDG 7/15/2011 - need scripts for additional fields
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
				
			$mode = $this->getMatchingMode($field);
			$renderer->renderSearchScript($field, $mode);
		}	

		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}	
	
	/**
	 * Write out the HTML for the search form.
	 * @see framework/AutoForm#drawForm()
	 */
	function drawForm()
	{
		if ($this->readOnlyForm) return $this->drawReadOnly();
		
		$obj =& $this->data;
		$pk = $obj->primary_key;
		
		$submitLabel = $this->submitLabel;
		if ($submitLabel == "")
		{
			$submitLabel = "Search ".pluralize($obj->prettifyClassName(true));
		}
		
		if (!$this->subordinate)
		{
			echo "<form id='{$this->id}' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
			if (!$this->ajaxSubmitHandler) echo " onsubmit='return onSubmit{$this->id}(this);'";
			echo ">\n";
		}
		
		if ($this->id)
		{
			echo "<input type='hidden' name='#FORMID#' value='{$this->id}'/>";
		}
		
		foreach(array_keys($this->hidden) as $hidden)
		{
			echo "<input id='{$hidden}' type='hidden' name='$hidden' value='".htmlSafe($obj->get($hidden))."'/>\n";
		}	
		
		$this->layout->startUngrouped();
		
		if ($this->buttons_at_top && !$this->subordinate) 
		{
			$this->layout->startButtonLine();
			
			$this->drawSubmitButtons($obj, $pk, $submitLabel);

			$this->drawButtons();

			$this->layout->endButtonLine();
		}

		$this->layout->errorBox();

		if ($this->markRequiredFields && $this->validator->hasRequiredFields())
		{
			$this->layout->requiredFields($this->requiredFieldsText);
		}

		$this->renderSearchFields();
					
		if (!$this->subordinate)
		{		
			$this->layout->startButtonLine();

			$this->drawSubmitButtons($obj, $pk, $submitLabel);
			$this->drawButtons();
	
			$this->layout->endButtonLine();
		}

		$this->layout->endUngrouped();
		
		if (!$this->subordinate)
		{
			echo "</form>\n";
		}
	}

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
				$this->renderOneSearchField($field);
			}
		}
		
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
			if (!array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
			{
				$this->renderOneSearchField($field);
			}	
		}
	}
	
	/*
	 * 3/12/10, JG, make separate function from drawForm
	 * so that inheriting classes can create a customized
	 * form layout and call this function from its own
	 * drawForm. 
	 */
	function renderOneSearchField($field)
	{
		$mode = $this->getMatchingMode($field);
		$renderer = $this->getRenderer($field);
			
		if(!$renderer && !$this->data->hasField($field) && $this->data->fields[$field] != "Timestamp")
		{
			foreach($this->additional as $r)
			{
				$renderer = $r['renderer'];
				$field = $r['field'];
				if(!$field)
					$field = $renderer->field;
			}
		}
			
		if ($renderer)
		{
			if(!$this->readOnlyForm)
			{
				$renderer->renderSearchField($field, $mode);
			}
			else if($this->readOnlyForm && (get_class($renderer) != DateFieldRenderer || $mode != "range"))
			{
				$value = $this->params->get($field, $mode);
				if($value)
				{
					// Set the value into the form's data obj so that the
					// renderReadOnly function works the same as with AutoForm
					$this->data->set($field, $value);
					$renderer->renderReadOnly($field);
					$this->hasSearchCriteria = true;
				}
			}
			else if($this->readOnlyForm)
			{
				$date_from = $this->params->get($field, "from");
				$date_to = $this->params->get($field, "to");
				if($date_from || $date_to)
				{
					$renderer->renderDateRangeReadOnly($field, $date_from, $date_to);
					$this->hasSearchCriteria = true;
				}
			}
			// count as rendered even if we didn't render because readonly and
			// no value so that we don't try to render again
			$this->renderedFields[$field] = true;
		}
	}
	
	function renderAdditionalFields()
	{
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;

			$mode = $this->getMatchingMode($field);
			
			if(!array_key_exists($field, $this->renderedFields))
			{
				$renderer->renderSearchField($field, $mode);
			}
			$this->renderedFields[$field] = true;
		}
	}
	
	/**
	 * Draws the form in read-only mode. Used to display
	 * the search criteria on the results screen.
	 */
	function drawReadOnly()
	{
		echo "<table";
		if ($this->formCSS) echo " class='{$this->formCSS}'";
		if ($this->style) echo " style='{$this->style}'";
		echo ">\n";

		$obj =& $this->data;
		$pk = $obj->primary_key;
		$filter = $obj->getFilter();
		
		foreach($this->params->params as $param => $value)
		{
			list($field, $modifier) = explode(":", $param);
			if(!array_key_exists($field, $this->modifiers))
				$this->setMatchingMode($modifier, $field);
		}
		
		if (count($this->groups) > 0)
		{
			foreach($this->groups as $legend => $fields)
			{			
				foreach($fields as $field)
				{
					if ($field != $pk && !array_key_exists($field, $this->hidden) && 
					!array_key_exists($field, $this->renderedFields) && 
					!($filter && $filter->isExcluded($field)))
					{
						$this->renderOneSearchField($field);
					}					
				}
			}		
		}
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
			{
				$this->renderOneSearchField($field);
			}
		}
		
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
			if (!array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
			{
				$this->renderOneSearchField($field);
			}	
		}		
					
		if(!$this->hasSearchCriteria)
		{
			echo "<tr>\n<td style='padding-top: 10px; padding-bottom: 10px'><i>{$this->emptyMessage}</i></td>\n</tr>\n";
		}
		
		ob_start();
		$this->drawButtons();
		$buttons .= ob_get_contents();
		ob_end_clean();
		
		if($buttons)
			echo "<tr><td>".  preg_replace("/^(&nbsp;)+/", "", $buttons) . "</td>\n<tr>\n";
		
		echo "</table>";
	}
}

?>