<?php
/**\addtogroup orm
 * @{
 */
/**************************************************************

Copyright (c) 2013 Sonjara, Inc

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

/**
 * AbstractPivotQuery provides the common base class for the 
 * shaped pivot query classes PivotQuery, IndexexPivotQuery and
 * GroupedPivotQuery.
 * 
 * Every pivot consists of a 'base', 'range' and 'pivot'. The
 * base is the root object, the pivot is the object that will
 * be pivoted (i.e. have rows converted to columns in the output)
 * and the range is the object that provides the range of values
 * over which this conversion will occur.
 * 
 * @author Andrew Green
 */
abstract class AbstractPivotQuery extends AbstractQuery
{
	var $baseClass;
	var $baseConstraint;
	var $baseItem;
	
	var $rangeClass;
	var $rangeConstraint;
	
	var $pivotClass;
	var $pivotConstraint;
	
	var $pivotItem;
	
	var $fields;
	var $fieldAliases;
	
	var $additionalFields = array();
	var $groupByFields = array();
	
	var $orderBy;
	var $totalField = null;
	
	/**
	 * Constructs a new AbstractPivotQuery
	 * @param string $base Class name of the DataItem that will be the base
	 * @param string $baseConstraint SQL constraint over which the base query will operate.
	 */
	function __construct($base, $baseConstraint = "")
	{
		$this->baseClass = $base;
		$this->baseConstraint = $baseConstraint;
	
		$this->baseItem = new $this->baseClass;
	
		foreach($this->baseItem->getFields() as $field => $type)
		{
			$this->baseConstraint = preg_replace("/\\b".$field."\\b/i", "b.$field", $this->baseConstraint);
		}
	
		$matches = array();
		
		if (preg_match("/ORDER BY.*/i", $this->baseConstraint, $matches))
		{
			$this->orderBy = $matches[0];
			$this->baseConstraint = str_replace($this->orderBy, "", $this->baseConstraint);
		}
		
		$this->pivotItem = null;
	
		$this->page = -1;
		$this->size = -1;
	
		$basePK = $this->baseItem->getPrimaryKey();
		$this->fields[$basePK] = $this->baseItem->getType($basePK);
	
		$this->additionalFields[] = "b.{$basePK} as {$basePK}";
		$this->groupByFields[] = "b.{$basePK}";
	}

	/**
	 * Adds the pivot to the query.
	 * @param string $pivotClass the class of DataItem that will be the pivot
	 * @param string $pivotConstraint SQL constraint over which the pivot query will operate
	 * @param string $pivotFunction default SQL aggregation function used to reduce rows when pivoting (default is SUM, but MAX, MIN, AVG, etc. can also be useful)
	 * @return AbstractPivotQuery
	 */
	function pivot($pivotClass, $pivotConstraint, $pivotFunction = "SUM")
	{
		$this->pivotClass = $pivotClass;
		$this->pivotConstraint = $pivotConstraint;
	
		$this->pivotItem = new $this->pivotClass;
	
		foreach($this->pivotItem->getFields() as $field => $type)
		{
			$this->pivotConstraint = preg_replace("/\\b".$field."\\b/i", "p.$field", $this->pivotConstraint);
		}
	
		return $this;
	}
	
	/**
	 * Adds the range to the query
	 * @param string $rangeClass the class of DataItem that will be supplying the range for the pivot
	 * @param string $rangeConstraint SQL constraint over which the range will operate
	 * @return AbstractPivotQuery
	 */
	function range($rangeClass, $rangeConstraint)
	{
		$this->rangeClass = $rangeClass;
		$this->rangeConstraint = $rangeConstraint;
		return $this;
	}
	
	/**
	 * Adds a fixed field to the output object. These fields can be from the base object
	 * or the pivot object. If they are from the pivot object, then the resulting output
	 * will be subgrouped by each fixed field.
	 * @param string $field field in the format "Object.field"
	 * @param string $alias Alias name for inclusion as labels or column headings in output
	 * @param string $type Type of the field
	 * @return AbstractPivotQuery
	 */
	function field($field, $alias = "", $type = String)
	{
		$field = preg_replace("/\\b".$this->baseClass."\\b/i", "b", $field);
		$field = preg_replace("/\\b".$this->pivotClass."\\b/i", "p", $field);
	
		$bareField = preg_replace("/^.*?\\./", "", $field);
	
		$this->fields[$bareField] = String;
		$this->additionalFields[] = "$field as $bareField";
		$this->groupByFields[] = $field;
	
		return $this;
	}
	
	/**
	 * Adds a pivot field format to the output object. One matching output field will be added
	 * for each entry in the range.
	 * @param string $format format specifier for the output field (i.e. "pivot_{pivot_id}")
	 * @param string $value value expression that the field will be populated with. 
	 * This could be simply a field in the pivot, or it might be a more complex SQL expression
	 * @param string $alias Alias name for inclusion as labels or column headings in output
	 * @param string $expression SQL aggregation function used to reduce rows when pivoting (default is SUM, but MAX, MIN, AVG, etc. can also be useful)
	 * @param string $type Type of the field
	 * @return AbstractPivotQuery
	 */
	function pivotField($format, $value, $alias = null, $expression = "SUM", $type = Number)
	{
		if (!$alias) $alias = $format;
	
		$this->fieldNameFormats[$format] = array("expression" => $expression, "alias" => $alias, "value" => $value, "type" => $type);
		return $this;
	}
	
	/**
	 * Builds the expressions for the pivot query
	 * @throws FakoliExpression
	 * @return Array
	 */
	protected function generateFieldExpressions()
	{
		if (!$this->rangeClass)
		{
			if (!$this->pivotClass)
			{
				throw new FakoliExpression("No Range or Pivot class provided");
			}
				
			$this->rangeClass = $this->pivotClass;
			$this->rangeConstaint = $this->pivotConstraint;
		}
	
		$obj = new $this->rangeClass;
		$pivot = $obj->getPrimaryKey();
	
		$rangeItems = Query::create($this->rangeClass, $this->rangeConstraint)->execute();
	
		$expressionMap = array();
	
		foreach($rangeItems as $item)
		{
			foreach($this->fieldNameFormats as $fieldName => $defn)
			{
				$fieldName = $item->format($fieldName);
				$value = $defn["value"];
				$expression = $defn["expression"];
				$alias = $defn["alias"];
	
				$empty = ($defn["type"] == String) ? "''" : 0;
				
				$expressionMap[$fieldName] = "$expression(IF (p.{$pivot}=".$item->quoteFieldValue($pivot).", IFNULL(p.{$value}, {$empty}), {$empty}))";
				$this->fields[$fieldName] = $defn["type"];
				$this->fieldAliases[$fieldName] = $item->format($alias);
			}
		}
	
		return $expressionMap;
	}
	
	/**
	 * Generates SQL for the full pivot query, with grouping.
	 * @return string
	 */
	protected function generateQuery()
	{
		// First generate fields
	
		$expressionMap = $this->generateFieldExpressions();
	
		$additionalFields = implode(", ", $this->additionalFields);
		$groupByFields = implode(", ", $this->groupByFields);
	
		$base = $this->baseItem;
		$pivot = $this->pivotItem;
	
		$basePK  = $base->getPrimaryKey();
		$pivotPK = $pivot->getPrimaryKey();
	
		$query = "SELECT $additionalFields";
		foreach($expressionMap as $field => $expression)
		{
			$query .= ", ";
			$query .= "\n\t{$expression} as $field";
			$first = false;
		}
	
		$query .= "\nFROM {$base->table} b left outer join {$pivot->table} p \n";
	
		$query .= "ON (p.{$basePK}=b.{$basePK} ";
	
		$constraint = preg_replace("/^\\s*WHERE /i", "AND ", $this->pivotConstraint);
		$query .= " ".$constraint;
	
		$constraint = $this->baseConstraint ? $this->baseConstraint : "";
		$query .= ") $constraint GROUP BY $groupByFields";
		$query .= " ".$this->orderBy;
		
		$this->query = $query;
		return $query;
	}
	
	/**
	 * Creates an empty PivotItem, configured with fields and field aliases based on the pivot
	 * and range.
	 * @return PivotItem
	 */
	function createPivotItem()
	{
		return new PivotItem($this->fields, $this->baseClass, $this->baseItem->getPrimaryKey(), $this->fieldAliases);
	}
	
	/**
	 * Register additional fields from the base to be included in the PivotItem output
	 */
	function additionalFields()
	{
		for($i = 0; $i < func_num_args(); ++$i)
		{
			$field = func_get_arg($i);
			$type = $this->baseItem->getType($field);
			if ($type)
			{
				$this->fields[$field] = $type;
				$this->additionalFields[] = "b.{$field} as {$field}";
			}
		}
		return $this;
	}
}

/**
 * Standard Pivot Query, with support for paging.
 * @author andy
 */
class PivotQuery extends AbstractPivotQuery
{
	var $page;
	var $size;

	function __construct($base, $baseConstraint = "")
	{
		parent::__construct($base, $baseConstraint);
	}
	
	/**
	 * Sets the page number and size for constraining the result set by page.
	 * By default results are not paged.
	 * @param $page integer the page number
	 * @param $size integer the number of items per page
	 * @return Query reference to the Query object, to allow for call chaining.
	 */
	function page($page, $size)
	{
		$this->page = $page;
		$this->size = $size;
		return $this;
	}
	
	static function create($base, $baseConstraint = "")
	{
		return new PivotQuery($base, $baseConstraint);
	}
	
	/**
	 * @see AbstractQuery::execute()
	 */
	function execute()
	{
		$query = $this->generateQuery();
		
		trace("$query", 3);
		trace("Page: $this->page Size: $this->size", 3);
		$items = array();
		
		$size = $this->size;
		
		try
		{
			$db = ConnectionManager::getConnection();
				
			$result = $db->prepare($query);
			$result->execute($this->params);
				
			if ($this->page > 0)
			{
				$count = ($this->page - 1) * $this->size;
				while($count--)
				{
					$result->fetch();
				}
			}
					
			while($line = $result->fetch())
			{
				$item = $this->createPivotItem();
				$item->populate($line);
				$items[] = $item;
		
				--$size;
				if ($size == 0) break;
			}
						
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "query() failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}
		
		trace(count($items)." items found", 3);
		return $items;		
	}
}


/**
 * Run a pivot query and return the results indexed by a selected field.
 * @author andy
 */
class IndexedPivotQuery extends AbstractPivotQuery
{
	var $indexBy;

	function __construct($base, $baseConstraint = "", $indexBy = "")
	{
		parent::__construct($base, $baseConstraint);
		$this->indexBy = $indexBy;
	}

	/**
	 * Sets the field by which the results should be indexed.
	 * @param string $indexBy the field by which the results should be indexed.
	 * @return IndexedPivotQuery
	 */
	function indexBy($indexBy)
	{
		$this->indexBy = $indexBy;
		return $this;
	}

	/**
	 * @see AbstractQuery::execute()
	 */
	function execute()
	{
		$query = $this->generateQuery();
		
		trace("$query", 3);
		$items = array();
		$field = $this->indexBy;

		try
		{
			$db = ConnectionManager::getConnection();

			$result = $db->prepare($query);
			$result->execute($this->params);
				
			while($line = $result->fetch())
			{
				$item = $this->createPivotItem();
				$item->populate($line);
				$val = $item->get($field);

				if (array_key_exists($val, $items))
				{
					// Implicitly promote to array if there is a collision
					if (!is_array($items[$val]))
					{
						$items[$val] = array($items[$val]);
					}
					$items[$val][] = $item;
				}
				else
				{
					$items[$val] = $item;
				}
			}
				
			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "IndexedPivotQuery failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}

		return $items;
	}

	/**
	 * Creates a new IndexedPivotQuery instance
	 * @param string $base class name for the base objects
	 * @param string $baseConstraint query constraint for the base objects
	 * @param string $indexBy the field by which the results should be indexed.
	 * @return IndexedPivotQuery
	 */
	static function create($base, $baseConstraint = "", $indexBy = "")
	{
		return new IndexedPivotQuery($base, $baseConstraint, $indexBy);
	}
}


/**
 * Performs a pivot query against the database, returning an array of arrays of PivotItems,
 * grouped by a particular field. If the indexing field has unique values, indexedQuery() might be more appropriate.
 * @author andy
 *
 */
class GroupedPivotQuery extends AbstractPivotQuery
{
	var $groupBy;

	public function __construct($base, $baseConstraint = "", $groupBy = "")
	{
		parent::__construct($base, $baseConstraint);
		$this->groupBy = $groupBy;
	}

	/**
	 * Set the field by which results should be grouped
	 * @param string $groupBy the field by which results should be grouped
	 * @return GroupedPivotQuery
	 */
	function groupBy($groupBy)
	{
		$this->groupBy = $groupBy;
		return $this;
	}

	/**
	 * @see AbstractQuery::execute()
	 */
	function execute()
	{
		$query = $this->generateQuery();
		
		trace($query, 3);

		$items = array();

		$field = $this->groupBy;

		try
		{
			$db = ConnectionManager::getConnection();
				
			$result = $db->prepare($query);
			$result->execute($this->params);

			while($line = $result->fetch())
			{
				$item = $this->createPivotItem();
				$item->populate($line);
				$items[$item->$field][] = $item;
			}

			unset($result);
		}
		catch(PDOException $e)
		{
			$err = "GroupedPivotQuery failed - " . $e->getMessage();
			trace($err, 2);
			throw new DataItemException($err);
		}

		return $items;
	}

	/**
	 * Create a new GroupedPivotQuery
	 * @param string $class the base class for the query
	 * @param string $constraints the base constraint for the query
	 * @param string $groupBy the field by which results should be grouped
	 * @return GroupedPivotQuery
	 */
	static function create($class, $constraints = "", $groupBy = "")
	{
		return new GroupedPivotQuery($class, $constraints, $groupBy);
	}
}


/**
 * PivotItems are dynamically constructed DataItems that map the results
 * of PivotQueries. They can be used interchangeably with standard DataItems
 * (for instance, in DataListViews, PagedLists, etc). PivotItems are automatically
 * provided with a relation back to the class at the base of the pivot.
 * @author Andy Green
 */
class PivotItem extends DataItem
{
	var $fields = array();
	var $fieldAliases = array();
	var $relations = array();
	
	var $baseClass;
	
	/**
	 * Creates a new PivotItem
	 * @param array $fields the field map for the PivotItem 
	 * @param string $baseClass the base class from the pivot
	 * @param unknown $primaryKey the primary key field (usually the primary key of the base class)
	 * @param unknown $fieldAliases the field aliases map for the PivotItem
	 */
	function __construct($fields, $baseClass, $primaryKey, $fieldAliases = array())
	{
		$this->fields = $fields;
		$this->primary_key = $primaryKey;
		$this->baseClass = $baseClass;
		$this->fieldAliases = $fieldAliases;
		
		$this->relations = array($baseClass => $baseClass);
		parent::__construct();
	}

	/**
	 * Provide a relation pseudo-method that traverses back to the object at
	 * the base of the pivot.
	 */
	function __call($method, $args)
    {
    	// Provide a relation traversal pseudo-method that 
    	// allows traversal back to the base object for the
    	// pivot
    	
        if ($this->baseClass == $method)
        {
        	return $this->getRelated($this->baseClass);
        }        
	}
	
	function getTotal()
	{
		$total = 0.0;
		
		foreach($this->fields as $field => $type)
		{
			if ($field != $this->primary_key)
			{
				$total += $this->get($field);
			}
		}

		return $total;
	}
}

/**
 * The PivotTableHelper class provides support for configuring display tables
 * (DataListViews and GroupedDataListViews) dynamically, based upon the structure
 * of a pivot query.
 * @author Andy Green
 */
class PivotTableHelper
{
	var $pivot;
	var $table;

	var $columnSpecs = array();
	
	/**
	 * Creates a new PivotTableHelper
	 * @param object $pivot the source PivotQuery or GroupedPivotQuery
	 * @param object $table the target DataListView or GroupedDataListView
	 */
	function __construct($pivot, $table)
	{
		$this->pivot = $pivot;
		$this->table = $table;
	}
	
	/**
	 * Adds a column specification. Column specifications match fields by regular expression
	 * and provide the output format, styles and hints for the generated columns.
	 * @param string $pattern The field matching pattern
	 * @param string $format The output format template
	 * @param boolean $sortable Whether the column is sortable (optional)
	 * @param string $style CSS styles for the column (optional)
	 * @param string $typeHint Type hint for the column (optional)
	 * @param string $onExport Data export callback for the column (optional)
	 * @param string $sortFormat sorting format for the column (optional)
	 * @return PivotTableHelper
	 */
	function column($pattern, $format, $sortable = true, $style = "", $typeHint = null, $onExport = null, $sortFormat = null, $footerTotal = false)
	{
		$this->columnSpecs[$pattern] = array("format" => $format, "sortable" => $sortable, "style" => $style, 
											 "typeHint" => $typeHint, "onExport" => $onExport, "sortFormat" => $sortFormat, 
											 "footerTotal" => $footerTotal);
		return $this;
	}
	
	/**
	 * Adds columns to the table based on the column specifications that have been provided. 
	 */
	function create()
	{
		$item = $this->pivot->createPivotItem();
		
		$fields = $item->getFields();
		
		foreach($fields as $field => $type)
		{
			foreach($this->columnSpecs as $spec => $details)
			{
				$matches = array();
				if (preg_match("/{$spec}/", $field, $matches))
				{
					$format = str_replace("%FIELD%", $field, $details["format"]);
					
					if ($matches[1]) $format = str_replace("%KEY%", $matches[1], $format);
					
					$this->table->column($item->prettifyFieldName($field), $format, $details["sortable"], $details["style"], 
										 $details["typeHint"], $details["onExport"], $details["sortFormat"]);
					if ($details["footerTotal"])
					{
						$this->table->footerTotal($format);
					}
					break;
				}
			}
		}
	}
}

/**@}*/?>