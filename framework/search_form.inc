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
	 * @param string $name the name of the field
	 * @param mixed $value the value to match
	 * @return string a SQL expression to perform the match
	 */
	function equal($name, $value)
	{
	}
	
	/**
	 * Generates the constraint expression for 'like' matching parameters, i.e.
	 * matching a substring or partial match.
	 * @param string $name the name of the field
	 * @param mixed $value the value to match
	 * @return string a SQL expression to perform the match
	 */
	function like($name, $value)
	{
	}
	
	/**
	 * Generates the constraint expression for 'startsWith' matching parameters, i.e.
	 * matching a substring or partial match at the start of the field only.
	 * @param string $name the name of the field
	 * @param string $value the value to match
	 * @return string a SQL expression to perform the match
	 */	
	function startsWith($name, $value)
	{
			
	}
	
	/**
	 * Generates the constraint expression for 'from' matching parameters, i.e.
	 * matching values that are greater than or equal to the specified value.
	 * @param string $name the name of the field
	 * @param mixed $from the lower bound
	 * @return string a SQL expression to perform the match
	 */
	function from($name, $from)
	{
		
	}
	
	/**
	 * Generates the constraint expression for 'to' matching parameters, i.e.
	 * matching values that are less than or equal to the specified value.
	 * @param string $name the name of the field
	 * @param mixed $to the upper bound
	 * @return string a SQL expression to perform the match
	 */
	function to($name, $to)
	{
		
	}

	/**
	 * Generates the constraint expression for 'range' matching parameters, i.e.
	 * matching values that within the range of the two specified values.
	 * @param string $name the name of the field
	 * @param string $from the lower bound
	 * @param mixed $to the value to match
	 * @return string a SQL expression to perform the match
	 */
	function range($name, $from, $to)
	{
	}
	
	
	/** 
	 * Generate the constraint expression for set membership matching
	 * paramters, matching values in a comma delimite set.
	 * 
	 * @param string $name the name of the field
	 * @param string $set a comma delimited set of values to match
	 * @return string a SQL expression to peform the match
	 */
	function member($name, $set)
	{
	}
	

	/**
	 * Generate the constraint expression for matching any
	 * word in a group of words.
	 * 
	 * @param string $name the name of the field
	 * @param string $value a string that may contain multiple words
	 * @return string a SQL expression to peform the match
	 */
	function any($name, $value)
	{	
	}
	
	/** 
	 * Generate the constraint expression for matching all
	 * words in a group of words but allowing other words
	 * in between.
	 * 
	 * @param string $name the name of the field
	 * @param string $value a string that may contain multiple words
	 * @return string a SQL expression to peform the match
	 */
	function all($name, $value)
	{	
	}
	
	/**
	 * Generate the constraint expression for matching
	 * the f_name and l_name fields to a given value.
	 * 
	 * @param string $f_name the name of the first name field
	 * @param string $l_name the name of the last name field
	 * @param string $value a string that may contain multiple words
	 * @return string a SQL expression to perform the match
	 */
	function fullName($f_name, $l_name, $value)
	{
		
	}
	
	/**
	 * Generate the constraint expresssion for matching boolean
	 * only when the UI element is selected.
	 * @param string $name
	 * @param mixed $value
	 */
	function checked($name, $value)
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

		if (strpos($value, ",") !== FALSE)
		{
			$value = explode(",", $value);
		}
		
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
		trace("## NUMBERSEARCHPARAMETERHANDLER", 3);
		
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
		$txt = ConnectionManager::quote($value);
		return "$name=$txt";
	}
	
	function like($name, $value)
	{
		$txt = ConnectionManager::quote("%{$value}%");
		return "$name LIKE $txt";
	}
	
	function startsWith($name, $value)
	{
		$txt = ConnectionManager::quote("{$value}%");
		return "$name LIKE $txt";
	}
	
	function from($name, $from)
	{
		$txt = ConnectionManager::quote($from);
		return "$name >= $txt";
	}
	
	function to($name, $to)
	{
		$txt = ConnectionManager::quote($to);
		return "$name <= $txt";
	}
	
	function range($name, $from, $to)
	{
		$from = ConnectionManager::quote($from);
		$to = ConnectionManager::quote($to);
		
		return "($name >= $from AND $name <= $to)";
	}
	
	function any($name, $value)
	{
		$value = preg_replace("/\\s+/", " ", $value);
		$words = explode(" ", $value);
		
		if (count($words) == 0) return;
				
		$query .= " (";
		$first = true;
		foreach($words as $word)
		{
			$txt = ConnectionManager::quote("%{$word}%");
			if (!$first) $query .= " OR ";
			$query .= "$name LIKE $txt";
			$first = false;
		}
		$query .= ")";	
		
		return $query;
	}
	
	function all($name, $value)
	{
		$value = preg_replace("/\\s+/", " ", $value);
		
		$words = explode(" ", $value);
			
		if (count($words) == 0) return;
						
		$query .= " (";
		$first = true;
		foreach($words as $word)
		{
			$txt = ConnectionManager::quote("%{$word}%");
			if (!$first) $query .= " AND ";
			$query .= "$name LIKE $txt";
			$first = false;
		}
		$query .= ")";	
		
		return $query;
	}
	

	function fullName($f_name, $l_name, $value)
	{
		$value = preg_replace("/\\s+/", " ", $value);
		$words = explode(" ", $value);
		
		if(count($words) < 2) return;
		
		$idx = 0;
		$query = " (";
		$first = true;
			
		while($idx < count($words)-1)
		{
			if (!$first) $query .= " OR (";
					
			$txt1 = ConnectionManager::quote("%{$words[$idx]}%");
			$txt2 = ConnectionManager::quote("%{$words[$idx+1]}%"); 
			$query .= "{$f_name} LIKE $txt1 AND {$l_name} LIKE $txt2";

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
		trace("## STRINGSEARCHPARAMETERHANDLER", 3);
		if(!$set || !isset($set)) return "";
		
		$values = $set;
		if(is_array($set))
		{
			$values = "'". implode("','", array_keys($set)) . "'";
		}
		else if($set)
		{
			$values = "'".implode("','", explode(",", $set))."'";
			$values = preg_replace("/''/", "'", $values);
		}
	
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
	
		return ConnectionManager::escape($date);
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
		
		if ($from > $to)
		{
			$tmp = $from;
			$from = $to;
			$to = $tmp;
		}
		
		return "($name BETWEEN '$from' AND '$to')";
	}
}


class CrossReferenceSearchParameterHandler extends SearchParameterHandler
{
	var $cl;
	var $xref;
	var $foreignKey;
	
	function __construct($class, $xref, $foreignKey)
	{
		$this->cl = $class;
		$this->xref = $xref;
		$this->foreignKey = $foreignKey;
	}

	function member($name, $set)
	{
		$values = $set;
		if(is_array($set))
			$values = implode(",", array_keys($set));

		if (!$set) return "";
		
		trace("## Generating cross reference terms for $name using values ($values)", 3);

		$xref = new $this->xref;
		$obj = new $this->cl;
		$pk = $obj->getPrimaryKey();

		return "$pk IN (SELECT {$pk} FROM {$xref->table} WHERE {$this->foreignKey} IN ($set))";
	}
}

class BooleanSearchParameterHandler extends SearchParameterHandler
{
	function equal($name, $value)
	{
		trace("FILTER: equal $name $value", 3);
		checkNumeric($value);
	
		return "$name=$value";
	}
	
	function checked($name, $value)
	{
		trace("FILTER: checked $name $value", 3);
		checkNumeric($value);
		$out = ($value) ? "$name=$value" : "";
		trace("FILTER: out $out", 3);
		return $out;
	}
}

class SearchParameterValue
{
	var $mode;
	var $value;
	
	function __construct($mode, $value)
	{
		$this->mode = $mode;
		$this->value = $value;
	}
}

/**
 * The SearchParameters class interprets the set of input parameters for a search
 * and generates the corresponding SQL constraints to perform the search against
 * the database.
 * 
 * @author andy
 */
class SearchParameters
{
	var $form;
	var $target;
	var $params;
	var $handlers;
	var $joinWith = "AND"; // AND or OR
	var $paramHandlers = array(); // custom search handlers
	var $secondaryFields = array();
	var $additional = array();
	var $empty = true; ///< true if no search parameters were specified, false if any parameter was found
	
	private static $serializedFields = array("params", "handlers", "joinWith", "secondaryFields", "additional", "empty");
	function __sleep()
	{
		return SearchParameters::$serializedFields;
	}
	
	function copy($src)
	{
		$this->params = $src->params;
		foreach(SearchParameters::$serializedFields as $field)
		{
			$this->$field = $src->$field;
		}
	}
	
	function SearchParameters($target, $form = null)
	{
		$this->target = $target;
		$this->form = $form;
		$this->params = array();
		
		$this->handlers = array();

		$this->handlers[String] 	= new StringSearchParameterHandler();
		$this->handlers[HTML] 		= new StringSearchParameterHandler();
		$this->handlers[Date]		= new DateSearchParameterHandler();
		$this->handlers[DateTime]	= new DateSearchParameterHandler();
		$this->handlers[Timestamp]	= new DateSearchParameterHandler();
		$this->handlers[Number]		= new NumberSearchParameterHandler();
		$this->handlers[Currency]	= new NumberSearchParameterHandler();
		$this->handlers[Boolean]	= new BooleanSearchParameterHandler();		
	}
	
	function get($field, $mode = null)
	{
		if (!isset($this->params[$field])) return null;
		
		$val = $this->params[$field];
		if ($val->mode == "range")
		{
			if ($mode == "from")
			{
				$value = $val->from;
			}
			else if ($mode == "to")
			{
				$value = $val->to;
			}
			else return null;
		}
		else
		{
			if ($mode !== null && $val->mode != $mode) return null;
		}
		
		$value = $val->value;
		trace("SearchParameters::get({$field}:$mode): $value", 3);
		return $value;
	}
	
	/**
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
		if (!$this->target->hasField($field))
		{
			$this->additional[] = $field;
		}	
	}
		
	function fromArray($array)
	{
		$this->empty = true;
		$this->isDefaultState = true;
		
		$fields = array_keys($this->target->getFields());
		foreach($array as $field => $value)
		{
			list($fieldname, $mode) = explode(":", $field);

			$skip = false;
			
			if ($this->target->filter && $this->target->filter->isExcluded($fieldname))
			{
				trace("## Skipping $fieldname", 4);
				continue;
			}
			
			if (!$this->target->hasField($fieldname) &&
				(!$this->form || !$this->form->hasAdditional($fieldname))) 
			{
				trace("### Skipping $fieldname", 4);
				continue;
			}
		
			$this->isDefaultState = false;
			//if (!$mode) continue;
			if ($value != "")
			{
				$this->empty = false;
				
				$this->setParam($fieldname, $mode, $value);
			}
		}
	}
	
	/*
	 * Set additional parameters that don't come from search
	 * form input.
	 */
	function setParam($fieldname, $mode, $value)
	{
		if (is_array($value)) $value = implode(",", array_values($value));
		
		if (isset($this->params[$fieldname]))
		{
			$p = $this->params[$fieldname];
			if ($mode)
			{
				if ($mode == "from" && $p->mode == "to")
				{
					$p->mode = "range";
					$p->to = $p->value;
					$p->from = $value;
					$p->value = null;
				}
				else if ($mode == "to" && $p->mode == "from")
				{
					$p->mode = "range";
					$p->to = $value;
					$p->from = $p->value;
					$p->value = null;
				}
				else
				{
					$p->mode = $mode;
					$p->value = $value;
				}
			}
			else
			{
				$p->value = $value;
			}
		}
		else
		{
			$this->params[$fieldname] = new SearchParameterValue($mode, $value);
		}
	}
	
	/**
	 * Clear any parameters set for the specified field
	 * @param string $field the name of the field
	 */
	function clearParam($field)
	{
		foreach($this->params as $name => $value)
		{
			list($f, $mode) = explode(":", $name);
			if ($f == $field) unset($this->params["$f:$mode"]);
		}
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
			}
		}
	}
	
	function copyField($old, $new)
	{
		$this->params[$new] = $this->params[$old];
	}
	
	/**
	 * Specifies additional data fields to search when querying for the
	 * given source field.
	 * @param string $src name of the primary field
	 * @param [string] the list of additional fields to search
	 */
	function secondaryFields($src)
	{
		if (!isset($this->secondaryFields[$src]))
		{
			$this->secondaryFields[$src] = array();
		}

		for($i = 1; $i < func_num_args(); ++$i)
		{
			$this->secondaryFields[$src][] = func_get_arg($i);
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
	
	/**
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
					
		trace(print_r($this->params, true), 3);
		
		foreach($this->params as $field => $param)
		{
			$joinWith = (!$first) ? $this->joinWith : "";
			 
			$modifier = $param->mode;
			$value = $param->value;
			
// 			if (strpos($param, ':') > 0)
// 			{
// 				list($field, $modifier) = explode(":", $param);
// 			}
// 			else
// 			{
// 				$field = $param;
// 				$modifier = "equal";
// 			}
			
			// JDG 10/25/2011 - allow callback function for clauses
			if(!array_key_exists($field, $this->paramHandlers))
			{
				$fields = $this->target->getFields();
				$handler = $this->handlers[$fields[$field]];
				
				//AJG: Default to String if field type is not known
				if (!$handler) $handler = new StringSearchParameterHandler();
				$clause = $this->getClause($handler, $modifier, $field, $value);			
			}
			else
			{
				$handler = $this->paramHandlers[$field];
				if(is_callable($handler))
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
		$clause = array();

		$clause[] = $this->_getClause($handler, $modifier, $field, $value);
		
		if (isset($this->secondaryFields[$field]))		
		{
			$secondaries = $this->secondaryFields[$field];

			foreach($secondaries as $secondary)
			{
				$clause[] = $this->_getClause($handler, $modifier, $secondary, $value);
			}
			
			return "(".implode(" OR ", $clause).") ";
		}
		else
		{
			return $clause[0];
		}		
	}
	
	function _getClause($handler, $modifier, $field, $value)
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
				}
				else
				{
					$clause = $handler->from($field, $value);
				}
				break;
				
			case "to":
				
				$clause = $handler->to($field, $value);
				break;
			
			case "range":
				
				$param = $this->params[$field];
				$clause = $handler->range($field, $param->from, $param->to);
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
				
			case "checked":
				
				$clause = $handler->checked($field, $value);
				break;
				
			default:
				break;
			
		} // end case statement
		
		return $clause;
	}	
	
/**
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
	var $preserveQueryString = false;
	
	var $searchModeTypes = array("like"	=>  "exact phrase match",
								 "all"	=>	"match all words in any order",
								 "any"	=>	"match any words");	
	/**
	 * Create a new Search Form.
	 * @param DataItem $target the target DataItem object
	 * @param string $method (optional) the HTTP method to use for submitting the search request
	 * @param string $action (optional) the URL of the recipient script for the search request
	 * @param string $id (optional) the DOM ID for the generated search form
	 * @return SearchForm
	 */
	function SearchForm($target, $method="POST", $action="", $id="")
	{
		$this->AutoForm($target, $method, $action, $id);
		$this->modifiers = array();
		$this->params = new SearchParameters($target, $this);
		
		$this->validator = new ValidationEngine();
		$this->validator->id = $this->id;
	}
	
	/**
	 * Sets the matching mode for the specified fields.
	 * @param string $mode the matching mode, which can be one of "equal", "like", "startsWith", "from" or "to"
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
	 * @param string $field the field to query
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
	
	/**
	 * Override an existing SearchParameter constraint
	 * clause handler or add one. 
	 * 
	 * @param $field string name of the field
	 * @param $handler callable callback function that creates the
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

		$this->configureValidators();
		
		$obj =& $this->data;
		$script = "";				
		foreach(array_keys($obj->getFields()) as $field)
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
		
		echo "<script type='text/javascript'>\n";
		echo $this->validator->writeScript();
		echo "\n\n";
		echo "function onSubmit{$this->id}(form)\n{\n";
		
		echo "\tif (validate_{$this->id} != null && !validate_{$this->id}(form)) return false;\n";
		
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !($filter && $filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->renderOnSubmitHandler($field);
				}
			}
		}

		foreach($this->additional as $r)
		{
			$r['renderer']->renderOnSubmitHandler($r['field']);
		}

		if ($this->onSubmitHandler)
		{
			echo "\t if (!{$this->onSubmitHandler}(form)) return false;\n;";
		}
		
		if ($this->interstitialMessage)
		{
			echo "\tinterstitial('".jsSafe($this->interstitialMessage)."');\n;";
		}
	
		echo "\treturn true;\n}\n\n";
		
		
		echo "\twindow.addEvent('domready', function() {\n";

		if ($this->allowPartialSave)
		{
			$managerOptions = "{partialSaveContainer: '{$this->partialSaveContainerID}', partialSaveLabel: '{$this->partialSaveLabel}'}";
		}
		else
		{
			$managerOptions = "{}";
		}
		
		echo "\tnew AutoFormManager('{$this->id}', {$managerOptions});\n";
		echo "});\n";
		
		echo "</script>\n";
	
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}	
	
	/**
	 * Creates default validators for fields in the form. Currently this adds Date format validators
	 * to all date fields - other fields are not affected.
	 *
	 * This method is called automatically before the form is rendered.
	 *
	 */
	function addDefaultValidators()
	{
		$filter = $this->data->getFilter();
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($filter && $filter->isExcluded($field)) continue;
			if ($this->readonly[$field]) continue; // No need to validate readonly fields
			
			$renderer = $this->getRenderer($field);
			if ($renderer)
			{
				// JDG 9/19/2010 - let each renderer supply its own validator
				$required = (array_search($field, $this->required) === FALSE) ? false : true;
				$mode = $this->getMatchingMode($field);
				$renderer->addSearchValidatorsToForm($field, $mode, $required);
			}
		}

		foreach($this->additional as $r)
		{
			// JDG 5/2012 - check if additional field is required
			$required = (array_search($r['field'], $this->required) === FALSE) ? false : true;
			$mode = $this->getMatchingMode($field);
			$r['renderer']->addSearchValidatorsToForm($r['field'], $mode, $required);
		}
	}
		
	
	/**
	 * After AutoForm configures the validators, modify the field name
	 * to include the mode with which the tag will drawn in the form.
	 * For date range, the mode is "from" and an additional validator needs
	 * to be created for the "to" field.
	 * 
	 * @param obj $renderer - field renderer
	 * @param String $field - field name
	 * @param String $mode - matching mode
	 */
	function setSearchValidator($renderer, $field, $mode)
	{
		foreach($this->validator->validators as $v)
		{
			if($v->field != $field) continue;
			if($mode != "range")
			{
				$v->field = "{$v->field}:{$mode}";
			}
			else
			{
				$v->field = "{$v->field}:from";
				$toLabel = preg_replace("/between|to/i", "To", $v->title);
				$toV = new DateValidator("{$field}:to", $toLabel);
				$this->validator->add($toV);
			}
		}
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

		if ($this->preserveQueryString)
		{
			$obj->fromREQUEST();
		}
		
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
		
		if ($this->preserveQueryString)
		{
			foreach($_GET as $key => $value)
			{
				if ($key == "identifier" || $key == "section" || $key == "submit" || array_key_exists($this->hidden, $key) || $obj->hasField($key)) continue;
				echo "<input id='{$this->id}_{$key}' type='hidden' name='$key' value='".htmlSafe($value)."'/>\n";
			}
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
					if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && 
							($this->hasAdditional($field) || !($filter && $filter->isExcluded($field))))
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
			if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) &&
					($this->hasAdditional($field) || !($filter && $filter->isExcluded($field))))
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
			
			trace("Rendering additional field $field", 3);
			
			if (!array_key_exists($field, $this->renderedFields))
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
			
		if(!$renderer && !$this->data->hasField($field) && $this->data->getType($field) != "Timestamp")
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
	
	function renderAdditionalFields($obj)
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